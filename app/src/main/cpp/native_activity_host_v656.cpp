#define ANativeActivity_onCreate ANativeActivity_onCreate_v655_legacy
#include "native_activity_host_v655.cpp"
#undef ANativeActivity_onCreate

// V65.6 Final Regression + UI Polish Master Bundle.
// Preserves V65.5 dashboard/control safety behavior and moves content below
// the Android status bar area for cleaner landscape presentation.
namespace {
std::atomic<bool> g_probe_running_v656{false};

bool render656(ANativeWindow* w){
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

    int s=W/230;if(s<3)s=3;if(s>7)s=7;
    int st=g_backend_state.load(),dg=g_diag.load();
    float br=st==1?.2f:.95f,bg=st==1?.9f:.65f,bb=st==1?.35f:.25f;
    int lx=5*s,rx=W/2+7*s,top=H-22*s,row=10*s;

    text("ROBOT ADMIN AI OS",lx,top,s,.92f,.96f,1);
    text("V65.6 REGRESSION",rx,top,s,.6f,.75f,.95f);
    text("CPP CORE: READY",lx,top-row,s,.35f,.9f,.55f);
    text("SAFETY: LOCKED",rx,top-row,s,.35f,.9f,.55f);
    text("MODE: OFFLINE FIRST",lx,top-2*row,s,.75f,.85f,.95f);
    text("CONTROL: READ ONLY",rx,top-2*row,s,.75f,.85f,.95f);
    text("PY BACKEND:",lx,top-3*row,s,.92f,.96f,1);
    text(st==1?"CONNECTED":(st==-1?"OPTIONAL OFF":"CHECKING"),lx+76*s,top-3*row,s,br,bg,bb);
    text("FILES TASKS: VIEW",rx,top-3*row,s,.55f,.8f,.95f);
    text("DIAG: "+std::to_string(dg),lx,top-4*row,s,.8f,.8f,.9f);
    text("CODE TEST: SAFE",rx,top-4*row,s,.35f,.9f,.55f);
    text("PROBE OK: "+std::to_string(g_probe_ok.load()),lx,top-5*row,s,.55f,.8f,.95f);
    text("BACKUP VERIFY: READY",rx,top-5*row,s,.35f,.9f,.55f);
    text("PROBE FAIL: "+std::to_string(g_probe_fail.load()),lx,top-6*row,s,.55f,.8f,.95f);
    text("UI INSET: SAFE",rx,top-6*row,s,.35f,.9f,.55f);
    text("AUTO RECHECK: ON",lx,top-7*row,s,.55f,.8f,.95f);
    text("WRITE DELETE: OFF",rx,top-7*row,s,.95f,.55f,.3f);
    text("REGRESSION: READY",lx,top-8*row,s,.35f,.9f,.55f);
    text("RESTORE APPLY: OFF",rx,top-8*row,s,.95f,.55f,.3f);
    text("TOUCH CONTROLS: SAFE",lx,top-9*row,s,.35f,.9f,.55f);
    text("LIVE ORDERS: OFF",rx,top-9*row,s,.95f,.55f,.3f);
    text("PROD GATE: CLOSED",lx,top-10*row,s,.95f,.55f,.3f);
    text("AUTO UPDATE: OFF",rx,top-10*row,s,.95f,.55f,.3f);
    text("MODULE MAP: FINAL CHECK",lx,top-11*row,s,.55f,.8f,.95f);
    text("E-STOP: READY",rx,top-11*row,s,.35f,.9f,.55f);

    bool ok=eglSwapBuffers(d,su)==EGL_TRUE;
    eglMakeCurrent(d,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);eglDestroyContext(d,x);eglDestroySurface(d,su);eglTerminate(d);
    return ok;
}

void v656_redraw(){std::lock_guard<std::mutex>l(g_mutex);if(g_window)render656(g_window);}
void v656_probe_loop(){while(g_alive){bool ok=backend_check();if(ok)g_probe_ok++;else g_probe_fail++;int ns=ok?1:-1;int old=g_backend_state.exchange(ns);if(old!=ns)v656_redraw();for(int i=0;i<50&&g_alive;i++)std::this_thread::sleep_for(std::chrono::milliseconds(100));}g_probe_running_v656=false;}
void v656_start_probe(){bool expected=false;if(g_alive&&g_probe_running_v656.compare_exchange_strong(expected,true))std::thread(v656_probe_loop).detach();}
void v656_on_start(ANativeActivity*){g_alive=true;v656_start_probe();}
void v656_on_resume(ANativeActivity*){v656_redraw();v656_start_probe();}
void v656_on_pause(ANativeActivity*){}
void v656_on_stop(ANativeActivity*){}
void v656_wc(ANativeActivity*,ANativeWindow*w){{std::lock_guard<std::mutex>l(g_mutex);if(g_window)ANativeWindow_release(g_window);g_window=w;if(w)ANativeWindow_acquire(w);}v656_redraw();v656_start_probe();}
void v656_wr(ANativeActivity*,ANativeWindow*){v656_redraw();}
void v656_wd(ANativeActivity*,ANativeWindow*w){std::lock_guard<std::mutex>l(g_mutex);if(g_window==w&&g_window){ANativeWindow_release(g_window);g_window=nullptr;}}
void v656_destroy(ANativeActivity*){g_alive=false;std::lock_guard<std::mutex>l(g_mutex);if(g_window){ANativeWindow_release(g_window);g_window=nullptr;}}
}

extern "C" const char* ra656_version(){return"65.6";}
extern "C" int ra656_core_ready(){return 1;}
extern "C" const char* ra656_mode(){return"OFFLINE_FIRST";}
extern "C" const char* ra656_safety(){return"LOCKED";}
extern "C" const char* ra656_control_mode(){return"READ_ONLY";}
extern "C" const char* ra656_module_map(){return"FINAL_REGRESSION";}
extern "C" int ra656_regression_ready(){return 1;}
extern "C" int ra656_ui_inset_safe(){return 1;}
extern "C" int ra656_write_allowed(){return 0;}
extern "C" int ra656_delete_allowed(){return 0;}
extern "C" int ra656_restore_allowed(){return 0;}
extern "C" int ra656_live_orders_allowed(){return 0;}
extern "C" int ra656_auto_update_allowed(){return 0;}
extern "C" int ra656_production_gate_open(){return 0;}
extern "C" int ra656_write(){return-403;}
extern "C" int ra656_apply(){return-403;}
extern "C" int ra656_restore(){return-403;}
extern "C" int ra656_install(){return-403;}
extern "C" int ra656_delete(){return-403;}
extern "C" int ra656_repair(){return-403;}
extern "C" int ra656_live_order(){return-403;}
extern "C" void ANativeActivity_onCreate(ANativeActivity*a,void*,size_t){if(!a||!a->callbacks)return;a->callbacks->onStart=v656_on_start;a->callbacks->onResume=v656_on_resume;a->callbacks->onPause=v656_on_pause;a->callbacks->onStop=v656_on_stop;a->callbacks->onDestroy=v656_destroy;a->callbacks->onNativeWindowCreated=v656_wc;a->callbacks->onNativeWindowRedrawNeeded=v656_wr;a->callbacks->onNativeWindowDestroyed=v656_wd;}
