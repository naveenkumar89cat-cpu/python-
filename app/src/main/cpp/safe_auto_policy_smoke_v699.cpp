#include <iostream>
#include "safe_auto_policy_bundle_v699.cpp"
int main(){ using namespace ra699;
 PolicySnapshot p{"v1","p1","c1",std::string(64,'a'),std::string(64,'b'),"sig",true};
 AutoLevelGate g{true,true,true,true,true,false,true,true};
 UpdateGate u{true,true,true,true,true,true,true,true,true,true};
 bool policy=policy_snapshot_valid(p);
 bool level=can_plan_next_level(g);
 bool update=update_ready_for_owner_apply(u);
 bool locks=ra699_safe_auto_execute_enabled()==0&&ra699_auto_update_apply_enabled()==0&&ra699_policy_self_modify_enabled()==0&&ra699_runtime_install_enabled()==0&&ra699_production_gate_open()==0;
 std::cout<<"POLICY="<<(policy?"PASS":"FAIL")<<'\n'<<"AUTO_LEVEL_GATE="<<(level?"PASS":"FAIL")<<'\n'<<"UPDATE_GATE="<<(update?"PASS":"FAIL")<<'\n'<<"PERMANENT_LOCKS="<<(locks?"PASS":"FAIL")<<'\n';
 return policy&&level&&update&&locks?0:1; }
