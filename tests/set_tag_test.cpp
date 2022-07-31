#include <gtest/gtest.h>
#include "Cache.hpp"

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

TEST(set_bits, set_bits_1) {
    auto c = new Cache("cache2", 2, 16, 32768, Cache::ReplacementPolicy::PolicyLRU, Cache::WritePolicy::PolicyWriteback);
    EXPECT_EQ(c->set_bits, 10);
    EXPECT_EQ(c->num_blocks, (32768 / 16));
}
