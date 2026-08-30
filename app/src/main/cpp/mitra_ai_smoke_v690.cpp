#include <cstring>
#include <iostream>

#include "mitra_ai_v690.cpp"

int main() {
    const bool pass = ra690_mitra_selftest() == 0 &&
        std::strcmp(ra690_mitra_version(), "69.0-mitra-ai-offline-readonly") == 0 &&
        ra690_mitra_write_blocked() == -403 &&
        ra690_mitra_delete_blocked() == -403 &&
        ra690_mitra_apply_blocked() == -403;
    std::cout << "MITRA_AI=" << (pass ? "PASS" : "FAIL") << '\n';
    std::cout << "SAFETY=LOCKED\n";
    return pass ? 0 : 1;
}

