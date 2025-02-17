#include <gtest/gtest.h>
#include "StringUtils.h"

TEST(StringUtilsTest, SliceTest) {
    EXPECT_EQ(StringUtils::Slice("Hello", 0, 5), "Hello");
    EXPECT_EQ(StringUtils::Slice("Hello", 1, 4), "ell");
    EXPECT_EQ(StringUtils::Slice("Hello", -4, -1), "ell");
    EXPECT_EQ(StringUtils::Slice("Hello", 0, -1), "Hell");
    EXPECT_EQ(StringUtils::Slice("", 0, 0), "");
}

TEST(StringUtilsTest, CapitalizeTest) {
    EXPECT_EQ(StringUtils::Capitalize("hello"), "Hello");
    EXPECT_EQ(StringUtils::Capitalize("HELLO"), "HELLO");
    EXPECT_EQ(StringUtils::Capitalize(""), "");
}

TEST(StringUtilsTest, UpperLowerTest) {
    EXPECT_EQ(StringUtils::Upper("Hello"), "HELLO");
    EXPECT_EQ(StringUtils::Lower("Hello"), "hello");
    EXPECT_EQ(StringUtils::Upper(""), "");
    EXPECT_EQ(StringUtils::Lower(""), "");
}

TEST(StringUtilsTest, StripTest) {
    EXPECT_EQ(StringUtils::LStrip("  Hello"), "Hello");
    EXPECT_EQ(StringUtils::RStrip("Hello  "), "Hello");
    EXPECT_EQ(StringUtils::Strip("  Hello  "), "Hello");
    EXPECT_EQ(StringUtils::Strip(""), "");
}

TEST(StringUtilsTest, JustifyTest) {
    EXPECT_EQ(StringUtils::Center("Hello", 11, '-'), "---Hello---");
    EXPECT_EQ(StringUtils::LJust("Hello", 8, '-'), "Hello---");
    EXPECT_EQ(StringUtils::RJust("Hello", 8, '-'), "---Hello");
}

TEST(StringUtilsTest, ReplaceTest) {
    EXPECT_EQ(StringUtils::Replace("Hello World", "World", "Earth"), "Hello Earth");
    EXPECT_EQ(StringUtils::Replace("Hello Hello", "Hello", "Hi"), "Hi Hi");
    EXPECT_EQ(StringUtils::Replace("Hello", "", "Hi"), "Hello");
}

TEST(StringUtilsTest, SplitJoinTest) {
    std::vector<std::string> words = {"Hello", "World"};
    EXPECT_EQ(StringUtils::Split("Hello World"), words);
    EXPECT_EQ(StringUtils::Split("Hello,World", ","), words);
    EXPECT_EQ(StringUtils::Join(", ", words), "Hello, World");
}

TEST(StringUtilsTest, ExpandTabsTest) {
    EXPECT_EQ(StringUtils::ExpandTabs("\tHello"), "    Hello");
    EXPECT_EQ(StringUtils::ExpandTabs("Hi\tHello", 8), "Hi      Hello");
}

TEST(StringUtilsTest, EditDistanceTest) {
    EXPECT_EQ(StringUtils::EditDistance("Hello", "Hello"), 0);
    EXPECT_EQ(StringUtils::EditDistance("Hello", "Hellp"), 1);
    EXPECT_EQ(StringUtils::EditDistance("Hello", "HELLO", true), 0);
    EXPECT_EQ(StringUtils::EditDistance("", ""), 0);
}