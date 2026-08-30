// ROBOT ADMIN AI OS — V68.5 ERROR + DIAGNOSTICS
// Android-only, offline-first, read-only. APK integration deferred.

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <string>
#include <vector>

namespace ra685 {

constexpr int ERR_BLOCKED = -403;

enum class Severity { INFO, WARNING, ERROR, CRITICAL };

enum class Code {
    OK = 0,
    FILE_MISSING = 1001,
    FILE_UNREADABLE = 1002,
    HASH_MISMATCH = 1101,
    CSV_INVALID = 1201,
    TASK_INVALID = 1301,
    PERMISSION_BLOCKED = 1403,
    MODULE_NOT_READY = 1501,
    MODULE_FAILED = 1502,
    RESOURCE_LIMIT = 1601,
    UNKNOWN = 1999
};

struct ModuleHealth {
    std::string name;
    bool ready = false;
    bool passed = false;
    Code code = Code::OK;
    std::string detail;
};

struct DiagnosticReport {
    std::size_t total = 0;
    std::size_t ready = 0;
    std::size_t passed = 0;
    std::size_t warnings = 0;
    std::size_t errors = 0;
    std::size_t critical = 0;
    bool system_ok = true;
    Code primary_code = Code::OK;
    std::string primary_reason = "NO ERROR";
};

static const char* code_name(Code c) {
    switch (c) {
        case Code::OK: return "OK";
        case Code::FILE_MISSING: return "FILE_MISSING";
        case Code::FILE_UNREADABLE: return "FILE_UNREADABLE";
        case Code::HASH_MISMATCH: return "HASH_MISMATCH";
        case Code::CSV_INVALID: return "CSV_INVALID";
        case Code::TASK_INVALID: return "TASK_INVALID";
        case Code::PERMISSION_BLOCKED: return "PERMISSION_BLOCKED";
        case Code::MODULE_NOT_READY: return "MODULE_NOT_READY";
        case Code::MODULE_FAILED: return "MODULE_FAILED";
        case Code::RESOURCE_LIMIT: return "RESOURCE_LIMIT";
        default: return "UNKNOWN";
    }
}

static Severity severity(Code c) {
    switch (c) {
        case Code::OK: return Severity::INFO;
        case Code::PERMISSION_BLOCKED: return Severity::INFO;
        case Code::CSV_INVALID:
        case Code::TASK_INVALID:
        case Code::RESOURCE_LIMIT: return Severity::WARNING;
        case Code::FILE_MISSING:
        case Code::FILE_UNREADABLE:
        case Code::MODULE_NOT_READY: return Severity::ERROR;
        case Code::HASH_MISMATCH:
        case Code::MODULE_FAILED: return Severity::CRITICAL;
        default: return Severity::ERROR;
    }
}

static std::string recommended_action(Code c) {
    switch (c) {
        case Code::OK: return "NONE";
        case Code::FILE_MISSING: return "CHECK FILE OR ASSET PATH";
        case Code::FILE_UNREADABLE: return "CHECK READ PERMISSION";
        case Code::HASH_MISMATCH: return "STOP AND VERIFY TRUSTED SOURCE";
        case Code::CSV_INVALID: return "REVIEW INVALID CSV ROWS";
        case Code::TASK_INVALID: return "REVIEW TASK FORMAT";
        case Code::PERMISSION_BLOCKED: return "OWNER APPROVAL REQUIRED";
        case Code::MODULE_NOT_READY: return "CHECK MODULE INITIALIZATION";
        case Code::MODULE_FAILED: return "ROLL BACK TO KNOWN GOOD CHECKPOINT";
        case Code::RESOURCE_LIMIT: return "REDUCE INPUT SIZE";
        default: return "MANUAL REVIEW REQUIRED";
    }
}

static DiagnosticReport summarize(const std::vector<ModuleHealth>& modules) {
    DiagnosticReport r;
    r.total = modules.size();
    int highest = -1;
    for (const auto& m : modules) {
        if (m.ready) ++r.ready;
        if (m.passed) ++r.passed;
        const auto s = severity(m.code);
        if (s == Severity::WARNING) ++r.warnings;
        else if (s == Severity::ERROR) ++r.errors;
        else if (s == Severity::CRITICAL) ++r.critical;

        int rank = s == Severity::CRITICAL ? 3 : s == Severity::ERROR ? 2 : s == Severity::WARNING ? 1 : 0;
        if (rank > highest) {
            highest = rank;
            r.primary_code = m.code;
            r.primary_reason = m.detail.empty() ? code_name(m.code) : m.detail;
        }
    }
    r.system_ok = (r.errors == 0 && r.critical == 0);
    return r;
}

extern "C" int ra685_auto_fix_blocked() { return ERR_BLOCKED; }
extern "C" int ra685_repair_blocked() { return ERR_BLOCKED; }
extern "C" int ra685_apply_fix_blocked() { return ERR_BLOCKED; }
extern "C" int ra685_restart_service_blocked() { return ERR_BLOCKED; }
extern "C" const char* ra685_error_engine_version() { return "68.5-error-diagnostics-readonly"; }

} // namespace ra685
