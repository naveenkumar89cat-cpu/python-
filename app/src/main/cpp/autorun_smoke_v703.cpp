#include <cstring>
#include "autorun_engine_v703.cpp"

int main() {
    if (ra703::ra703_autorun_mandatory() != 1) return 1;
    if (ra703::ra703_high_risk_auto_apply_enabled() != 0) return 2;
    if (ra703::ra703_auto_restore_enabled() != 0) return 3;
    if (ra703::ra703_trading_write_enabled() != 0) return 4;
    if (ra703::ra703_production_gate_open() != 0) return 5;
    if (std::strcmp(ra703::ra703_version(), "70.3-r3-safe-autorun") != 0) return 6;
    return 0;
}
