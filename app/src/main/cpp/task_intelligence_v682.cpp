// ROBOT ADMIN AI OS — V68.2 TASK INTELLIGENCE
// Android-only, offline-first, read-only source module.
// APK integration intentionally deferred until FAST MASTER checkpoint.

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

namespace ra682 {

constexpr int OK = 0;
constexpr int ERR_INVALID = -400;
constexpr int ERR_BLOCKED = -403;
constexpr std::size_t MAX_TASKS = 512;

struct TaskItem {
    std::string id;
    std::string title;
    std::string priority = "NORMAL";
    std::string status = "PENDING";
    std::string note;
};

struct TaskReport {
    std::size_t total = 0;
    std::size_t pending = 0;
    std::size_t done = 0;
    std::size_t blocked = 0;
    std::size_t high = 0;
    std::size_t normal = 0;
    std::size_t low = 0;
    std::size_t invalid = 0;
    bool truncated = false;
};

static std::string trim(const std::string& s) {
    std::size_t a = 0;
    while (a < s.size() && std::isspace(static_cast<unsigned char>(s[a]))) ++a;
    std::size_t b = s.size();
    while (b > a && std::isspace(static_cast<unsigned char>(s[b - 1]))) --b;
    return s.substr(a, b - a);
}

static std::string upper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return static_cast<char>(std::toupper(c));
    });
    return s;
}

static bool valid_priority(const std::string& p) {
    const auto u = upper(trim(p));
    return u == "HIGH" || u == "NORMAL" || u == "LOW";
}

static bool valid_status(const std::string& s) {
    const auto u = upper(trim(s));
    return u == "PENDING" || u == "DONE" || u == "BLOCKED";
}

static bool parse_line(const std::string& line, TaskItem& out) {
    // Format: ID|TITLE|PRIORITY|STATUS|NOTE
    std::vector<std::string> parts;
    std::string cur;
    for (char c : line) {
        if (c == '|') {
            parts.push_back(trim(cur));
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    parts.push_back(trim(cur));
    if (parts.size() < 2) return false;

    out.id = parts[0];
    out.title = parts[1];
    if (parts.size() > 2 && !parts[2].empty()) out.priority = upper(parts[2]);
    if (parts.size() > 3 && !parts[3].empty()) out.status = upper(parts[3]);
    if (parts.size() > 4) out.note = parts[4];

    if (out.id.empty() || out.title.empty()) return false;
    if (!valid_priority(out.priority)) out.priority = "NORMAL";
    if (!valid_status(out.status)) out.status = "PENDING";
    return true;
}

static std::vector<TaskItem> parse_text(const std::string& text, std::size_t* invalid_count = nullptr, bool* truncated = nullptr) {
    std::vector<TaskItem> out;
    std::size_t invalid = 0;
    bool cut = false;
    std::istringstream in(text);
    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        TaskItem t;
        if (!parse_line(line, t)) {
            ++invalid;
            continue;
        }
        if (out.size() >= MAX_TASKS) {
            cut = true;
            break;
        }
        out.push_back(t);
    }
    if (invalid_count) *invalid_count = invalid;
    if (truncated) *truncated = cut;
    return out;
}

static TaskReport summarize(const std::vector<TaskItem>& tasks, std::size_t invalid = 0, bool truncated = false) {
    TaskReport r;
    r.total = std::min(tasks.size(), MAX_TASKS);
    r.invalid = invalid;
    r.truncated = truncated || tasks.size() > MAX_TASKS;
    for (std::size_t i = 0; i < r.total; ++i) {
        const auto p = upper(tasks[i].priority);
        const auto s = upper(tasks[i].status);
        if (s == "DONE") ++r.done;
        else if (s == "BLOCKED") ++r.blocked;
        else ++r.pending;

        if (p == "HIGH") ++r.high;
        else if (p == "LOW") ++r.low;
        else ++r.normal;
    }
    return r;
}

static std::vector<TaskItem> search(const std::vector<TaskItem>& tasks, const std::string& query) {
    std::vector<TaskItem> out;
    const auto q = upper(trim(query));
    if (q.empty()) return out;
    for (const auto& t : tasks) {
        const auto hay = upper(t.id + "\n" + t.title + "\n" + t.priority + "\n" + t.status + "\n" + t.note);
        if (hay.find(q) != std::string::npos) {
            out.push_back(t);
            if (out.size() >= MAX_TASKS) break;
        }
    }
    return out;
}

static std::vector<TaskItem> filter_status(const std::vector<TaskItem>& tasks, const std::string& status) {
    std::vector<TaskItem> out;
    const auto target = upper(trim(status));
    for (const auto& t : tasks) {
        if (upper(t.status) == target) {
            out.push_back(t);
            if (out.size() >= MAX_TASKS) break;
        }
    }
    return out;
}

static std::vector<TaskItem> filter_priority(const std::vector<TaskItem>& tasks, const std::string& priority) {
    std::vector<TaskItem> out;
    const auto target = upper(trim(priority));
    for (const auto& t : tasks) {
        if (upper(t.priority) == target) {
            out.push_back(t);
            if (out.size() >= MAX_TASKS) break;
        }
    }
    return out;
}

// Permanent V68.x safety wall: intelligence only, no task mutation or execution.
extern "C" int ra682_task_execute_blocked() { return ERR_BLOCKED; }
extern "C" int ra682_task_create_blocked() { return ERR_BLOCKED; }
extern "C" int ra682_task_update_blocked() { return ERR_BLOCKED; }
extern "C" int ra682_task_delete_blocked() { return ERR_BLOCKED; }
extern "C" int ra682_task_schedule_blocked() { return ERR_BLOCKED; }

extern "C" const char* ra682_task_engine_version() {
    return "68.2-task-intelligence-readonly";
}

} // namespace ra682
