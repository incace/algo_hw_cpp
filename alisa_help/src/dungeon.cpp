#include "dungeon.hpp"
#include <algorithm>
#include <climits>
#include <queue>

namespace alisa {

void GameRules::setTarget(const std::string& name) {
    if      (name == "iron") values[IRON] *= 2;
    else if (name == "gold") values[GOLD] *= 2;
    else if (name == "gems") values[GEMS] *= 2;
    else if (name == "exp")  values[EXP]  *= 2;
}

void Room::printState(std::ostream& out) const {
    out << "state " << id;
    for (int i = 0; i < 4; ++i) {
        if (resources[i] == -1) out << " _";
        else                    out << " " << resources[i];
    }
    out << "\n";
}

Dungeon::Dungeon(int n, std::vector<Room> rooms, int food, const std::string& target)
    : n_(n)
    , rooms_(std::move(rooms))
    , initialFood_(food)
    , food_(food)
    , current_(0)
{
    for (auto& r : rooms_)
        std::sort(r.neighbors.begin(), r.neighbors.end());
    rules_.setTarget(target);
    rooms_[0].visited = true;
}

void Dungeon::moveTo(int id, std::ostream& out) {
    food_--;
    current_ = id;
    rooms_[id].visited = true;
    out << "go " << id << "\n";
    if (id != 0)
        rooms_[id].printState(out);
}

void Dungeon::collectBest(std::ostream& out) {
    static const char* names[] = {"iron", "gold", "gems", "exp"};
    int best = -1, bestVal = -1;
    for (int i = 0; i < 4; ++i) {
        if (rooms_[current_].resources[i] > 0 && rules_.values[i] > bestVal) {
            bestVal = rules_.values[i];
            best    = i;
        }
    }
    if (best == -1) return;
    collected_[best]                 += rooms_[current_].resources[best];
    rooms_[current_].resources[best]  = -1;
    out << "collect " << names[best] << "\n";
    rooms_[current_].printState(out);
}

std::vector<int> Dungeon::pathToNearestUnvisited() const {
    std::vector<int> dist(n_ + 1, -1);
    std::vector<int> parent(n_ + 1, -1);
    std::queue<int> q;
    dist[current_] = 0;
    q.push(current_);

    int target = -1, minDist = INT_MAX;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        if (dist[u] > minDist) break;
        if (!rooms_[u].visited) {
            if (target == -1 || u < target) { target = u; minDist = dist[u]; }
            continue;
        }
        for (int v : rooms_[u].neighbors) {
            if (dist[v] == -1) {
                dist[v] = dist[u] + 1;
                parent[v] = u;
                q.push(v);
            }
        }
    }

    if (target == -1) return {};
    std::vector<int> path;
    for (int v = target; v != -1; v = parent[v])
        path.push_back(v);
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<int> Dungeon::pathToRoom0() const {
    std::vector<int> dist(n_ + 1, -1);
    std::vector<int> parent(n_ + 1, -1);
    std::queue<int> q;
    dist[0] = 0;
    q.push(0);

    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int v : rooms_[u].neighbors) {
            if (rooms_[v].visited && dist[v] == -1) {
                dist[v]   = dist[u] + 1;
                parent[v] = u;
                q.push(v);
            }
        }
    }

    if (dist[current_] == -1) return {};
    std::vector<int> path;
    for (int v = current_; v != -1; v = parent[v])
        path.push_back(v);
    return path;
}

void Dungeon::printResult(std::ostream& out) const {
    long long total = 0;
    for (int i = 0; i < 4; ++i)
        total += static_cast<long long>(collected_[i]) * rules_.values[i];
    out << "result "
        << collected_[IRON] << " " << collected_[GOLD] << " "
        << collected_[GEMS] << " " << collected_[EXP]  << " "
        << total << "\n";
}

void Dungeon::discoveryPhase(std::ostream& out) {
    const int budget = initialFood_ / 2;
    int spent = 0;

    while (spent < budget) {
        int next = -1;
        for (int nb : rooms_[current_].neighbors) {
            if (!rooms_[nb].visited) { next = nb; break; }
        }

        if (next != -1) {
            moveTo(next, out);
            collectBest(out);
            spent++;
        } else {
            std::vector<int> path = pathToNearestUnvisited();
            if (path.size() < 2) break;

            for (size_t i = 1; i < path.size() && spent < budget; ++i) {
                bool wasVisited = rooms_[path[i]].visited;
                moveTo(path[i], out);
                if (!wasVisited) collectBest(out);
                spent++;
                if (!wasVisited) break;
            }
        }
    }
}

void Dungeon::returnPhase(std::ostream& out) {
    std::vector<int> path = pathToRoom0();
    if (path.empty()) return;

    for (size_t i = 0; i < path.size(); ++i) {
        current_ = path[i];
        int stepsLeft = static_cast<int>(path.size()) - 1 - static_cast<int>(i);

        while (food_ > stepsLeft) {
            bool hasAny = false, alreadyCollected = false;
            for (int r = 0; r < 4; ++r) {
                if (rooms_[current_].resources[r] >  0) hasAny            = true;
                if (rooms_[current_].resources[r] == -1) alreadyCollected = true;
            }
            if (!hasAny) break;
            if (alreadyCollected) food_--;
            collectBest(out);
        }

        if (i + 1 < path.size()) {
            int nextId = path[i + 1];
            food_--;
            current_ = nextId;
            rooms_[current_].visited = true;
            out << "go " << nextId << "\n";
            if (current_ != 0)
                rooms_[current_].printState(out);
        }
    }

    printResult(out);
}

} // namespace alisa
