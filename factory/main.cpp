#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <vector>


struct Workpiece {
    int id;
    int stage;
};

struct Workstation {
    std::queue<int> inbox;
    bool            active = false;
};


enum class Tag : int { Finish = 0, Start = 1, Wait = 2, Ready = 3 };

struct Event {
    long long time;
    Tag       tag;
    int       piece_id;
    int       station_id;
    int       queue_pos;
};

struct ByPriority {
    bool operator()(const Event& a, const Event& b) const {
        if (a.time       != b.time)       return a.time       > b.time;
        if (a.tag        != b.tag)        return static_cast<int>(a.tag) > static_cast<int>(b.tag);
        if (a.station_id != b.station_id) return a.station_id > b.station_id;
        return a.piece_id > b.piece_id;
    }
};


struct Config {
    int m, n;
    std::vector<std::vector<int>> dur;
    std::vector<std::vector<int>> queues;
    int total_pieces;
};


enum class ReadStatus { Ok, BadLine, NoFile };

static ReadStatus reject(const std::string& line) {
    std::cout << line << "\n";
    return ReadStatus::BadLine;
}

static bool tokenize(const std::string& line, std::vector<long long>& out) {
    out.clear();
    std::istringstream ss(line);
    std::string tok;
    while (ss >> tok) {
        size_t consumed = 0;
        long long val;
        try { val = std::stoll(tok, &consumed); } catch (...) { return false; }
        if (consumed != tok.size()) return false;
        out.push_back(val);
    }
    return true;
}

static bool blank(const std::string& line) {
    for (char c : line)
        if (!std::isspace(static_cast<unsigned char>(c))) return false;
    return true;
}

static ReadStatus read_config(const std::string& path, Config& cfg) {
    std::ifstream file(path);
    if (!file) return ReadStatus::NoFile;

    std::vector<std::string> lines;
    std::string raw;
    while (std::getline(file, raw)) {
        if (!raw.empty() && raw.back() == '\r') raw.pop_back();
        lines.push_back(raw);
    }

    size_t idx = 0;
    auto next = [&]() -> const std::string* {
        return idx < lines.size() ? &lines[idx++] : nullptr;
    };

    std::vector<long long> nums;
    const std::string* ln;

    ln = next();
    if (!ln || !tokenize(*ln, nums) || nums.size() != 2) return reject(ln ? *ln : "");
    if (nums[0] < 1 || nums[0] > 100 || nums[1] < 1 || nums[1] > 100) return reject(*ln);
    cfg.m = static_cast<int>(nums[0]);
    cfg.n = static_cast<int>(nums[1]);

    cfg.dur.assign(cfg.m - 1, std::vector<int>(cfg.n));
    for (int op = 0; op < cfg.m - 1; ++op) {
        ln = next();
        if (!ln || !tokenize(*ln, nums) || static_cast<int>(nums.size()) != cfg.n)
            return reject(ln ? *ln : "");
        for (int j = 0; j < cfg.n; ++j) {
            if (nums[j] < 0 || nums[j] > 10000) return reject(*ln);
            cfg.dur[op][j] = static_cast<int>(nums[j]);
        }
    }

    cfg.queues.assign(cfg.n, {});
    long long total = 0;
    for (int j = 0; j < cfg.n; ++j) {
        ln = next();
        if (!ln || !tokenize(*ln, nums) || nums.empty()) return reject(ln ? *ln : "");
        if (nums[0] < 0) return reject(*ln);
        const int qsz = static_cast<int>(nums[0]);
        if (static_cast<int>(nums.size()) != qsz + 1) return reject(*ln);
        cfg.queues[j].reserve(qsz);
        for (int p = 0; p < qsz; ++p) {
            if (nums[p + 1] < 0 || nums[p + 1] > cfg.m - 2) return reject(*ln);
            cfg.queues[j].push_back(static_cast<int>(nums[p + 1]));
        }
        total += qsz;
        if (total > 100000) return reject(*ln);
    }
    cfg.total_pieces = static_cast<int>(total);

    for (; idx < lines.size(); ++idx)
        if (!blank(lines[idx])) return reject(lines[idx]);

    return ReadStatus::Ok;
}


static int pick_station(const std::vector<long long>& backlog) {
    int best = 0;
    for (int j = 1; j < static_cast<int>(backlog.size()); ++j)
        if (backlog[j] < backlog[best]) best = j;
    return best;
}

int main(int argc, char* argv[]) {
    if (argc < 2) { std::cerr << "usage: factory <input_file>\n"; return 1; }

    Config cfg;
    const ReadStatus rs = read_config(argv[1], cfg);
    if (rs == ReadStatus::NoFile) { std::cerr << "cannot open file\n"; return 1; }
    if (rs == ReadStatus::BadLine) return 0;

    std::vector<Workpiece>    pieces;    pieces.reserve(cfg.total_pieces);
    std::vector<Workstation>  stations(cfg.n);
    std::vector<long long>    backlog(cfg.n, 0);

    int next_id = 0;
    for (int j = 0; j < cfg.n; ++j) {
        for (int stage : cfg.queues[j]) {
            pieces.push_back({next_id++, stage});
            stations[j].inbox.push(pieces.back().id);
            backlog[j] += cfg.dur[stage][j];
        }
    }

    std::priority_queue<Event, std::vector<Event>, ByPriority> events;

    auto schedule_start = [&](long long t, int piece_id, int st_id) {
        stations[st_id].active = true;
        events.push({t, Tag::Start, piece_id, st_id, -1});
    };

    for (int j = 0; j < cfg.n; ++j) {
        if (!stations[j].inbox.empty()) {
            int first = stations[j].inbox.front();
            stations[j].inbox.pop();
            backlog[j] -= cfg.dur[pieces[first].stage][j];
            schedule_start(0, first, j);
        }
    }

    int       finished   = 0;
    long long stop_time  = 0;

    while (!events.empty()) {
        const Event ev = events.top();
        events.pop();

        Workpiece&   wp = pieces[ev.piece_id];
        Workstation& ws = stations[ev.station_id];

        if (ev.tag == Tag::Finish) {
            ws.active = false;
            std::cout << "finish " << ev.time << " " << wp.id
                      << " " << wp.stage << " " << ev.station_id << "\n";

            ++wp.stage;

            if (wp.stage == cfg.m - 1) {
                events.push({ev.time, Tag::Ready, wp.id, ev.station_id, -1});
            } else {
                int dst = pick_station(backlog);
                if (!stations[dst].active) {
                    schedule_start(ev.time, wp.id, dst);
                } else {
                    const int pos = static_cast<int>(stations[dst].inbox.size());
                    stations[dst].inbox.push(wp.id);
                    backlog[dst] += cfg.dur[wp.stage][dst];
                    events.push({ev.time, Tag::Wait, wp.id, dst, pos});
                }
            }

            if (!ws.inbox.empty()) {
                int nxt = ws.inbox.front();
                ws.inbox.pop();
                backlog[ev.station_id] -= cfg.dur[pieces[nxt].stage][ev.station_id];
                schedule_start(ev.time, nxt, ev.station_id);
            }
            continue;
        }

        if (ev.tag == Tag::Start) {
            const long long finish_at = ev.time + cfg.dur[wp.stage][ev.station_id];
            std::cout << "start " << ev.time << " " << wp.id
                      << " " << wp.stage << " " << ev.station_id << "\n";
            events.push({finish_at, Tag::Finish, wp.id, ev.station_id, -1});
            continue;
        }

        if (ev.tag == Tag::Wait) {
            std::cout << "wait " << ev.time << " " << wp.id
                      << " " << wp.stage << " " << ev.station_id
                      << " " << ev.queue_pos << "\n";
            continue;
        }

        std::cout << "ready " << ev.time << " " << wp.id
                  << " " << ev.station_id << "\n";
        if (++finished == cfg.total_pieces)
            stop_time = ev.time;
    }

    std::cout << "stop " << stop_time << "\n";
    return 0;
}
