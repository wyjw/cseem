// Parser
class Parser {
    public:
    Parser ();
    
    void Load(const std::string &path) {
        this->path = path;
        std::ifstream f(path);
        if (!f)
            throw Error(std::fmt("%s: File read error ", path.c_str()));

        Parse(dynamic_cast<std::istream *>(&f));
        f.close();
    }

    void Parse(std::istream *f);
};

class LDFMParser : public Parser {
    
};
