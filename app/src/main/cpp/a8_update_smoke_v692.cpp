#include <iostream>
#include "controlled_update_v692.cpp"

int main() {
    using namespace ra692;
    PermanentLocks locks;
    UpdateEvidence e;
    OwnerGate owner;

    bool defaults_block = evaluate(e, owner, locks, false).state == UpdateState::CHECK_ONLY;

    e.version_known = true;
    e.signature_verified = true;
    e.checksum_verified = true;
    e.compatibility_pass = true;
    e.sandbox_pass = true;
    e.dry_run_pass = true;
    e.backup_verified = true;
    bool owner_gate = evaluate(e, owner, locks, false).state == UpdateState::READY_FOR_OWNER_REVIEW;

    owner.authenticated = true;
    owner.approved = true;
    owner.kannada_reason = "ಪರಿಶೀಲಿಸಿದ ಸುರಕ್ಷಿತ update proposal";
    e.canary_pass = true;
    e.health_verified = true;
    Decision d = evaluate(e, owner, locks, false);
    bool verified_no_apply = d.state == UpdateState::READY_FOR_OWNER_REVIEW &&
                             d.reason == "CONTROLLED_PLAN_VERIFIED_NO_APPLY";

    bool emergency = evaluate(e, owner, locks, true).state == UpdateState::PAUSED;
    bool permanent = ra692_auto_apply_enabled() == 0 &&
                     ra692_runtime_install_enabled() == 0 &&
                     ra692_production_gate_open() == 0;

    std::cout << "UPDATE_VERSION=" << ra692_version() << '\n';
    std::cout << "DEFAULT_SAFE_GATE=" << (defaults_block ? "PASS" : "FAIL") << '\n';
    std::cout << "OWNER_GATE=" << (owner_gate ? "PASS" : "FAIL") << '\n';
    std::cout << "VERIFIED_NO_APPLY=" << (verified_no_apply ? "PASS" : "FAIL") << '\n';
    std::cout << "EMERGENCY_STOP=" << (emergency ? "PASS" : "FAIL") << '\n';
    std::cout << "PERMANENT_LOCKS=" << (permanent ? "PASS" : "FAIL") << '\n';

    return defaults_block && owner_gate && verified_no_apply && emergency && permanent ? 0 : 1;
}
