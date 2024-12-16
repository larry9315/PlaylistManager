#include <gtest/gtest.h>

// Example test case
TEST(ExampleTest, BasicAssertions) {
    EXPECT_EQ(1 + 1, 2);  // Check basic addition
    EXPECT_TRUE(true);    // Check a boolean condition
}

// Entry point for GoogleTest
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}