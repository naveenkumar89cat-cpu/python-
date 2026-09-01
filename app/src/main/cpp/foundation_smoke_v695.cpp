#include <iostream>
#include "foundation_bundle_v695.cpp"
int main(){ using namespace ra695; Acceptance a; StateRecord s;
 bool isolation=!cross_project_allowed(Project::ROBOT_ADMIN,Project::TRADING)&&policy(Project::TRADING).write_allowed==false;
 bool incomplete=!can_complete(a);
 bool path=transition(s,Level::DEVELOPING,a)&&transition(s,Level::BUILT,a)&&transition(s,Level::TESTED,a)&&transition(s,Level::VERIFIED,a)&&transition(s,Level::APPROVED,a);
 bool premature=!transition(s,Level::COMPLETED,a);
 a={true,true,true,true,true,true,true}; bool complete=transition(s,Level::COMPLETED,a);
 bool locks=ra695_trading_write_enabled()==0&&ra695_cross_project_write_enabled()==0;
 std::cout<<"ISOLATION="<<(isolation?"PASS":"FAIL")<<'\n'<<"STATE_MACHINE="<<(path?"PASS":"FAIL")<<'\n'<<"NO_PROOF_NO_COMPLETE="<<(premature?"PASS":"FAIL")<<'\n'<<"ACCEPTANCE="<<(complete?"PASS":"FAIL")<<'\n'<<"LOCKS="<<(locks?"PASS":"FAIL")<<'\n';
 return isolation&&incomplete&&path&&premature&&complete&&locks?0:1; }
