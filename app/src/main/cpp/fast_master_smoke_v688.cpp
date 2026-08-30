// ROBOT ADMIN AI OS — V68.8 FAST MASTER HOST-INDEPENDENT SMOKE TEST
// Source/regression test only. Not linked into Android APK.

#include "fast_master_integration_v688.cpp"
#include <iostream>

int main() {
    const int regression = ra688::ra688_fast_master_selftest();
    const bool safety =
        ra688::ra688_master_apply_blocked() == -403 &&
        ra688::ra688_master_restore_blocked() == -403 &&
        ra688::ra688_master_delete_blocked() == -403 &&
        ra688::ra688_master_auto_update_blocked() == -403 &&
        ra688::ra688_master_live_orders_blocked() == -403;

    std::cout << "FAST_MASTER_VERSION=" << ra688::ra688_fast_master_version() << '\n';
    std::cout << "REGRESSION=" << (regression == 0 ? "PASS" : "FAIL") << '\n';
    std::cout << "SAFETY=" << (safety ? "LOCKED" : "FAIL") << '\n';

    return (regression == 0 && safety) ? 0 : 1;
}
