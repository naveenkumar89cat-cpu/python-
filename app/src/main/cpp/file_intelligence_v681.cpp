// ROBOT ADMIN AI OS — V68.1 FILE INTELLIGENCE
// Android-only, offline-first, read-only source module.
// APK integration intentionally deferred until FAST MASTER checkpoint.

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <string>
#include <vector>

namespace ra681 {

constexpr int OK = 0;
constexpr int ERR_INVALID = -400;
constexpr int ERR_BLOCKED = -403;
constexpr int ERR_LIMIT = -413;
constexpr std::size_t MAX_FILES = 512;
constexpr std::uint64_t MAX_PREVIEW_BYTES = 256 * 1024;

struct FileItem {
    std::string name;
    std::string path;
    std::uint64_t size = 0;
    bool directory = false;
    bool readable = false;
    std::string extension;
};

struct FileReport {
    std::size_t total = 0;
    std::size_t files = 0;
    std::size_t folders = 0;
    std::size_t readable = 0;
    std::size_t unreadable = 0;
    std::uint64_t bytes = 0;
    bool truncated = false;
};

static std::string lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return s;
}

static std::string extension_of(const std::string& name) {
    const auto slash = name.find_last_of("/\\");
    const auto dot = name.find_last_of('.');
    if (dot == std::string::npos || (slash != std::string::npos && dot < slash) || dot + 1 >= name.size()) {
        return {};
    }
    return lower(name.substr(dot + 1));
}

static bool is_text_preview_type(const std::string& name) {
    const std::string ext = extension_of(name);
    return ext == "txt" || ext == "csv" || ext == "json" || ext == "xml" ||
           ext == "html" || ext == "htm" || ext == "css" || ext == "js" ||
           ext == "py" || ext == "c" || ext == "cc" || ext == "cpp" ||
           ext == "h" || ext == "hpp" || ext == "java" || ext == "md" ||
           ext == "log" || ext == "ini" || ext == "cfg";
}

static FileReport summarize(const std::vector<FileItem>& input) {
    FileReport r;
    const std::size_t n = std::min(input.size(), MAX_FILES);
    r.total = n;
    r.truncated = input.size() > MAX_FILES;
    for (std::size_t i = 0; i < n; ++i) {
        const auto& x = input[i];
        if (x.directory) ++r.folders;
        else {
            ++r.files;
            r.bytes += x.size;
        }
        if (x.readable) ++r.readable;
        else ++r.unreadable;
    }
    return r;
}

static std::vector<FileItem> search(const std::vector<FileItem>& input, const std::string& query) {
    std::vector<FileItem> out;
    const std::string q = lower(query);
    if (q.empty()) return out;
    for (const auto& x : input) {
        const std::string hay = lower(x.name + "\n" + x.path);
        if (hay.find(q) != std::string::npos) {
            out.push_back(x);
            if (out.size() >= MAX_FILES) break;
        }
    }
    return out;
}

static bool preview_allowed(const FileItem& item) {
    return !item.directory && item.readable && item.size <= MAX_PREVIEW_BYTES && is_text_preview_type(item.name);
}

// Permanent V68.x safety wall: inspection only.
extern "C" int ra681_file_write_blocked() { return ERR_BLOCKED; }
extern "C" int ra681_file_delete_blocked() { return ERR_BLOCKED; }
extern "C" int ra681_file_rename_blocked() { return ERR_BLOCKED; }
extern "C" int ra681_file_move_blocked() { return ERR_BLOCKED; }
extern "C" int ra681_file_copy_blocked() { return ERR_BLOCKED; }
extern "C" int ra681_file_mkdir_blocked() { return ERR_BLOCKED; }
extern "C" int ra681_file_restore_blocked() { return ERR_BLOCKED; }

extern "C" const char* ra681_file_engine_version() {
    return "68.1-file-intelligence-readonly";
}

} // namespace ra681
