// Robot Admin AI OS V64.8.2
// Focused runtime recovery build.
// Reuses the runtime-proven V64.7 socket/parser/probe path unchanged.
// IMPORTANT: no continuous reconnect monitor. Lifecycle is intentionally
// restored to the exact V64.7 pattern that already proved CONNECTED/200/OK.

#define render v647_render
#define redraw v647_redraw
#define on_start v647_on_start
#define on_resume v647_on_resume
#define on_pause v647_on_pause
#define on_stop v647_on_stop
#define wc v647_wc
#define wr v647_wr
#define wd v647_wd
#define destroy v647_destroy
#define ANativeActivity_onCreate ANativeActivity_onCreate_v647_embedded_6482
#include "native_activity_host_v647.cpp"
#undef render
#undef redraw
#undef on_start
#undef on_resume
#undef on_pause
#undef on_stop
#undef wc
#undef wr
#undef wd
#undef destroy
#undef ANativeActivity_onCreate

namespace {

bool render6482(ANativeWindow*w){
 if(!w)return false;
 EGLDisplay d=eglGetDisplay(EGL_DEFAULT_DISPLAY);if(d==EGL_NO_DISPLAY||!eglInitialize(d,nullptr,nullptr))return false;
 const EGLint ca[]={EGL_SURFACE_TYPE,EGL_WINDOW_BIT,EGL_RENDERABLE_TYPE,EGL_OPENGL_ES2_BIT,EGL_RED_SIZE,8,EGL_GREEN_SIZE,8,EGL_BLUE_SIZE,8,EGL_NONE};
 EGLConfig c=nullptr;EGLint n=0;if(!eglChooseConfig(d,ca,&c,1,&n)||n<1){eglTerminate(d);return false;}
 EGLint f=0;eglGetConfigAttrib(d,c,EGL_NATIVE_VISUAL_ID,&f);ANativeWindow_setBuffersGeometry(w,0,0,f);
 EGLSurface su=eglCreateWindowSurface(d,c,w,nullptr);const EGLint xa[]={EGL_CONTEXT_CLIENT_VERSION,2,EGL_NONE};EGLContext x=eglCreateContext(d,c,EGL_NO_CONTEXT,xa);
 if(su==EGL_NO_SURFACE||x==EGL_NO_CONTEXT||!eglMakeCurrent(d,su,su,x)){if(x!=EGL_NO_CONTEXT)eglDestroyContext(d,x);if(su!=EGL_NO_SURFACE)eglDestroySurface(d,su);eglTerminate(d);return false;}
 EGLint W=0,H=0;eglQuerySurface(d,su,EGL_WIDTH,&W);eglQuerySurface(d,su,EGL_HEIGHT,&H);glViewport(0,0,W,H);glClearColor(.025f,.07f,.15f,1);glClear(GL_COLOR_BUFFER_BIT);
 int s=W/220;if(s<2)s=2;if(s>6)s=6;
 int st=g_state.load(),dg=g_diag.load();float r=st==1?.2f:(st==-1?.9f:.95f),g=st==1?.9f:(st==-1?.25f:.7f),b=st==1?.35f:(st==-1?.25f:.2f);
 text("ROBOT ADMIN AI OS",8*s,H-25*s,s,.92f,.96f,1);
 text("VERSION: 64.8.2",8*s,H-39*s,s,.6f,.75f,.95f);
 text("BACKEND:",8*s,H-57*s,s,.92f,.96f,1);
 text(st==1?"CONNECTED":(st==-1?"DISCONNECTED":"CHECKING"),60*s,H-57*s,s,r,g,b);
 text("DIAG: "+std::to_string(dg),8*s,H-71*s,s,.8f,.8f,.9f);
 text(st==1?"HEALTH: OK":(st==-1?"HEALTH: RETRY":"HEALTH: CHECKING"),8*s,H-85*s,s,r,g,b);
 text("RECONNECT: RESUME SAFE",8*s,H-99*s,s,.75f,.85f,.95f);
 text("MODE: READ ONLY",8*s,H-113*s,s,.75f,.85f,.95f);
 text("SAFETY: LOCKED",8*s,H-127*s,s,.35f,.9f,.55f);
 text("LIVE ORDERS: OFF",8*s,H-141*s,s,.95f,.55f,.3f);
 text("AUTO UPDATE: OFF",8*s,H-155*s,s,.95f,.55f,.3f);
 text("PRODUCTION GATE: CLOSED",8*s,H-169*s,s,.95f,.55f,.3f);
 text("EMERGENCY STOP: READY",8*s,H-183*s,s,.35f,.9f,.55f);
 bool ok=eglSwapBuffers(d,su)==EGL_TRUE;
 eglMakeCurrent(d,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);eglDestroyContext(d,x);eglDestroySurface(d,su);eglTerminate(d);return ok;
}

void redraw6482(){std::lock_guard<std::mutex>l(g_mutex);if(g_window)render6482(g_window);}

// Exact V64.7 lifecycle strategy: probe on resume/window creation only.
void on_start6482(ANativeActivity*){g_alive=true;g_state=0;g_diag=0;}
void on_resume6482(ANativeActivity*){redraw6482();start_probe();}
void on_pause6482(ANativeActivity*){}
void on_stop6482(ANativeActivity*){}
void wc6482(ANativeActivity*,ANativeWindow*w){{std::lock_guard<std::mutex>l(g_mutex);if(g_window)ANativeWindow_release(g_window);g_window=w;if(w)ANativeWindow_acquire(w);}redraw6482();start_probe();}
void wr6482(ANativeActivity*,ANativeWindow*){redraw6482();}
void wd6482(ANativeActivity*,ANativeWindow*w){std::lock_guard<std::mutex>l(g_mutex);if(g_window==w&&g_window){ANativeWindow_release(g_window);g_window=nullptr;}}
void destroy6482(ANativeActivity*){g_alive=false;std::lock_guard<std::mutex>l(g_mutex);if(g_window){ANativeWindow_release(g_window);g_window=nullptr;}}
}

extern "C" const char* ra6482_version(){return"64.8.2";}
extern "C" int ra6482_write(){return-403;}
extern "C" int ra6482_apply(){return-403;}
extern "C" int ra6482_restore(){return-403;}
extern "C" int ra6482_install(){return-403;}
extern "C" int ra6482_delete(){return-403;}
extern "C" int ra6482_repair(){return-403;}
extern "C" int ra6482_live_order(){return-403;}
extern "C" void ANativeActivity_onCreate(ANativeActivity*a,void*,size_t){if(!a||!a->callbacks)return;a->callbacks->onStart=on_start6482;a->callbacks->onResume=on_resume6482;a->callbacks->onPause=on_pause6482;a->callbacks->onStop=on_stop6482;a->callbacks->onDestroy=destroy6482;a->callbacks->onNativeWindowCreated=wc6482;a->callbacks->onNativeWindowRedrawNeeded=wr6482;a->callbacks->onNativeWindowDestroyed=wd6482;}
