#include "Parser.hpp"

Parser::Parser() {
}

Parser::~Parser() {

}

int string_to_read_op(std::string rd) {
    if (rd == "l") {
        return 1;
    }
    else if (rd == "s") {
        return 0;
    }

    return -1;
}

void Parser::Parse(std::ifstream *f, std::unique_ptr<Cache> &_c, const std::string format) {
    std::string line;
    while (std::getline(*f, line))
    {
        std::istringstream iss(line);
        if (format == "simple") { 
            std::string a;
            int b;

            if (!(iss >> a >> std::hex >> b)) {
                throw std::runtime_error("Format is off!");
                break; 
            } // error 

            // do stuff with line
            _c->do_cache_op(b, string_to_read_op(a));
        } 
        else if (format == "hw") {
            char ch;
            int a;
            int b;
            int c;
            
            if (!(iss >> ch  >> a >> std::hex >> b >> c)) { 
                throw std::runtime_error("Format is off!");
                break;
            }

            _c->do_cache_op(b, a);
        } 
    }
    // f->close();
}

std::unique_ptr<std::vector<std::pair<int,int>>> Parser::ParseWithHistory(std::ifstream *f, std::unique_ptr<Cache> &_c, const std::string format) {
    std::string line;
    std::unique_ptr<std::vector<std::pair<int, int>>> res = std::make_unique<std::vector<std::pair<int, int>>>();
    while (std::getline(*f, line))
    {
        std::istringstream iss(line);
        if (format == "simple") { 
            std::string a;
            int b;

            if (!(iss >> a >> std::hex >> b)) {
                throw std::runtime_error("Format is off!");
                break; 
            } // error 

            // do stuff with line
            _c->do_cache_op(b, string_to_read_op(a));
            res->push_back(std::pair<int, int>(b, string_to_read_op(a)));
        } 
        else if (format == "hw") {
            char ch;
            int a;
            int b;
            int c;
            
            if (!(iss >> ch  >> a >> std::hex >> b >> c)) { 
                throw std::runtime_error("Format is off!");
                break;
            }

            _c->do_cache_op(b, a);
            res->push_back(std::pair<int, int>(b, a));
        } 
    }
    // f->close();
    return res;
}
    
void Parser::Load(const std::string &path, std::unique_ptr<Cache> &c, const std::string format) {
    this->path = path;
    std::ifstream f(path);
    if (!f)
    {
        std::cout << "No such file" << std::endl;
        throw std::runtime_error("File read error " + path);
    }

    Parse(dynamic_cast<std::ifstream *>(&f), c, format);

    // this->Parse(f);
    f.close();
}
