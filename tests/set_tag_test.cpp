#include <gtest/gtest.h>
#include "Cache.hpp"
#include <cmath>

TEST(tag_field, check_addr) {
    // p405 risv h&p
    auto c = new Cache("cache1", 1, 16, 1024, Cache::ReplacementPolicy::PolicyLRU, Cache::WritePolicy::PolicyWriteback);
    // log2(tag) | log2(set) | log2(blocks)
    // EXPECT_EQ(c->get_set(1200), 1);
    EXPECT_EQ(c->num_blocks, 64);
}

TEST(tag_field, get_set_0) {
    // https://connick.me/cache_calculator/
    // cache_associativity: 1 (direct-mapped)
    // cache_line_size: 256
    // address_size: 64
    // cache_data_size: 32
    auto c = new Cache("cache1", 1, 256, 32768, Cache::ReplacementPolicy::PolicyLRU, Cache::WritePolicy::PolicyWriteback);
    EXPECT_EQ(c->get_set(1313), 5);
    EXPECT_EQ(c->get_set(133412136), 53);
    EXPECT_EQ(c->get_set(87998), 87);
}

TEST(tag_field, get_set_1) {
    // https://connick.me/cache_calculator/
    // cache_associativity: 2
    // cache_line_size: 32
    // address_size: 32
    // cache_data_size: 1
    auto c = new Cache("cache1", 2, 32, 1024, Cache::ReplacementPolicy::PolicyLRU, Cache::WritePolicy::PolicyWriteback);
    EXPECT_EQ(c->get_set(2323), 8);
    EXPECT_EQ(c->get_set(2), 0);
    EXPECT_EQ(c->get_set(5512), 12);
}

TEST(tag_field, get_set_2) {
    // https://connick.me/cache_calculator/
    // cache_associativity: 4
    // cache_line_size: 128
    // address_size: 64
    // cache_data_size: 4
    auto c = new Cache("cache1", 4, 128, 4096, Cache::ReplacementPolicy::PolicyLRU, Cache::WritePolicy::PolicyWriteback);
    EXPECT_EQ(c->get_set(2123), 0);
    EXPECT_EQ(c->get_set(21355), 6);
    EXPECT_EQ(c->get_set(5588991), 7);
}

TEST(tag_field, get_set_3) {
    // https://connick.me/cache_calculator/
    // cache_associativity: 4
    // cache_line_size: 64
    // address_size: 64
    // cache_data_size: 32
    auto c = new Cache("cache1", 4, 64, 32768, Cache::ReplacementPolicy::PolicyLRU, Cache::WritePolicy::PolicyWriteback);
    EXPECT_EQ(c->get_set(392240), 112);
    EXPECT_EQ(c->get_set(66996), 22);
    EXPECT_EQ(c->get_set(808082), 82);
    EXPECT_EQ(c->get_set(647223), 0);
}

TEST(tag_field, get_set_4) {
    // https://www3.ntu.edu.sg/home/smitha/ParaCache/Paracache/dmc.html
    // cache size: 1024
    // memory size: 32768
    // offset bits: 5
    auto c = new Cache("cache1", 1, 32, 1024, Cache::ReplacementPolicy::PolicyLRU, Cache::WritePolicy::PolicyWriteback);
    // std::cout << c->get_tag(0x5167) << "tag" << std::endl;
    // EXPECT_EQ(c->get_tag(0x5167));
    EXPECT_EQ(c->get_set(0x4223), 0b10001);
    EXPECT_EQ(c->get_set(0x4223), 17);
    EXPECT_EQ(c->get_set(0x1bb0), 0b11101);
    EXPECT_EQ(c->get_set(0x1bb0), 29);
}

TEST(tag_field, get_tag_0) {
    // https://www3.ntu.edu.sg/home/smitha/ParaCache/Paracache/dmc.html
    // cache size: 1024
    // memory size: 32768
    // offset bits: 5
    auto c = new Cache("cache1", 1, 32, 1024, Cache::ReplacementPolicy::PolicyLRU, Cache::WritePolicy::PolicyWriteback);
    // std::cout << c->get_tag(0x5167) << "tag" << std::endl;
    // EXPECT_EQ(c->get_tag(0x5167));
    EXPECT_EQ(c->get_tag(0x4152), 0b10000);
    EXPECT_EQ(c->get_tag(0x34fa), 0b01101);
    EXPECT_EQ(c->get_tag(0x1bac), 0b00110);
}

TEST(tag_field, get_tag_1) {
    // https://www3.ntu.edu.sg/home/smitha/ParaCache/Paracache/dmc.html
    // cache size: 256
    // memory size: 32768
    // offset bits: 4
    auto c = new Cache("cache1", 1, 16, 256, Cache::ReplacementPolicy::PolicyLRU, Cache::WritePolicy::PolicyWriteback);
    // std::cout << c->get_tag(0x5167) << "tag" << std::endl;
    // EXPECT_EQ(c->get_tag(0x5167));
    EXPECT_EQ(c->get_tag(0x4152), 0b1000001);
    EXPECT_EQ(c->get_tag(0x34fa), 0b0110100);
    EXPECT_EQ(c->get_tag(0x1bac), 0b0011011);
    EXPECT_EQ(c->get_tag(0x4aac), 0b1001010);
    EXPECT_EQ(c->get_tag(0x44f1), 0b1000100);
}

TEST(set_bits, set_bits_1) {
    auto c = new Cache("cache2", 2, 16, 32768, Cache::ReplacementPolicy::PolicyLRU, Cache::WritePolicy::PolicyWriteback);
    EXPECT_EQ(c->set_bits, 10);
    EXPECT_EQ(c->num_blocks, (32768 / 16));
}

int _log2(int index) {
    int targetlevel = 0;
    while (index >>= 1) ++targetlevel;
    return targetlevel;
}

TEST(popcount, pop_count_0) {
    for (unsigned i = 2; i <= pow(2, 14); i *= 2) {
        EXPECT_EQ(std::bit_width(i) - 1, std::popcount(i - 1));
        EXPECT_EQ(std::bit_width(i) - 1, _log2(i));
    }
}
