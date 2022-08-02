#include "Cache.hpp"

Cache::Cache (const std::string &name, 
        unsigned associativity, 
        // unsigned num_ways, 
        unsigned block_size,
        unsigned capacity, 
        ReplacementPolicy rp, 
        WritePolicy wp)
{
    this->block_size = block_size;
    this->associativity = associativity;
    this->capacity = capacity;
    this->rp = PolicyLRU;
    this->wp = PolicyWriteback; 
    
    this->set_offset = std::popcount(block_size - 1);
    this->sets = capacity / (block_size * associativity);
    this->set_mask = sets - 1;
    this->set_bits = std::popcount(set_mask);
    this->num_blocks = capacity / block_size;

    this->caches = std::vector<CacheLine>();
    this->caches.resize(num_blocks);
    this->tag_offset = set_offset + set_bits;
}

Cache::~Cache ()
{
}

int Cache::check_cache_hit(unsigned addr, int* invalid_index, int type) {
    auto set = get_set(addr);
    auto tag = get_tag(addr);
    auto base = set * associativity;
    std::span local{this->caches.data() + base, associativity}; //TODO: why?

    *invalid_index = -1;

    int index;
    bool hit;

    for (auto i = 0;  i < local.size(); i++) {
        if (!local[i].valid) {
            *invalid_index = i;
            continue;
        }
        auto ltag = local[i].__tag;
        if (tag != ltag) {
            // not valid tag
            continue;
        }

        hit = true;
        index = i;

        // update dirty flag, if needed
        local[i].dirty |= type;

        break; 
    }
    return hit;
}

unsigned Cache::get_set(unsigned address) {
    auto shifted_address = address >> this->set_offset;
    return shifted_address & set_mask;
}

unsigned Cache::get_tag(unsigned address) {
    return address >> this->tag_offset;
}

int Cache::get_free_line(unsigned addr, int invalid_index) {
    auto set = get_set(addr);
    auto tag = get_tag(addr);
    auto base = set * associativity;
    std::span local{this->caches.data() + base, associativity}; //TODO: why?
    
    auto dirty_wb = false;
   // try to get an invalid index
    int index;
    if (invalid_index >= 0) {
        index = invalid_index;
        local[index].valid = true;

        // check that local points to the same thing
        assert(&local[index] == &this->caches.data()[base + index]);
    }
    else {
        auto max_element = std::max_element(this->caches.begin(), this->caches.end(), 
                [](const class CacheLine &a, const class CacheLine &b) 
                {
                    return a.CacheMeta.__count < b.CacheMeta.__count;
                });
        index = std::distance(this->caches.begin(), max_element);
        // dirty_wb = local_dirty[index];
    }
    
    local[index].__tag = tag;
    // this->caches[index].dirty |= type; 

    this->do_updates(addr, index);
    
    return index;
}

int Cache::do_updates(unsigned addr, int index) {
    std::transform(std::begin(this->caches), std::end(this->caches),
            std::begin(this->caches), 
            [&](CacheLine a) {
                if (a.CacheMeta.__count < this->caches[index].CacheMeta.__count
                        && a.CacheMeta.__count < associativity)
                    a.CacheMeta.__count += 1;
                return a;
            });

    // current block has highest
    this->caches[index].CacheMeta.__count = 0;
    this->caches[index].CacheMeta.__lru_count = 0;
    this->caches[index].CacheMeta.__fifo_count = 0;

    return 0; 
}

int Cache::do_cache_op(unsigned addr, int is_read)
{  
    int invalid_index;
    auto index = this->check_cache_hit(addr, &invalid_index);
    if (index) {
        this->__stats.cache_hit_count++;
    }
    else {
        auto index = this->get_free_line(addr, invalid_index);
        // this->set_cache_item(item, addr);
        if (is_read) {
            this->__stats.cache_read_count++;
        }
        else {
            this->__stats.cache_write_count++;
        }
        this->__stats.cache_miss_count++;
    }
    return 0;
}

void Cache::dump_stats() {
    std::cout << "CACHE SETTINGS:" << std::endl;
    std::cout << "HITS:" << this->__stats.cache_hit_count << std::endl;
    std::cout << "MISS:" << this->__stats.cache_miss_count << std::endl;
    std::cout << "READS:" << this->__stats.cache_read_count << std::endl;
    std::cout << "WRITES:" << this->__stats.cache_write_count << std::endl;
    std::cout << "HITRATE:" << this->__stats.hit_rate() << std::endl;
}

// pretty print the cache state
void Cache::dump_state(bool meta) {
    // std::cout << "" << std::endl;
    std::vector<std::string> columns;
    std::vector<int> column_widths;
    std::string separator = "|";
    int table_width;

    columns.push_back("Index");
    column_widths.push_back(8);
    table_width += 8 + separator.size();

    columns.push_back("Valid");
    column_widths.push_back(8);
    table_width += 8 + separator.size();

    columns.push_back("Tag");
    column_widths.push_back(12);
    table_width += 12 + separator.size();

    columns.push_back("Data");
    column_widths.push_back(12);
    table_width += 12 + separator.size();

    columns.push_back("DirtyBit");
    column_widths.push_back(8);
    table_width += 8 + separator.size();
   
    if (meta) {
        columns.push_back("Count");
        column_widths.push_back(8);
        table_width += 8 + separator.size();

        columns.push_back("LRUCount");
        column_widths.push_back(12);
        table_width += 12 + separator.size();

        columns.push_back("FIFOCount");
        column_widths.push_back(12);
        table_width += 12 + separator.size();
    }

    // first line -- header of table
    for (auto j = 0; j < columns.size(); j++) {
        std::cout << std::setw(column_widths[j]) << columns[j];
        std::cout << separator;
    }
    std::cout << std::endl;
    
    int len = 0;
    for (auto c : caches) {
     
        for (auto i = 0; i < columns.size(); i++) {
            // columns[i] 
            // column_widths[i]

            if (columns[i] == "Index") {
                std::cout << std::setw(column_widths[i]) << len;
            }
            else if (columns[i] == "Valid") {
                std::cout << std::setw(column_widths[i]) << c.valid;
            } else if (columns[i] == "Tag") {
                std::cout << std::setw(column_widths[i]) << c.__tag;
            } else if (columns[i] == "Data") {
                std::cout << std::setw(column_widths[i]) << c.buf;
            } else if (columns[i] == "DirtyBit") {
                std::cout << std::setw(column_widths[i]) << c.dirty;
            } else if (columns[i] == "Count") {
                std::cout << std::setw(column_widths[i]) << c.CacheMeta.__count;
            } else if (columns[i] == "LRUCount") {
                std::cout << std::setw(column_widths[i]) << c.CacheMeta.__lru_count;
            } else if (columns[i] == "FIFOCount") {
                std::cout << std::setw(column_widths[i]) << c.CacheMeta.__fifo_count;
            }
            std::cout << separator;
        }
        std::cout << std::endl;
        len++;
    } 
}
