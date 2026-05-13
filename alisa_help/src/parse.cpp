#include "parse.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>

namespace alisa {

bool ParseResult::isNumber(const std::string& s) {
    if (s.empty()) return false;
    return std::all_of(s.begin(), s.end(),
                       [](unsigned char c) { return std::isdigit(c); });
}

bool ParseResult::isValidNeighborList(const std::string& s) {
    if (s.empty()) return true;
    for (char c : s)
        if (!std::isdigit(static_cast<unsigned char>(c)) && c != ',') return false;
    if (s.front() == ',' || s.back() == ',') return false;
    if (s.find(",,") != std::string::npos) return false;
    return true;
}

ParseResult::ParseResult(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) return;

    if (!std::getline(file, line)) return;
    if (!isNumber(line)) { errorLine = line; return; }
    n = std::stoi(line);
    if (n < 1 || n > 255) { errorLine = line; return; }

    rooms.resize(n + 1);

    for (int i = 0; i <= n; ++i) {
        if (!std::getline(file, line)) return;

        std::istringstream ss(line);
        std::vector<std::string> tokens;
        std::string tok;
        while (ss >> tok) tokens.push_back(tok);

        if (tokens.size() != 6) { errorLine = line; return; }

        if (!isNumber(tokens[0])) { errorLine = line; return; }
        int id = std::stoi(tokens[0]);
        if (id < 0 || id > n) { errorLine = line; return; }
        rooms[id].id = id;

        if (!isValidNeighborList(tokens[1])) { errorLine = line; return; }
        std::istringstream nss(tokens[1]);
        std::string nb;
        while (std::getline(nss, nb, ',')) {
            if (nb.empty()) continue;
            if (!isNumber(nb)) { errorLine = line; return; }
            int nbId = std::stoi(nb);
            if (nbId < 0 || nbId > n) { errorLine = line; return; }
            rooms[id].neighbors.push_back(nbId);
        }

        for (int r = 0; r < 4; ++r) {
            if (!isNumber(tokens[r + 2])) { errorLine = line; return; }
            int v = std::stoi(tokens[r + 2]);
            if (v < 0 || v > 255) { errorLine = line; return; }
            rooms[id].resources[r] = v;
        }
    }

    if (!std::getline(file, line)) return;
    std::istringstream ls(line);
    std::string mStr, tStr;
    if (!(ls >> mStr >> tStr)) { errorLine = line; return; }
    if (!isNumber(mStr)) { errorLine = line; return; }
    m = std::stoi(mStr);
    if (m < 2 || m > 255) { errorLine = line; return; }
    if (tStr != "iron" && tStr != "gold" && tStr != "gems" && tStr != "exp") {
        errorLine = line; return;
    }
    target = tStr;

    success = true;
}

} // namespace alisa
