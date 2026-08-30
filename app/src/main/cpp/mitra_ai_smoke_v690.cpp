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
        std::strcmp(ra690_mitra_last_intent(), "GREETING") == 0;
    std::cout << "MITRA_AI=" << (pass ? "PASS" : "FAIL") << '\n';
    std::cout << "SAFETY=LOCKED\n";
    return pass ? 0 : 1;
}
