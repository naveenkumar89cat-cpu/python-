#define ANativeActivity_onCreate ANativeActivity_onCreate_v654_legacy
#include "native_activity_host_v654.cpp"
#undef ANativeActivity_onCreate

// V65.5 Dashboard + Controls Master Bundle.
// Preserves V65.4 proven offline-first core and safety locks.
// Adds a larger aligned dashboard and read-only control foundation.
namespace {
std::atomic<bool> g_probe_running_v655{false};

bool render655(ANativeWindow* w){
    if(!w)return false;
    EGLDisplay d=eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(d==EGL_NO_DISPLAY||!eglInitialize(d,nullptr,nullptr))return false;
    const EGLint ca[]={EGL_SURFACE_TYPE,EGL_WINDOW_BIT,EGL_RENDERABLE_TYPE,EGL_OPENGL_ES2_BIT,EGL_RED_SIZE,8,EGL_GREEN_SIZE,8,EGL_BLUE_SIZE,8,EGL_NONE};
    EGLConfig c=nullptr;EGLint n=0;
    if(!eglChooseConfig(d,ca,&c,1,&n)||n<1){eglTerminate(d);return false;}
    EGLint f=0;eglGetConfigAttrib(d,c,EGL_NATIVE_VISUAL_ID,&f);ANativeWindow_setBuffersGeometry(w,0,0,f);
    EGLSurface su=eglCreateWindowSurface(d,c,w,nullptr);
    const EGLint xa[]={EGL_CONTEXT_CLIENT_VERSION,2,EGL_NONE};
    EGLContext x=eglCreateContext(d,c,EGL_NO_CONTEXT,xa);
    if(su==EGL_NO_SURFACE||x==EGL_NO_CONTEXT||!eglMakeCurrent(d,su,su,x)){
        if(x!=EGL_NO_CONTEXT)eglDestroyContext(d,x);if(su!=EGL_NO_SURFACE)eglDestroySurface(d,su);eglTerminate(d);return false;
    }
    EGLint W=0,H=0;eglQuerySurface(d,su,EGL_WIDTH,&W);eglQuerySurface(d,su,EGL_HEIGHT,&H);
    glViewport(0,0,W,H);glClearColor(.025f,.07f,.15f,1);glClear(GL_COLOR_BUFFER_BIT);

    int s=W/220;if(s<3)s=3;if(s>8)s=8;
    int st=g_backend_state.load(),dg=g_diag.load();
    float br=st==1?.2f:.95f,bg=st==1?.9f:.65f,bb=st==1?.35f:.25f;
    int lx=4*s,rx=W/2+5*s,top=H-13*s,row=12*s;

    text("ROBOT ADMIN AI OS",lx,top,s,.92f,.96f,1);
    text("V65.5 DASHBOARD",rx,top,s,.6f,.75f,.95f);

    text("CPP CORE: READY",lx,top-row,s,.35f,.9f,.55f);
    text("SAFETY: LOCKED",rx,top-row,s,.35f,.9f,.55f);

    text("MODE: OFFLINE FIRST",lx,top-2*row,s,.75f,.85f,.95f);
    text("CONTROL MODE: READ ONLY",rx,top-2*row,s,.75f,.85f,.95f);

    text("PY BACKEND:",lx,top-3*row,s,.92f,.96f,1);
    text(st==1?"CONNECTED":(st==-1?"OPTIONAL OFF":"CHECKING"),lx+76*s,top-3*row,s,br,bg,bb);
    text("FILES: VIEW ONLY",rx,top-3*row,s,.55f,.8f,.95f);

    text("DIAG: "+std::to_string(dg),lx,top-4*row,s,.8f,.8f,.9f);
    text("TASKS: VIEW ONLY",rx,top-4*row,s,.55f,.8f,.95f);

    text("PROBE OK: "+std::to_string(g_probe_ok.load()),lx,top-5*row,s,.55f,.8f,.95f);
    text("CODE TEST: SAFE",rx,top-5*row,s,.35f,.9f,.55f);

    text("PROBE FAIL: "+std::to_string(g_probe_fail.load()),lx,top-6*row,s,.55f,.8f,.95f);
    text("BACKUP VERIFY: READY",rx,top-6*row,s,.35f,.9f,.55f);

    text("AUTO RECHECK: ON",lx,top-7*row,s,.55f,.8f,.95f);
    text("WRITE DELETE: OFF",rx,top-7*row,s,.95f,.55f,.3f);

    text("MODULE MAP: DASHBOARD",lx,top-8*row,s,.55f,.8f,.95f);
    text("RESTORE APPLY: OFF",rx,top-8*row,s,.95f,.55f,.3f);

    text("TOUCH CONTROLS: SAFE",lx,top-9*row,s,.35f,.9f,.55f);
    text("LIVE ORDERS UPDATE: OFF",rx,top-9*row,s,.95f,.55f,.3f);

    text("PROD GATE: CLOSED",lx,top-10*row,s,.95f,.55f,.3f);
    text("E-STOP: READY",rx,top-10*row,s,.35f,.9f,.55f);

    bool ok=eglSwapBuffers(d,su)==EGL_TRUE;
    eglMakeCurrent(d,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);eglDestroyContext(d,x);eglDestroySurface(d,su);eglTerminate(d);
    return ok;
}

void v655_redraw(){std::lock_guard<std::mutex>l(g_mutex);if(g_window)render655(g_window);}
void v655_probe_loop(){
    while(g_alive){
        bool ok=backend_check();if(ok)g_probe_ok++;else g_probe_fail++;
        int ns=ok?1:-1;int old=g_backend_state.exchange(ns);if(old!=ns)v655_redraw();
        for(int i=0;i<50&&g_alive;i++)std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    g_probe_running_v655=false;
}
void v655_start_probe(){bool expected=false;if(g_alive&&g_probe_running_v655.compare_exchange_strong(expected,true))std::thread(v655_probe_loop).detach();}
void v655_on_start(ANativeActivity*){g_alive=true;v655_start_probe();}
void v655_on_resume(ANativeActivity*){v655_redraw();v655_start_probe();}
void v655_on_pause(ANativeActivity*){}
void v655_on_stop(ANativeActivity*){}
void v655_wc(ANativeActivity*,ANativeWindow*w){{std::lock_guard<std::mutex>l(g_mutex);if(g_window)ANativeWindow_release(g_window);g_window=w;if(w)ANativeWindow_acquire(w);}v655_redraw();v655_start_probe();}
void v655_wr(ANativeActivity*,ANativeWindow*){v655_redraw();}
void v655_wd(ANativeActivity*,ANativeWindow*w){std::lock_guard<std::mutex>l(g_mutex);if(g_window==w&&g_window){ANativeWindow_release(g_window);g_window=nullptr;}}
void v655_destroy(ANativeActivity*){g_alive=false;std::lock_guard<std::mutex>l(g_mutex);if(g_window){ANativeWindow_release(g_window);g_window=nullptr;}}
}

extern "C" const char* ra655_version(){return"65.5";}
extern "C" int ra655_core_ready(){return 1;}
extern "C" const char* ra655_mode(){return"OFFLINE_FIRST";}
extern "C" const char* ra655_safety(){return"LOCKED";}
extern "C" const char* ra655_module_map(){return"DASHBOARD_CONTROLS";}
extern "C" const char* ra655_control_mode(){return"READ_ONLY";}
extern "C" int ra655_files_view_allowed(){return 1;}
extern "C" int ra655_tasks_view_allowed(){return 1;}
extern "C" int ra655_code_test_safe(){return 1;}
extern "C" int ra655_backup_verify_ready(){return 1;}
extern "C" int ra655_write_allowed(){return 0;}
extern "C" int ra655_delete_allowed(){return 0;}
extern "C" int ra655_restore_allowed(){return 0;}
extern "C" int ra655_live_orders_allowed(){return 0;}
extern "C" int ra655_auto_update_allowed(){return 0;}
extern "C" int ra655_production_gate_open(){return 0;}
extern "C" int ra655_write(){return-403;}
extern "C" int ra655_apply(){return-403;}
extern "C" int ra655_restore(){return-403;}
extern "C" int ra655_install(){return-403;}
extern "C" int ra655_delete(){return-403;}
extern "C" int ra655_repair(){return-403;}
extern "C" int ra655_live_order(){return-403;}

extern "C" void ANativeActivity_onCreate(ANativeActivity*a,void*,size_t){
    if(!a||!a->callbacks)return;
    a->callbacks->onStart=v655_on_start;
    a->callbacks->onResume=v655_on_resume;
    a->callbacks->onPause=v655_on_pause;
    a->callbacks->onStop=v655_on_stop;
    a->callbacks->onDestroy=v655_destroy;
    a->callbacks->onNativeWindowCreated=v655_wc;
    a->callbacks->onNativeWindowRedrawNeeded=v655_wr;
    a->callbacks->onNativeWindowDestroyed=v655_wd;
}
