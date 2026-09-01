// ROBOT ADMIN AI OS — V69.1 A7 SECURITY SMOKE TEST
#include "security_hardening_v691.cpp"
#include <iostream>

int main() {
    using namespace ra691;

    SecurityPolicy p;
    SessionState locked;
    Request apply;
    apply.scope = Scope::APPLY;
    const auto locked_result = evaluate(apply, locked, p);

    SessionState owner;
    owner.owner_authenticated = true;
    owner.biometric_or_pin_ok = true;
    owner.session_locked = false;
    owner.age_seconds = 10;

    Request needs_approval;
    needs_approval.scope = Scope::SECURITY_CHANGE;
    const auto approval_result = evaluate(needs_approval, owner, p);

    Request injected;
    injected.scope = Scope::READ;
    injected.prompt_injection_suspected = true;
    const auto injection_result = evaluate(injected, owner, p);

    Request escalation;
    escalation.scope = Scope::READ;
    escalation.attempts_authority_change = true;
    const auto escalation_result = evaluate(escalation, owner, p);

    Request safe_read;
    safe_read.scope = Scope::READ;
    const auto safe_result = evaluate(safe_read, owner, p);

    const bool locks =
        ra691_auto_apply_enabled() == 0 &&
        ra691_auto_restore_enabled() == 0 &&
        ra691_auto_delete_enabled() == 0 &&
        ra691_production_gate_open() == 0 &&
        ra691_authority_self_escalation_enabled() == 0 &&
        ra691_secrets_export_enabled() == 0;

    const bool ok =
        locked_result.decision == Decision::BLOCK &&
        approval_result.decision == Decision::REQUIRE_OWNER_APPROVAL &&
        injection_result.decision == Decision::BLOCK &&
        escalation_result.decision == Decision::BLOCK &&
        safe_result.decision == Decision::ALLOW &&
        locks;

    std::cout << "SECURITY_VERSION=" << ra691_security_version() << '\n';
    std::cout << "SESSION_GATE=" << (locked_result.decision == Decision::BLOCK ? "PASS" : "FAIL") << '\n';
    std::cout << "OWNER_APPROVAL_GATE=" << (approval_result.decision == Decision::REQUIRE_OWNER_APPROVAL ? "PASS" : "FAIL") << '\n';
    std::cout << "PROMPT_INJECTION_GATE=" << (injection_result.decision == Decision::BLOCK ? "PASS" : "FAIL") << '\n';
    std::cout << "AUTHORITY_ESCALATION_GATE=" << (escalation_result.decision == Decision::BLOCK ? "PASS" : "FAIL") << '\n';
    std::cout << "PERMANENT_LOCKS=" << (locks ? "PASS" : "FAIL") << '\n';
    return ok ? 0 : 1;
}
