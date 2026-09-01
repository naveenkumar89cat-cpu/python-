// ROBOT ADMIN AI OS — V69.0 A6 MASTER DASHBOARD SMOKE TEST
#include "master_dashboard_v690.cpp"
#include <iostream>

int main() {
    ra690::SafetyLocks locks;
    ra690::ProposalCard p;
    p.project_name = "Robot Admin AI OS";
    p.current_level = "V68.9";
    p.target_level = "V69.0";
    p.bundle = "A6 Master Dashboard";
    p.plus = "Approval, verifier, backup and emergency-stop visibility";
    p.minus = "Runtime UI integration not yet applied";
    p.build = ra690::Status::PASS;
    p.tests = ra690::Status::PASS;
    p.verifier = ra690::Status::PASS;
    p.backup = ra690::Status::PASS;

    const auto no_reason = ra690::evaluate(p, locks, false, false);
    const bool blank_reason_blocked =
        no_reason.status == ra690::Status::WAITING_APPROVAL &&
        !no_reason.approve_enabled;

    p.owner_reason_kn = "ಪರೀಕ್ಷೆಗಳು PASS ಆಗಿವೆ; controlled reviewಗಾಗಿ ಅನುಮೋದನೆ ಪರಿಶೀಲನೆ.";
    const auto waiting = ra690::evaluate(p, locks, false, false);
    const bool approval_waiting =
        waiting.status == ra690::Status::WAITING_APPROVAL &&
        waiting.approve_enabled;

    const auto approved = ra690::evaluate(p, locks, true, false);
    const bool controlled_eligible =
        approved.status == ra690::Status::PASS &&
        approved.approve_enabled;

    const auto stopped = ra690::evaluate(p, locks, true, true);
    const bool emergency_stop = stopped.status == ra690::Status::STOPPED;

    const bool permanent_locks =
        ra690::ra690_auto_apply_enabled() == 0 &&
        ra690::ra690_auto_restore_enabled() == 0 &&
        ra690::ra690_auto_delete_enabled() == 0 &&
        ra690::ra690_production_gate_open() == 0 &&
        ra690::ra690_live_orders_enabled() == 0;

    std::cout << "DASHBOARD_VERSION=" << ra690::ra690_dashboard_version() << '\n';
    std::cout << "BLANK_REASON_GATE=" << (blank_reason_blocked ? "PASS" : "FAIL") << '\n';
    std::cout << "OWNER_APPROVAL_GATE=" << (approval_waiting ? "PASS" : "FAIL") << '\n';
    std::cout << "CONTROLLED_ELIGIBILITY=" << (controlled_eligible ? "PASS" : "FAIL") << '\n';
    std::cout << "EMERGENCY_STOP=" << (emergency_stop ? "PASS" : "FAIL") << '\n';
    std::cout << "PERMANENT_LOCKS=" << (permanent_locks ? "PASS" : "FAIL") << '\n';

    return blank_reason_blocked && approval_waiting && controlled_eligible &&
           emergency_stop && permanent_locks ? 0 : 1;
}
