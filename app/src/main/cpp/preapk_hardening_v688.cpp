// ROBOT ADMIN AI OS — V68.8 PRE-APK CORE HARDENING
// Pure C++17 safety kernel. Android integration intentionally deferred while APK work is on hold.

#include <cstdint>

namespace ra688_hardening {

enum class Capability : std::uint8_t {
    Read = 0,
    Analyze,
    Write,
    Delete,
    Restore,
    AutoFix,
    AutoApply,
    AutoUpdateApply,
    RuntimeInstall,
    LiveOrders,
    Production
};

struct PolicyState {
    bool read{true};
    bool analyze{true};
    bool write{false};
    bool remove{false};
    bool restore{false};
    bool auto_fix{false};
    bool auto_apply{false};
    bool auto_update_apply{false};
    bool runtime_install{false};
    bool live_orders{false};
    bool production_gate{false};
    bool emergency_stop{false};
};

constexpr int kAllowed = 0;
constexpr int kBlocked = -403;
constexpr int kEmergencyStop = -503;

constexpr bool is_high_risk(Capability c) {
    return c == Capability::Write || c == Capability::Delete ||
           c == Capability::Restore || c == Capability::AutoFix ||
           c == Capability::AutoApply || c == Capability::AutoUpdateApply ||
           c == Capability::RuntimeInstall || c == Capability::LiveOrders ||
           c == Capability::Production;
}

constexpr bool enabled(const PolicyState& s, Capability c) {
    switch (c) {
        case Capability::Read: return s.read;
        case Capability::Analyze: return s.analyze;
        case Capability::Write: return s.write;
        case Capability::Delete: return s.remove;
        case Capability::Restore: return s.restore;
        case Capability::AutoFix: return s.auto_fix;
        case Capability::AutoApply: return s.auto_apply;
        case Capability::AutoUpdateApply: return s.auto_update_apply;
        case Capability::RuntimeInstall: return s.runtime_install;
        case Capability::LiveOrders: return s.live_orders;
        case Capability::Production: return s.production_gate;
    }
    return false;
}

constexpr int authorize(const PolicyState& s, Capability c, bool owner_approved) {
    if (s.emergency_stop && c != Capability::Read) return kEmergencyStop;
    if (!enabled(s, c)) return kBlocked;
    if (is_high_risk(c) && !owner_approved) return kBlocked;
    return kAllowed;
}

constexpr bool default_policy_locked() {
    constexpr PolicyState s{};
    return authorize(s, Capability::Read, false) == kAllowed &&
           authorize(s, Capability::Analyze, false) == kAllowed &&
           authorize(s, Capability::Write, true) == kBlocked &&
           authorize(s, Capability::Delete, true) == kBlocked &&
           authorize(s, Capability::Restore, true) == kBlocked &&
           authorize(s, Capability::AutoFix, true) == kBlocked &&
           authorize(s, Capability::AutoApply, true) == kBlocked &&
           authorize(s, Capability::AutoUpdateApply, true) == kBlocked &&
           authorize(s, Capability::RuntimeInstall, true) == kBlocked &&
           authorize(s, Capability::LiveOrders, true) == kBlocked &&
           authorize(s, Capability::Production, true) == kBlocked;
}

constexpr bool owner_gate_required_when_explicitly_enabled() {
    PolicyState s{};
    s.write = true;
    s.restore = true;
    return authorize(s, Capability::Write, false) == kBlocked &&
           authorize(s, Capability::Write, true) == kAllowed &&
           authorize(s, Capability::Restore, false) == kBlocked &&
           authorize(s, Capability::Restore, true) == kAllowed;
}

constexpr bool emergency_stop_fail_closed() {
    PolicyState s{};
    s.write = true;
    s.emergency_stop = true;
    return authorize(s, Capability::Read, false) == kAllowed &&
           authorize(s, Capability::Analyze, false) == kEmergencyStop &&
           authorize(s, Capability::Write, true) == kEmergencyStop;
}

static_assert(default_policy_locked(), "default safety policy must fail closed");
static_assert(owner_gate_required_when_explicitly_enabled(), "owner gate must remain mandatory");
static_assert(emergency_stop_fail_closed(), "emergency stop must fail closed");

extern "C" const char* ra688_hardening_version() {
    return "68.8-preapk-core-hardening-source";
}

extern "C" int ra688_hardening_selftest() {
    return default_policy_locked() &&
           owner_gate_required_when_explicitly_enabled() &&
           emergency_stop_fail_closed() ? 0 : 1;
}

} // namespace ra688_hardening

#ifdef RA688_HARDENING_STANDALONE_TEST
int main() { return ra688_hardening::ra688_hardening_selftest(); }
#endif
