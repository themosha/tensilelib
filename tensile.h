#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <vector>

namespace tensile {

// Test helper abstract class to facilitate comparing expected and actual results.
// Possible implementation to use EXPECT_EQ macro.
class ITestComparer {
public:
    virtual ~ITestComparer() {}
    virtual void ExpectEq(const std::string &expected, const std::string &actual) = 0;
};

// Abstract class representing feature in SQL that we want to find limits of.
// For example it can be length of identifier or number of nested subselects.
class ISQLFeature {
public:
    virtual ~ISQLFeature() {}

    // Human readable name of the feature
    virtual std::string name() = 0;

    // Generates valid SQL query which has feature with given cardinality @n
    virtual std::string GenerateSQL(size_t n) = 0;

    // Test only method for derived class instance to be able test that it generates desired SQL.
    // This will usually call GenerateSQL for low values of @n = 1,2,... and allow reader to inspect
    // generated SQL structure.
    virtual void SelfTest(ITestComparer *cmp) {
        // Do nothing
    }

    virtual bool is_exponential() const { return false; }

protected:
    // Since we expect to call GenerateSQL in the loop multiple times, it is useful to keep
    // strubg buffer between calls to avoid extra allocations.
    // Derived classes don't have to use it.
    std::string sql_;
};

// Get list of all SQL features built into Tensile. The list can be extended.
std::vector<std::unique_ptr<ISQLFeature>> GetBuiltinFeatures();

// Abstract class representing SQL backend which knows how to process given SQL query.
// It can be just a parser, or full-blown executor, or anything in between.
class ISQLProvider {
public:
    virtual void Init() {}

    virtual ~ISQLProvider() {}

    // Human readable name
    virtual std::string name() const = 0;

    // Take given SQL query, and run it (can be parse, analyze, execute etc)
    virtual bool Run(const std::string &sql, std::string *error_msg) = 0;
};

// Register custom SQL provider to be automatically checked by the driver
void RegisterSQLProvider(std::unique_ptr<ISQLProvider> provider);

// Status of checking SQL against provider.
class Status {
public:
    // Status codes ordered by increasing severity
    typedef int Code;
    static const Code SUCCESS = 0;
    static const Code ERROR = 1;
    static const Code TIMEOUT = 2;
    static const Code CRASH = 3;

    Status() {}

    explicit Status(Code code) : code_(code) {}

    Status(Code code, std::string message) : code_(code), message_(std::move(message)) {}

    Code code() const { return code_; }

    const std::string &message() const { return message_; }

    // Update status to most severe
    void Update(Status status) {
        // Codes are ordered by increasing severity, hence Update takes the highest code.
        code_ = std::max(status.code(), code_);
        if (!status.message_.empty()) {
            message_ = status.message_;
        }
    }

    std::string ToString() const;

    char ToChar() const;

private:
    int code_ = SUCCESS;
    std::string message_;
};

// Result of checking SQL
struct Result {
    // Name of SQL provider checked
    std::string provider;
    // Name of SQL feature checked
    std::string feature;
    // Maximum value for which feature was successful
    size_t limit;
    // Reason for failure for values above @limit
    Status status;
};

class Driver {
public:
    Driver() {}

    Driver(int argc, char **argv);

    // Register custom SQL provider to be checked by the driver
    void AddProvider(std::unique_ptr<ISQLProvider> provider) {
        providers_.emplace_back(std::move(provider));
    }

    std::vector<Result> Run();

    Result Run(ISQLProvider *provider, ISQLFeature *feature);

    // Accessors
    // =========
    // Whether to run tests in process or out of process
    void set_check_crash(bool value) { check_crash_ = value; }

    // How long to wait for provider to process single SQL statement before timing out
    void set_timeout(std::chrono::milliseconds value) { timeout_ = value; }

    // Which providers should be tested - empty string means all
    void set_provider_names(std::string value) { provider_names_to_check_ = std::move(value); }

    // Which features to test
    void set_feature_names(std::string value) { feature_names_to_check_ = std::move(value); }

    void set_perftrace(bool value) { perftrace_ = value; }

    bool perftrace() const { return perftrace_; }

private:
    std::vector<std::unique_ptr<ISQLProvider>> providers_;
    bool check_crash_ = true;
    std::chrono::milliseconds timeout_ = std::chrono::milliseconds(100);
    std::string provider_names_to_check_;
    std::string feature_names_to_check_;
    bool perftrace_ = false;

    // Checks if given feature succeeds or fails for the given provider.
    // This function can also detect crashes and execution longer than given timeout.
    Status CheckFeature(size_t n, ISQLFeature *feature, ISQLProvider *provider);
};

}  // namespace tensile

