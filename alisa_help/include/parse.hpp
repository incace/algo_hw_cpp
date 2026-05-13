#pragma once
#include <string>
#include <vector>
#include "dungeon.hpp"

namespace alisa {

struct ParseResult {
    bool              success  = false;
    int               n        = 0;
    std::vector<Room> rooms;
    int               m        = 0;
    std::string       target;
    std::string       errorLine;

    explicit ParseResult(const std::string& filename);

private:
    static bool isNumber(const std::string& s);
    static bool isValidNeighborList(const std::string& s);
};

} // namespace alisa
