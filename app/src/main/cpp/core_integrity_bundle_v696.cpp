// Robot Admin V69.6 Core Integrity: metadata/policy verification only.
#include <string>
#include <set>
namespace ra696 {
struct Dependency { std::string name, version, checksum; bool compatible=false, vulnerability_checked=false, cached_offline=false; };
static bool dependency_ready(const Dependency& d){ return !d.name.empty()&&!d.version.empty()&&d.checksum.size()==64&&d.compatible&&d.vulnerability_checked&&d.cached_offline; }
struct Migration { int from_schema=0,to_schema=0; bool preview=false,backup=false,transactional=false,integrity=false,old_compatible=false,owner_approved=false; };
static bool migration_ready(const Migration& m){ return m.to_schema>m.from_schema&&m.preview&&m.backup&&m.transactional&&m.integrity&&m.old_compatible&&m.owner_approved; }
struct AuditRecord { std::string timestamp,project,worker,action,approval_id,files,before_hash,after_hash,test_result,commit_tag,rollback; };
static bool audit_complete(const AuditRecord& a){ return !a.timestamp.empty()&&!a.project.empty()&&!a.worker.empty()&&!a.action.empty()&&!a.before_hash.empty()&&!a.after_hash.empty()&&!a.test_result.empty(); }
struct Approval { std::string token,project,action,file_scope,worker; unsigned long long expires_at=0; bool used=false,critical=false,double_confirmed=false; };
static bool approval_valid(const Approval& a,const std::string& project,const std::string& action,const std::string& worker,unsigned long long now){ return !a.token.empty()&&!a.used&&now<a.expires_at&&a.project==project&&a.action==action&&a.worker==worker&&(!a.critical||a.double_confirmed); }
extern "C" int ra696_runtime_install_enabled(){ return 0; }
extern "C" int ra696_audit_agent_edit_enabled(){ return 0; }
extern "C" int ra696_audit_agent_delete_enabled(){ return 0; }
extern "C" const char* ra696_version(){ return "69.6-core-integrity-safe"; }
}
