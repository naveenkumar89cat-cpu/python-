// ROBOT ADMIN AI OS — V68.9 BACKUP CHECKPOINT / ROLLBACK PROPOSAL
// Safe metadata engine only. Does not create, delete, restore, or apply backups.

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <string>
#include <vector>

namespace ra689_backup {

enum class CheckpointState {
    READY,
    INCOMPLETE,
    HASH_MISMATCH,
    MISSING,
    BLOCKED
};

struct CheckpointItem {
    std::string path;
    std::string expected_sha256;
    std::string actual_sha256;
    bool exists = false;
    bool readable = false;
};

struct CheckpointReport {
    CheckpointState state = CheckpointState::INCOMPLETE;
    std::size_t total = 0;
    std::size_t verified = 0;
    std::size_t failed = 0;
    bool restore_simulation_pass = false;
    bool last_known_good_recorded = false;
    bool safe_to_propose_apply = false;
    std::string kannada_reason;
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

static bool item_verified(const CheckpointItem& item) {
    if (!item.exists || !item.readable) return false;
    if (!valid_sha256(item.expected_sha256) || !valid_sha256(item.actual_sha256)) return false;
    return normalize_hex(item.expected_sha256) == normalize_hex(item.actual_sha256);
}

static CheckpointReport verify_checkpoint(const std::vector<CheckpointItem>& items,
                                          bool restore_simulation_pass,
                                          bool last_known_good_recorded,
                                          bool owner_approved) {
    CheckpointReport r;
    r.total = items.size();
    r.restore_simulation_pass = restore_simulation_pass;
    r.last_known_good_recorded = last_known_good_recorded;

    for (const auto& item : items) {
        if (item_verified(item)) ++r.verified;
        else ++r.failed;
    }

    if (items.empty() || r.failed != 0) {
        r.state = CheckpointState::INCOMPLETE;
        r.kannada_reason = "Backup checkpoint ಪೂರ್ಣವಾಗಿ verify ಆಗಿಲ್ಲ; change/apply proposal block ಮಾಡಲಾಗಿದೆ.";
        return r;
    }

    if (!restore_simulation_pass) {
        r.state = CheckpointState::BLOCKED;
        r.kannada_reason = "Restore simulation PASS ಆಗಿಲ್ಲ; rollback readiness confirm ಆಗುವವರೆಗೆ ಮುಂದುವರಿಸಲಾಗುವುದಿಲ್ಲ.";
        return r;
    }

    if (!last_known_good_recorded) {
        r.state = CheckpointState::BLOCKED;
        r.kannada_reason = "Last-known-good version ದಾಖಲಾಗಿಲ್ಲ; safe rollback point ಕಡ್ಡಾಯ.";
        return r;
    }

    if (!owner_approved) {
        r.state = CheckpointState::BLOCKED;
        r.kannada_reason = "Backup verified ಆಗಿದೆ, ಆದರೆ owner approval ಇಲ್ಲ; apply/restore action ಅನುಮತಿಸಲಾಗಿಲ್ಲ.";
        return r;
    }

    r.state = CheckpointState::READY;
    r.safe_to_propose_apply = true;
    r.kannada_reason = "Backup hash, restore simulation ಮತ್ತು last-known-good checks PASS; controlled apply proposalಗೆ ready.";
    return r;
}

// Permanent destructive-action locks.
extern "C" int ra689_auto_restore_enabled() { return 0; }
extern "C" int ra689_auto_delete_enabled() { return 0; }
extern "C" int ra689_production_gate_open() { return 0; }
extern "C" const char* ra689_backup_checkpoint_version() {
    return "68.9-backup-checkpoint-readonly";
}

} // namespace ra689_backup
