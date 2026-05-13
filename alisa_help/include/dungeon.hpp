#pragma once
#include <array>
#include <iostream>
#include <string>
#include <vector>

namespace alisa {

enum ResourceType { IRON = 0, GOLD = 1, GEMS = 2, EXP = 3 };

struct GameRules {
    std::array<int, 4> values = {7, 11, 23, 1};
    void setTarget(const std::string& name);
};

struct Room {
    int              id = 0;
    std::vector<int> neighbors;
    std::array<int, 4> resources = {};
    bool             visited = false;

    void printState(std::ostream& out) const;
};

class Dungeon {
    int               n_;
    std::vector<Room> rooms_;
    int               initialFood_;
    int               food_;
    int               current_;
    GameRules         rules_;
    std::array<int, 4> collected_{};

    void moveTo(int id, std::ostream& out);
    void collectBest(std::ostream& out);
    std::vector<int> pathToNearestUnvisited() const;
    std::vector<int> pathToRoom0() const;
    void printResult(std::ostream& out) const;

public:
    Dungeon(int n, std::vector<Room> rooms, int food, const std::string& target);
    void discoveryPhase(std::ostream& out);
    void returnPhase(std::ostream& out);
};

} // namespace alisa
