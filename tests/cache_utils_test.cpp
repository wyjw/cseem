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

TEST(CacheTest, test_cache_1) {
    int assoc = 1;
    auto c = std::make_unique<Cache>("cache1", assoc, 16, 1024, Cache::ReplacementPolicy::PolicyLRU, Cache::WritePolicy::PolicyWriteback);

    c->do_cache_op(0x10000001, 1);
    auto s = c->get_set(0x10000001) * assoc;
    EXPECT_EQ(c->caches[s].valid, true);
    
    c->do_cache_op(0x3fffaa33, 1);
    auto g = c->get_set(0x3fffaa33) * assoc;
    EXPECT_EQ(c->caches[g].valid, true);
}

TEST(CacheTest, test_cache_2) {
    int assoc = 2;
    auto c = std::make_unique<Cache>("cache1", assoc, 1, 1024, Cache::ReplacementPolicy::PolicyLRU, Cache::WritePolicy::PolicyWriteback);

    c->do_cache_op(0x10000001, 1);
    c->dump_state();
    auto s = c->get_set(0x10000001) + 1;
    EXPECT_EQ(c->caches[s].valid, true);
    
    c->do_cache_op(0x3fffaa33, 1);
    c->dump_state();
    auto g = c->get_set(0x3fffaa33) + 1;
    EXPECT_EQ(c->caches[g].valid, true);
}

TEST(CacheTest, test_cache_3) {
    // https://www3.ntu.edu.sg/home/smitha/ParaCache/Paracache/dmc.html
    // cache size: 1024
    // memory size: 32768
    // offset bits: 5
    int assoc = 1;
    auto c = std::make_unique<Cache>("cache1", assoc, 32, 1024, Cache::ReplacementPolicy::PolicyLRU, Cache::WritePolicy::PolicyWriteback);

    c->do_cache_op(0x3ff3, 1);
    c->dump_state();
    EXPECT_EQ(c->caches[31].valid, true);
    EXPECT_EQ(c->caches[31].__tag, 0b01111);

    c->do_cache_op(0x2b2a, 1);
    c->dump_state();
    EXPECT_EQ(c->caches[25].valid, true);
    EXPECT_EQ(c->caches[25].__tag, 0b01010);
    for (int i = 0; i < 24; i++) {
        EXPECT_EQ(c->caches[i].valid, false);
    }
}

TEST(CacheTest, test_cache_4) {
    // same read
    // https://www3.ntu.edu.sg/home/smitha/ParaCache/Paracache/dmc.html
    // cache size: 1024
    // memory size: 32768
    // offset bits: 5
    int assoc = 1;
    auto c = std::make_unique<Cache>("cache1", assoc, 32, 1024, Cache::ReplacementPolicy::PolicyLRU, Cache::WritePolicy::PolicyWriteback);

    c->do_cache_op(0x3ff3, 1);
    c->dump_state();
    EXPECT_EQ(c->caches[31].valid, true);
    EXPECT_EQ(c->caches[31].__tag, 0b01111);
    EXPECT_EQ(c->__stats.cache_miss_count, 1);
    EXPECT_EQ(c->__stats.cache_hit_count, 0);

    c->do_cache_op(0x3ff3, 0);
    c->dump_state();
    for (int i = 0; i < 30; i++) {
        EXPECT_EQ(c->caches[i].valid, false);
    }
    EXPECT_EQ(c->__stats.cache_miss_count, 1);
    EXPECT_EQ(c->__stats.cache_hit_count, 1);
}

TEST(CacheTest, test_cache_5) {
    // same read
    // https://www3.ntu.edu.sg/home/smitha/ParaCache/Paracache/dmc.html
    // cache size: 1024
    // memory size: 32768
    // offset bits: 5
    int assoc = 1;
    auto c = std::make_unique<Cache>("cache1", assoc, 32, 1024, Cache::ReplacementPolicy::PolicyLRU, Cache::WritePolicy::PolicyWriteback);

    c->do_cache_op(0x3ff3, 1);
    c->dump_state();
    EXPECT_EQ(c->caches[31].valid, true);
    EXPECT_EQ(c->caches[31].__tag, 0b01111);
    EXPECT_EQ(c->__stats.cache_miss_count, 1);
    EXPECT_EQ(c->__stats.cache_hit_count, 0);

    for (int y = 0; y <= 5; y++) {
        c->do_cache_op(0x3ff3, 0);
        c->dump_state();
        for (int i = 0; i < 30; i++) {
            EXPECT_EQ(c->caches[i].valid, false);
        }
        EXPECT_EQ(c->__stats.cache_miss_count, 1);
        EXPECT_EQ(c->__stats.cache_hit_count, y+1);
    }
}

TEST(CacheTest, test_cache_6) {
    // same read
    // https://www3.ntu.edu.sg/home/smitha/ParaCache/Paracache/dmc.html
    // cache size: 8
    // memory size: 2048
    // offset bits: 2
    int assoc = 1;
    auto c = std::make_unique<Cache>("cache1", assoc, 4, 8, Cache::ReplacementPolicy::PolicyLRU, Cache::WritePolicy::PolicyWriteback);

    c->do_cache_op(0x23, 1);
    c->dump_state();
    EXPECT_EQ(c->caches[0].valid, true);
    EXPECT_EQ(c->caches[0].__tag, 0b00000100);
    c->do_cache_op(0x233, 1);
    EXPECT_EQ(c->caches[0].valid, true);
    EXPECT_EQ(c->caches[0].__tag, 0b01000110);
    
    EXPECT_EQ(c->caches[0].CacheMeta.__count, 0);
}

TEST(CacheTest, test_prior_1) {
    // same read
    // https://www3.ntu.edu.sg/home/smitha/ParaCache/Paracache/dmc.html
    // cache size: 8
    // memory size: 2048
    // offset bits: 2
    int assoc = 1;
    auto c = std::make_unique<Cache>("cache1", assoc, 4, 8, Cache::ReplacementPolicy::PolicyLRU, Cache::WritePolicy::PolicyWriteback);

    c->do_cache_op(0x23, 1);
    c->dump_state();
    EXPECT_EQ(c->caches[0].valid, true);
    EXPECT_EQ(c->caches[0].__tag, 0b00000100);
    c->do_cache_op(0x233, 1);
    EXPECT_EQ(c->caches[0].valid, true);
    EXPECT_EQ(c->caches[0].__tag, 0b01000110);
    
    EXPECT_EQ(c->caches[0].CacheMeta.__count, 0);
}


TEST(PriorTest, test_prior_2) {
    // same read
    // https://www3.ntu.edu.sg/home/smitha/ParaCache/Paracache/dmc.html
    // cache size: 8
    // memory size: 2048
    // offset bits: 2
    int assoc = 1;
    auto c = std::make_unique<Cache>("cache1", assoc, 4, 8, Cache::ReplacementPolicy::PolicyLRU, Cache::WritePolicy::PolicyWriteback);

    c->do_cache_op(0x23, 1);
    c->dump_state(true);
    EXPECT_EQ(c->caches[0].valid, true);
    EXPECT_EQ(c->caches[0].__tag, 0b00000100);
    c->do_cache_op(0x233, 1);
    EXPECT_EQ(c->caches[0].valid, true);
    EXPECT_EQ(c->caches[0].__tag, 0b01000110);
    c->do_cache_op(0x233, 1);
    c->do_cache_op(0x233, 1);
    c->dump_state(true);
    
    EXPECT_EQ(c->caches[0].CacheMeta.__count, 0);
}

TEST(PriorTest, test_prior_3) {
    // same read
    // https://www3.ntu.edu.sg/home/smitha/ParaCache/Paracache/dmc.html
    // cache size: 8
    // memory size: 2048
    // offset bits: 2
    int assoc = 1;
    auto c = std::make_unique<Cache>("cache1", assoc, 4, 8, Cache::ReplacementPolicy::PolicyLRU, Cache::WritePolicy::PolicyWriteback);

    c->do_cache_op(0x23, 1);
    c->do_cache_op(0x233, 1);
    c->do_cache_op(0x136, 1);
    c->do_cache_op(0x133, 1);
    EXPECT_EQ(c->get_free_line(0x323, -1), 0);
    c->dump_state(true);
    
    EXPECT_EQ(c->caches[0].CacheMeta.__count, 0);
}
