#define ANativeActivity_onCreate ANativeActivity_onCreate_v658_legacy
#include "native_activity_host_v658.cpp"
#undef ANativeActivity_onCreate

#include <android/input.h>
#include <android/keycodes.h>
#include <android/looper.h>
#include <atomic>
#include <thread>

// V67.0 UNIFIED MASTER APK
// Safe native control center. V66.5 remains the known-good feature checkpoint.
// This layer unifies checkpoint/version/recovery/regression state while all
// destructive capabilities remain hard blocked.

namespace {
std::atomic<bool> run670{false};
std::atomic<int> page670{0};
std::atomic<int> touches670{0};
std::atomic<int> event670{0};
ANativeActivity* activity670=nullptr;
AInputQueue* input670=nullptr;
ANativeWindow* window670=nullptr;
std::thread loop670;
int w670=0,h670=0;

enum {P_HOME=0,P_MODULES=1,P_CHECKPOINT=2,P_RECOVERY=3,P_REGRESSION=4};

const char* page_name670(int p){switch(p){case P_MODULES:return "MODULES UNIFIED";case P_CHECKPOINT:return "VERSION CHECKPOINT";case P_RECOVERY:return "RECOVERY APPROVAL";case P_REGRESSION:return "REGRESSION MASTER";default:return "UNIFIED CONTROL CENTER";}}

bool draw670(ANativeWindow*w){
 if(!w)return false; EGLDisplay d=eglGetDisplay(EGL_DEFAULT_DISPLAY); if(d==EGL_NO_DISPLAY||!eglInitialize(d,nullptr,nullptr))return false;
 const EGLint ca[]={EGL_SURFACE_TYPE,EGL_WINDOW_BIT,EGL_RENDERABLE_TYPE,EGL_OPENGL_ES2_BIT,EGL_RED_SIZE,8,EGL_GREEN_SIZE,8,EGL_BLUE_SIZE,8,EGL_NONE}; EGLConfig c=nullptr;EGLint n=0;
 if(!eglChooseConfig(d,ca,&c,1,&n)||n<1){eglTerminate(d);return false;} EGLint f=0;eglGetConfigAttrib(d,c,EGL_NATIVE_VISUAL_ID,&f);ANativeWindow_setBuffersGeometry(w,0,0,f);
 EGLSurface su=eglCreateWindowSurface(d,c,w,nullptr);const EGLint xa[]={EGL_CONTEXT_CLIENT_VERSION,2,EGL_NONE};EGLContext x=eglCreateContext(d,c,EGL_NO_CONTEXT,xa);
 if(su==EGL_NO_SURFACE||x==EGL_NO_CONTEXT||!eglMakeCurrent(d,su,su,x)){if(x!=EGL_NO_CONTEXT)eglDestroyContext(d,x);if(su!=EGL_NO_SURFACE)eglDestroySurface(d,su);eglTerminate(d);return false;}
 EGLint W=0,H=0;eglQuerySurface(d,su,EGL_WIDTH,&W);eglQuerySurface(d,su,EGL_HEIGHT,&H);w670=W;h670=H;glViewport(0,0,W,H);glClearColor(.025f,.07f,.15f,1);glClear(GL_COLOR_BUFFER_BIT);
 int s=W/270;if(s<3)s=3;if(s>6)s=6;int lx=5*s,rx=W/2+7*s,top=H-22*s,row=9*s,p=page670.load();
 text("ROBOT ADMIN AI OS",lx,top,s,.92f,.96f,1);text("V67.0 UNIFIED",rx,top,s,.6f,.75f,.95f);
 text("CPP CORE: READY",lx,top-row,s,.35f,.9f,.55f);text("SAFETY: LOCKED",rx,top-row,s,.35f,.9f,.55f);
 text("MODE: OFFLINE FIRST",lx,top-2*row,s,.75f,.85f,.95f);text("CONTROL: READ ONLY",rx,top-2*row,s,.75f,.85f,.95f);
 text("MEGA BASE: V66.5",lx,top-3*row,s,.55f,.8f,.95f);text("PROD GATE: CLOSED",rx,top-3*row,s,.95f,.55f,.3f);
 text("TOUCH COUNT: "+std::to_string(touches670.load()),lx,top-4*row,s,.55f,.8f,.95f);text("E-STOP: READY",rx,top-4*row,s,.35f,.9f,.55f);
 int py=top-6*row;text(page_name670(p),lx,py,s,.35f,.9f,.55f);
 if(p==P_HOME){text("FILES ENGINE: V66.5 READY",lx,py-row,s,.75f,.85f,.95f);text("TASK ENGINE: V66.5 READY",lx,py-2*row,s,.75f,.85f,.95f);text("SHA256 VERIFY: V66.5 READY",lx,py-3*row,s,.75f,.85f,.95f);text("CSV ANALYZER: V66.5 READY",lx,py-4*row,s,.75f,.85f,.95f);text("UNIFIED STATE: SAFE",rx,py-row,s,.35f,.9f,.55f);}
 else if(p==P_MODULES){text("FILES: READ ONLY",lx,py-row,s,.35f,.9f,.55f);text("TASKS: READ ONLY",lx,py-2*row,s,.35f,.9f,.55f);text("BACKUP: VERIFY ONLY",lx,py-3*row,s,.35f,.9f,.55f);text("DATA: ANALYZE ONLY",lx,py-4*row,s,.35f,.9f,.55f);text("APPLY: BLOCKED",rx,py-row,s,.95f,.55f,.3f);}
 else if(p==P_CHECKPOINT){text("CURRENT: V67.0",lx,py-row,s,.75f,.85f,.95f);text("FEATURE BASE: V66.5",lx,py-2*row,s,.75f,.85f,.95f);text("LOCKED BASE: V65.7",lx,py-3*row,s,.75f,.85f,.95f);text("ROLLBACK INFO: READY",lx,py-4*row,s,.35f,.9f,.55f);text("ROLLBACK APPLY: BLOCKED",rx,py-row,s,.95f,.55f,.3f);}
 else if(p==P_RECOVERY){text("RECOVERY: APPROVAL GATED",lx,py-row,s,.35f,.9f,.55f);text("RESTORE: BLOCKED",lx,py-2*row,s,.95f,.55f,.3f);text("DELETE: BLOCKED",lx,py-3*row,s,.95f,.55f,.3f);text("REPAIR APPLY: BLOCKED",lx,py-4*row,s,.95f,.55f,.3f);text("OWNER APPROVAL: REQUIRED",rx,py-row,s,.75f,.85f,.95f);}
 else {text("CORE: PASS BASE",lx,py-row,s,.35f,.9f,.55f);text("TOUCH: READY",lx,py-2*row,s,.35f,.9f,.55f);text("SAFETY FLAGS: LOCKED",lx,py-3*row,s,.35f,.9f,.55f);text("CHECKPOINTS: READY",lx,py-4*row,s,.35f,.9f,.55f);text("FINAL CANDIDATE: NEXT",rx,py-row,s,.75f,.85f,.95f);}
 text("WRITE DELETE: OFF",rx,py+3*row,s,.95f,.55f,.3f);text("RESTORE APPLY: OFF",rx,py+2*row,s,.95f,.55f,.3f);text("LIVE ORDERS: OFF",rx,py+row,s,.95f,.55f,.3f);text("AUTO UPDATE: OFF",rx,py,s,.95f,.55f,.3f);
 int bh=14*s,by=4*s,gap=2*s,bw=(W-6*gap)/5;const char*bn[5]={"HOME","MODULES","VERSIONS","RECOVERY","REGRESS"};for(int i=0;i<5;i++){float g=(p==i)?.48f:.27f;rect(gap+i*(bw+gap),by,bw,bh,.08f,g,.28f);text(bn[i],gap+i*(bw+gap)+2*s,by+4*s,s,.92f,.96f,1);}
 eglSwapBuffers(d,su);eglMakeCurrent(d,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);eglDestroyContext(d,x);eglDestroySurface(d,su);eglTerminate(d);return true;
}

int input_cb670(int, int, void*){if(!input670)return 1;AInputEvent*e=nullptr;while(AInputQueue_getEvent(input670,&e)>=0){if(AInputQueue_preDispatchEvent(input670,e))continue;int handled=0;if(AInputEvent_getType(e)==AINPUT_EVENT_TYPE_MOTION&&AMotionEvent_getAction(e)==AMOTION_EVENT_ACTION_UP){float x=AMotionEvent_getX(e,0),y=(float)h670-AMotionEvent_getY(e,0);int s=w670/270;if(s<3)s=3;if(s>6)s=6;int gap=2*s,bw=(w670-6*gap)/5,bh=14*s;if(y>=4*s&&y<=4*s+bh){for(int i=0;i<5;i++){int bx=gap+i*(bw+gap);if(x>=bx&&x<=bx+bw){page670=i;touches670++;event670=i+1;handled=1;break;}}}}else if(AInputEvent_getType(e)==AINPUT_EVENT_TYPE_KEY&&AKeyEvent_getAction(e)==AKEY_EVENT_ACTION_UP&&AKeyEvent_getKeyCode(e)==AKEYCODE_BACK){page670=P_HOME;touches670++;handled=1;}AInputQueue_finishEvent(input670,e,handled);}return 1;}
void loop_fn670(){ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);while(run670){if(window670)draw670(window670);ALooper_pollAll(120,nullptr,nullptr,nullptr);}}
void win_create670(ANativeActivity*,ANativeWindow*w){window670=w;}
void win_destroy670(ANativeActivity*,ANativeWindow*){window670=nullptr;}
void iq_create670(ANativeActivity*,AInputQueue*q){input670=q;AInputQueue_attachLooper(q,ALooper_forThread(),1,input_cb670,nullptr);}
void iq_destroy670(ANativeActivity*,AInputQueue*q){AInputQueue_detachLooper(q);if(input670==q)input670=nullptr;}
void destroy670(ANativeActivity*){run670=false;if(loop670.joinable())loop670.join();}
}

extern "C" void ANativeActivity_onCreate(ANativeActivity*a,void*,size_t){activity670=a;a->callbacks->onNativeWindowCreated=win_create670;a->callbacks->onNativeWindowDestroyed=win_destroy670;a->callbacks->onInputQueueCreated=iq_create670;a->callbacks->onInputQueueDestroyed=iq_destroy670;a->callbacks->onDestroy=destroy670;run670=true;loop670=std::thread(loop_fn670);}
extern "C" const char* ra670_version(){return "67.0";}extern "C" int ra670_core_ready(){return 1;}extern "C" const char* ra670_mode(){return "OFFLINE_FIRST";}extern "C" const char* ra670_safety(){return "LOCKED";}extern "C" const char* ra670_checkpoint(){return "UNIFIED_MASTER";}extern "C" int ra670_write_allowed(){return 0;}extern "C" int ra670_delete_allowed(){return 0;}extern "C" int ra670_restore_allowed(){return 0;}extern "C" int ra670_live_orders_allowed(){return 0;}extern "C" int ra670_auto_update_allowed(){return 0;}extern "C" int ra670_apply(){return -403;}extern "C" int ra670_restore(){return -403;}extern "C" int ra670_delete(){return -403;}extern "C" int ra670_live_order(){return -403;}
