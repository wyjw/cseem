#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <exception>
#include <utility>
#include "Cache.hpp"

// Parser
class Parser {
    public:
    Parser ();
    ~Parser ();
    void Load(const std::string &path, std::unique_ptr<Cache> &c, const std::string format);
    void Parse(std::ifstream *f, std::unique_ptr<Cache> &c, const std::string format);
    // load is 1, store is 0
    std::unique_ptr<std::vector<std::pair<int,int>>> ParseWithHistory(std::ifstream *f, std::unique_ptr<Cache> &_c, const std::string format);

    std::string format;
    std::string path;
};

class LDFMParser : public Parser {
    public:
    std::string input = "l 0x20022000";
     
};

#endif
