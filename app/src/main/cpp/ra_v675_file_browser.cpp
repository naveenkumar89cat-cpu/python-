#include <android/asset_manager.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <string>
#include <vector>

namespace ra675 {
struct Entry {
    std::string source;
    std::string name;
    bool readable;
    bool is_dir;
};

static bool readable_file(const std::string& p) {
    struct stat st{};
    return stat(p.c_str(), &st) == 0 && S_ISREG(st.st_mode) && access(p.c_str(), R_OK) == 0;
}

static void list_dir_ro(const std::string& root, const char* source, std::vector<Entry>& out, size_t max_items) {
    if (root.empty() || out.size() >= max_items) return;
    DIR* d = opendir(root.c_str());
    if (!d) return;
    while (dirent* e = readdir(d)) {
        if (out.size() >= max_items) break;
        std::string n = e->d_name;
        if (n == "." || n == "..") continue;
        std::string p = root + "/" + n;
        struct stat st{};
        bool ok = stat(p.c_str(), &st) == 0;
        out.push_back({source, n, readable_file(p), ok && S_ISDIR(st.st_mode)});
    }
    closedir(d);
}

static void list_assets_ro(AAssetManager* mgr, const char* path, std::vector<Entry>& out, size_t max_items) {
    if (!mgr || out.size() >= max_items) return;
    AAssetDir* d = AAssetManager_openDir(mgr, path ? path : "");
    if (!d) return;
    while (const char* n = AAssetDir_getNextFileName(d)) {
        if (out.size() >= max_items) break;
        out.push_back({"APK_ASSET", n, true, false});
    }
    AAssetDir_close(d);
}

std::vector<Entry> scan(AAssetManager* mgr,
                        const std::string& internal_path,
                        const std::string& external_path,
                        size_t max_items) {
    if (max_items == 0) max_items = 64;
    if (max_items > 128) max_items = 128;
    std::vector<Entry> out;
    out.reserve(std::min<size_t>(max_items, 64));
    list_assets_ro(mgr, "v673", out, max_items);
    list_dir_ro(internal_path, "APP_INTERNAL", out, max_items);
    list_dir_ro(external_path, "APP_EXTERNAL", out, max_items);
    return out;
}

int write_allowed() { return 0; }
int delete_allowed() { return 0; }
int rename_allowed() { return 0; }
}

extern "C" const char* ra675_version() { return "67.5"; }
extern "C" int ra675_read_only() { return 1; }
extern "C" int ra675_write() { return -403; }
extern "C" int ra675_delete() { return -403; }
extern "C" int ra675_rename() { return -403; }
