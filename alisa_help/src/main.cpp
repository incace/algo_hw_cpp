#include "dungeon.hpp"
#include "parse.hpp"
#include <fstream>
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: alisa_bot <input_file>\n";
        return 1;
    }

    std::ofstream out("result.txt");
    if (!out.is_open()) {
        std::cerr << "Cannot open result.txt for writing\n";
        return 1;
    }

    alisa::ParseResult pr(argv[1]);

    if (!pr.success) {
        if (!pr.errorLine.empty())
            out << pr.errorLine << "\n";
        return 0;
    }

    alisa::Dungeon dungeon(pr.n, pr.rooms, pr.m, pr.target);
    dungeon.discoveryPhase(out);
    dungeon.returnPhase(out);

    return 0;
}
