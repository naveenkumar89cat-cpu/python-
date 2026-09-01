// Robot Admin AI OS A8 / V69.2 - Controlled Update Safety Planner
// Planning/gating only. It cannot download, install, apply, restore, delete, or publish updates.
#include <string>
#include <vector>

namespace ra692 {

enum class UpdateState {
    BLOCKED,
    CHECK_ONLY,
    READY_FOR_OWNER_REVIEW,
    PAUSED
};

struct UpdateEvidence {
    bool version_known = false;
    bool signature_verified = false;
    bool checksum_verified = false;
    bool compatibility_pass = false;
    bool sandbox_pass = false;
    bool dry_run_pass = false;
    bool backup_verified = false;
    bool canary_pass = false;
    bool health_verified = false;
};

struct OwnerGate {
    bool authenticated = false;
    bool approved = false;
    std::string kannada_reason;
};

struct PermanentLocks {
    bool auto_apply = false;
    bool auto_restore = false;
    bool auto_delete = false;
    bool auto_publish = false;
    bool runtime_install = false;
    bool production_gate = false;
};

struct Decision {
    UpdateState state = UpdateState::BLOCKED;
    std::string reason;
};

static bool has_reason(const OwnerGate& owner) {
    return !owner.kannada_reason.empty();
}

static bool locks_safe(const PermanentLocks& locks) {
    return !locks.auto_apply && !locks.auto_restore && !locks.auto_delete &&
           !locks.auto_publish && !locks.runtime_install && !locks.production_gate;
}

static bool pre_owner_evidence_pass(const UpdateEvidence& e) {
    return e.version_known && e.signature_verified && e.checksum_verified &&
           e.compatibility_pass && e.sandbox_pass && e.dry_run_pass &&
           e.backup_verified;
}

Decision evaluate(const UpdateEvidence& e, const OwnerGate& owner,
                  const PermanentLocks& locks, bool emergency_stop) {
    if (emergency_stop) return {UpdateState::PAUSED, "EMERGENCY_STOP"};
    if (!locks_safe(locks)) return {UpdateState::BLOCKED, "PERMANENT_LOCK_CHANGED"};
    if (!pre_owner_evidence_pass(e)) return {UpdateState::CHECK_ONLY, "EVIDENCE_INCOMPLETE"};
    if (!owner.authenticated || !owner.approved || !has_reason(owner))
        return {UpdateState::READY_FOR_OWNER_REVIEW, "OWNER_APPROVAL_REQUIRED"};

    // Even after approval this module remains a planner. No update is applied here.
    if (!e.canary_pass || !e.health_verified)
        return {UpdateState::READY_FOR_OWNER_REVIEW, "CANARY_HEALTH_EVIDENCE_REQUIRED"};
    return {UpdateState::READY_FOR_OWNER_REVIEW, "CONTROLLED_PLAN_VERIFIED_NO_APPLY"};
}

extern "C" const char* ra692_version() { return "69.2-controlled-update-safe"; }
extern "C" int ra692_auto_apply_enabled() { return 0; }
extern "C" int ra692_runtime_install_enabled() { return 0; }
extern "C" int ra692_production_gate_open() { return 0; }

} // namespace ra692
