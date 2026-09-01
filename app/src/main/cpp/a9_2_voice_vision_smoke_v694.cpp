#include <iostream>
#include "voice_vision_interface_v694.cpp"

int main() {
    using namespace ra694;
    MediaPolicy policy;
    UserAction user;

    bool default_block = evaluate(MediaMode::VOICE, user, policy).state == DecisionState::BLOCKED;

    user.explicit_action = true;
    user.owner_authenticated = true;
    bool permission_gate = evaluate(MediaMode::CAMERA, user, policy).state == DecisionState::READY_FOR_USER_ACTION;

    user.permission_granted = true;
    bool local_ready = evaluate(MediaMode::VISION_ANALYSIS, user, policy).state == DecisionState::READY_FOR_LOCAL_ANALYSIS;

    bool permanent_locks = ra694_background_recording_enabled() == 0 &&
                           ra694_autonomous_capture_enabled() == 0 &&
                           ra694_network_upload_enabled() == 0 &&
                           ra694_cloud_analysis_enabled() == 0 &&
                           ra694_runtime_install_enabled() == 0;

    std::cout << "MEDIA_VERSION=" << ra694_version() << '\n';
    std::cout << "DEFAULT_BLOCK=" << (default_block ? "PASS" : "FAIL") << '\n';
    std::cout << "PERMISSION_GATE=" << (permission_gate ? "PASS" : "FAIL") << '\n';
    std::cout << "LOCAL_READY=" << (local_ready ? "PASS" : "FAIL") << '\n';
    std::cout << "PERMANENT_LOCKS=" << (permanent_locks ? "PASS" : "FAIL") << '\n';

    return default_block && permission_gate && local_ready && permanent_locks ? 0 : 1;
}
