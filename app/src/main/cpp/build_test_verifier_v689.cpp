// ROBOT ADMIN AI OS — V68.9 BUILD / TEST / VERIFIER CORE
// Android-only, offline-first, no production apply, no trading integration.

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

namespace ra689 {

enum class Verdict { PASS, FAIL, BLOCKED, TIMEOUT, UNKNOWN };

enum class CheckType {
    SYNTAX,
    COMPILE,
    UNIT,
    REGRESSION,
    DETERMINISTIC,
    BACKUP_HASH,
    RESOURCE_LIMIT
};

struct CheckResult {
    CheckType type = CheckType::SYNTAX;
    Verdict verdict = Verdict::UNKNOWN;
    std::string name;
    std::string evidence;
    std::uint64_t elapsed_ms = 0;
};

struct VerifyPolicy {
    std::uint64_t timeout_ms = 120000;
    std::uint64_t memory_limit_mb = 512;
    unsigned retry_limit = 1;
    bool require_regression = true;
    bool require_deterministic = true;
    bool require_backup_hash = true;
    bool owner_approval_required = true;
    bool auto_apply = false;
};

struct VerifyReport {
    Verdict verdict = Verdict::UNKNOWN;
    std::size_t passed = 0;
    std::size_t failed = 0;
    std::size_t blocked = 0;
    std::size_t timeout = 0;
    bool promotion_allowed = false;
    std::string kannada_reason;
};

static bool is_required(CheckType t, const VerifyPolicy& p) {
    if (t == CheckType::REGRESSION) return p.require_regression;
    if (t == CheckType::DETERMINISTIC) return p.require_deterministic;
    if (t == CheckType::BACKUP_HASH) return p.require_backup_hash;
    return true;
}

static VerifyReport verify(const std::vector<CheckResult>& checks,
                           const VerifyPolicy& policy,
                           bool owner_approved) {
    VerifyReport r;
    bool required_failure = false;

    for (const auto& c : checks) {
        switch (c.verdict) {
            case Verdict::PASS: ++r.passed; break;
            case Verdict::FAIL: ++r.failed; break;
            case Verdict::BLOCKED: ++r.blocked; break;
            case Verdict::TIMEOUT: ++r.timeout; break;
            default: break;
        }
        if (is_required(c.type, policy) && c.verdict != Verdict::PASS) {
            required_failure = true;
        }
    }

    if (required_failure) {
        r.verdict = Verdict::FAIL;
        r.kannada_reason = "ಅಗತ್ಯ build/test/verification checks ಎಲ್ಲವೂ PASS ಆಗಿಲ್ಲ; level promotion block ಮಾಡಲಾಗಿದೆ.";
        return r;
    }

    if (policy.owner_approval_required && !owner_approved) {
        r.verdict = Verdict::BLOCKED;
        r.kannada_reason = "ಎಲ್ಲ checks PASS ಆಗಿವೆ, ಆದರೆ owner approval ಇನ್ನೂ ಇಲ್ಲ; apply/level-up block ಮಾಡಲಾಗಿದೆ.";
        return r;
    }

    if (policy.auto_apply) {
        r.verdict = Verdict::BLOCKED;
        r.kannada_reason = "AUTO_APPLY safety policyಗೆ ವಿರುದ್ಧವಾಗಿದೆ; manual owner approval mode ಮಾತ್ರ ಅನುಮತಿಸಲಾಗಿದೆ.";
        return r;
    }

    r.verdict = Verdict::PASS;
    r.promotion_allowed = true;
    r.kannada_reason = "Build, test, verifier ಮತ್ತು owner approval gates PASS; controlled level promotionಗೆ eligible.";
    return r;
}

extern "C" const char* ra689_verifier_version() {
    return "68.9-build-test-verifier-safe";
}

extern "C" int ra689_auto_apply_enabled() {
    return 0;
}

} // namespace ra689
