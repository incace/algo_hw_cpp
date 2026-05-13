#include "dungeon.hpp"
#include "parse.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

static std::string runBot(const std::string& inputFile) {
    alisa::ParseResult pr(inputFile);
    std::ostringstream oss;
    if (!pr.success) {
        if (!pr.errorLine.empty())
            oss << pr.errorLine << "\n";
        return oss.str();
    }
    alisa::Dungeon dungeon(pr.n, pr.rooms, pr.m, pr.target);
    dungeon.discoveryPhase(oss);
    dungeon.returnPhase(oss);
    return oss.str();
}

static std::string readFile(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) return "";
    std::ostringstream oss;
    oss << f.rdbuf();
    return oss.str();
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: test_runner <input.in> <expected.out>\n";
        return 1;
    }

    const std::string actual   = runBot(argv[1]);
    const std::string expected = readFile(argv[2]);

    if (actual == expected) {
        std::cout << "PASS: " << argv[1] << "\n";
        return 0;
    }

    std::cout << "FAIL: " << argv[1] << "\n";
    std::cout << "── expected ──────────────────────\n" << expected;
    std::cout << "── got ───────────────────────────\n" << actual;
    return 1;
}
