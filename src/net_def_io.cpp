#include "../third_party/nlohmann/json.hpp"
#include <iostream>
#include <fstream>

using json = nlohmann::json;

int read_netdef(std::string path) {
    std::ifstream file(path);
    json data = json::parse(file);



    return 0;
}
