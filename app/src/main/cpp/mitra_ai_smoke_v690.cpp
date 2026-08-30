#include <cstring>
#include <iostream>

#include "mitra_ai_v690.cpp"

int main() {
    const bool pass = ra690_mitra_selftest() == 0 &&
        std::strcmp(ra690_mitra_version(), "69.0-mitra-ai-offline-readonly") == 0 &&
        ra690_mitra_write_blocked() == -403 &&
        ra690_mitra_delete_blocked() == -403 &&
        ra690_mitra_apply_blocked() == -403 &&
        std::strstr(ra690_mitra_respond("status"), "read-only") != nullptr &&
        std::strstr(ra690_mitra_respond("backup"), "blocked") != nullptr &&
        std::strcmp(ra690_mitra_respond("ನಮಸ್ಕಾರ"),
                    "ನಮಸ್ಕಾರ. ಮಿತ್ರ AI ಸಹಾಯಕ್ಕೆ ಸಿದ್ಧವಾಗಿದೆ.") == 0 &&
        ra690_mitra_memory_count() >= 3 &&
        std::strcmp(ra690_mitra_last_intent(), "GREETING") == 0 &&
        std::strstr(ra690_mitra_respond("more"), "ready to help") != nullptr &&
        ra690_mitra_approval_required("delete file") == 1 &&
        std::strstr(ra690_mitra_respond("delete file"), "PREVIEW #1: DELETE") != nullptr &&
        std::strcmp(ra690_mitra_gate_state(), "PREVIEW_PENDING") == 0 &&
        std::strcmp(ra690_mitra_pending_action(), "DELETE") == 0 &&
        std::strstr(ra690_mitra_pending_preview(), "NO ACTION EXECUTED") != nullptr &&
        ra690_mitra_has_pending_approval() == 1 &&
        std::strstr(ra690_mitra_respond("write file"), "approval is pending") != nullptr &&
        std::strstr(ra690_mitra_respond("cancel"), "CANCELLED: DELETE") != nullptr &&
        std::strcmp(ra690_mitra_gate_state(), "CANCELLED") == 0 &&
        ra690_mitra_has_pending_approval() == 0 &&
        std::strstr(ra690_mitra_respond("restore backup"), "PREVIEW #2: RESTORE") != nullptr &&
        std::strstr(ra690_mitra_respond("approve"), "APPROVED: RESTORE") != nullptr &&
        std::strcmp(ra690_mitra_gate_state(), "APPROVED_LOCKED") == 0 &&
        ra690_mitra_has_pending_approval() == 0 &&
        ra690_mitra_executed_actions() == 0 &&
        std::strstr(ra690_mitra_respond("ಅಳಿಸು"), "PREVIEW #3: DELETE") != nullptr &&
        std::strstr(ra690_mitra_respond("ರದ್ದು"), "CANCELLED: DELETE") != nullptr &&
        ra690_mitra_history_count() == 8 &&
        std::strcmp(ra690_mitra_history_input(7), "ರದ್ದು") == 0 &&
        std::strstr(ra690_mitra_history_reply(7), "CANCELLED: DELETE") != nullptr &&
        ra690_mitra_executed_actions() == 0 &&
        ra690_mitra_execute_blocked() == -403;
    std::cout << "MITRA_AI=" << (pass ? "PASS" : "FAIL") << '\n';
    std::cout << "SAFETY=LOCKED\n";
    return pass ? 0 : 1;
}
