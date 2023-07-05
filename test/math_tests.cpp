#include <gtest/gtest.h>
#include "math.h"

TEST(QuadraticTests, quadratic_negative_disc) {
    auto roots = SolveQuadratic(1,-4,5);
    EXPECT_FALSE(roots);
}

TEST(QuadraticTests, quadratic_zero_disc) {
    auto roots = SolveQuadratic(1,4,4);
    EXPECT_TRUE(roots);
    auto[t1, t2] = roots.value();
    EXPECT_EQ(t1,-2);
    EXPECT_EQ(t1, t2);
}