#include <iostream>
#include "offline_ai_router_v693.cpp"

int main() {
    using namespace ra693;
    Policy policy;
    OwnerGate owner;

    Command low{"status", Language::BILINGUAL, Risk::LOW};
    Command high{"change protected setting", Language::KANNADA, Risk::HIGH};

    const bool low_ready = route(low, policy, owner).state == State::LOCAL_READY;
    const bool high_gate = route(high, policy, owner).state == State::REVIEW_REQUIRED;

    owner.authenticated = true;
    owner.approved = true;
    owner.kannada_reason = "ಪರಿಶೀಲಿಸಿದ ಪ್ರಸ್ತಾವನೆ";
    const bool approved_still_review = route(high, policy, owner).state == State::REVIEW_REQUIRED;

    Policy unsafe = policy;
    unsafe.network_enabled = true;
    const bool network_block = route(low, unsafe, owner).state == State::BLOCKED;

    const bool locks = ra693_network_enabled() == 0 &&
                       ra693_runtime_install_enabled() == 0 &&
                       ra693_production_gate_open() == 0;

    std::cout << "AI_VERSION=" << ra693_version() << '\n';
    std::cout << "LOW_RISK_LOCAL=" << (low_ready ? "PASS" : "FAIL") << '\n';
    std::cout << "HIGH_RISK_REVIEW_GATE=" << (high_gate ? "PASS" : "FAIL") << '\n';
    std::cout << "APPROVED_STILL_NO_EXECUTE=" << (approved_still_review ? "PASS" : "FAIL") << '\n';
    std::cout << "NETWORK_BLOCK=" << (network_block ? "PASS" : "FAIL") << '\n';
    std::cout << "PERMANENT_LOCKS=" << (locks ? "PASS" : "FAIL") << '\n';

    return low_ready && high_gate && approved_still_review && network_block && locks ? 0 : 1;
}
