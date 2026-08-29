#include "native_activity_host_v652.cpp"

// V65.3 Native Status Master Bundle.
// Keeps the proven V65.2 offline-first runtime and safety locks intact.
// Adds stable native status exports for later dashboard/modules without enabling writes.
extern "C" const char* ra653_version(){return "65.3";}
extern "C" int ra653_core_ready(){return 1;}
extern "C" const char* ra653_mode(){return "OFFLINE_FIRST";}
extern "C" const char* ra653_safety(){return "LOCKED";}
extern "C" int ra653_write_allowed(){return 0;}
extern "C" int ra653_live_orders_allowed(){return 0;}
extern "C" int ra653_auto_update_allowed(){return 0;}
extern "C" int ra653_production_gate_open(){return 0;}
