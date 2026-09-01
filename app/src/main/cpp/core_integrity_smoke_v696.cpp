#include <iostream>
#include "core_integrity_bundle_v696.cpp"
int main(){ using namespace ra696;
 Dependency d{"sqlite","3.x",std::string(64,'a'),true,true,true};
 Migration m{1,2,true,true,true,true,true,true};
 AuditRecord ar{"2026-09-01T19:00:00+05:30","ROBOT_ADMIN","worker-1","verify","approval-1","file","before","after","PASS","commit","rollback"};
 Approval a{"once","ROBOT_ADMIN","MIGRATE","db.sqlite","worker-1",200,false,true,true};
 bool dep=dependency_ready(d); bool mig=migration_ready(m); bool audit=audit_complete(ar);
 bool ok=approval_valid(a,"ROBOT_ADMIN","MIGRATE","worker-1",100);
 bool replay=!approval_valid(Approval{"once","ROBOT_ADMIN","MIGRATE","db.sqlite","worker-1",200,true,true,true},"ROBOT_ADMIN","MIGRATE","worker-1",100);
 bool wrong=!approval_valid(a,"TRADING","MIGRATE","worker-1",100);
 bool locks=ra696_runtime_install_enabled()==0&&ra696_audit_agent_edit_enabled()==0&&ra696_audit_agent_delete_enabled()==0;
 std::cout<<"DEPENDENCY="<<(dep?"PASS":"FAIL")<<'\n'<<"MIGRATION="<<(mig?"PASS":"FAIL")<<'\n'<<"AUDIT="<<(audit?"PASS":"FAIL")<<'\n'<<"APPROVAL="<<(ok?"PASS":"FAIL")<<'\n'<<"REPLAY_BLOCK="<<(replay?"PASS":"FAIL")<<'\n'<<"WRONG_PROJECT_BLOCK="<<(wrong?"PASS":"FAIL")<<'\n'<<"LOCKS="<<(locks?"PASS":"FAIL")<<'\n';
 return dep&&mig&&audit&&ok&&replay&&wrong&&locks?0:1; }
