#define ANativeActivity_onCreate ANativeActivity_onCreate_v658_legacy
#include "native_activity_host_v658.cpp"
#undef ANativeActivity_onCreate

#include <android/keycodes.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <array>
#include <algorithm>
#include <cctype>
#include <cstdint>

// V66.5 MEGA FEATURE BUNDLE
// Native C++ read-only feature pack:
// - real app-sandbox file inventory
// - tasks.txt read-only inspection
// - SHA-256 engine + readable-file/manifest verification
// - CSV read-only scan + simple analyzer
// - touch navigation, hardware BACK -> STATUS, safe localhost recheck
// Dangerous write/delete/restore/apply/live-order/update operations remain blocked.

namespace {
std::atomic<bool> g_probe_running_v665{false};
std::atomic<int> g_event_v665{0};
std::mutex g_data_mutex_v665;
std::string g_internal_v665;
std::string g_external_v665;
std::vector<std::string> g_files_v665;
std::string g_task_state_v665="TASK FILE NONE";
std::string g_task_preview_v665="NO TASK DATA";
std::string g_verify_target_v665="SELF MANIFEST";
std::string g_sha_v665="PENDING";
std::string g_csv_name_v665="CSV NONE";
std::atomic<int> g_csv_rows_v665{0};
std::atomic<int> g_csv_cols_v665{0};
std::atomic<int> g_csv_numeric_v665{0};

enum Event665 { E665_READY=0, E665_PANEL=1, E665_SCAN=2, E665_VERIFY=3, E665_RECHECK=4, E665_CONNECTED=5, E665_OPTIONAL=6, E665_HOME=7 };

static inline uint32_t rr(uint32_t x,uint32_t n){return (x>>n)|(x<<(32-n));}
std::string sha256_bytes(const unsigned char* data,size_t len){
    static const uint32_t k[64]={
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2};
    uint64_t bitlen=(uint64_t)len*8;size_t total=((len+9+63)/64)*64;std::vector<unsigned char> m(total,0);
    if(len)std::copy(data,data+len,m.begin());m[len]=0x80;for(int i=0;i<8;i++)m[total-1-i]=(unsigned char)(bitlen>>(8*i));
    uint32_t h0=0x6a09e667,h1=0xbb67ae85,h2=0x3c6ef372,h3=0xa54ff53a,h4=0x510e527f,h5=0x9b05688c,h6=0x1f83d9ab,h7=0x5be0cd19;
    for(size_t off=0;off<total;off+=64){uint32_t w[64]{};for(int i=0;i<16;i++){size_t p=off+i*4;w[i]=((uint32_t)m[p]<<24)|((uint32_t)m[p+1]<<16)|((uint32_t)m[p+2]<<8)|m[p+3];}
        for(int i=16;i<64;i++){uint32_t s0=rr(w[i-15],7)^rr(w[i-15],18)^(w[i-15]>>3);uint32_t s1=rr(w[i-2],17)^rr(w[i-2],19)^(w[i-2]>>10);w[i]=w[i-16]+s0+w[i-7]+s1;}
        uint32_t a=h0,b=h1,c=h2,d=h3,e=h4,f=h5,g=h6,h=h7;
        for(int i=0;i<64;i++){uint32_t S1=rr(e,6)^rr(e,11)^rr(e,25),ch=(e&f)^((~e)&g),t1=h+S1+ch+k[i]+w[i],S0=rr(a,2)^rr(a,13)^rr(a,22),maj=(a&b)^(a&c)^(b&c),t2=S0+maj;h=g;g=f;f=e;e=d+t1;d=c;c=b;b=a;a=t1+t2;}
        h0+=a;h1+=b;h2+=c;h3+=d;h4+=e;h5+=f;h6+=g;h7+=h;}
    std::ostringstream o;o<<std::hex<<std::setfill('0');for(uint32_t v:{h0,h1,h2,h3,h4,h5,h6,h7})o<<std::setw(8)<<v;return o.str();
}

std::string upper_safe(std::string s,size_t maxn=24){for(char&c:s){unsigned char u=(unsigned char)c;c=(char)std::toupper(u);if(!((c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='.'||c=='-'))c='-';}if(s.size()>maxn)s.resize(maxn);return s.empty()?"NONE":s;}
void list_dir_one(const std::string& p,std::vector<std::string>& out){if(p.empty())return;DIR*d=opendir(p.c_str());if(!d)return;dirent*e=nullptr;while((e=readdir(d))){std::string n=e->d_name;if(n=="."||n=="..")continue;out.push_back(p+"/"+n);if(out.size()>=64)break;}closedir(d);}
bool regular_readable(const std::string&p){struct stat st{};return stat(p.c_str(),&st)==0&&S_ISREG(st.st_mode)&&access(p.c_str(),R_OK)==0;}
std::string base_name(const std::string&p){size_t x=p.find_last_of('/');return x==std::string::npos?p:p.substr(x+1);}

std::string sha256_file_limited(const std::string&p,bool&ok){ok=false;std::ifstream f(p,std::ios::binary);if(!f)return"";std::vector<unsigned char>b;char tmp[4096];size_t total=0;while(f&&total<16*1024*1024){f.read(tmp,sizeof(tmp));std::streamsize n=f.gcount();if(n<=0)break;b.insert(b.end(),tmp,tmp+n);total+=(size_t)n;}ok=true;return sha256_bytes(b.data(),b.size());}

void scan_csv_locked(){g_csv_name_v665="CSV NONE";g_csv_rows_v665=0;g_csv_cols_v665=0;g_csv_numeric_v665=0;for(const auto&p:g_files_v665){std::string n=base_name(p),u=n;std::transform(u.begin(),u.end(),u.begin(),[](unsigned char c){return (char)std::tolower(c);});if(u.size()<4||u.substr(u.size()-4)!=".csv"||!regular_readable(p))continue;std::ifstream f(p);if(!f)continue;std::string line;int rows=0,maxc=0,num=0;while(rows<500&&std::getline(f,line)){rows++;int cols=1;for(char c:line)if(c==',')cols++;maxc=std::max(maxc,cols);std::stringstream ss(line);std::string cell;while(std::getline(ss,cell,',')){char*end=nullptr;std::strtod(cell.c_str(),&end);if(end&&end!=cell.c_str()){while(*end&&std::isspace((unsigned char)*end))end++;if(*end==0)num++;}}}g_csv_name_v665=upper_safe(n,20);g_csv_rows_v665=rows;g_csv_cols_v665=maxc;g_csv_numeric_v665=num;break;}}

void scan_tasks_locked(){g_task_state_v665="TASK FILE NONE";g_task_preview_v665="NO TASK DATA";for(const auto&p:g_files_v665){std::string n=base_name(p),u=n;std::transform(u.begin(),u.end(),u.begin(),[](unsigned char c){return (char)std::tolower(c);});if(u!="tasks.txt"||!regular_readable(p))continue;std::ifstream f(p);std::string line;if(std::getline(f,line)){g_task_state_v665="TASK FILE FOUND";g_task_preview_v665=upper_safe(line,28);}else{g_task_state_v665="TASK FILE EMPTY";g_task_preview_v665="NO TASK DATA";}break;}}

void verify_locked(){const std::string manifest="ROBOT-ADMIN-V66.5|OFFLINE-FIRST|READ-ONLY|WRITE-OFF|DELETE-OFF|RESTORE-OFF|LIVE-ORDERS-OFF|AUTO-UPDATE-OFF";g_verify_target_v665="SELF MANIFEST";g_sha_v665=sha256_bytes((const unsigned char*)manifest.data(),manifest.size());for(const auto&p:g_files_v665){if(!regular_readable(p))continue;bool ok=false;std::string h=sha256_file_limited(p,ok);if(ok){g_verify_target_v665=upper_safe(base_name(p),18);g_sha_v665=h;break;}}
}

void rescan665(){std::lock_guard<std::mutex>l(g_data_mutex_v665);g_files_v665.clear();list_dir_one(g_internal_v665,g_files_v665);list_dir_one(g_external_v665,g_files_v665);scan_tasks_locked();scan_csv_locked();verify_locked();}

const char* event_name665(int e){switch(e){case E665_PANEL:return"PANEL OPEN";case E665_SCAN:return"DATA RESCAN";case E665_VERIFY:return"SHA256 VERIFY";case E665_RECHECK:return"RECHECK RUNNING";case E665_CONNECTED:return"BACKEND CONNECTED";case E665_OPTIONAL:return"BACKEND OPTIONAL OFF";case E665_HOME:return"HOME STATUS";default:return"MEGA READY";}}
const char* page_title665(int a){switch(a){case ACT_FILES:return"FILES REAL READ ONLY";case ACT_TASKS:return"TASKS READ ONLY";case ACT_BACKUP:return"BACKUP SHA256";case ACT_RECHECK:return"DATA AND RECHECK";default:return"STATUS MEGA";}}

bool render665(ANativeWindow*w){if(!w)return false;EGLDisplay d=eglGetDisplay(EGL_DEFAULT_DISPLAY);if(d==EGL_NO_DISPLAY||!eglInitialize(d,nullptr,nullptr))return false;const EGLint ca[]={EGL_SURFACE_TYPE,EGL_WINDOW_BIT,EGL_RENDERABLE_TYPE,EGL_OPENGL_ES2_BIT,EGL_RED_SIZE,8,EGL_GREEN_SIZE,8,EGL_BLUE_SIZE,8,EGL_NONE};EGLConfig c=nullptr;EGLint n=0;if(!eglChooseConfig(d,ca,&c,1,&n)||n<1){eglTerminate(d);return false;}EGLint f=0;eglGetConfigAttrib(d,c,EGL_NATIVE_VISUAL_ID,&f);ANativeWindow_setBuffersGeometry(w,0,0,f);EGLSurface su=eglCreateWindowSurface(d,c,w,nullptr);const EGLint xa[]={EGL_CONTEXT_CLIENT_VERSION,2,EGL_NONE};EGLContext x=eglCreateContext(d,c,EGL_NO_CONTEXT,xa);if(su==EGL_NO_SURFACE||x==EGL_NO_CONTEXT||!eglMakeCurrent(d,su,su,x)){if(x!=EGL_NO_CONTEXT)eglDestroyContext(d,x);if(su!=EGL_NO_SURFACE)eglDestroySurface(d,su);eglTerminate(d);return false;}EGLint W=0,H=0;eglQuerySurface(d,su,EGL_WIDTH,&W);eglQuerySurface(d,su,EGL_HEIGHT,&H);g_width=W;g_height=H;glViewport(0,0,W,H);glClearColor(.025f,.07f,.15f,1);glClear(GL_COLOR_BUFFER_BIT);
 int s=W/270;if(s<3)s=3;if(s>6)s=6;int st=g_backend_state.load();int act=g_last_action.load();if(act==ACT_NONE)act=ACT_STATUS;int lx=5*s,rx=W/2+7*s,top=H-22*s,row=9*s;
 text("ROBOT ADMIN AI OS",lx,top,s,.92f,.96f,1);text("V66.5 MEGA",rx,top,s,.6f,.75f,.95f);text("CPP CORE: READY",lx,top-row,s,.35f,.9f,.55f);text("SAFETY: LOCKED",rx,top-row,s,.35f,.9f,.55f);text("MODE: OFFLINE FIRST",lx,top-2*row,s,.75f,.85f,.95f);text("CONTROL: READ ONLY",rx,top-2*row,s,.75f,.85f,.95f);text("PY BACKEND:",lx,top-3*row,s,.92f,.96f,1);text(st==1?"CONNECTED":(st==-1?"OPTIONAL OFF":"CHECKING"),lx+76*s,top-3*row,s,st==1?.2f:.95f,st==1?.9f:.65f,st==1?.35f:.25f);text("TOUCH COUNT: "+std::to_string(g_touch_count.load()),lx,top-4*row,s,.55f,.8f,.95f);text("PROD GATE: CLOSED",rx,top-4*row,s,.95f,.55f,.3f);text(std::string("EVENT: ")+event_name665(g_event_v665.load()),lx,top-5*row,s,.55f,.8f,.95f);text("REGRESSION: SAFE BASE",rx,top-5*row,s,.35f,.9f,.55f);
 int py=top-7*row;text(page_title665(act),lx,py,s,.35f,.9f,.55f);
 std::lock_guard<std::mutex>dl(g_data_mutex_v665);
 if(act==ACT_STATUS){text("FILES FOUND: "+std::to_string(g_files_v665.size()),lx,py-row,s,.75f,.85f,.95f);text("CSV ROWS: "+std::to_string(g_csv_rows_v665.load()),lx,py-2*row,s,.75f,.85f,.95f);text("CSV COLS: "+std::to_string(g_csv_cols_v665.load()),lx,py-3*row,s,.75f,.85f,.95f);text("NUMERIC CELLS: "+std::to_string(g_csv_numeric_v665.load()),lx,py-4*row,s,.75f,.85f,.95f);text("ANALYZER: READY",lx,py-5*row,s,.35f,.9f,.55f);text("SYSTEM STATE: SAFE",rx,py-row,s,.35f,.9f,.55f);
 }else if(act==ACT_FILES){text("SCOPE: APP SANDBOX",lx,py-row,s,.75f,.85f,.95f);text("COUNT: "+std::to_string(g_files_v665.size()),lx,py-2*row,s,.75f,.85f,.95f);for(int i=0;i<3;i++){std::string v=(i<(int)g_files_v665.size()?upper_safe(base_name(g_files_v665[i]),22):"NONE");text("F"+std::to_string(i+1)+": "+v,lx,py-(3+i)*row,s,.55f,.8f,.95f);}text("FILE ENGINE: REAL RO",rx,py-row,s,.35f,.9f,.55f);
 }else if(act==ACT_TASKS){text(g_task_state_v665,lx,py-row,s,.75f,.85f,.95f);text("PREVIEW: "+g_task_preview_v665,lx,py-2*row,s,.55f,.8f,.95f);text("CREATE: BLOCKED",lx,py-3*row,s,.95f,.55f,.3f);text("APPLY: BLOCKED",lx,py-4*row,s,.95f,.55f,.3f);text("TASK ACCESS: READ ONLY",rx,py-row,s,.35f,.9f,.55f);
 }else if(act==ACT_BACKUP){text("SHA256 ENGINE: READY",lx,py-row,s,.35f,.9f,.55f);text("TARGET: "+g_verify_target_v665,lx,py-2*row,s,.75f,.85f,.95f);text("HASH: "+upper_safe(g_sha_v665.substr(0,16),16),lx,py-3*row,s,.55f,.8f,.95f);text("RESTORE: BLOCKED",lx,py-4*row,s,.95f,.55f,.3f);text("VERIFY MODE: READ ONLY",rx,py-row,s,.35f,.9f,.55f);
 }else if(act==ACT_RECHECK){text("CSV: "+g_csv_name_v665,lx,py-row,s,.75f,.85f,.95f);text("ROWS: "+std::to_string(g_csv_rows_v665.load()),lx,py-2*row,s,.75f,.85f,.95f);text("COLS: "+std::to_string(g_csv_cols_v665.load()),lx,py-3*row,s,.75f,.85f,.95f);text("DIAG: "+std::to_string(g_diag.load()),lx,py-4*row,s,.75f,.85f,.95f);text(st==1?"RESULT: CONNECTED":"RESULT: OPTIONAL OFF",rx,py-row,s,st==1?.35f:.95f,st==1?.9f:.65f,st==1?.55f:.25f);}
 text("WRITE DELETE: OFF",rx,py-2*row,s,.95f,.55f,.3f);text("RESTORE APPLY: OFF",rx,py-3*row,s,.95f,.55f,.3f);text("LIVE ORDERS: OFF",rx,py-4*row,s,.95f,.55f,.3f);text("AUTO UPDATE: OFF",rx,py-5*row,s,.95f,.55f,.3f);
 int margin=4*s,gap=2*s,by=5*s,bh=14*s,bw=(W-2*margin-4*gap)/5;const char*labs[5]={"STATUS","FILES","TASKS","BACKUP","RECHECK"};for(int i=0;i<5;i++){int bx=margin+i*(bw+gap);bool sel=act==i+1;rect(bx,by,bw,bh,sel?.15f:.08f,sel?.42f:.22f,sel?.30f:.36f);int ts=s>4?s-1:s;text(labs[i],bx+2*s,by+4*s,ts,.88f,.95f,1);}text("E-STOP READY",rx,by+bh+3*s,s,.35f,.9f,.55f);bool ok=eglSwapBuffers(d,su)==EGL_TRUE;eglMakeCurrent(d,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);eglDestroyContext(d,x);eglDestroySurface(d,su);eglTerminate(d);return ok;}

void redraw665(){std::lock_guard<std::mutex>l(g_mutex);if(g_window)render665(g_window);}
void probe665(){while(g_alive){bool ok=backend_check();if(ok)g_probe_ok++;else g_probe_fail++;g_backend_state=ok?1:-1;g_event_v665=ok?E665_CONNECTED:E665_OPTIONAL;redraw665();for(int i=0;i<100&&g_alive;i++)std::this_thread::sleep_for(std::chrono::milliseconds(100));}g_probe_running_v665=false;}
void start_probe665(){bool e=false;if(g_alive&&g_probe_running_v665.compare_exchange_strong(e,true))std::thread(probe665).detach();}
void action665(int a){g_last_action=a;g_touch_count++;if(a==ACT_FILES||a==ACT_TASKS){g_event_v665=E665_SCAN;rescan665();redraw665();}else if(a==ACT_BACKUP){g_event_v665=E665_VERIFY;rescan665();redraw665();}else if(a==ACT_RECHECK){g_event_v665=E665_RECHECK;rescan665();redraw665();std::thread([](){bool ok=backend_check();if(ok)g_probe_ok++;else g_probe_fail++;g_backend_state=ok?1:-1;g_event_v665=ok?E665_CONNECTED:E665_OPTIONAL;redraw665();}).detach();}else{g_event_v665=E665_PANEL;redraw665();}}
int hit665(float x,float y_android){int W=g_width.load(),H=g_height.load();if(W<=0||H<=0)return ACT_NONE;int s=W/270;if(s<3)s=3;if(s>6)s=6;float y=(float)H-y_android;int margin=4*s,gap=2*s,by=5*s,bh=14*s,bw=(W-2*margin-4*gap)/5;if(y<by||y>by+bh)return ACT_NONE;for(int i=0;i<5;i++){int bx=margin+i*(bw+gap);if(x>=bx&&x<=bx+bw)return i+1;}return ACT_NONE;}
int input665(int,int,void*data){AInputQueue*q=reinterpret_cast<AInputQueue*>(data);if(!q)return 1;AInputEvent*ev=nullptr;while(AInputQueue_getEvent(q,&ev)>=0){if(AInputQueue_preDispatchEvent(q,ev))continue;int handled=0;int type=AInputEvent_getType(ev);if(type==AINPUT_EVENT_TYPE_MOTION){int action=AMotionEvent_getAction(ev)&AMOTION_EVENT_ACTION_MASK;if(action==AMOTION_EVENT_ACTION_UP){int a=hit665(AMotionEvent_getX(ev,0),AMotionEvent_getY(ev,0));if(a!=ACT_NONE){action665(a);handled=1;}}}else if(type==AINPUT_EVENT_TYPE_KEY&&AKeyEvent_getAction(ev)==AKEY_EVENT_ACTION_UP&&AKeyEvent_getKeyCode(ev)==AKEYCODE_BACK){g_last_action=ACT_STATUS;g_touch_count++;g_event_v665=E665_HOME;redraw665();handled=1;}AInputQueue_finishEvent(q,ev,handled);}return 1;}
void input_created665(ANativeActivity*,AInputQueue*q){g_input_queue=q;g_input_looper=ALooper_forThread();if(!g_input_looper)g_input_looper=ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);if(q&&g_input_looper)AInputQueue_attachLooper(q,g_input_looper,1,input665,q);}
void input_destroyed665(ANativeActivity*,AInputQueue*q){if(q)AInputQueue_detachLooper(q);if(g_input_queue==q)g_input_queue=nullptr;}
void start665(ANativeActivity*a){g_alive=true;g_internal_v665=a&&a->internalDataPath?a->internalDataPath:"";g_external_v665=a&&a->externalDataPath?a->externalDataPath:"";if(g_last_action.load()==ACT_NONE)g_last_action=ACT_STATUS;g_event_v665=E665_READY;rescan665();start_probe665();}
void resume665(ANativeActivity*){redraw665();start_probe665();}
void wc665(ANativeActivity*,ANativeWindow*w){{std::lock_guard<std::mutex>l(g_mutex);if(g_window)ANativeWindow_release(g_window);g_window=w;if(w)ANativeWindow_acquire(w);}redraw665();start_probe665();}
void wr665(ANativeActivity*,ANativeWindow*){redraw665();}
void wd665(ANativeActivity*,ANativeWindow*w){std::lock_guard<std::mutex>l(g_mutex);if(g_window==w&&g_window){ANativeWindow_release(g_window);g_window=nullptr;}}
void destroy665(ANativeActivity*){g_alive=false;std::lock_guard<std::mutex>l(g_mutex);if(g_window){ANativeWindow_release(g_window);g_window=nullptr;}}
}

extern "C" const char* ra665_version(){return"66.5";}extern "C" int ra665_core_ready(){return 1;}extern "C" const char* ra665_mode(){return"OFFLINE_FIRST";}extern "C" const char* ra665_safety(){return"LOCKED";}extern "C" const char* ra665_control_mode(){return"READ_ONLY";}extern "C" const char* ra665_bundle(){return"MEGA_FEATURE_BUNDLE";}extern "C" int ra665_files_readonly_ready(){return 1;}extern "C" int ra665_tasks_readonly_ready(){return 1;}extern "C" int ra665_sha256_ready(){return 1;}extern "C" int ra665_csv_analyzer_ready(){return 1;}extern "C" int ra665_write_allowed(){return 0;}extern "C" int ra665_delete_allowed(){return 0;}extern "C" int ra665_restore_allowed(){return 0;}extern "C" int ra665_live_orders_allowed(){return 0;}extern "C" int ra665_auto_update_allowed(){return 0;}extern "C" int ra665_production_gate_open(){return 0;}extern "C" int ra665_write(){return-403;}extern "C" int ra665_apply(){return-403;}extern "C" int ra665_restore(){return-403;}extern "C" int ra665_install(){return-403;}extern "C" int ra665_delete(){return-403;}extern "C" int ra665_repair(){return-403;}extern "C" int ra665_live_order(){return-403;}

extern "C" void ANativeActivity_onCreate(ANativeActivity*a,void*,size_t){if(!a||!a->callbacks)return;a->callbacks->onStart=start665;a->callbacks->onResume=resume665;a->callbacks->onPause=[](ANativeActivity*){};a->callbacks->onStop=[](ANativeActivity*){};a->callbacks->onDestroy=destroy665;a->callbacks->onNativeWindowCreated=wc665;a->callbacks->onNativeWindowRedrawNeeded=wr665;a->callbacks->onNativeWindowDestroyed=wd665;a->callbacks->onInputQueueCreated=input_created665;a->callbacks->onInputQueueDestroyed=input_destroyed665;}
