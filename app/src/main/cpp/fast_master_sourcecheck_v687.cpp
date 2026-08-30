// ROBOT ADMIN AI OS — V68.1..V68.7 FAST MASTER SOURCE CHECK
// Pure C++ host-side regression harness. No Android APK integration.

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "file_intelligence_v681.cpp"
#include "task_intelligence_v682.cpp"
#include "data_intelligence_v683.cpp"
#include "backup_recovery_v684.cpp"
#include "error_diagnostics_v685.cpp"
#include "code_admin_intelligence_v686.cpp"
#include "master_regression_v687.cpp"

static bool nonempty(const char* s) {
    return s != nullptr && std::strlen(s) > 0;
}

int main() {
    const std::vector<ra681::FileItem> files = {
        {"tasks.txt", "v673/tasks.txt", 128, false, true, "txt"},
        {"sample.csv", "v673/sample.csv", 256, false, true, "csv"},
        {"private.bin", "v673/private.bin", 64, false, false, "bin"},
        {"v673", "v673", 0, true, true, ""}
    };
    const auto file_report = ra681::summarize(files);
    const auto file_hits = ra681::search(files, "SAMPLE");
    const bool file_ready = nonempty(ra681::ra681_file_engine_version()) &&
        file_report.total == 4 && file_report.files == 3 && file_report.folders == 1 &&
        file_report.readable == 3 && file_report.unreadable == 1 &&
        file_report.bytes == 448 && file_hits.size() == 1 &&
        ra681::preview_allowed(files[0]) && !ra681::preview_allowed(files[2]);

    std::size_t invalid_tasks = 0;
    bool tasks_truncated = false;
    const auto tasks = ra682::parse_text(
        "T1|Compile core|HIGH|PENDING|native\n"
        "ID:T2|TITLE:Run regression|PRIORITY:NORMAL|STATUS:DONE\n"
        "Legacy safe task\n", &invalid_tasks, &tasks_truncated);
    const auto task_report = ra682::summarize(tasks, invalid_tasks, tasks_truncated);
    const bool task_ready = nonempty(ra682::ra682_task_engine_version()) &&
        task_report.total == 3 && task_report.pending == 2 && task_report.done == 1 &&
        task_report.high == 1 && task_report.normal == 2 && invalid_tasks == 0 &&
        ra682::search(tasks, "regression").size() == 1 &&
        ra682::filter_status(tasks, "done").size() == 1 &&
        ra682::filter_priority(tasks, "high").size() == 1;

    const std::string csv = "name,value\nalpha,10\nbeta,20\nbeta,20\n";
    const auto data_report = ra683::analyze_csv(csv);
    const bool data_ready = nonempty(ra683::ra683_data_engine_version()) &&
        data_report.rows == 4 && data_report.cols == 2 && data_report.numeric_cells == 3 &&
        data_report.duplicate_rows == 1 && data_report.invalid_rows == 0 &&
        data_report.min_value == 10.0 && data_report.max_value == 20.0 &&
        ra683::search_rows(csv, "beta").size() == 2 &&
        ra683::json_candidate(" {\"safe\":true} ") && !ra683::json_candidate("unsafe");

    const std::string hash_a(64, 'a');
    const std::string hash_b(64, 'b');
    std::vector<ra684::BackupItem> backups = {
        {"verified", "a", hash_a, hash_a, 10, true, true},
        {"mismatch", "b", hash_a, hash_b, 10, true, true},
        {"missing", "c", hash_a, "", 0, false, false},
        {"corrupt", "d", "bad", "bad", 10, true, true},
        {"unverified", "e", "", "", 10, true, true}
    };
    const auto backup_report = ra684::summarize(backups);
    const bool backup_ready = nonempty(ra684::ra684_backup_engine_version()) &&
        backup_report.total == 5 && backup_report.verified == 1 && backup_report.mismatch == 1 &&
        backup_report.missing == 1 && backup_report.corrupt == 1 && backup_report.unverified == 1 &&
        std::string(ra684::state_name(backups[0].state)) == "VERIFIED";

    const std::vector<ra685::ModuleHealth> health = {
        {"FILE", true, true, ra685::Code::OK, "READY"},
        {"DATA", true, true, ra685::Code::CSV_INVALID, "REVIEW"}
    };
    const auto diag_report = ra685::summarize(health);
    const bool diag_ready = nonempty(ra685::ra685_error_engine_version()) &&
        diag_report.total == 2 && diag_report.ready == 2 && diag_report.warnings == 1 &&
        diag_report.errors == 0 && diag_report.critical == 0 && diag_report.system_ok &&
        ra685::recommended_action(ra685::Code::HASH_MISMATCH) == "STOP AND VERIFY TRUSTED SOURCE";

    const std::vector<ra686::SourceFile> sources = {
        {"core.cpp", "", 100, true},
        {"engine.hpp", "", 50, true},
        {"build.gradle", "", 20, true},
        {"notes.bin", "", 5, false}
    };
    const auto project_report = ra686::summarize_project(sources);
    const auto clean_build = ra686::analyze_build_log("compile complete", 0);
    const auto failed_build = ra686::analyze_build_log("warning: check\nerror: failed", 1);
    const bool code_ready = nonempty(ra686::ra686_code_admin_version()) &&
        project_report.total_files == 4 && project_report.readable_files == 3 &&
        project_report.source_files == 2 && project_report.config_files == 1 &&
        project_report.unknown_files == 1 && project_report.total_lines == 175 &&
        ra686::filter_language(sources, "c++").size() == 2 && clean_build.pass &&
        !failed_build.pass && failed_build.error_lines == 1 && failed_build.warning_lines == 1;

    const bool write_blocked =
        ra681::ra681_file_write_blocked() == -403 &&
        ra683::ra683_data_write_blocked() == -403 &&
        ra686::ra686_code_modify_blocked() == -403;

    const bool delete_blocked =
        ra681::ra681_file_delete_blocked() == -403 &&
        ra682::ra682_task_delete_blocked() == -403 &&
        ra684::ra684_delete_backup_blocked() == -403 &&
        ra686::ra686_code_delete_blocked() == -403;

    const bool restore_blocked =
        ra681::ra681_file_restore_blocked() == -403 &&
        ra684::ra684_restore_blocked() == -403;

    const bool apply_blocked =
        ra684::ra684_apply_rollback_blocked() == -403 &&
        ra685::ra685_apply_fix_blocked() == -403 &&
        ra686::ra686_fix_apply_blocked() == -403 &&
        ra687::ra687_regression_apply_blocked() == -403;

    // V68.x policy constants: live orders and auto-update remain disabled;
    // production gate remains closed. No mutating path exists in this harness.
    const bool live_orders_off = true;
    const bool auto_update_off = true;
    const bool prod_gate_closed = true;

    auto report = ra687::run_core_regression(
        file_ready, task_ready, data_ready, backup_ready, diag_ready, code_ready,
        write_blocked, delete_blocked, restore_blocked, apply_blocked,
        live_orders_off, auto_update_off, prod_gate_closed);

    for (const auto& c : report.checks) {
        std::cout << (c.passed ? "PASS " : "FAIL ") << c.name << " : " << c.detail << '\n';
    }

    std::cout << "TOTAL=" << report.total
              << " PASS=" << report.passed
              << " FAIL=" << report.failed << '\n';
    std::cout << "SAFETY=" << (report.safety_locked ? "LOCKED" : "UNSAFE") << '\n';
    std::cout << "PROD_GATE=" << (report.production_gate_closed ? "CLOSED" : "OPEN") << '\n';

    return ra687::overall_pass(report) ? 0 : 1;
}
