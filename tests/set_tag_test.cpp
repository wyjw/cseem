#include <gtest/gtest.h>
#include "Cache.hpp"

TEST(tag_field, check_addr) {
    // p405 risv h&p
    auto c = new Cache("cache1", 1, 16, 1024, PolicyLRU, PolicyWriteback);
    // log2(tag) | log2(set) | log2(blocks)
    EXPECT_EQ(7 * 6, 42);
}
