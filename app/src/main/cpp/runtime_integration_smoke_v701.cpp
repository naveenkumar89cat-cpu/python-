#include <iostream>
#include "runtime_integration_v701.cpp"

int main() {
    using namespace ra701;
    RuntimeGate g;
    bool blocked_before = !runtime_ready(g);
    g.foundation_pass = true;
    g.core_integrity_pass = true;
    g.recovery_pass = true;
    g.dashboard_pass = true;
    g.safe_auto_policy_pass = true;
    g.final_release_pass = true;
    g.trading_firewall_locked = true;
    g.emergency_stop_ready = true;
    g.owner_runtime_approved = true;
    bool ready = runtime_ready(g);
    bool locks = ra701_runtime_apply_enabled() == 0 &&
                 ra701_production_gate_open() == 0 &&
                 ra701_apk_build_enabled() == 0 &&
                 ra701_trading_write_enabled() == 0;
    std::cout << "R1_BLOCK_BEFORE=" << (blocked_before ? "PASS" : "FAIL") << '\n'
              << "R1_READY_GATE=" << (ready ? "PASS" : "FAIL") << '\n'
              << "R1_LOCKS=" << (locks ? "PASS" : "FAIL") << '\n';
    return blocked_before && ready && locks ? 0 : 1;
}
