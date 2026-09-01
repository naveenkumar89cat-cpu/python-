// Robot Admin V70.2 R2 Persistence & Isolation Gate
// Safe runtime model: project separation, persistent-state readiness, Trading read-only.
#include <string>

namespace ra702 {
struct ProjectSpace {
    std::string project_id;
    std::string state_path;
    std::string queue_path;
    std::string log_path;
    std::string backup_path;
    bool write_allowed=false;
};

static bool same_project(const ProjectSpace& a, const ProjectSpace& b) {
    return a.project_id == b.project_id;
}

static bool cross_project_write_allowed(const ProjectSpace& from, const ProjectSpace& to) {
    return same_project(from, to) && to.write_allowed;
}

struct PersistenceEvidence {
    bool state_path_present=false;
    bool queue_path_present=false;
    bool log_path_present=false;
    bool backup_path_present=false;
    bool restart_resume_supported=false;
    bool atomic_replace_required=true;
};

static bool persistence_ready(const PersistenceEvidence& e) {
    return e.state_path_present && e.queue_path_present && e.log_path_present &&
           e.backup_path_present && e.restart_resume_supported && e.atomic_replace_required;
}

extern "C" int ra702_trading_write_enabled() { return 0; }
extern "C" int ra702_cross_project_write_default() { return 0; }
extern "C" int ra702_auto_restore_enabled() { return 0; }
extern "C" int ra702_production_gate_open() { return 0; }
extern "C" const char* ra702_version() { return "70.2-r2-persistence-isolation-safe-gate"; }
}
