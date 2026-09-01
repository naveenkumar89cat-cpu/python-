// ROBOT ADMIN AI OS — V69.1 A7 SECURITY HARDENING CORE
// Android-only, offline-first, safety-first. No production apply and no authority escalation.

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <string>
#include <vector>

namespace ra691 {

enum class Decision { ALLOW, BLOCK, REQUIRE_OWNER_APPROVAL };
enum class Scope { READ, PLAN, TEST, BACKUP_VERIFY, APPLY, RESTORE, DELETE_, SECURITY_CHANGE };

struct SessionState {
    bool owner_authenticated = false;
    bool session_locked = true;
    bool biometric_or_pin_ok = false;
    std::uint64_t age_seconds = 0;
    std::uint64_t max_age_seconds = 900;
};

struct SecurityPolicy {
    bool auto_apply = false;
    bool auto_restore = false;
    bool auto_delete = false;
    bool production_gate_open = false;
    bool authority_self_escalation = false;
    bool secrets_export_allowed = false;
    bool owner_required_for_security_change = true;
};

struct Request {
    Scope scope = Scope::READ;
    bool owner_approved = false;
    bool prompt_injection_suspected = false;
    bool audit_chain_valid = true;
    bool touches_secrets = false;
    bool attempts_authority_change = false;
};

struct SecurityResult {
    Decision decision = Decision::BLOCK;
    std::string reason_kn;
};

static bool session_valid(const SessionState& s) {
    return s.owner_authenticated && s.biometric_or_pin_ok && !s.session_locked &&
           s.age_seconds <= s.max_age_seconds;
}

static bool high_risk_scope(Scope s) {
    return s == Scope::APPLY || s == Scope::RESTORE || s == Scope::DELETE_ ||
           s == Scope::SECURITY_CHANGE;
}

static SecurityResult evaluate(const Request& r,
                               const SessionState& session,
                               const SecurityPolicy& p) {
    SecurityResult out;

    if (r.prompt_injection_suspected) {
        out.reason_kn = "Prompt-injection ಅನುಮಾನ ಪತ್ತೆಯಾಗಿದೆ; request block ಮಾಡಲಾಗಿದೆ.";
        return out;
    }
    if (!r.audit_chain_valid) {
        out.reason_kn = "Audit integrity verify ಆಗಿಲ್ಲ; privileged action block ಮಾಡಲಾಗಿದೆ.";
        return out;
    }
    if (r.attempts_authority_change || p.authority_self_escalation) {
        out.reason_kn = "AI ತನ್ನ authority ಹೆಚ್ಚಿಸಲು ಅನುಮತಿ ಇಲ್ಲ; escalation block ಮಾಡಲಾಗಿದೆ.";
        return out;
    }
    if (r.touches_secrets && p.secrets_export_allowed) {
        out.reason_kn = "Secrets export policy safetyಗೆ ವಿರುದ್ಧವಾಗಿದೆ; action block ಮಾಡಲಾಗಿದೆ.";
        return out;
    }
    if (p.auto_apply || p.auto_restore || p.auto_delete || p.production_gate_open) {
        out.reason_kn = "Permanent safety locks violation; privileged action block ಮಾಡಲಾಗಿದೆ.";
        return out;
    }

    if (high_risk_scope(r.scope)) {
        if (!session_valid(session)) {
            out.reason_kn = "Valid owner PIN/biometric session ಇಲ್ಲ; high-risk action block ಮಾಡಲಾಗಿದೆ.";
            return out;
        }
        if (!r.owner_approved) {
            out.decision = Decision::REQUIRE_OWNER_APPROVAL;
            out.reason_kn = "Owner approval ಕಡ್ಡಾಯ; approval ಇಲ್ಲದೆ apply/restore/delete/security change ನಡೆಯುವುದಿಲ್ಲ.";
            return out;
        }
    }

    out.decision = Decision::ALLOW;
    out.reason_kn = "Security policy checks PASS; requested scope policy ಒಳಗೆ ಅನುಮತಿಸಲಾಗಿದೆ.";
    return out;
}

extern "C" const char* ra691_security_version() { return "69.1-security-hardening-safe"; }
extern "C" int ra691_auto_apply_enabled() { return 0; }
extern "C" int ra691_auto_restore_enabled() { return 0; }
extern "C" int ra691_auto_delete_enabled() { return 0; }
extern "C" int ra691_production_gate_open() { return 0; }
extern "C" int ra691_authority_self_escalation_enabled() { return 0; }
extern "C" int ra691_secrets_export_enabled() { return 0; }

} // namespace ra691
