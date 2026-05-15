#include <gtest/gtest.h>
#include "tensile.h"

namespace tensile {
namespace {

class GUnitComparer : public ITestComparer {
public:
    void ExpectEq(const std::string& expected, const std::string& actual) override {
        EXPECT_EQ(expected, actual);
    }
};

TEST(Features, All) {
    GUnitComparer cmp;
    auto features = std::move(GetBuiltinFeatures());
    for (auto& feature : features) {
        SCOPED_TRACE(feature->name());
        feature->SelfTest(&cmp);
        // Validate generated SQL at n=1 and n=2 for syntactic sanity
        std::string reason;
        EXPECT_TRUE(feature->ValidateSQL(1, &reason)) << feature->name() << " n=1: " << reason;
        EXPECT_TRUE(feature->ValidateSQL(2, &reason)) << feature->name() << " n=2: " << reason;
    }
}

TEST(Features, NonEmpty) {
    EXPECT_FALSE(GetBuiltinFeatures().empty());
}

TEST(SetOp, IntersectGenerateSQL) {
    auto features = GetBuiltinFeatures();
    for (auto& feature : features) {
        if (feature->name() == "intersect") {
            EXPECT_EQ(
                "select 1 x intersect select 1 x",
                feature->GenerateSQL(1));
            EXPECT_EQ(
                "select 1 x intersect select 1 x intersect select 1 x",
                feature->GenerateSQL(2));
            return;
        }
    }
    FAIL() << "intersect feature not found";
}

TEST(SetOp, IntersectAllGenerateSQL) {
    auto features = GetBuiltinFeatures();
    for (auto& feature : features) {
        if (feature->name() == "intersect all") {
            EXPECT_EQ(
                "select 1 x intersect all select 1 x",
                feature->GenerateSQL(1));
            EXPECT_EQ(
                "select 1 x intersect all select 1 x intersect all select 1 x",
                feature->GenerateSQL(2));
            return;
        }
    }
    FAIL() << "intersect all feature not found";
}

TEST(SetOp, ExceptAllGenerateSQL) {
    auto features = GetBuiltinFeatures();
    for (auto& feature : features) {
        if (feature->name() == "except all") {
            EXPECT_EQ(
                "select 1 x except all select 1 x",
                feature->GenerateSQL(1));
            EXPECT_EQ(
                "select 1 x except all select 1 x except all select 1 x",
                feature->GenerateSQL(2));
            return;
        }
    }
    FAIL() << "except all feature not found";
}

TEST(SetOp, CorrelatedSubqueryGenerateSQL) {
    auto features = GetBuiltinFeatures();
    for (auto& feature : features) {
        if (feature->name() == "correlated subquery") {
            EXPECT_EQ(
                "select * from (select 1 x) t0 where exists ("
                "select 1 from (select 1 x) t1 where t0.x = t1.x)",
                feature->GenerateSQL(1));
            EXPECT_EQ(
                "select * from (select 1 x) t0 where exists ("
                "select 1 from (select 1 x) t1 where t0.x = t1.x and "
                "exists (select 1 from (select 1 x) t2 where t1.x = t2.x))",
                feature->GenerateSQL(2));
            return;
        }
    }
    FAIL() << "correlated subquery feature not found";
}

TEST(WindowTest, NamedWindowGenerateSQL) {
    auto features = GetBuiltinFeatures();
    for (auto& feature : features) {
        if (feature->name() == "named window") {
            // Validate SQL at multiple depths — would have caught w0 as ()
            EXPECT_EQ(
                "select row_number() over w1 window w0 as (order by 1) ,w1 as (order by 1)",
                feature->GenerateSQL(1));
            EXPECT_EQ(
                "select row_number() over w2 window w0 as (order by 1) ,w1 as (order by 1) ,w2 as (order by 1)",
                feature->GenerateSQL(2));
            EXPECT_EQ(
                "select row_number() over w3 window w0 as (order by 1) ,w1 as (order by 1) ,w2 as (order by 1) ,w3 as (order by 1)",
                feature->GenerateSQL(3));
            // ValidateSQL catches syntactic errors
            std::string reason;
            EXPECT_TRUE(feature->ValidateSQL(1, &reason)) << reason;
            EXPECT_TRUE(feature->ValidateSQL(10, &reason)) << reason;
            return;
        }
    }
    FAIL() << "named window feature not found";
}

TEST(SetOp, IsDistinctFromGenerateSQL) {
    auto features = GetBuiltinFeatures();
    for (auto& feature : features) {
        if (feature->name() == "is distinct from") {
            EXPECT_EQ("select true is distinct from false", feature->GenerateSQL(1));
            EXPECT_EQ(
                "select (true is distinct from true) is distinct from false",
                feature->GenerateSQL(2));
            return;
        }
    }
    FAIL() << "is distinct from feature not found";
}

}  // namespace
}  // namespace tensile
