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
    // EXPECT_EQ(c->caches[s].valid, true);
    
    c->do_cache_op(0x3fffaa33, 1);
    c->dump_state();
    auto g = c->get_set(0x3fffaa33) + 1;
    // EXPECT_EQ(c->caches[g].valid, true);
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
    
    // EXPECT_EQ(c->caches[0].CacheMeta.__count, 0);
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
    
    // EXPECT_EQ(c->caches[0].CacheMeta.__count, 0);
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
    
    // EXPECT_EQ(c->caches[0].CacheMeta.__count, 0);
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
    EXPECT_EQ(std::get<0>(c->get_free_line(0x323, true, -1)), 0);
    c->dump_state(true);
    
    // EXPECT_EQ(c->caches[0].CacheMeta.__count, 0);
}

TEST(CacheTest, test_cache_8) {
    // https://www3.ntu.edu.sg/home/smitha/ParaCache/Paracache/dmc.html
    // cache size: 1024
    // memory size: 32768
    // offset bits: 5
    int assoc = 4;
    auto c = std::make_unique<Cache>("cache1", assoc, 32, 1024, Cache::ReplacementPolicy::PolicyLRU, Cache::WritePolicy::PolicyWriteback);

    c->do_cache_op(0x133b, 1);
    c->dump_state(true);
    EXPECT_EQ(c->caches[4].valid, true);
    EXPECT_EQ(c->caches[4].__tag, 0x13);

    c->do_cache_op(0x2b2a, 1);
    c->dump_state(true);
    EXPECT_EQ(c->caches[5].valid, true);
    EXPECT_EQ(c->caches[5].__tag, 0x2b);
    
    c->do_cache_op(0x3b2a, 1);
    c->dump_state(true);
    EXPECT_EQ(c->caches[6].valid, true);
    EXPECT_EQ(c->caches[6].__tag, 0x3b);
    
    c->do_cache_op(0x4b2a, 1);
    c->dump_state(true);
    EXPECT_EQ(c->caches[7].valid, true);
    EXPECT_EQ(c->caches[7].__tag, 0x4b);
    
    c->do_cache_op(0x5b2a, 1);
    c->dump_state(true);
    EXPECT_EQ(c->caches[4].valid, true);
    EXPECT_EQ(c->caches[4].__tag, 0x5b);
}

TEST(CacheTest, test_cache_9) {
    // https://www3.ntu.edu.sg/home/smitha/ParaCache/Paracache/dmc.html
    // cache size: 16
    // memory size: 32768
    // offset bits: 2
    // FIFO
    int assoc = 2;
    auto c = std::make_unique<Cache>("cache1", assoc, 4, 16, Cache::ReplacementPolicy::PolicyFIFO, Cache::WritePolicy::PolicyWriteback);

    c->do_cache_op(0x1a2a, 1);
    c->dump_state(true);
    EXPECT_EQ(c->caches[0].valid, true);
    EXPECT_EQ(c->caches[0].__tag, 0x345);

    c->do_cache_op(0x2a2, 1);
    c->dump_state(true);
    EXPECT_EQ(c->caches[1].valid, true);
    EXPECT_EQ(c->caches[1].__tag, 0x54);
    
    c->do_cache_op(0x1b1, 1);
    c->dump_state(true);
    EXPECT_EQ(c->caches[0].valid, true);
    EXPECT_EQ(c->caches[0].__tag, 0x36);
    
    c->do_cache_op(0x23a, 1);
    c->dump_state(true);
    EXPECT_EQ(c->caches[1].valid, true);
    EXPECT_EQ(c->caches[1].__tag, 0x47);
}

TEST(CacheTest, test_cache_10) {
    // https://www3.ntu.edu.sg/home/smitha/ParaCache/Paracache/dmc.html
    // cache size: 16
    // memory size: 2048
    // offset bits: 2
    // FIFO
    int assoc = 4;
    auto c = std::make_unique<Cache>("cache1", assoc, 4, 16, Cache::ReplacementPolicy::PolicyFIFO, Cache::WritePolicy::PolicyWriteback);

    c->do_cache_op(0x12, 1);
    c->dump_state(true);
    EXPECT_EQ(c->caches[0].valid, true);
    EXPECT_EQ(c->caches[0].__tag, 0x4);

    c->do_cache_op(0x23a, 1);
    c->dump_state(true);
    EXPECT_EQ(c->caches[1].valid, true);
    EXPECT_EQ(c->caches[1].__tag, 0x8e);
    
    c->do_cache_op(0x12, 1);
    c->dump_state(true);
    
    c->do_cache_op(0x24a, 1);
    c->dump_state(true);
    EXPECT_EQ(c->caches[2].valid, true);
    EXPECT_EQ(c->caches[2].__tag, 0x92);
    
    c->do_cache_op(0x2ff, 1);
    c->dump_state(true);
    EXPECT_EQ(c->caches[3].valid, true);
    EXPECT_EQ(c->caches[3].__tag, 0xbf);
    
    c->do_cache_op(0x12, 1);
    c->dump_state(true);
     
    c->do_cache_op(0x26a, 1);
    c->dump_state(true);
    EXPECT_EQ(c->caches[0].valid, true);
    EXPECT_EQ(c->caches[0].__tag, 0x9a);
}

TEST(CacheTest, test_cache_11) {
    // https://www3.ntu.edu.sg/home/smitha/ParaCache/Paracache/dmc.html
    // cache size: 16
    // memory size: 2048
    // offset bits: 2
    // FIFO
    int assoc = 4;
    auto c = std::make_unique<Cache>("cache1", assoc, 4, 16, Cache::ReplacementPolicy::PolicyLRU, Cache::WritePolicy::PolicyWriteback);

    c->do_cache_op(0x12, 1);
    c->dump_state(true);
    EXPECT_EQ(c->caches[0].valid, true);
    EXPECT_EQ(c->caches[0].__tag, 0x4);

    c->do_cache_op(0x23a, 1);
    c->dump_state(true);
    EXPECT_EQ(c->caches[1].valid, true);
    EXPECT_EQ(c->caches[1].__tag, 0x8e);
    
    c->do_cache_op(0x12, 1);
    c->dump_state(true);
    
    c->do_cache_op(0x24a, 1);
    c->dump_state(true);
    EXPECT_EQ(c->caches[2].valid, true);
    EXPECT_EQ(c->caches[2].__tag, 0x92);
    
    c->do_cache_op(0x2ff, 1);
    c->dump_state(true);
    EXPECT_EQ(c->caches[3].valid, true);
    EXPECT_EQ(c->caches[3].__tag, 0xbf);
    
    c->do_cache_op(0x12, 1);
    c->dump_state(true);
     
    c->do_cache_op(0x26a, 1);
    c->dump_state(true);
    EXPECT_EQ(c->caches[1].valid, true);
    EXPECT_EQ(c->caches[1].__tag, 0x9a);
}

TEST(CacheTest, test_cache_12) {
    // https://www3.ntu.edu.sg/home/smitha/ParaCache/Paracache/dmc.html
    // cache size: 16
    // memory size: 4096
    // offset bits: 2
    int assoc = 4;
    auto c = std::make_unique<Cache>("cache1", assoc, 4, 16, Cache::ReplacementPolicy::PolicyLRU, Cache::WritePolicy::PolicyWriteback);

    c->do_cache_op(0x12, 1);
    c->dump_state(true);
    EXPECT_EQ(c->caches[0].valid, true);
    EXPECT_EQ(c->caches[0].__tag, 0b0100);

    c->do_cache_op(0x12, 0);
    c->dump_state(true);
    EXPECT_EQ(c->caches[0].valid, true);
    EXPECT_EQ(c->caches[0].__tag, 0b0100);
    EXPECT_EQ(c->caches[0].dirty, true);
   
    c->do_cache_op(0x12, 1);
    c->dump_state(true);
    EXPECT_EQ(c->caches[0].valid, true);
    EXPECT_EQ(c->caches[0].__tag, 0b0100);
    EXPECT_EQ(c->caches[0].dirty, true); 

    c->do_cache_op(0x16, 1);
    c->dump_state(true);
    EXPECT_EQ(c->caches[1].valid, true);
    EXPECT_EQ(c->caches[1].__tag, 0b0101);
    EXPECT_EQ(c->caches[0].dirty, true); 
    
    c->do_cache_op(0x24, 1);
    c->dump_state(true);
    EXPECT_EQ(c->caches[2].valid, true);
    EXPECT_EQ(c->caches[2].__tag, 0b1001);
    EXPECT_EQ(c->caches[2].dirty, false); 
    
    c->do_cache_op(0x324, 1);
    c->dump_state(true);
    EXPECT_EQ(c->caches[3].valid, true);
    EXPECT_EQ(c->caches[3].dirty, false); 
    
    c->do_cache_op(0x200, 1);
    c->dump_state(true);
    EXPECT_EQ(c->caches[0].__tag, 0b010000000);
    EXPECT_EQ(c->caches[0].valid, true);
    EXPECT_EQ(c->caches[0].dirty, false); 
}
