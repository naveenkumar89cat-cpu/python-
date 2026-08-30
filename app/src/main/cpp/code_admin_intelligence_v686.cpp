// ROBOT ADMIN AI OS — V68.6 CODE / ADMIN INTELLIGENCE
// Android-only, offline-first, read-only inspection engine.
// APK integration intentionally deferred until FAST MASTER checkpoint.

#include <algorithm>
#include <cctype>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace ra686 {

constexpr int ERR_BLOCKED = -403;
constexpr std::size_t MAX_FILES = 2048;
constexpr std::size_t MAX_ERRORS = 512;

struct SourceFile {
    std::string path;
    std::string language;
    std::size_t lines = 0;
    bool readable = false;
};

struct ProjectReport {
    std::size_t total_files = 0;
    std::size_t readable_files = 0;
    std::size_t source_files = 0;
    std::size_t config_files = 0;
    std::size_t unknown_files = 0;
    std::size_t total_lines = 0;
    bool truncated = false;
    std::map<std::string, std::size_t> language_counts;
};

struct BuildResult {
    int exit_code = 0;
    std::size_t error_lines = 0;
    std::size_t warning_lines = 0;
    bool pass = true;
    std::vector<std::string> important_lines;
};

static std::string lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return s;
}

static std::string extension_of(const std::string& path) {
    const auto slash = path.find_last_of("/\\");
    const auto dot = path.find_last_of('.');
    if (dot == std::string::npos || (slash != std::string::npos && dot < slash) || dot + 1 >= path.size()) return {};
    return lower(path.substr(dot + 1));
}

static std::string detect_language(const std::string& path) {
    const std::string ext = extension_of(path);
    if (ext == "cpp" || ext == "cc" || ext == "cxx" || ext == "hpp" || ext == "hh") return "C++";
    if (ext == "c" || ext == "h") return "C";
    if (ext == "py") return "Python";
    if (ext == "java") return "Java";
    if (ext == "js" || ext == "mjs" || ext == "cjs") return "JavaScript";
    if (ext == "html" || ext == "htm") return "HTML";
    if (ext == "css") return "CSS";
    if (ext == "xml") return "XML";
    if (ext == "json") return "JSON";
    if (ext == "sql") return "SQL";
    if (ext == "sh") return "Shell";
    return "Unknown";
}

static bool is_config_file(const std::string& path) {
    const std::string ext = extension_of(path);
    return ext == "json" || ext == "xml" || ext == "yaml" || ext == "yml" ||
           ext == "toml" || ext == "ini" || ext == "cfg" || ext == "properties" ||
           ext == "gradle" || ext == "cmake";
}

static ProjectReport summarize_project(const std::vector<SourceFile>& input) {
    ProjectReport r;
    const std::size_t n = std::min(input.size(), MAX_FILES);
    r.total_files = n;
    r.truncated = input.size() > MAX_FILES;

    for (std::size_t i = 0; i < n; ++i) {
        const auto& f = input[i];
        if (f.readable) ++r.readable_files;
        r.total_lines += f.lines;

        std::string lang = f.language.empty() ? detect_language(f.path) : f.language;
        if (lang != "Unknown") {
            ++r.source_files;
            ++r.language_counts[lang];
        } else if (is_config_file(f.path)) {
            ++r.config_files;
        } else {
            ++r.unknown_files;
        }
    }
    return r;
}

static BuildResult analyze_build_log(const std::string& log, int exit_code) {
    BuildResult r;
    r.exit_code = exit_code;
    r.pass = exit_code == 0;

    std::istringstream in(log);
    std::string line;
    while (std::getline(in, line)) {
        const std::string l = lower(line);
        const bool is_error = l.find("error:") != std::string::npos ||
                              l.find("fatal error") != std::string::npos ||
                              l.find("undefined reference") != std::string::npos ||
                              l.find("failed") != std::string::npos;
        const bool is_warning = l.find("warning:") != std::string::npos;

        if (is_error) {
            ++r.error_lines;
            r.pass = false;
        }
        if (is_warning) ++r.warning_lines;

        if ((is_error || is_warning) && r.important_lines.size() < MAX_ERRORS) {
            r.important_lines.push_back(line);
        }
    }
    return r;
}

static std::vector<SourceFile> filter_language(const std::vector<SourceFile>& files, const std::string& language) {
    std::vector<SourceFile> out;
    const std::string want = lower(language);
    for (const auto& f : files) {
        const std::string lang = f.language.empty() ? detect_language(f.path) : f.language;
        if (lower(lang) == want) {
            out.push_back(f);
            if (out.size() >= MAX_FILES) break;
        }
    }
    return out;
}

// Safety wall: this module can inspect code/build/test results only.
extern "C" int ra686_code_create_blocked() { return ERR_BLOCKED; }
extern "C" int ra686_code_modify_blocked() { return ERR_BLOCKED; }
extern "C" int ra686_code_delete_blocked() { return ERR_BLOCKED; }
extern "C" int ra686_compile_execute_blocked() { return ERR_BLOCKED; }
extern "C" int ra686_test_execute_blocked() { return ERR_BLOCKED; }
extern "C" int ra686_fix_apply_blocked() { return ERR_BLOCKED; }
extern "C" int ra686_version_apply_blocked() { return ERR_BLOCKED; }
extern "C" const char* ra686_code_admin_version() {
    return "68.6-code-admin-intelligence-readonly";
}

} // namespace ra686
