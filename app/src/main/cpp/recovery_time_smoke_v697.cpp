#include <iostream>
#include "recovery_time_bundle_v697.cpp"
int main(){ using namespace ra697;
 CrashState c{true,true,true,true,true}; ResourceState r{9,128,46,false}; TimeState t{true,false,false,true,false};
 auto p=evaluate(c,r,t);
 bool recovery=p.atomic_replace&&p.queue_recover&&p.db_recover&&p.lkg_start;
 bool pause=p.pause_required;
 bool sched=schedule_ok(t);
 TimeState bad{true,true,false,true,false}; bool dup=!schedule_ok(bad);
 bool locks=ra697_auto_resume_after_emergency()==0&&ra697_auto_restore_enabled()==0;
 std::cout<<"RECOVERY="<<(recovery?"PASS":"FAIL")<<'\n'<<"RESOURCE_PAUSE="<<(pause?"PASS":"FAIL")<<'\n'<<"TIME_INTEGRITY="<<(sched?"PASS":"FAIL")<<'\n'<<"DUPLICATE_BLOCK="<<(dup?"PASS":"FAIL")<<'\n'<<"LOCKS="<<(locks?"PASS":"FAIL")<<'\n';
 return recovery&&pause&&sched&&dup&&locks?0:1; }
