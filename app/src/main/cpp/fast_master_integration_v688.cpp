// ROBOT ADMIN AI OS — V68.8 FAST MASTER INTEGRATION
// Android-only, offline-first, read-only integration unit.
// This wires V68.1–V68.7 into one compilation unit and exposes a self-test.

#include <cstring>
#include <string>
#include <vector>

#include "file_intelligence_v681.cpp"
#include "task_intelligence_v682.cpp"
#include "data_intelligence_v683.cpp"
#include "backup_recovery_v684.cpp"
#include "error_diagnostics_v685.cpp"
#include "code_admin_intelligence_v686.cpp"
#include "master_regression_v687.cpp"

namespace ra688 {

static bool version_ready(const char* s) {
    return s != nullptr && *s != '\0';
}

extern "C" int ra688_fast_master_selftest() {
    const std::vector<ra681::FileItem> files = {
        {"tasks.txt", "v673/tasks.txt", 64, false, true, "txt"},
        {"sample.csv", "v673/sample.csv", 90, false, true, "csv"},
        {"verify.txt", "v673/verify.txt", 40, false, true, "txt"}
    };
    const auto file_report = ra681::summarize(files);
    const bool file_ready = version_ready(ra681::ra681_file_engine_version()) &&
        file_report.files == 3 && file_report.readable == 3 &&
        ra681::search(files, "sample").size() == 1 && ra681::preview_allowed(files[0]);

    std::size_t invalid_tasks = 0;
    bool task_cut = false;
    const auto tasks = ra682::parse_text(
        "Check Robot Admin core\nVerify safety locks\nReview backup digest\n",
        &invalid_tasks, &task_cut);
    const auto task_report = ra682::summarize(tasks, invalid_tasks, task_cut);
    const bool task_ready = version_ready(ra682::ra682_task_engine_version()) &&
        task_report.total == 3 && task_report.pending == 3 && task_report.normal == 3 &&
        invalid_tasks == 0 && !task_cut && ra682::search(tasks, "safety").size() == 1 &&
        ra682::filter_status(tasks, "PENDING").size() == 3 &&
        ra682::filter_priority(tasks, "NORMAL").size() == 3;

    const std::string csv = "name,value\nalpha,10\nbeta,20\n";
    const auto data_report = ra683::analyze_csv(csv);
    const bool data_ready = version_ready(ra683::ra683_data_engine_version()) &&
        data_report.rows == 3 && data_report.cols == 2 && data_report.invalid_rows == 0 &&
        data_report.numeric_cells == 2 && ra683::search_rows(csv, "beta").size() == 1 &&
        ra683::json_candidate("{\"safe\":true}");

    const std::string hash(64, 'a');
    std::vector<ra684::BackupItem> backups = {
        {"verify", "v673/verify.txt", hash, hash, 40, true, true}
    };
    const auto backup_report = ra684::summarize(backups);
    const bool backup_ready = version_ready(ra684::ra684_backup_engine_version()) &&
        backup_report.verified == 1 &&
        std::string(ra684::state_name(backups[0].state)) == "VERIFIED";

    const std::vector<ra685::ModuleHealth> health = {
        {"FILE", true, file_ready, file_ready ? ra685::Code::OK : ra685::Code::MODULE_FAILED, "FILE"},
        {"TASK", true, task_ready, task_ready ? ra685::Code::OK : ra685::Code::TASK_INVALID, "TASK"},
        {"DATA", true, data_ready, data_ready ? ra685::Code::OK : ra685::Code::CSV_INVALID, "DATA"},
        {"BACKUP", true, backup_ready, backup_ready ? ra685::Code::OK : ra685::Code::HASH_MISMATCH, "BACKUP"}
    };
    const auto diag_report = ra685::summarize(health);
    const bool diag_ready = version_ready(ra685::ra685_error_engine_version()) &&
        diag_report.system_ok && diag_report.passed == 4 &&
        ra685::recommended_action(ra685::Code::OK) == "NONE";

    const std::vector<ra686::SourceFile> sources = {
        {"file_intelligence_v681.cpp", "", 1, true},
        {"task_intelligence_v682.cpp", "", 1, true},
        {"data_intelligence_v683.cpp", "", 1, true},
        {"backup_recovery_v684.cpp", "", 1, true},
        {"error_diagnostics_v685.cpp", "", 1, true},
        {"code_admin_intelligence_v686.cpp", "", 1, true},
        {"master_regression_v687.cpp", "", 1, true}
    };
    const auto code_report = ra686::summarize_project(sources);
    const auto clean_build = ra686::analyze_build_log("compile complete", 0);
    const bool code_ready = version_ready(ra686::ra686_code_admin_version()) &&
        code_report.total_files == 7 && code_report.source_files == 7 &&
        ra686::filter_language(sources, "C++").size() == 7 && clean_build.pass;

    const bool write_blocked =
        ra681::ra681_file_write_blocked() == -403 &&
        ra683::ra683_data_write_blocked() == -403;

    const bool delete_blocked =
        ra681::ra681_file_delete_blocked() == -403 &&
        ra682::ra682_task_delete_blocked() == -403 &&
        ra684::ra684_delete_backup_blocked() == -403 &&
        ra686::ra686_code_delete_blocked() == -403;

    const bool restore_blocked = ra684::ra684_restore_blocked() == -403;
    const bool apply_blocked =
        ra684::ra684_apply_rollback_blocked() == -403 &&
        ra685::ra685_apply_fix_blocked() == -403 &&
        ra686::ra686_fix_apply_blocked() == -403 &&
        ra687::ra687_regression_apply_blocked() == -403;

    const auto report = ra687::run_core_regression(
        file_ready,
        task_ready,
        data_ready,
        backup_ready,
        diag_ready,
        code_ready,
        write_blocked,
        delete_blocked,
        restore_blocked,
        apply_blocked,
        true,   // LIVE ORDERS OFF
        true,   // AUTO UPDATE OFF
        true    // PROD GATE CLOSED
    );

    return ra687::overall_pass(report) ? 0 : 1;
}

extern "C" const char* ra688_fast_master_version() {
    return "68.8-fast-master-integrated-readonly";
}

extern "C" int ra688_master_apply_blocked() { return -403; }
extern "C" int ra688_master_restore_blocked() { return -403; }
extern "C" int ra688_master_delete_blocked() { return -403; }
extern "C" int ra688_master_auto_update_blocked() { return -403; }
extern "C" int ra688_master_live_orders_blocked() { return -403; }

} // namespace ra688
