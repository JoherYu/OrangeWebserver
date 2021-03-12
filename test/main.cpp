#include "gtest/gtest.h"

#include "utils.h"

TEST(split_in_2_test, all)
{    char a[] = "/login/login.html";
    EXPECT_STREQ(split_in_2(a, "/")->front().data(), "login");
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}