// ROBOT ADMIN AI OS — V69.0 MASTER DASHBOARD STATE CORE
// Android-only, offline-first, approval-gated. No trading integration.
// This module models dashboard state only; it does not apply, restore, delete,
// publish, upload, deploy, or execute production actions.

#include <string>

namespace ra690 {

enum class Status { PASS, FAIL, BLOCKED, WAITING_APPROVAL, PAUSED, STOPPED, UNKNOWN };
enum class Risk { LOW, MEDIUM, HIGH };

struct SafetyLocks {
    bool auto_apply = false;
    bool auto_restore = false;
    bool auto_delete = false;
    bool auto_upload = false;
    bool auto_publish = false;
    bool live_orders = false;
    bool production_gate_open = false;
    bool owner_approval_required = true;
};

struct ProposalCard {
    std::string project_name;
    std::string current_level;
    std::string target_level;
    std::string bundle;
    std::string description_en;
    std::string description_kn;
    std::string plus;
    std::string minus;
    std::string expected_result;
    Risk risk = Risk::LOW;
    Status build = Status::UNKNOWN;
    Status tests = Status::UNKNOWN;
    Status verifier = Status::UNKNOWN;
    Status backup = Status::UNKNOWN;
    Status approval = Status::WAITING_APPROVAL;
    std::string last_stable_commit;
    std::string owner_reason_kn;
};

struct DashboardDecision {
    Status status = Status::BLOCKED;
    bool approve_enabled = false;
    bool reject_enabled = true;
    bool rollback_enabled = false;
    bool emergency_stop_enabled = true;
    std::string kannada_reason;
};

static bool all_preapproval_checks_pass(const ProposalCard& p) {
    return p.build == Status::PASS &&
           p.tests == Status::PASS &&
           p.verifier == Status::PASS &&
           p.backup == Status::PASS;
}

static bool reason_present(const std::string& reason) {
    for (unsigned char c : reason) {
        if (c > ' ') return true;
    }
    return false;
}

static DashboardDecision evaluate(const ProposalCard& p,
                                  const SafetyLocks& locks,
                                  bool owner_approved,
                                  bool emergency_stop) {
    DashboardDecision d;

    if (emergency_stop) {
        d.status = Status::STOPPED;
        d.kannada_reason = "GLOBAL EMERGENCY STOP ಸಕ್ರಿಯವಾಗಿದೆ; ಎಲ್ಲಾ controlled actions block ಮಾಡಲಾಗಿದೆ.";
        return d;
    }

    if (locks.auto_apply || locks.auto_restore || locks.auto_delete ||
        locks.auto_upload || locks.auto_publish || locks.live_orders ||
        locks.production_gate_open || !locks.owner_approval_required) {
        d.status = Status::BLOCKED;
        d.kannada_reason = "Permanent safety lock policy mismatch; approval/apply block ಮಾಡಲಾಗಿದೆ.";
        return d;
    }

    if (!all_preapproval_checks_pass(p)) {
        d.status = Status::BLOCKED;
        d.kannada_reason = "Build, Test, Verifier ಮತ್ತು Backup checks ಎಲ್ಲವೂ PASS ಆಗಿಲ್ಲ.";
        return d;
    }

    if (!reason_present(p.owner_reason_kn)) {
        d.status = Status::WAITING_APPROVAL;
        d.kannada_reason = "Approve ಮಾಡಲು ಕನ್ನಡ ಕಾರಣ ಕಡ್ಡಾಯ; reason box ಖಾಲಿಯಾಗಿದೆ.";
        return d;
    }

    d.approve_enabled = true;
    d.rollback_enabled = true;

    if (!owner_approved) {
        d.status = Status::WAITING_APPROVAL;
        d.kannada_reason = "ಎಲ್ಲ checks PASS; ಕನ್ನಡ ಕಾರಣ ದಾಖಲಾಗಿದೆ; owner approval ಕಾಯುತ್ತಿದೆ.";
        return d;
    }

    d.status = Status::PASS;
    d.kannada_reason = "Owner approval gate PASS; controlled apply proposal ಮಾತ್ರ eligible. AUTO_APPLY ಇನ್ನೂ OFF.";
    return d;
}

extern "C" int ra690_auto_apply_enabled() { return 0; }
extern "C" int ra690_auto_restore_enabled() { return 0; }
extern "C" int ra690_auto_delete_enabled() { return 0; }
extern "C" int ra690_production_gate_open() { return 0; }
extern "C" int ra690_live_orders_enabled() { return 0; }
extern "C" const char* ra690_dashboard_version() { return "69.0-master-dashboard-safe"; }

} // namespace ra690
