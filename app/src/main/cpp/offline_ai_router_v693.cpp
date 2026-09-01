// Robot Admin AI OS A9 / V69.3 - Offline Bilingual AI Router
// Local planning only. No network and no device-changing actions.
#include <string>

namespace ra693 {

enum class Language { KANNADA, ENGLISH, BILINGUAL };
enum class Risk { LOW, MEDIUM, HIGH };
enum class State { LOCAL_READY, REVIEW_REQUIRED, BLOCKED };

struct Policy {
    bool offline_only = true;
    bool network_enabled = false;
    bool write_enabled = false;
    bool delete_enabled = false;
    bool restore_enabled = false;
    bool runtime_install_enabled = false;
    bool production_gate = false;
};

struct Command {
    std::string text;
    Language language = Language::BILINGUAL;
    Risk risk = Risk::LOW;
};

struct OwnerGate {
    bool authenticated = false;
    bool approved = false;
    std::string kannada_reason;
};

struct Decision {
    State state = State::BLOCKED;
    std::string message_kn;
    std::string message_en;
};

static bool safe_policy(const Policy& p) {
    return p.offline_only && !p.network_enabled && !p.write_enabled &&
           !p.delete_enabled && !p.restore_enabled &&
           !p.runtime_install_enabled && !p.production_gate;
}

Decision route(const Command& cmd, const Policy& policy, const OwnerGate& owner) {
    if (!safe_policy(policy)) {
        return {State::BLOCKED,
                "Safety policy ಬದಲಾಗಿದೆ; command block ಮಾಡಲಾಗಿದೆ.",
                "Safety policy changed; command blocked."};
    }
    if (cmd.risk == Risk::HIGH) {
        if (!owner.authenticated || !owner.approved || owner.kannada_reason.empty()) {
            return {State::REVIEW_REQUIRED,
                    "High-risk commandಗೆ owner approval ಮತ್ತು ಕನ್ನಡ ಕಾರಣ ಕಡ್ಡಾಯ.",
                    "High-risk command requires owner approval and a Kannada reason."};
        }
        return {State::REVIEW_REQUIRED,
                "Owner approval ದಾಖಲಾಗಿದೆ; proposal review ಮಾತ್ರ ಅನುಮತಿಸಲಾಗಿದೆ.",
                "Owner approval recorded; proposal review only."};
    }
    return {State::LOCAL_READY,
            "Offline local AI command ಸುರಕ್ಷಿತವಾಗಿ route ಆಗಿದೆ.",
            "Offline local AI command routed safely."};
}

extern "C" const char* ra693_version() { return "69.3-offline-ai-router-safe"; }
extern "C" int ra693_network_enabled() { return 0; }
extern "C" int ra693_runtime_install_enabled() { return 0; }
extern "C" int ra693_production_gate_open() { return 0; }

} // namespace ra693
