#include "gtest/gtest.h"

#include "utils.h"

TEST(split_in_2_test, first)
{
    char generic_1[] = "/login/login.html";
    char generic_2[] = "login/login.html";
    char generic_3[] = "/login";
    char generic_4[] = "login";
    char border_1[] = " ";
    char border_2[] = "";

    char delim_1[] = "/";
    char delim_2[] = "";
    EXPECT_STREQ(split_in_2(generic_1, delim_1)->front().data(), "login");
    EXPECT_STREQ(split_in_2(generic_2, delim_1)->front().data(), "login");
    EXPECT_STREQ(split_in_2(generic_3, delim_1)->front().data(), "login");
    EXPECT_STREQ(split_in_2(generic_4, delim_1)->front().data(), "login");
    EXPECT_STREQ(split_in_2(border_1, delim_1)->front().data(), " ");
    EXPECT_STREQ(split_in_2(border_2, delim_1)->front().data(), " ");
    EXPECT_STREQ(split_in_2(generic_2, delim_2)->front().data(), " ");
}

TEST(split_in_2_test, second)
{
    char generic_1[] = "/login/login.html";
    char generic_2[] = "login/login.html";
    char generic_3[] = "/login";
    char generic_4[] = "login";

    char delim_1[] = "/";
    EXPECT_STREQ(split_in_2(generic_1, delim_1)->back().data(), "login.html");
    EXPECT_STREQ(split_in_2(generic_2, delim_1)->back().data(), "login.html");
    EXPECT_STREQ(split_in_2(generic_3, delim_1)->back().data(), "");
    EXPECT_STREQ(split_in_2(generic_4, delim_1)->back().data(), "");
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}