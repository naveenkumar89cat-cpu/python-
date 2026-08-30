#define ANativeActivity_onCreate ANativeActivity_onCreate_v658_legacy
#include "native_activity_host_v658.cpp"
#undef ANativeActivity_onCreate

// V65.9 Read-Only Module Screens
// Keeps V65.8 touch engine foundation and adds safe module panels.
namespace {
std::atomic<bool> g_probe_running_v659{false};

const char* page_title659(int a){
    switch(a){
        case ACT_STATUS:return "STATUS PANEL";
        case ACT_FILES:return "FILES PANEL";
        case ACT_TASKS:return "TASKS PANEL";
        case ACT_BACKUP:return "BACKUP PANEL";
        case ACT_RECHECK:return "RECHECK PANEL";
        default:return "STATUS PANEL";
    }
}

bool render659(ANativeWindow*w){
    if(!w)return false;
    EGLDisplay d=eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(d==EGL_NO_DISPLAY||!eglInitialize(d,nullptr,nullptr))return false;
    const EGLint ca[]={EGL_SURFACE_TYPE,EGL_WINDOW_BIT,EGL_RENDERABLE_TYPE,EGL_OPENGL_ES2_BIT,EGL_RED_SIZE,8,EGL_GREEN_SIZE,8,EGL_BLUE_SIZE,8,EGL_NONE};
    EGLConfig c=nullptr;EGLint n=0;
    if(!eglChooseConfig(d,ca,&c,1,&n)||n<1){eglTerminate(d);return false;}
    EGLint f=0;eglGetConfigAttrib(d,c,EGL_NATIVE_VISUAL_ID,&f);ANativeWindow_setBuffersGeometry(w,0,0,f);
    EGLSurface su=eglCreateWindowSurface(d,c,w,nullptr);const EGLint xa[]={EGL_CONTEXT_CLIENT_VERSION,2,EGL_NONE};EGLContext x=eglCreateContext(d,c,EGL_NO_CONTEXT,xa);
    if(su==EGL_NO_SURFACE||x==EGL_NO_CONTEXT||!eglMakeCurrent(d,su,su,x)){if(x!=EGL_NO_CONTEXT)eglDestroyContext(d,x);if(su!=EGL_NO_SURFACE)eglDestroySurface(d,su);eglTerminate(d);return false;}
    EGLint W=0,H=0;eglQuerySurface(d,su,EGL_WIDTH,&W);eglQuerySurface(d,su,EGL_HEIGHT,&H);g_width=W;g_height=H;
    glViewport(0,0,W,H);glClearColor(.025f,.07f,.15f,1);glClear(GL_COLOR_BUFFER_BIT);

    int s=W/260;if(s<3)s=3;if(s>6)s=6;int st=g_backend_state.load();int act=g_last_action.load();if(act==ACT_NONE)act=ACT_STATUS;
    int lx=5*s,rx=W/2+7*s,top=H-22*s,row=9*s;
    text("ROBOT ADMIN AI OS",lx,top,s,.92f,.96f,1);text("V65.9 MODULES",rx,top,s,.6f,.75f,.95f);
    text("CPP CORE: READY",lx,top-row,s,.35f,.9f,.55f);text("SAFETY: LOCKED",rx,top-row,s,.35f,.9f,.55f);
    text("MODE: OFFLINE FIRST",lx,top-2*row,s,.75f,.85f,.95f);text("CONTROL: READ ONLY",rx,top-2*row,s,.75f,.85f,.95f);
    text("PY BACKEND:",lx,top-3*row,s,.92f,.96f,1);text(st==1?"CONNECTED":(st==-1?"OPTIONAL OFF":"CHECKING"),lx+76*s,top-3*row,s,st==1?.2f:.95f,st==1?.9f:.65f,st==1?.35f:.25f);
    text("TOUCH COUNT: "+std::to_string(g_touch_count.load()),lx,top-4*row,s,.55f,.8f,.95f);text("PROD GATE: CLOSED",rx,top-4*row,s,.95f,.55f,.3f);

    int py=top-6*row;
    text(page_title659(act),lx,py,s,.35f,.9f,.55f);
    if(act==ACT_STATUS){
        text("DIAG: "+std::to_string(g_diag.load()),lx,py-row,s,.75f,.85f,.95f);
        text("PROBE OK: "+std::to_string(g_probe_ok.load()),lx,py-2*row,s,.75f,.85f,.95f);
        text("PROBE FAIL: "+std::to_string(g_probe_fail.load()),lx,py-3*row,s,.75f,.85f,.95f);
        text("SYSTEM STATE: SAFE",rx,py-row,s,.35f,.9f,.55f);
    }else if(act==ACT_FILES){
        text("ACCESS: VIEW ONLY",lx,py-row,s,.75f,.85f,.95f);
        text("WRITE: BLOCKED",lx,py-2*row,s,.95f,.55f,.3f);
        text("DELETE: BLOCKED",lx,py-3*row,s,.95f,.55f,.3f);
        text("FILE ENGINE: FOUNDATION",rx,py-row,s,.55f,.8f,.95f);
    }else if(act==ACT_TASKS){
        text("ACCESS: VIEW ONLY",lx,py-row,s,.75f,.85f,.95f);
        text("CREATE APPLY: BLOCKED",lx,py-2*row,s,.95f,.55f,.3f);
        text("TASK ENGINE: FOUNDATION",rx,py-row,s,.55f,.8f,.95f);
    }else if(act==ACT_BACKUP){
        text("VERIFY: READY",lx,py-row,s,.35f,.9f,.55f);
        text("RESTORE: BLOCKED",lx,py-2*row,s,.95f,.55f,.3f);
        text("BACKUP MODE: READ ONLY",rx,py-row,s,.55f,.8f,.95f);
    }else if(act==ACT_RECHECK){
        text("LOCAL PROBE: SAFE",lx,py-row,s,.35f,.9f,.55f);
        text("DIAG: "+std::to_string(g_diag.load()),lx,py-2*row,s,.75f,.85f,.95f);
        text(st==1?"RESULT: CONNECTED":"RESULT: OPTIONAL OFF",rx,py-row,s,st==1?.35f:.95f,st==1?.9f:.65f,st==1?.55f:.25f);
    }

    text("WRITE DELETE: OFF",rx,py-2*row,s,.95f,.55f,.3f);
    text("RESTORE APPLY: OFF",rx,py-3*row,s,.95f,.55f,.3f);
    text("LIVE ORDERS: OFF",rx,py-4*row,s,.95f,.55f,.3f);
    text("AUTO UPDATE: OFF",rx,py-5*row,s,.95f,.55f,.3f);

    int margin=4*s,gap=2*s,by=5*s,bh=14*s,bw=(W-2*margin-4*gap)/5;
    const char* labs[5]={"STATUS","FILES","TASKS","BACKUP","RECHECK"};
    for(int i=0;i<5;i++){int bx=margin+i*(bw+gap);bool sel=act==i+1;rect(bx,by,bw,bh,sel?.15f:.08f,sel?.42f:.22f,sel?.30f:.36f);int ts=s>4?s-1:s;text(labs[i],bx+2*s,by+4*s,ts,.88f,.95f,1);}
    text("E-STOP READY",rx,by+bh+3*s,s,.35f,.9f,.55f);
    bool ok=eglSwapBuffers(d,su)==EGL_TRUE;eglMakeCurrent(d,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);eglDestroyContext(d,x);eglDestroySurface(d,su);eglTerminate(d);return ok;
}

void redraw659(){std::lock_guard<std::mutex>l(g_mutex);if(g_window)render659(g_window);}
void probe659(){while(g_alive){bool ok=backend_check();if(ok)g_probe_ok++;else g_probe_fail++;int ns=ok?1:-1;int old=g_backend_state.exchange(ns);if(old!=ns)redraw659();for(int i=0;i<50&&g_alive;i++)std::this_thread::sleep_for(std::chrono::milliseconds(100));}g_probe_running_v659=false;}
void start_probe659(){bool e=false;if(g_alive&&g_probe_running_v659.compare_exchange_strong(e,true))std::thread(probe659).detach();}
void action659(int a){g_last_action=a;g_touch_count++;if(a==ACT_RECHECK){std::thread([](){bool ok=backend_check();if(ok)g_probe_ok++;else g_probe_fail++;g_backend_state=ok?1:-1;redraw659();}).detach();}else redraw659();}
int hit659(float x,float y_android){int W=g_width.load(),H=g_height.load();if(W<=0||H<=0)return ACT_NONE;int s=W/260;if(s<3)s=3;if(s>6)s=6;float y=(float)H-y_android;int margin=4*s,gap=2*s,by=5*s,bh=14*s,bw=(W-2*margin-4*gap)/5;if(y<by||y>by+bh)return ACT_NONE;for(int i=0;i<5;i++){int bx=margin+i*(bw+gap);if(x>=bx&&x<=bx+bw)return i+1;}return ACT_NONE;}
int input659(int,int,void*data){AInputQueue*q=reinterpret_cast<AInputQueue*>(data);if(!q)return 1;AInputEvent*ev=nullptr;while(AInputQueue_getEvent(q,&ev)>=0){if(AInputQueue_preDispatchEvent(q,ev))continue;int handled=0;if(AInputEvent_getType(ev)==AINPUT_EVENT_TYPE_MOTION){int action=AMotionEvent_getAction(ev)&AMOTION_EVENT_ACTION_MASK;if(action==AMOTION_EVENT_ACTION_UP){int a=hit659(AMotionEvent_getX(ev,0),AMotionEvent_getY(ev,0));if(a!=ACT_NONE){action659(a);handled=1;}}}AInputQueue_finishEvent(q,ev,handled);}return 1;}
void input_created659(ANativeActivity*,AInputQueue*q){g_input_queue=q;g_input_looper=ALooper_forThread();if(!g_input_looper)g_input_looper=ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);if(q&&g_input_looper)AInputQueue_attachLooper(q,g_input_looper,1,input659,q);}
void input_destroyed659(ANativeActivity*,AInputQueue*q){if(q)AInputQueue_detachLooper(q);if(g_input_queue==q)g_input_queue=nullptr;}
void start659(ANativeActivity*){g_alive=true;if(g_last_action.load()==ACT_NONE)g_last_action=ACT_STATUS;start_probe659();}
void resume659(ANativeActivity*){redraw659();start_probe659();}
void wc659(ANativeActivity*,ANativeWindow*w){{std::lock_guard<std::mutex>l(g_mutex);if(g_window)ANativeWindow_release(g_window);g_window=w;if(w)ANativeWindow_acquire(w);}redraw659();start_probe659();}
void wr659(ANativeActivity*,ANativeWindow*){redraw659();}
void wd659(ANativeActivity*,ANativeWindow*w){std::lock_guard<std::mutex>l(g_mutex);if(g_window==w&&g_window){ANativeWindow_release(g_window);g_window=nullptr;}}
void destroy659(ANativeActivity*){g_alive=false;std::lock_guard<std::mutex>l(g_mutex);if(g_window){ANativeWindow_release(g_window);g_window=nullptr;}}
}

extern "C" const char* ra659_version(){return"65.9";}
extern "C" int ra659_core_ready(){return 1;}
extern "C" const char* ra659_mode(){return"OFFLINE_FIRST";}
extern "C" const char* ra659_safety(){return"LOCKED";}
extern "C" const char* ra659_control_mode(){return"READ_ONLY";}
extern "C" int ra659_touch_ready(){return 1;}
extern "C" int ra659_module_screens_ready(){return 1;}
extern "C" int ra659_write_allowed(){return 0;} extern "C" int ra659_delete_allowed(){return 0;} extern "C" int ra659_restore_allowed(){return 0;} extern "C" int ra659_live_orders_allowed(){return 0;} extern "C" int ra659_auto_update_allowed(){return 0;} extern "C" int ra659_production_gate_open(){return 0;}
extern "C" int ra659_write(){return-403;} extern "C" int ra659_apply(){return-403;} extern "C" int ra659_restore(){return-403;} extern "C" int ra659_install(){return-403;} extern "C" int ra659_delete(){return-403;} extern "C" int ra659_repair(){return-403;} extern "C" int ra659_live_order(){return-403;}

extern "C" void ANativeActivity_onCreate(ANativeActivity*a,void*,size_t){
    if(!a||!a->callbacks)return;
    a->callbacks->onStart=start659;
    a->callbacks->onResume=resume659;
    a->callbacks->onPause=[](ANativeActivity*){};
    a->callbacks->onStop=[](ANativeActivity*){};
    a->callbacks->onDestroy=destroy659;
    a->callbacks->onNativeWindowCreated=wc659;
    a->callbacks->onNativeWindowRedrawNeeded=wr659;
    a->callbacks->onNativeWindowDestroyed=wd659;
    a->callbacks->onInputQueueCreated=input_created659;
    a->callbacks->onInputQueueDestroyed=input_destroyed659;
}
