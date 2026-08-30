// ROBOT ADMIN AI OS — V68.3 DATA INTELLIGENCE
// Android-only, offline-first, read-only. APK integration deferred.

#include <algorithm>
#include <cctype>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace ra683 {

constexpr int ERR_BLOCKED = -403;
constexpr std::size_t MAX_ROWS = 20000;
constexpr std::size_t MAX_COLS = 256;

struct CsvReport {
    std::size_t rows = 0;
    std::size_t cols = 0;
    std::size_t numeric_cells = 0;
    std::size_t text_cells = 0;
    std::size_t empty_cells = 0;
    std::size_t invalid_rows = 0;
    std::size_t duplicate_rows = 0;
    bool truncated = false;
    bool has_numeric = false;
    double min_value = 0.0;
    double max_value = 0.0;
    double average = 0.0;
};

static std::vector<std::string> parse_csv_line(const std::string& line) {
    std::vector<std::string> out;
    std::string cell;
    bool quoted = false;
    for (std::size_t i = 0; i < line.size(); ++i) {
        const char c = line[i];
        if (c == '"') {
            if (quoted && i + 1 < line.size() && line[i + 1] == '"') {
                cell.push_back('"'); ++i;
            } else quoted = !quoted;
        } else if (c == ',' && !quoted) {
            out.push_back(cell); cell.clear();
        } else cell.push_back(c);
    }
    out.push_back(cell);
    return out;
}

static bool number(const std::string& s, double& value) {
    if (s.empty()) return false;
    char* end = nullptr;
    errno = 0;
    value = std::strtod(s.c_str(), &end);
    if (errno != 0 || end == s.c_str()) return false;
    while (*end && std::isspace(static_cast<unsigned char>(*end))) ++end;
    return *end == '\0' && std::isfinite(value);
}

static CsvReport analyze_csv(const std::string& text) {
    CsvReport r;
    std::istringstream in(text);
    std::string line;
    std::size_t expected_cols = 0;
    std::set<std::string> seen;
    double sum = 0.0;

    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        if (r.rows >= MAX_ROWS) { r.truncated = true; break; }

        auto cells = parse_csv_line(line);
        if (cells.size() > MAX_COLS) { ++r.invalid_rows; r.truncated = true; continue; }
        if (expected_cols == 0) expected_cols = cells.size();
        else if (cells.size() != expected_cols) ++r.invalid_rows;
        r.cols = std::max(r.cols, cells.size());

        if (!seen.insert(line).second) ++r.duplicate_rows;

        for (const auto& cell : cells) {
            if (cell.empty()) { ++r.empty_cells; continue; }
            double v = 0.0;
            if (number(cell, v)) {
                if (!r.has_numeric) { r.min_value = r.max_value = v; r.has_numeric = true; }
                else { r.min_value = std::min(r.min_value, v); r.max_value = std::max(r.max_value, v); }
                sum += v; ++r.numeric_cells;
            } else ++r.text_cells;
        }
        ++r.rows;
    }
    if (r.numeric_cells) r.average = sum / static_cast<double>(r.numeric_cells);
    return r;
}

static std::vector<std::string> search_rows(const std::string& text, const std::string& query) {
    std::vector<std::string> out;
    if (query.empty()) return out;
    std::istringstream in(text);
    std::string line;
    while (std::getline(in, line) && out.size() < 512) {
        if (line.find(query) != std::string::npos) out.push_back(line);
    }
    return out;
}

// JSON/SQLite are intentionally capability markers only until dedicated parsers/read-only adapters land.
static bool json_candidate(const std::string& text) {
    auto first = text.find_first_not_of(" \t\r\n");
    auto last = text.find_last_not_of(" \t\r\n");
    if (first == std::string::npos || last == std::string::npos) return false;
    return (text[first] == '{' && text[last] == '}') || (text[first] == '[' && text[last] == ']');
}

extern "C" int ra683_data_write_blocked() { return ERR_BLOCKED; }
extern "C" int ra683_data_delete_blocked() { return ERR_BLOCKED; }
extern "C" int ra683_data_update_blocked() { return ERR_BLOCKED; }
extern "C" int ra683_sql_execute_blocked() { return ERR_BLOCKED; }
extern "C" const char* ra683_data_engine_version() { return "68.3-data-intelligence-readonly"; }

} // namespace ra683
