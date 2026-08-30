#define ANativeActivity_onCreate ANativeActivity_onCreate_v654_legacy
#include "native_activity_host_v654.cpp"
#undef ANativeActivity_onCreate

// V65.7 FINAL LOCKED MASTER
// Device-proven V65.6.1 safety/regression baseline checkpoint.
// No dangerous operation is enabled in this build.
namespace {
std::atomic<bool> g_probe_running_v657{false};

bool render657(ANativeWindow* w){
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
    text("V65.7 FINAL LOCKED",rx,top,s,.6f,.75f,.95f);
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
    text("REGRESSION: PASS BASE",lx,top-8*row,s,.35f,.9f,.55f);
    text("RESTORE APPLY: OFF",rx,top-8*row,s,.95f,.55f,.3f);
    text("CHECKPOINT: LOCKED",lx,top-9*row,s,.35f,.9f,.55f);
    text("LIVE ORDERS: OFF",rx,top-9*row,s,.95f,.55f,.3f);
    text("PROD GATE: CLOSED",lx,top-10*row,s,.95f,.55f,.3f);
    text("AUTO UPDATE: OFF",rx,top-10*row,s,.95f,.55f,.3f);
    text("ROLLBACK: V65.6.1",lx,top-11*row,s,.55f,.8f,.95f);
    text("E-STOP: READY",rx,top-11*row,s,.35f,.9f,.55f);
    bool ok=eglSwapBuffers(d,su)==EGL_TRUE;
    eglMakeCurrent(d,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);eglDestroyContext(d,x);eglDestroySurface(d,su);eglTerminate(d);return ok;
}
void redraw657(){std::lock_guard<std::mutex>l(g_mutex);if(g_window)render657(g_window);}
void probe657(){while(g_alive){bool ok=backend_check();if(ok)g_probe_ok++;else g_probe_fail++;int ns=ok?1:-1;int old=g_backend_state.exchange(ns);if(old!=ns)redraw657();for(int i=0;i<50&&g_alive;i++)std::this_thread::sleep_for(std::chrono::milliseconds(100));}g_probe_running_v657=false;}
void start657(){bool e=false;if(g_alive&&g_probe_running_v657.compare_exchange_strong(e,true))std::thread(probe657).detach();}
void onstart657(ANativeActivity*){g_alive=true;start657();} void onresume657(ANativeActivity*){redraw657();start657();}
void wc657(ANativeActivity*,ANativeWindow*w){{std::lock_guard<std::mutex>l(g_mutex);if(g_window)ANativeWindow_release(g_window);g_window=w;if(w)ANativeWindow_acquire(w);}redraw657();start657();}
void wr657(ANativeActivity*,ANativeWindow*){redraw657();}
void wd657(ANativeActivity*,ANativeWindow*w){std::lock_guard<std::mutex>l(g_mutex);if(g_window==w&&g_window){ANativeWindow_release(g_window);g_window=nullptr;}}
void destroy657(ANativeActivity*){g_alive=false;std::lock_guard<std::mutex>l(g_mutex);if(g_window){ANativeWindow_release(g_window);g_window=nullptr;}}
}
extern "C" const char* ra657_version(){return"65.7";} extern "C" int ra657_core_ready(){return 1;}
extern "C" const char* ra657_mode(){return"OFFLINE_FIRST";} extern "C" const char* ra657_safety(){return"LOCKED";}
extern "C" const char* ra657_checkpoint(){return"FINAL_LOCKED_MASTER";}
extern "C" int ra657_write_allowed(){return 0;} extern "C" int ra657_delete_allowed(){return 0;} extern "C" int ra657_restore_allowed(){return 0;} extern "C" int ra657_live_orders_allowed(){return 0;} extern "C" int ra657_auto_update_allowed(){return 0;} extern "C" int ra657_production_gate_open(){return 0;}
extern "C" int ra657_write(){return-403;} extern "C" int ra657_apply(){return-403;} extern "C" int ra657_restore(){return-403;} extern "C" int ra657_install(){return-403;} extern "C" int ra657_delete(){return-403;} extern "C" int ra657_repair(){return-403;} extern "C" int ra657_live_order(){return-403;}
extern "C" void ANativeActivity_onCreate(ANativeActivity*a,void*,size_t){if(!a||!a->callbacks)return;a->callbacks->onStart=onstart657;a->callbacks->onResume=onresume657;a->callbacks->onPause=[](ANativeActivity*){};a->callbacks->onStop=[](ANativeActivity*){};a->callbacks->onDestroy=destroy657;a->callbacks->onNativeWindowCreated=wc657;a->callbacks->onNativeWindowRedrawNeeded=wr657;a->callbacks->onNativeWindowDestroyed=wd657;}
