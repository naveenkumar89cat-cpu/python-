// ROBOT ADMIN AI OS — V68.7 FAST MASTER INTEGRATED HOST
// Android-only, offline-first, read-only.
// Direct proven V65.8 NativeActivity base; no nested later wrappers.

#define ANativeActivity_onCreate ANativeActivity_onCreate_v658_legacy
#include "native_activity_host_v658.cpp"
#undef ANativeActivity_onCreate

#include "sha256_v680.hpp"
#include "unified_engine_v680.cpp"
#include "file_intelligence_v681.cpp"
#include "task_intelligence_v682.cpp"
#include "data_intelligence_v683.cpp"
#include "backup_recovery_v684.cpp"
#include "error_diagnostics_v685.cpp"
#include "code_admin_intelligence_v686.cpp"
#include "master_regression_v687.cpp"

#include <atomic>
#include <mutex>
#include <thread>

namespace {
std::atomic<bool> run687{false};
std::atomic<int> page687{0}, touch687{0};
AInputQueue* iq687=nullptr;
ANativeWindow* win687=nullptr;
std::thread th687;
int W687=0,H687=0;
std::mutex mu687;

enum Page687 { HOME=0, FILES=1, TASKS=2, DATA=3, BACKUP=4, DIAG=5, CODE=6, MASTER=7 };

struct FastState687 {
    ra680::State base;
    ra681::FileReport files;
    ra682::TaskReport tasks;
    ra683::CsvReport data;
    ra684::BackupReport backup;
    ra685::DiagnosticReport diag;
    ra686::ProjectReport code;
    ra687::RegressionReport regression;
};

FastState687 st687;

static void load687(ANativeActivity* a) {
    std::lock_guard<std::mutex> g(mu687);
    st687=FastState687{};
    AAssetManager* m=a?a->assetManager:nullptr;
    st687.base=ra680::load(m,sha256_680);

    std::string tasks_text,csv_text,verify_text;
    const bool asset_tasks=ra680::read_asset(m,"v673/tasks.txt",tasks_text);
    const bool asset_csv=ra680::read_asset(m,"v673/sample.csv",csv_text);
    const bool asset_verify=ra680::read_asset(m,"v673/verify.txt",verify_text);

    std::vector<ra681::FileItem> file_items;
    if(asset_tasks) file_items.push_back({"tasks.txt","v673/tasks.txt",static_cast<std::uint64_t>(tasks_text.size()),false,true,"txt"});
    if(asset_csv) file_items.push_back({"sample.csv","v673/sample.csv",static_cast<std::uint64_t>(csv_text.size()),false,true,"csv"});
    if(asset_verify) file_items.push_back({"verify.txt","v673/verify.txt",static_cast<std::uint64_t>(verify_text.size()),false,true,"txt"});
    st687.files=ra681::summarize(file_items);

    std::size_t invalid_tasks=0;
    bool task_cut=false;
    const auto parsed_tasks=ra682::parse_text(tasks_text,&invalid_tasks,&task_cut);
    st687.tasks=ra682::summarize(parsed_tasks,invalid_tasks,task_cut);
    st687.data=ra683::analyze_csv(csv_text);

    std::vector<ra684::BackupItem> backups;
    if(asset_verify){
        ra684::BackupItem b;
        b.id="V673_VERIFY_ASSET";
        b.path="v673/verify.txt";
        b.expected_sha256=ra680::EXPECTED_VERIFY_SHA256;
        b.actual_sha256=sha256_680(reinterpret_cast<const unsigned char*>(verify_text.data()),verify_text.size());
        b.size=static_cast<std::uint64_t>(verify_text.size());
        b.exists=true;
        b.readable=true;
        backups.push_back(b);
    }
    st687.backup=ra684::summarize(backups);

    std::vector<ra686::SourceFile> source_meta={
        {"file_intelligence_v681.cpp","",0,true},
        {"task_intelligence_v682.cpp","",0,true},
        {"data_intelligence_v683.cpp","",0,true},
        {"backup_recovery_v684.cpp","",0,true},
        {"error_diagnostics_v685.cpp","",0,true},
        {"code_admin_intelligence_v686.cpp","",0,true},
        {"master_regression_v687.cpp","",0,true}
    };
    st687.code=ra686::summarize_project(source_meta);

    const bool file_ok=st687.files.total==3 && ra681::ra681_file_write_blocked()==-403 && ra681::ra681_file_delete_blocked()==-403;
    const bool task_ok=st687.tasks.total==3 && st687.tasks.invalid==0 && ra682::ra682_task_execute_blocked()==-403;
    const bool data_ok=st687.data.rows>0 && st687.data.invalid_rows==0 && ra683::ra683_data_write_blocked()==-403;
    const bool backup_ok=st687.backup.verified==1 && st687.backup.mismatch==0 && ra684::ra684_restore_blocked()==-403;
    const bool code_ok=st687.code.total_files==7 && ra686::ra686_code_modify_blocked()==-403 && ra686::ra686_compile_execute_blocked()==-403;

    std::vector<ra685::ModuleHealth> health={
        {"FILE",file_ok,file_ok,file_ok?ra685::Code::OK:ra685::Code::MODULE_FAILED,file_ok?"READY":"FILE CHECK FAILED"},
        {"TASK",task_ok,task_ok,task_ok?ra685::Code::OK:ra685::Code::TASK_INVALID,task_ok?"READY":"TASK CHECK FAILED"},
        {"DATA",data_ok,data_ok,data_ok?ra685::Code::OK:ra685::Code::CSV_INVALID,data_ok?"READY":"DATA CHECK FAILED"},
        {"BACKUP",backup_ok,backup_ok,backup_ok?ra685::Code::OK:ra685::Code::HASH_MISMATCH,backup_ok?"READY":"VERIFY CHECK FAILED"},
        {"CODE",code_ok,code_ok,code_ok?ra685::Code::OK:ra685::Code::MODULE_FAILED,code_ok?"READY":"CODE CHECK FAILED"}
    };
    st687.diag=ra685::summarize(health);
    const bool diag_ok=st687.diag.system_ok;

    st687.regression=ra687::run_core_regression(
        file_ok,task_ok,data_ok,backup_ok,diag_ok,code_ok,
        ra681::ra681_file_write_blocked()==-403 && ra683::ra683_data_write_blocked()==-403,
        ra681::ra681_file_delete_blocked()==-403 && ra682::ra682_task_delete_blocked()==-403,
        ra684::ra684_restore_blocked()==-403,
        ra684::ra684_apply_rollback_blocked()==-403 && ra685::ra685_apply_fix_blocked()==-403,
        ra680_live_order()==-403,
        ra680_auto_update()==-403,
        true);
}

static const char* title687(int p){switch(p){case FILES:return"V68.1 FILE INTELLIGENCE";case TASKS:return"V68.2 TASK INTELLIGENCE";case DATA:return"V68.3 DATA INTELLIGENCE";case BACKUP:return"V68.4 BACKUP VERIFY";case DIAG:return"V68.5 DIAGNOSTICS";case CODE:return"V68.6 CODE ADMIN";case MASTER:return"V68.7 MASTER REGRESSION";default:return"V68.7 FAST MASTER";}}

static bool draw687(ANativeWindow*w){
    if(!w)return false;
    EGLDisplay d=eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(d==EGL_NO_DISPLAY||!eglInitialize(d,nullptr,nullptr))return false;
    const EGLint ca[]={EGL_SURFACE_TYPE,EGL_WINDOW_BIT,EGL_RENDERABLE_TYPE,EGL_OPENGL_ES2_BIT,EGL_RED_SIZE,8,EGL_GREEN_SIZE,8,EGL_BLUE_SIZE,8,EGL_NONE};
    EGLConfig c=nullptr; EGLint n=0;
    if(!eglChooseConfig(d,ca,&c,1,&n)||n<1){eglTerminate(d);return false;}
    EGLint f=0; eglGetConfigAttrib(d,c,EGL_NATIVE_VISUAL_ID,&f); ANativeWindow_setBuffersGeometry(w,0,0,f);
    EGLSurface su=eglCreateWindowSurface(d,c,w,nullptr);
    const EGLint xa[]={EGL_CONTEXT_CLIENT_VERSION,2,EGL_NONE};
    EGLContext x=eglCreateContext(d,c,EGL_NO_CONTEXT,xa);
    if(su==EGL_NO_SURFACE||x==EGL_NO_CONTEXT||!eglMakeCurrent(d,su,su,x)){
        if(x!=EGL_NO_CONTEXT)eglDestroyContext(d,x);
        if(su!=EGL_NO_SURFACE)eglDestroySurface(d,su);
        eglTerminate(d); return false;
    }
    EGLint W=0,H=0; eglQuerySurface(d,su,EGL_WIDTH,&W); eglQuerySurface(d,su,EGL_HEIGHT,&H); W687=W; H687=H;
    glViewport(0,0,W,H); glClearColor(.025f,.07f,.15f,1); glClear(GL_COLOR_BUFFER_BIT);
    int s=W/330;if(s<3)s=3;if(s>5)s=5;int lx=5*s,rx=W/2+14*s,top=H-18*s,row=8*s,p=page687.load();
    text("ROBOT ADMIN AI OS",lx,top,s,.92f,.96f,1); text("FAST MASTER V68.7",rx,top,s,.6f,.75f,.95f);
    text("CPP CORE: INTEGRATED",lx,top-row,s,.35f,.9f,.55f); text("SAFETY: LOCKED",rx,top-row,s,.35f,.9f,.55f);
    text("MODE: OFFLINE FIRST",lx,top-2*row,s,.75f,.85f,.95f); text("CONTROL: READ ONLY",rx,top-2*row,s,.75f,.85f,.95f);
    text("TOUCH: "+std::to_string(touch687.load()),lx,top-3*row,s,.55f,.8f,.95f); text("PROD GATE: CLOSED",rx,top-3*row,s,.95f,.55f,.3f);
    text("WRITE/DELETE/RESTORE/APPLY: OFF",rx,top-4*row,s,.95f,.55f,.3f);
    int py=top-5*row; text(title687(p),lx,py,s,.35f,.9f,.55f);

    std::lock_guard<std::mutex>g(mu687);const auto&z=st687;
    if(p==HOME){
        text("ASSETS: "+std::to_string(z.base.assets)+" / 3",lx,py-row,s,.75f,.85f,.95f);
        text("TASKS: "+std::to_string(z.tasks.total)+" INVALID: "+std::to_string(z.tasks.invalid),lx,py-2*row,s,.75f,.85f,.95f);
        text("CSV ROWS: "+std::to_string(z.data.rows)+" INVALID: "+std::to_string(z.data.invalid_rows),lx,py-3*row,s,.75f,.85f,.95f);
        text("MASTER: "+std::string(ra687::overall_pass(z.regression)?"PASS":"FAIL"),lx,py-4*row,s,ra687::overall_pass(z.regression)?.35f:.95f,ra687::overall_pass(z.regression)?.9f:.55f,.3f);
    }else if(p==FILES){
        text("FILES: "+std::to_string(z.files.files)+" FOLDERS: "+std::to_string(z.files.folders),lx,py-row,s,.75f,.85f,.95f);
        text("READABLE: "+std::to_string(z.files.readable)+" BYTES: "+std::to_string(z.files.bytes),lx,py-2*row,s,.75f,.85f,.95f);
        text("WRITE/DELETE: BLOCKED",lx,py-3*row,s,.95f,.55f,.3f);
    }else if(p==TASKS){
        text("TOTAL: "+std::to_string(z.tasks.total)+" INVALID: "+std::to_string(z.tasks.invalid),lx,py-row,s,.75f,.85f,.95f);
        text("PENDING: "+std::to_string(z.tasks.pending)+" NORMAL: "+std::to_string(z.tasks.normal),lx,py-2*row,s,.75f,.85f,.95f);
        text("EXECUTE/UPDATE: BLOCKED",lx,py-3*row,s,.95f,.55f,.3f);
    }else if(p==DATA){
        text("ROWS: "+std::to_string(z.data.rows)+" COLS: "+std::to_string(z.data.cols),lx,py-row,s,.75f,.85f,.95f);
        text("NUMERIC: "+std::to_string(z.data.numeric_cells)+" INVALID: "+std::to_string(z.data.invalid_rows),lx,py-2*row,s,.75f,.85f,.95f);
        text("DUPLICATES: "+std::to_string(z.data.duplicate_rows),lx,py-3*row,s,.75f,.85f,.95f);
    }else if(p==BACKUP){
        text("VERIFIED: "+std::to_string(z.backup.verified),lx,py-row,s,.75f,.85f,.95f);
        text("MISMATCH: "+std::to_string(z.backup.mismatch)+" MISSING: "+std::to_string(z.backup.missing),lx,py-2*row,s,.75f,.85f,.95f);
        text("RESTORE/APPLY: BLOCKED",lx,py-3*row,s,.95f,.55f,.3f);
    }else if(p==DIAG){
        text("READY: "+std::to_string(z.diag.ready)+" / "+std::to_string(z.diag.total),lx,py-row,s,.75f,.85f,.95f);
        text("ERRORS: "+std::to_string(z.diag.errors)+" CRITICAL: "+std::to_string(z.diag.critical),lx,py-2*row,s,.75f,.85f,.95f);
        text("SYSTEM: "+std::string(z.diag.system_ok?"OK":"CHECK"),lx,py-3*row,s,z.diag.system_ok?.35f:.95f,z.diag.system_ok?.9f:.55f,.3f);
    }else if(p==CODE){
        text("MODULE FILES: "+std::to_string(z.code.total_files),lx,py-row,s,.75f,.85f,.95f);
        text("C++ DETECTED: "+std::to_string(z.code.language_counts.count("C++")?z.code.language_counts.at("C++"):0),lx,py-2*row,s,.75f,.85f,.95f);
        text("COMPILE/APPLY: BLOCKED",lx,py-3*row,s,.95f,.55f,.3f);
    }else{
        text("CHECKS: "+std::to_string(z.regression.passed)+" / "+std::to_string(z.regression.total),lx,py-row,s,.75f,.85f,.95f);
        text("FAILED: "+std::to_string(z.regression.failed),lx,py-2*row,s,.75f,.85f,.95f);
        text("SAFETY: "+std::string(z.regression.safety_locked?"LOCKED":"FAIL"),lx,py-3*row,s,.75f,.85f,.95f);
        text("RESULT: "+std::string(ra687::overall_pass(z.regression)?"PASS":"FAIL"),lx,py-4*row,s,ra687::overall_pass(z.regression)?.35f:.95f,ra687::overall_pass(z.regression)?.9f:.55f,.3f);
    }

    int gap=s,bh=12*s,by=2*s,bw=(W-9*gap)/8;const char*bn[8]={"HOME","FILES","TASKS","DATA","BACKUP","DIAG","CODE","MASTER"};
    for(int i=0;i<8;i++){float gg=(p==i)?.48f:.27f;rect(gap+i*(bw+gap),by,bw,bh,.08f,gg,.28f);text(bn[i],gap+i*(bw+gap)+s,by+4*s,s,.92f,.96f,1);}
    eglSwapBuffers(d,su);eglMakeCurrent(d,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);eglDestroyContext(d,x);eglDestroySurface(d,su);eglTerminate(d);return true;
}

static int cb687(int,int,void*){if(!iq687)return 1;AInputEvent*e=nullptr;while(AInputQueue_getEvent(iq687,&e)>=0){if(AInputQueue_preDispatchEvent(iq687,e))continue;int h=0;if(AInputEvent_getType(e)==AINPUT_EVENT_TYPE_MOTION&&(AMotionEvent_getAction(e)&AMOTION_EVENT_ACTION_MASK)==AMOTION_EVENT_ACTION_UP){float px=AMotionEvent_getX(e,0),py=(float)H687-AMotionEvent_getY(e,0);int s=W687/330;if(s<3)s=3;if(s>5)s=5;int gap=s,bh=12*s,bw=(W687-9*gap)/8;if(py>=2*s&&py<=2*s+bh)for(int i=0;i<8;i++){int bx=gap+i*(bw+gap);if(px>=bx&&px<=bx+bw){page687=i;touch687++;h=1;break;}}}else if(AInputEvent_getType(e)==AINPUT_EVENT_TYPE_KEY&&AKeyEvent_getAction(e)==AKEY_EVENT_ACTION_UP&&AKeyEvent_getKeyCode(e)==AKEYCODE_BACK){page687=HOME;touch687++;h=1;}AInputQueue_finishEvent(iq687,e,h);}return 1;}
static void loop687(){ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);while(run687){if(win687)draw687(win687);ALooper_pollOnce(120,nullptr,nullptr,nullptr);}}
static void wc687(ANativeActivity*,ANativeWindow*w){win687=w;}static void wd687(ANativeActivity*,ANativeWindow*){win687=nullptr;}static void qc687(ANativeActivity*,AInputQueue*q){iq687=q;ALooper*l=ALooper_forThread();if(!l)l=ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);AInputQueue_attachLooper(q,l,1,cb687,nullptr);}static void qd687(ANativeActivity*,AInputQueue*q){AInputQueue_detachLooper(q);if(iq687==q)iq687=nullptr;}static void destroy687(ANativeActivity*){run687=false;if(th687.joinable())th687.join();}
}

extern "C" void ANativeActivity_onCreate(ANativeActivity*a,void*,size_t){load687(a);a->callbacks->onNativeWindowCreated=wc687;a->callbacks->onNativeWindowDestroyed=wd687;a->callbacks->onInputQueueCreated=qc687;a->callbacks->onInputQueueDestroyed=qd687;a->callbacks->onDestroy=destroy687;run687=true;th687=std::thread(loop687);}
extern "C" const char* ra687_host_version(){return "68.7-fast-master-integrated-readonly";}
