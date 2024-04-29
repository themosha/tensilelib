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
    }
}

TEST(Features, NonEmpty) {
    EXPECT_FALSE(GetBuiltinFeatures().empty());
}

}  // namespace
}  // namespace tensile
