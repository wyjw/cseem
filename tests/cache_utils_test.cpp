#include <gtest/gtest.h>
#include "Cache.hpp"
#include "Parser.hpp"

TEST(CacheUtilsTest, test_cache_utils_1) {
    auto c = std::make_unique<Cache>("cache1", 1, 16, 1024, Cache::ReplacementPolicy::PolicyLRU, Cache::WritePolicy::PolicyWriteback);

    c->do_cache_op(0x10000001, 1);
    c->dump_state();
    EXPECT_EQ(c->__stats.cache_miss_count, 1);
    EXPECT_EQ(c->__stats.cache_read_count, 1);
    EXPECT_EQ(c->__stats.cache_write_count, 0);
    
    c->do_cache_op(0x3fffaa33, 1);
    c->dump_state();
    EXPECT_EQ(c->__stats.cache_miss_count, 2);
    EXPECT_EQ(c->__stats.cache_read_count, 2);
    EXPECT_EQ(c->__stats.cache_write_count, 0);
}
