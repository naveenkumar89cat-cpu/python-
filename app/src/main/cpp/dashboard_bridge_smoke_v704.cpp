#include <iostream>
#include "dashboard_bridge_v704.cpp"

int main() {
    using namespace ra704;
    DashboardState s;
    bool blocked = !safe_to_connect(s);
    s.backend_connected = true;
    s.autorun_ready = true;
    bool ready = safe_to_connect(s);
    bool pause_ok = apply_control(s, ControlAction::Pause);
    bool resume_ok = apply_control(s, ControlAction::Resume);
    bool approve_blocked = !apply_control(s, ControlAction::Approve, false);
    bool approve_ok = apply_control(s, ControlAction::Approve, true);
    bool stop_ok = apply_control(s, ControlAction::EmergencyStop);
    bool resume_blocked = !apply_control(s, ControlAction::Resume);
    bool locks = ra704_dashboard_write_trading_enabled() == 0 && ra704_dashboard_production_enable() == 0;
    std::cout << "R4_BLOCK_BEFORE=" << (blocked ? "PASS" : "FAIL") << '\n'
              << "R4_READY=" << (ready ? "PASS" : "FAIL") << '\n'
              << "R4_CONTROLS=" << ((pause_ok && resume_ok && approve_blocked && approve_ok && stop_ok && resume_blocked) ? "PASS" : "FAIL") << '\n'
              << "R4_LOCKS=" << (locks ? "PASS" : "FAIL") << '\n';
    return blocked && ready && pause_ok && resume_ok && approve_blocked && approve_ok && stop_ok && resume_blocked && locks ? 0 : 1;
}
