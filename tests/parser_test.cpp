#include <gtest/gtest.h>
#include "Cache.hpp"
#include "Parser.hpp"

TEST(ParserTest, test_parse_1) {
    auto c = std::make_unique<Cache>("cache1", 1, 16, 1024, Cache::ReplacementPolicy::PolicyLRU, Cache::WritePolicy::PolicyWriteback);

    auto v = Parser();
    std::ifstream f("examples/hwparse.txt");
    if (!f)
    {
        std::cout << "No such file!" << std::endl;
    }
    auto _hist = v.ParseWithHistory(&f, c, "hw");
    
    EXPECT_EQ(_hist->size(), 7);
    EXPECT_EQ(_hist->data()[0].first, 0x7fffed80);
    EXPECT_EQ(_hist->data()[0].second, 0);
    EXPECT_EQ(_hist->data()[1].first, 0x10010000);
    EXPECT_EQ(_hist->data()[1].second, 0);
    EXPECT_EQ(_hist->data()[2].first, 0x10010060);
    EXPECT_EQ(_hist->data()[2].second, 0);
}

TEST(ParserTest, test_parse_2) {
    auto c = std::make_unique<Cache>("cache1", 4, 32, 32768, Cache::ReplacementPolicy::PolicyLRU, Cache::WritePolicy::PolicyWriteback);

    auto v = Parser();
    std::ifstream f("examples/hwparse2.txt");
    if (!f)
    {
        std::cout << "No such file!" << std::endl;
    }
    auto _hist = v.ParseWithHistory(&f, c, "hw");
    
    EXPECT_EQ(_hist->size(), 9);
    EXPECT_EQ(_hist->data()[3].first, 0x1001eeee);
    EXPECT_EQ(_hist->data()[3].second, 1);
    EXPECT_EQ(_hist->data()[1].first, 0x100103aa);
    EXPECT_EQ(_hist->data()[1].second, 1);
    EXPECT_EQ(_hist->data()[4].first, 0x1001ffff);
    EXPECT_EQ(_hist->data()[4].second, 0);
}

TEST(ParserTest, test_parse_3) {
    auto c = std::make_unique<Cache>("cache1", 4, 32, 32768, Cache::ReplacementPolicy::PolicyLRU, Cache::WritePolicy::PolicyWriteback);

    auto v = Parser();
    std::ifstream f("examples/hwparse4.txt");
    if (!f)
    {
        std::cout << "No such file!" << std::endl;
    }
    auto _hist = v.ParseWithHistory(&f, c, "simple");
    
    EXPECT_EQ(_hist->size(), 11);
    EXPECT_EQ(_hist->data()[3].first, 0x1fffff90);
    EXPECT_EQ(_hist->data()[3].second, 1);
    EXPECT_EQ(_hist->data()[1].second, 1);
    EXPECT_EQ(_hist->data()[6].second, 0);
}
