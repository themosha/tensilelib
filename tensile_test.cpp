#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include "tensile.h"

namespace tensile {
namespace {

TEST(Status, Update) {
    Status status;
    EXPECT_EQ(Status::SUCCESS, status.code());
    status.Update(Status(Status::ERROR));
    EXPECT_EQ(Status::ERROR, status.code());
    status.Update(Status(Status::TIMEOUT));
    status.Update(Status(Status::ERROR));
    EXPECT_EQ(Status::TIMEOUT, status.code());
    EXPECT_EQ(Status::TIMEOUT, status.code());
    status.Update(Status(Status::CRASH));
    status.Update(Status(Status::TIMEOUT));
    status.Update(Status(Status::ERROR));
    EXPECT_EQ(Status::CRASH, status.code());
    status.Update(Status(Status::SUCCESS));
    EXPECT_EQ(Status::CRASH, status.code());
}

TEST(Status, ToString) {
    EXPECT_EQ("Success", Status().ToString());
    EXPECT_EQ("Error", Status(Status::ERROR).ToString());

    EXPECT_THAT(Status(-1).ToString(), testing::HasSubstr("Unknown"));
    EXPECT_THAT(Status(1000).ToString(), testing::HasSubstr("Unknown"));
}

TEST(Status, ToChar) {
    EXPECT_EQ('.', Status().ToChar());
    EXPECT_EQ('T', Status(Status::TIMEOUT).ToChar());

    EXPECT_EQ('?', Status(-2).ToChar());
    EXPECT_EQ('?', Status(20000).ToChar());
}

class TestFeature : public ISQLFeature {
public:
    std::string name() override { return "Test"; }
    std::string GenerateSQL(size_t n) override {
        // Not really a SQL, just a string which is n characters long.
        return std::string(n, 'x');
    }
};

class TestProvider : public ISQLProvider {
public:
    TestProvider(size_t n) : n_(n) {}
    std::string name() const override { return "Test"; }

protected:
    // Switch from success to failure when SQL is that long.
    size_t n_;
};

class ErrorProvider : public TestProvider {
public:
    ErrorProvider(size_t n) : TestProvider(n) {}
    bool Run(const std::string& sql, std::string* error_msg) override {
        return sql.size() <= n_;
    }
};

class TimeoutProvider : public TestProvider {
public:
    TimeoutProvider(size_t n) : TestProvider(n) {}
    bool Run(const std::string& sql, std::string* error_msg) override {
        if (sql.size() <= n_) {
            return true;
        }
        // Go into infinite loop so driver can detect timeout
        for (;;) {
            sleep(10);
        }
        return true;
    }
};

class CrashProvider : public TestProvider {
public:
    CrashProvider(size_t n) : TestProvider(n) {}
    bool Run(const std::string& sql, std::string* error_msg) override {
        if (sql.size() <= n_) {
            return true;
        }
        // Crash
        abort();
    }
};

TEST(Driver, Error) {
    Driver d;
    TestFeature f;
    ErrorProvider e(100);
    Result r = d.Run(&e, &f);
    EXPECT_EQ(Status::ERROR, r.status.code());
    EXPECT_EQ(100, r.limit);
}

TEST(Driver, Timeout) {
    Driver d;
    TestFeature f;
    TimeoutProvider e(9);
    Result r = d.Run(&e, &f);
    EXPECT_EQ(Status::TIMEOUT, r.status.code());
    EXPECT_EQ(9, r.limit);
}

TEST(Driver, Crash) {
    Driver d;
    TestFeature f;
    CrashProvider e(42);
    Result r = d.Run(&e, &f);
    EXPECT_EQ(Status::CRASH, r.status.code());
    EXPECT_EQ(42, r.limit);
}

}  // namespace
}  // namespace tensile

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return ret;
}
