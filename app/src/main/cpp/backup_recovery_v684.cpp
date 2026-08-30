// ROBOT ADMIN AI OS — V68.4 BACKUP + RECOVERY INTELLIGENCE
// Android-only, offline-first, read-only. APK integration deferred.

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <string>
#include <vector>

namespace ra684 {

constexpr int ERR_BLOCKED = -403;
constexpr std::size_t MAX_BACKUPS = 512;

enum class BackupState {
    VERIFIED,
    UNVERIFIED,
    HASH_MISMATCH,
    MISSING,
    CORRUPT,
    UNKNOWN
};

struct BackupItem {
    std::string id;
    std::string path;
    std::string expected_sha256;
    std::string actual_sha256;
    std::uint64_t size = 0;
    bool exists = false;
    bool readable = false;
    BackupState state = BackupState::UNKNOWN;
};

struct BackupReport {
    std::size_t total = 0;
    std::size_t verified = 0;
    std::size_t unverified = 0;
    std::size_t mismatch = 0;
    std::size_t missing = 0;
    std::size_t corrupt = 0;
    std::size_t unknown = 0;
    bool truncated = false;
};

static std::string normalize_hex(std::string s) {
    s.erase(std::remove_if(s.begin(), s.end(), [](unsigned char c) {
        return std::isspace(c) != 0;
    }), s.end());
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return s;
}

static bool valid_sha256(const std::string& value) {
    const std::string s = normalize_hex(value);
    if (s.size() != 64) return false;
    return std::all_of(s.begin(), s.end(), [](unsigned char c) {
        return std::isxdigit(c) != 0;
    });
}

static BackupState classify(const BackupItem& item) {
    if (!item.exists) return BackupState::MISSING;
    if (!item.readable) return BackupState::CORRUPT;
    if (item.expected_sha256.empty() || item.actual_sha256.empty()) return BackupState::UNVERIFIED;
    if (!valid_sha256(item.expected_sha256) || !valid_sha256(item.actual_sha256)) return BackupState::CORRUPT;
    return normalize_hex(item.expected_sha256) == normalize_hex(item.actual_sha256)
        ? BackupState::VERIFIED : BackupState::HASH_MISMATCH;
}

static BackupReport summarize(std::vector<BackupItem>& items) {
    BackupReport r;
    const std::size_t n = std::min(items.size(), MAX_BACKUPS);
    r.total = n;
    r.truncated = items.size() > MAX_BACKUPS;
    for (std::size_t i = 0; i < n; ++i) {
        items[i].state = classify(items[i]);
        switch (items[i].state) {
            case BackupState::VERIFIED: ++r.verified; break;
            case BackupState::UNVERIFIED: ++r.unverified; break;
            case BackupState::HASH_MISMATCH: ++r.mismatch; break;
            case BackupState::MISSING: ++r.missing; break;
            case BackupState::CORRUPT: ++r.corrupt; break;
            default: ++r.unknown; break;
        }
    }
    return r;
}

static const char* state_name(BackupState s) {
    switch (s) {
        case BackupState::VERIFIED: return "VERIFIED";
        case BackupState::UNVERIFIED: return "UNVERIFIED";
        case BackupState::HASH_MISMATCH: return "HASH_MISMATCH";
        case BackupState::MISSING: return "MISSING";
        case BackupState::CORRUPT: return "CORRUPT";
        default: return "UNKNOWN";
    }
}

// Recovery actions remain approval-gated and disabled in V68.x.
extern "C" int ra684_backup_create_blocked() { return ERR_BLOCKED; }
extern "C" int ra684_restore_blocked() { return ERR_BLOCKED; }
extern "C" int ra684_delete_backup_blocked() { return ERR_BLOCKED; }
extern "C" int ra684_apply_rollback_blocked() { return ERR_BLOCKED; }
extern "C" int ra684_repair_blocked() { return ERR_BLOCKED; }
extern "C" const char* ra684_backup_engine_version() { return "68.4-backup-recovery-readonly"; }

} // namespace ra684
