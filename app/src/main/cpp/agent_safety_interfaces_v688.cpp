// ROBOT ADMIN AI OS — V68.8 PRE-APK AGENT SAFETY INTERFACES
// Pure C++17, bounded-memory, fail-closed request/result/verification contracts.
// Android/APK integration is intentionally deferred while APK work is on hold.

#include <array>
#include <cstddef>
#include <cstdint>

namespace ra688_agent {

constexpr std::size_t kMaxAgentName = 32;
constexpr std::size_t kMaxPayload = 256;
constexpr std::size_t kMaxReason = 160;

enum class Action : std::uint8_t {
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

enum class Decision : std::uint8_t {
    Allow = 0,
    Block,
    NeedOwnerApproval,
    EmergencyStopped,
    Invalid
};

struct AgentRequest {
    std::uint64_t request_id{0};
    Action action{Action::Read};
    std::array<char, kMaxAgentName> agent{};
    std::array<char, kMaxPayload> payload{};
    bool owner_approved{false};
};

struct AgentResult {
    std::uint64_t request_id{0};
    int status{0};
    std::array<char, kMaxPayload> output{};
};

struct VerificationResult {
    Decision decision{Decision::Invalid};
    int code{-400};
    std::array<char, kMaxReason> reason{};
};

struct SafetySnapshot {
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

constexpr bool high_risk(Action a) {
    return a == Action::Write || a == Action::Delete || a == Action::Restore ||
           a == Action::AutoFix || a == Action::AutoApply ||
           a == Action::AutoUpdateApply || a == Action::RuntimeInstall ||
           a == Action::LiveOrders || a == Action::Production;
}

constexpr bool enabled(const SafetySnapshot& s, Action a) {
    switch (a) {
        case Action::Read: return s.read;
        case Action::Analyze: return s.analyze;
        case Action::Write: return s.write;
        case Action::Delete: return s.remove;
        case Action::Restore: return s.restore;
        case Action::AutoFix: return s.auto_fix;
        case Action::AutoApply: return s.auto_apply;
        case Action::AutoUpdateApply: return s.auto_update_apply;
        case Action::RuntimeInstall: return s.runtime_install;
        case Action::LiveOrders: return s.live_orders;
        case Action::Production: return s.production_gate;
    }
    return false;
}

constexpr Decision verify(const SafetySnapshot& s, const AgentRequest& r) {
    if (r.request_id == 0) return Decision::Invalid;
    if (s.emergency_stop && r.action != Action::Read) return Decision::EmergencyStopped;
    if (!enabled(s, r.action)) return Decision::Block;
    if (high_risk(r.action) && !r.owner_approved) return Decision::NeedOwnerApproval;
    return Decision::Allow;
}

constexpr int decision_code(Decision d) {
    switch (d) {
        case Decision::Allow: return 0;
        case Decision::Block: return -403;
        case Decision::NeedOwnerApproval: return -401;
        case Decision::EmergencyStopped: return -503;
        case Decision::Invalid: return -400;
    }
    return -400;
}

constexpr bool default_contract_fail_closed() {
    constexpr SafetySnapshot s{};
    AgentRequest read{};
    read.request_id = 1;
    read.action = Action::Read;

    AgentRequest analyze{};
    analyze.request_id = 2;
    analyze.action = Action::Analyze;

    AgentRequest write{};
    write.request_id = 3;
    write.action = Action::Write;
    write.owner_approved = true;

    AgentRequest live{};
    live.request_id = 4;
    live.action = Action::LiveOrders;
    live.owner_approved = true;

    return verify(s, read) == Decision::Allow &&
           verify(s, analyze) == Decision::Allow &&
           verify(s, write) == Decision::Block &&
           verify(s, live) == Decision::Block;
}

constexpr bool owner_gate_contract() {
    SafetySnapshot s{};
    s.write = true;

    AgentRequest r{};
    r.request_id = 10;
    r.action = Action::Write;
    r.owner_approved = false;
    const bool denied = verify(s, r) == Decision::NeedOwnerApproval;

    r.owner_approved = true;
    const bool allowed = verify(s, r) == Decision::Allow;
    return denied && allowed;
}

constexpr bool emergency_stop_contract() {
    SafetySnapshot s{};
    s.emergency_stop = true;

    AgentRequest read{};
    read.request_id = 20;
    read.action = Action::Read;

    AgentRequest analyze{};
    analyze.request_id = 21;
    analyze.action = Action::Analyze;

    return verify(s, read) == Decision::Allow &&
           verify(s, analyze) == Decision::EmergencyStopped;
}

static_assert(sizeof(AgentRequest) <= 320, "AgentRequest must remain bounded");
static_assert(sizeof(AgentResult) <= 288, "AgentResult must remain bounded");
static_assert(sizeof(VerificationResult) <= 192, "VerificationResult must remain bounded");
static_assert(default_contract_fail_closed(), "default agent contract must fail closed");
static_assert(owner_gate_contract(), "owner approval must remain mandatory for enabled high-risk actions");
static_assert(emergency_stop_contract(), "emergency stop contract must fail closed");

extern "C" const char* ra688_agent_interfaces_version() {
    return "68.8-preapk-agent-safety-interfaces-source";
}

extern "C" int ra688_agent_interfaces_selftest() {
    return default_contract_fail_closed() && owner_gate_contract() && emergency_stop_contract() ? 0 : 1;
}

} // namespace ra688_agent

#ifdef RA688_AGENT_INTERFACES_STANDALONE_TEST
int main() { return ra688_agent::ra688_agent_interfaces_selftest(); }
#endif
