// ROBOT ADMIN AI OS — V68.8 PRE-APK VERIFICATION + EXECUTOR GATE
// Pure C++17. This bundle intentionally excludes Android/APK integration.
// Build this translation unit alone; it owns the included V68.8 pre-APK modules.

#include "preapk_hardening_v688.cpp"
#include "agent_safety_interfaces_v688.cpp"

namespace ra688_executor {

using ra688_agent::Action;
using ra688_agent::AgentRequest;
using ra688_agent::Decision;
using ra688_agent::SafetySnapshot;

struct GateResult {
    Decision decision{Decision::Invalid};
    int code{-400};
    bool executable{false};
};

constexpr ra688_hardening::Capability capability_for(Action a) {
    switch (a) {
        case Action::Read: return ra688_hardening::Capability::Read;
        case Action::Analyze: return ra688_hardening::Capability::Analyze;
        case Action::Write: return ra688_hardening::Capability::Write;
        case Action::Delete: return ra688_hardening::Capability::Delete;
        case Action::Restore: return ra688_hardening::Capability::Restore;
        case Action::AutoFix: return ra688_hardening::Capability::AutoFix;
        case Action::AutoApply: return ra688_hardening::Capability::AutoApply;
        case Action::AutoUpdateApply: return ra688_hardening::Capability::AutoUpdateApply;
        case Action::RuntimeInstall: return ra688_hardening::Capability::RuntimeInstall;
        case Action::LiveOrders: return ra688_hardening::Capability::LiveOrders;
        case Action::Production: return ra688_hardening::Capability::Production;
    }
    return ra688_hardening::Capability::Read;
}

constexpr ra688_hardening::PolicyState policy_from(const SafetySnapshot& s) {
    ra688_hardening::PolicyState p{};
    p.read = s.read;
    p.analyze = s.analyze;
    p.write = s.write;
    p.remove = s.remove;
    p.restore = s.restore;
    p.auto_fix = s.auto_fix;
    p.auto_apply = s.auto_apply;
    p.auto_update_apply = s.auto_update_apply;
    p.runtime_install = s.runtime_install;
    p.live_orders = s.live_orders;
    p.production_gate = s.production_gate;
    p.emergency_stop = s.emergency_stop;
    return p;
}

constexpr GateResult evaluate(const SafetySnapshot& s, const AgentRequest& r) {
    const Decision verification = ra688_agent::verify(s, r);
    if (verification != Decision::Allow) {
        return {verification, ra688_agent::decision_code(verification), false};
    }

    const int kernel = ra688_hardening::authorize(
        policy_from(s), capability_for(r.action), r.owner_approved);
    if (kernel != ra688_hardening::kAllowed) {
        return {kernel == ra688_hardening::kEmergencyStop ? Decision::EmergencyStopped : Decision::Block,
                kernel, false};
    }
    return {Decision::Allow, 0, true};
}

constexpr AgentRequest request(std::uint64_t id, Action action, bool owner = false) {
    AgentRequest r{};
    r.request_id = id;
    r.action = action;
    r.owner_approved = owner;
    return r;
}

constexpr bool negative_regression() {
    SafetySnapshot s{};
    if (!evaluate(s, request(1, Action::Read)).executable) return false;
    if (!evaluate(s, request(2, Action::Analyze)).executable) return false;
    if (evaluate(s, request(3, Action::Write, true)).code != -403) return false;
    if (evaluate(s, request(4, Action::Delete, true)).code != -403) return false;
    if (evaluate(s, request(5, Action::Restore, true)).code != -403) return false;
    if (evaluate(s, request(6, Action::AutoApply, true)).code != -403) return false;
    if (evaluate(s, request(7, Action::AutoUpdateApply, true)).code != -403) return false;
    if (evaluate(s, request(8, Action::RuntimeInstall, true)).code != -403) return false;
    if (evaluate(s, request(9, Action::LiveOrders, true)).code != -403) return false;
    if (evaluate(s, request(10, Action::Production, true)).code != -403) return false;
    if (evaluate(s, request(0, Action::Read)).code != -400) return false;

    s.write = true;
    if (evaluate(s, request(11, Action::Write, false)).code != -401) return false;
    if (!evaluate(s, request(12, Action::Write, true)).executable) return false;

    s.emergency_stop = true;
    if (evaluate(s, request(13, Action::Analyze)).code != -503) return false;
    if (!evaluate(s, request(14, Action::Read)).executable) return false;
    return true;
}

static_assert(sizeof(GateResult) <= 16, "GateResult must remain bounded");
static_assert(negative_regression(), "verification/executor gate must fail closed");

extern "C" const char* ra688_executor_gate_version() {
    return "68.8-preapk-verification-executor-gate-source";
}

extern "C" int ra688_executor_gate_selftest() {
    return ra688_hardening::ra688_hardening_selftest() == 0 &&
           ra688_agent::ra688_agent_interfaces_selftest() == 0 &&
           negative_regression() ? 0 : 1;
}

} // namespace ra688_executor

#ifdef RA688_EXECUTOR_GATE_STANDALONE_TEST
int main() { return ra688_executor::ra688_executor_gate_selftest(); }
#endif
