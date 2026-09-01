// ROBOT ADMIN AI OS — V68.9 A4+A5 HOST-INDEPENDENT SMOKE TEST
// Verifier + backup checkpoint only. No APK/runtime/trading integration.

#include "build_test_verifier_v689.cpp"
#include "backup_checkpoint_v689.cpp"
#include <iostream>
#include <string>
#include <vector>

int main() {
    using namespace ra689;

    const std::vector<CheckResult> checks = {
        {CheckType::SYNTAX, Verdict::PASS, "syntax", "ok", 1},
        {CheckType::COMPILE, Verdict::PASS, "compile", "ok", 1},
        {CheckType::UNIT, Verdict::PASS, "unit", "ok", 1},
        {CheckType::REGRESSION, Verdict::PASS, "regression", "ok", 1},
        {CheckType::DETERMINISTIC, Verdict::PASS, "deterministic", "ok", 1},
        {CheckType::BACKUP_HASH, Verdict::PASS, "backup-hash", "ok", 1},
        {CheckType::RESOURCE_LIMIT, Verdict::PASS, "resource-limit", "ok", 1}
    };

    VerifyPolicy policy;
    policy.auto_apply = false;

    const VerifyReport waiting = verify(checks, policy, false);
    const VerifyReport approved = verify(checks, policy, true);

    const std::string hash(64, 'a');
    const std::vector<ra689_backup::CheckpointItem> items = {
        {"checkpoint.bundle", hash, hash, true, true},
        {"working-tree.tar", hash, hash, true, true}
    };

    const auto backup_waiting = ra689_backup::verify_checkpoint(items, true, true, false);
    const auto backup_ready = ra689_backup::verify_checkpoint(items, true, true, true);

    const bool safety =
        ra689_auto_apply_enabled() == 0 &&
        ra689_backup::ra689_auto_restore_enabled() == 0 &&
        ra689_backup::ra689_auto_delete_enabled() == 0 &&
        ra689_backup::ra689_production_gate_open() == 0;

    const bool gates =
        waiting.verdict == Verdict::BLOCKED && !waiting.promotion_allowed &&
        approved.verdict == Verdict::PASS && approved.promotion_allowed &&
        backup_waiting.state == ra689_backup::CheckpointState::BLOCKED &&
        !backup_waiting.safe_to_propose_apply &&
        backup_ready.state == ra689_backup::CheckpointState::READY &&
        backup_ready.safe_to_propose_apply;

    std::cout << "VERIFIER_VERSION=" << ra689_verifier_version() << '\n';
    std::cout << "BACKUP_VERSION=" << ra689_backup::ra689_backup_checkpoint_version() << '\n';
    std::cout << "OWNER_APPROVAL_GATE=" << (gates ? "PASS" : "FAIL") << '\n';
    std::cout << "PERMANENT_SAFETY_LOCKS=" << (safety ? "LOCKED" : "FAIL") << '\n';
    std::cout << "A4_A5_SMOKE=" << ((gates && safety) ? "PASS" : "FAIL") << '\n';

    return (gates && safety) ? 0 : 1;
}
