#include <iostream>
#include "persistence_isolation_v702.cpp"
int main(){ using namespace ra702;
 ProjectSpace robot{"robot-admin","state/robot","queue/robot","logs/robot","backup/robot",true};
 ProjectSpace trading{"trading","state/trading","queue/trading","logs/trading","backup/trading",false};
 PersistenceEvidence e{true,true,true,true,true,true};
 bool same_ok=cross_project_write_allowed(robot,robot);
 bool cross_block=!cross_project_write_allowed(robot,trading);
 bool locks=ra702_trading_write_enabled()==0&&ra702_cross_project_write_default()==0&&ra702_auto_restore_enabled()==0&&ra702_production_gate_open()==0;
 bool persist=persistence_ready(e);
 std::cout<<"SAME_PROJECT_WRITE="<<(same_ok?"PASS":"FAIL")<<'\n'
          <<"CROSS_PROJECT_BLOCK="<<(cross_block?"PASS":"FAIL")<<'\n'
          <<"PERSISTENCE_GATE="<<(persist?"PASS":"FAIL")<<'\n'
          <<"SAFETY_LOCKS="<<(locks?"PASS":"FAIL")<<'\n';
 return same_ok&&cross_block&&persist&&locks?0:1; }
