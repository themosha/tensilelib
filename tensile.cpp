#include "tensile.h"

#include "argh/argh.h"
#include <iostream>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <signal.h>

namespace tensile {

const Status::Code Status::SUCCESS;
const Status::Code Status::ERROR;
const Status::Code Status::TIMEOUT;
const Status::Code Status::CRASH;

static const std::vector<std::string> code_to_text{"Success", "Error", "Timeout", "Crash"};
static const std::vector<char> code_to_char{'.', 'E', 'T', '#'};

std::string Status::ToString() const {
    if (code() < 0 || static_cast<size_t>(code()) > code_to_text.size()) {
        return "Unknown: " + std::to_string(code());
    }
    return code_to_text[code()] + (message_.empty() ? "" : ": " + message_);
}

char Status::ToChar() const {
    if (code() < 0 || static_cast<size_t>(code()) > code_to_char.size()) {
        return '?';
    }
    return code_to_char[code()];
}

Driver::Driver(int argc, char **argv) {
    argh::parser cmdl(argv);

    set_perftrace(cmdl["perftrace"]);
    set_check_crash(cmdl["check_crash"]);

    int timeout_ms;
    cmdl("timeout", 100) >> timeout_ms;
    set_timeout(std::chrono::milliseconds(timeout_ms));

    std::string provider_names;
    cmdl("providers") >> provider_names;
    set_provider_names(provider_names);

    std::string feature_names;
    cmdl("features") >> feature_names;
    set_feature_names(feature_names);
}

std::vector<Result> Driver::Run() {
    std::vector<Result> results;
    for (auto &provider: providers_) {
        if (!provider_names_to_check_.empty()) {
            if (provider_names_to_check_.find(provider->name()) == std::string::npos) {
                continue;
            }
        }
        provider->Init();
        std::cout << provider->name() << std::endl;
        for (auto &feature: GetBuiltinFeatures()) {
            if (!feature_names_to_check_.empty()) {
                if (feature->name().find(feature_names_to_check_) == std::string::npos) {
                    continue;
                }
            }
            Result result = Run(provider.get(), feature.get());
            results.emplace_back(result);
        }
    }
    return results;
}

Result Driver::Run(ISQLProvider *provider, ISQLFeature *feature) {
    if (!perftrace()) {
        std::cout << feature->name() << ":";
        std::flush(std::cout);
    }

    Status status;
    std::string sql;
    // First, run the feature doubling @n until it fails
    // TODO(moshap): Use Incrementer class to support different strategies.
    size_t n1 = 1;
    size_t n = n1;
    if (feature->is_exponential()) {
        for (; n < std::numeric_limits<size_t>::max(); n++) {
            Status current_status = CheckFeature(n, feature, provider);
            if (!perftrace_) {
                std::cout << current_status.ToChar();
                std::flush(std::cout);
            }
            status.Update(current_status);
            if (status.code() != Status::SUCCESS) {
                n1 = n;
                break;
            }
        }
    } else {
        for (;;) {
            Status current_status = CheckFeature(n, feature, provider);
            if (!perftrace_) {
                std::cout << current_status.ToChar();
                std::flush(std::cout);
            }
            status.Update(current_status);
            if (status.code() != Status::SUCCESS) {
                break;
            }
            n1 = n;
            // Watch size_t overflows
            if (n < std::numeric_limits<size_t>::max() / 2) {
                n *= 2;
            } else if (n < std::numeric_limits<size_t>::max()) {
                n = std::numeric_limits<size_t>::max();
            } else {
                break;
            }
        }
        // Now run binary search to discover the largest possible @n which succeeds
        if (!perftrace_ && status.code() != Status::SUCCESS) {
            size_t n2 = n;
            for (;;) {
                // Safe way to find half-point between n1 and n2 without calling n1+n2 which could overflow
                n = n1 + (n2 - n1) / 2;
                if (n1 == n || n2 == n) {
                    break;
                }
                Status current_status = CheckFeature(n, feature, provider);
                if (!perftrace_) {
                    std::cout << current_status.ToChar();
                    std::flush(std::cout);
                }
                status.Update(current_status);
                if (current_status.code() == Status::SUCCESS) {
                    n1 = n;
                } else {
                    n2 = n;
                }
            }
        }
    }
    if (!perftrace()) {
        std::cout << " limit = " << n1 << " status = " << status.ToString() << std::endl;
        std::flush(std::cout);
    }

    Result result;
    result.provider = provider->name();
    result.feature = feature->name();
    result.limit = n1;
    result.status = status;
    return result;
}

Status Driver::CheckFeature(size_t n, ISQLFeature *feature, ISQLProvider *provider) {
    // Fork the main execution, and then fork into two processes:
    // * checker - the one which actually checks SQL against provider
    // * timeout - the one which waits specified timeout and kills checkers
    //             process if it exceeded the timeout.
    std::string sql = feature->GenerateSQL(n);
    std::string error_msg;
    pid_t pid = check_crash_ ? fork() : 0;
    if (pid == 0) {
        pid_t checker_pid = check_crash_ ? fork() : 0;
        if (checker_pid == 0) {
            // Silence stderr, since some code writes there in case of errors.
            // TODO(moshap): Ideally we should detect whether provider code wrote to stderr and report it as a problem,
            // especially when provider is a library
            FILE *f = freopen("/dev/null", "w", stderr);
            clearerr(f);

            // Run SQL through provider
            auto start = std::chrono::high_resolution_clock::now();
            bool ok = provider->Run(sql, &error_msg);
            auto finish = std::chrono::high_resolution_clock::now();
            if (perftrace_) {
                std::cout << provider->name() << "," << feature->name() << "," << n << ","
                          << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count() << ","
                          << (ok ? "OK" : "ERROR")
                          << std::endl;
            }
            if (!check_crash_) {
                if (!ok) {
                    return Status(Status::ERROR, error_msg);
                }
                if (finish - start > timeout_) {
                    return Status(Status::TIMEOUT);
                }
                return Status(Status::SUCCESS);
            }
            exit(ok ? Status::SUCCESS : Status::ERROR);
        }

        pid_t timeout_pid = fork();
        if (timeout_pid == 0) {
            std::this_thread::sleep_for(timeout_);
            exit(0);
        }

        Status status;
        int exit_code;
        pid_t exited_pid = wait(&exit_code);
        if (exited_pid == checker_pid) {
            // If checked process finished first, kill timeout process
            kill(timeout_pid, SIGKILL);
            // If checker process finished normally, propagate its exit code
            if (WIFEXITED(exit_code)) {
                status = Status(WEXITSTATUS(exit_code));
            } else {
                // If checker processed finished abnormally, it indicates crash
                status = Status(Status::CRASH);
            }
        } else {
            // If timeout process finished first, kill checker process
            kill(checker_pid, SIGKILL);
            status = Status(Status::TIMEOUT);
        }
        // Wait for the other (killed) process to finish
        wait(nullptr);
        exit(status.code());
    }
    // Wait for the intermediary process to finish and propagate its exit code
    int exit_code;
    waitpid(pid, &exit_code, 0);
    if (WIFEXITED(exit_code)) {
        Status::Code code = WEXITSTATUS(exit_code);
        if (code == Status::ERROR) {
            // Rerun to get error message
            provider->Run(sql, &error_msg);
        }
        return Status(code, error_msg);
    }
    return Status(Status::CRASH);
}

}  // namespace tensile
