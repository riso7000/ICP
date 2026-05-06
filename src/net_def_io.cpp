#include "../third_party/nlohmann/json.hpp"
#include <iostream>
#include <fstream>

using json = nlohmann::json;

int read_ned_def(std::string path) {
    std::ifstream file(path);
    json data = json::parse(file);



    return 0;
}
