#include <android/native_activity.h>
#include <android/native_window.h>
#include <android/input.h>
#include <android/looper.h>
#include <android/log.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <cerrno>
#include <cstring>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

#define LOG_TAG "RobotAdminV658"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

namespace {
std::mutex g_mutex;
ANativeWindow* g_window=nullptr;
std::atomic<int> g_backend_state{0};
std::atomic<int> g_diag{0};
std::atomic<bool> g_alive{false};
std::atomic<bool> g_probe_running{false};
std::atomic<unsigned long> g_probe_ok{0};
std::atomic<unsigned long> g_probe_fail{0};
std::atomic<unsigned long> g_touch_count{0};
std::atomic<int> g_last_action{0};
std::atomic<int> g_width{0};
std::atomic<int> g_height{0};
AInputQueue* g_input_queue=nullptr;
ALooper* g_input_looper=nullptr;

enum Action { ACT_NONE=0, ACT_STATUS=1, ACT_FILES=2, ACT_TASKS=3, ACT_BACKUP=4, ACT_RECHECK=5 };

bool backend_check(){
    int fd=socket(AF_INET,SOCK_STREAM,0);
    if(fd<0){g_diag=1;return false;}
    int flags=fcntl(fd,F_GETFL,0);if(flags>=0)fcntl(fd,F_SETFL,flags|O_NONBLOCK);
    sockaddr_in a{};a.sin_family=AF_INET;a.sin_port=htons(8080);inet_pton(AF_INET,"127.0.0.1",&a.sin_addr);
    int rc=connect(fd,reinterpret_cast<sockaddr*>(&a),sizeof(a));
    if(rc!=0&&errno!=EINPROGRESS){g_diag=2;close(fd);return false;}
    if(rc!=0){pollfd p{fd,POLLOUT,0};if(poll(&p,1,700)<=0){g_diag=2;close(fd);return false;}int e=0;socklen_t l=sizeof(e);getsockopt(fd,SOL_SOCKET,SO_ERROR,&e,&l);if(e){g_diag=2;close(fd);return false;}}
    if(flags>=0)fcntl(fd,F_SETFL,flags);
    timeval tv{};tv.tv_sec=1;tv.tv_usec=500000;setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv));
    const char*q="GET /native/foundation/summary HTTP/1.1\r\nHost: 127.0.0.1:8080\r\nConnection: close\r\n\r\n";
    if(send(fd,q,strlen(q),0)<=0){g_diag=3;close(fd);return false;}
    std::string out;char b[2048];for(;;){ssize_t n=recv(fd,b,sizeof(b),0);if(n<=0)break;out.append(b,(size_t)n);if(out.size()>65536)break;}close(fd);
    if(out.empty()){g_diag=4;return false;}
    bool http_ok=out.find("HTTP/1.1 200")!=std::string::npos||out.find("HTTP/1.0 200")!=std::string::npos;
    bool pass_ok=out.find("\"status\":\"PASS\"")!=std::string::npos||out.find("\"status\": \"PASS\"")!=std::string::npos;
    bool ok=http_ok&&pass_ok;g_diag=ok?200:5;return ok;
}

const unsigned char* glyph(char c){
 static const unsigned char z[7]={0,0,0,0,0,0,0};
#define G(n,a,b,c,d,e,f,g) static const unsigned char n[7]={a,b,c,d,e,f,g}
 G(A,14,17,17,31,17,17,17);G(B,30,17,17,30,17,17,30);G(C,14,17,16,16,16,17,14);G(D,30,17,17,17,17,17,30);G(E,31,16,16,30,16,16,31);G(F,31,16,16,30,16,16,16);G(G,14,17,16,23,17,17,15);G(H,17,17,17,31,17,17,17);G(I,31,4,4,4,4,4,31);G(J,7,2,2,2,18,18,12);G(K,17,18,20,24,20,18,17);G(L,16,16,16,16,16,16,31);G(M,17,27,21,21,17,17,17);G(N,17,25,21,19,17,17,17);G(O,14,17,17,17,17,17,14);G(P,30,17,17,30,16,16,16);G(Q,14,17,17,17,21,18,13);G(R,30,17,17,30,20,18,17);G(S,15,16,16,14,1,1,30);G(T,31,4,4,4,4,4,4);G(U,17,17,17,17,17,17,14);G(V,17,17,17,17,17,10,4);G(W,17,17,17,21,21,21,10);G(X,17,17,10,4,10,17,17);G(Y,17,17,10,4,4,4,4);G(Z,31,1,2,4,8,16,31);G(N0,14,17,19,21,25,17,14);G(N1,4,12,4,4,4,4,14);G(N2,14,17,1,2,4,8,31);G(N3,30,1,1,14,1,1,30);G(N4,2,6,10,18,31,2,2);G(N5,31,16,16,30,1,1,30);G(N6,14,16,16,30,17,17,14);G(N7,31,1,2,4,8,8,8);G(N8,14,17,17,14,17,17,14);G(N9,14,17,17,15,1,1,14);G(COL,0,4,4,0,4,4,0);G(DOT,0,0,0,0,0,4,4);G(DASH,0,0,0,31,0,0,0);
#undef G
 switch(c){case'A':return A;case'B':return B;case'C':return C;case'D':return D;case'E':return E;case'F':return F;case'G':return G;case'H':return H;case'I':return I;case'J':return J;case'K':return K;case'L':return L;case'M':return M;case'N':return N;case'O':return O;case'P':return P;case'Q':return Q;case'R':return R;case'S':return S;case'T':return T;case'U':return U;case'V':return V;case'W':return W;case'X':return X;case'Y':return Y;case'Z':return Z;case'0':return N0;case'1':return N1;case'2':return N2;case'3':return N3;case'4':return N4;case'5':return N5;case'6':return N6;case'7':return N7;case'8':return N8;case'9':return N9;case':':return COL;case'.':return DOT;case'-':return DASH;default:return z;}
}
void rect(int x,int y,int w,int h,float r,float g,float b){if(w<=0||h<=0)return;glEnable(GL_SCISSOR_TEST);glScissor(x,y,w,h);glClearColor(r,g,b,1);glClear(GL_COLOR_BUFFER_BIT);glDisable(GL_SCISSOR_TEST);}
void text(const std::string&t,int x,int y,int s,float r,float g,float b){int p=x;for(char c:t){auto q=glyph(c);for(int a=0;a<7;a++)for(int d=0;d<5;d++)if(q[a]&(1u<<(4-d)))rect(p+d*s,y+(6-a)*s,s,s,r,g,b);p+=6*s;}}
const char* action_name(int a){switch(a){case ACT_STATUS:return"STATUS";case ACT_FILES:return"FILES";case ACT_TASKS:return"TASKS";case ACT_BACKUP:return"BACKUP";case ACT_RECHECK:return"RECHECK";default:return"NONE";}}

bool render658(ANativeWindow*w){
 if(!w)return false;EGLDisplay d=eglGetDisplay(EGL_DEFAULT_DISPLAY);if(d==EGL_NO_DISPLAY||!eglInitialize(d,nullptr,nullptr))return false;
 const EGLint ca[]={EGL_SURFACE_TYPE,EGL_WINDOW_BIT,EGL_RENDERABLE_TYPE,EGL_OPENGL_ES2_BIT,EGL_RED_SIZE,8,EGL_GREEN_SIZE,8,EGL_BLUE_SIZE,8,EGL_NONE};EGLConfig c=nullptr;EGLint n=0;if(!eglChooseConfig(d,ca,&c,1,&n)||n<1){eglTerminate(d);return false;}
 EGLint f=0;eglGetConfigAttrib(d,c,EGL_NATIVE_VISUAL_ID,&f);ANativeWindow_setBuffersGeometry(w,0,0,f);EGLSurface su=eglCreateWindowSurface(d,c,w,nullptr);const EGLint xa[]={EGL_CONTEXT_CLIENT_VERSION,2,EGL_NONE};EGLContext x=eglCreateContext(d,c,EGL_NO_CONTEXT,xa);
 if(su==EGL_NO_SURFACE||x==EGL_NO_CONTEXT||!eglMakeCurrent(d,su,su,x)){if(x!=EGL_NO_CONTEXT)eglDestroyContext(d,x);if(su!=EGL_NO_SURFACE)eglDestroySurface(d,su);eglTerminate(d);return false;}
 EGLint W=0,H=0;eglQuerySurface(d,su,EGL_WIDTH,&W);eglQuerySurface(d,su,EGL_HEIGHT,&H);g_width=W;g_height=H;glViewport(0,0,W,H);glClearColor(.025f,.07f,.15f,1);glClear(GL_COLOR_BUFFER_BIT);
 int s=W/250;if(s<3)s=3;if(s>6)s=6;int st=g_backend_state.load();int lx=5*s,rx=W/2+7*s,top=H-22*s,row=9*s;
 text("ROBOT ADMIN AI OS",lx,top,s,.92f,.96f,1);text("V65.8 TOUCH",rx,top,s,.6f,.75f,.95f);
 text("CPP CORE: READY",lx,top-row,s,.35f,.9f,.55f);text("SAFETY: LOCKED",rx,top-row,s,.35f,.9f,.55f);
 text("MODE: OFFLINE FIRST",lx,top-2*row,s,.75f,.85f,.95f);text("CONTROL: READ ONLY",rx,top-2*row,s,.75f,.85f,.95f);
 text("PY BACKEND:",lx,top-3*row,s,.92f,.96f,1);text(st==1?"CONNECTED":(st==-1?"OPTIONAL OFF":"CHECKING"),lx+76*s,top-3*row,s,st==1?.2f:.95f,st==1?.9f:.65f,st==1?.35f:.25f);
 text("DIAG: "+std::to_string(g_diag.load()),lx,top-4*row,s,.8f,.8f,.9f);text("WRITE DELETE: OFF",rx,top-4*row,s,.95f,.55f,.3f);
 text("TOUCH COUNT: "+std::to_string(g_touch_count.load()),lx,top-5*row,s,.55f,.8f,.95f);text("RESTORE APPLY: OFF",rx,top-5*row,s,.95f,.55f,.3f);
 text(std::string("LAST TOUCH: ")+action_name(g_last_action.load()),lx,top-6*row,s,.35f,.9f,.55f);text("LIVE ORDERS: OFF",rx,top-6*row,s,.95f,.55f,.3f);
 text("TOUCH ENGINE: READY",lx,top-7*row,s,.35f,.9f,.55f);text("AUTO UPDATE: OFF",rx,top-7*row,s,.95f,.55f,.3f);
 text("ROLLBACK: V65.7",lx,top-8*row,s,.55f,.8f,.95f);text("PROD GATE: CLOSED",rx,top-8*row,s,.95f,.55f,.3f);

 int margin=4*s,gap=2*s,by=5*s,bh=14*s,bw=(W-2*margin-4*gap)/5;
 const char* labs[5]={"STATUS","FILES","TASKS","BACKUP","RECHECK"};
 for(int i=0;i<5;i++){int bx=margin+i*(bw+gap);bool sel=g_last_action.load()==i+1;rect(bx,by,bw,bh,sel?.15f:.08f,sel?.42f:.22f,sel?.30f:.36f);int ts=s>4?s-1:s;text(labs[i],bx+2*s,by+4*s,ts,.88f,.95f,1);}
 text("E-STOP READY",rx,by+bh+3*s,s,.35f,.9f,.55f);
 bool ok=eglSwapBuffers(d,su)==EGL_TRUE;eglMakeCurrent(d,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);eglDestroyContext(d,x);eglDestroySurface(d,su);eglTerminate(d);return ok;
}
void redraw658(){std::lock_guard<std::mutex>l(g_mutex);if(g_window)render658(g_window);}
void probe658(){while(g_alive){bool ok=backend_check();if(ok)g_probe_ok++;else g_probe_fail++;int ns=ok?1:-1;int old=g_backend_state.exchange(ns);if(old!=ns)redraw658();for(int i=0;i<50&&g_alive;i++)std::this_thread::sleep_for(std::chrono::milliseconds(100));}g_probe_running=false;}
void start_probe658(){bool e=false;if(g_alive&&g_probe_running.compare_exchange_strong(e,true))std::thread(probe658).detach();}
void do_action(int a){g_last_action=a;g_touch_count++;if(a==ACT_RECHECK){std::thread([](){bool ok=backend_check();if(ok)g_probe_ok++;else g_probe_fail++;g_backend_state=ok?1:-1;redraw658();}).detach();}else redraw658();}
int hit_action(float x,float y_android){int W=g_width.load(),H=g_height.load();if(W<=0||H<=0)return ACT_NONE;int s=W/250;if(s<3)s=3;if(s>6)s=6;float y=(float)H-y_android;int margin=4*s,gap=2*s,by=5*s,bh=14*s,bw=(W-2*margin-4*gap)/5;if(y<by||y>by+bh)return ACT_NONE;for(int i=0;i<5;i++){int bx=margin+i*(bw+gap);if(x>=bx&&x<=bx+bw)return i+1;}return ACT_NONE;}
int input_cb(int, int, void* data){AInputQueue*q=reinterpret_cast<AInputQueue*>(data);if(!q)return 1;AInputEvent*ev=nullptr;while(AInputQueue_getEvent(q,&ev)>=0){if(AInputQueue_preDispatchEvent(q,ev))continue;int handled=0;if(AInputEvent_getType(ev)==AINPUT_EVENT_TYPE_MOTION){int action=AMotionEvent_getAction(ev)&AMOTION_EVENT_ACTION_MASK;if(action==AMOTION_EVENT_ACTION_UP){int a=hit_action(AMotionEvent_getX(ev,0),AMotionEvent_getY(ev,0));if(a!=ACT_NONE){do_action(a);handled=1;}}}AInputQueue_finishEvent(q,ev,handled);}return 1;}
void input_created(ANativeActivity*,AInputQueue*q){g_input_queue=q;g_input_looper=ALooper_forThread();if(!g_input_looper)g_input_looper=ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);if(q&&g_input_looper)AInputQueue_attachLooper(q,g_input_looper,1,input_cb,q);}
void input_destroyed(ANativeActivity*,AInputQueue*q){if(q)AInputQueue_detachLooper(q);if(g_input_queue==q)g_input_queue=nullptr;}
void start658(ANativeActivity*){g_alive=true;start_probe658();}void resume658(ANativeActivity*){redraw658();start_probe658();}
void wc658(ANativeActivity*,ANativeWindow*w){{std::lock_guard<std::mutex>l(g_mutex);if(g_window)ANativeWindow_release(g_window);g_window=w;if(w)ANativeWindow_acquire(w);}redraw658();start_probe658();}
void wr658(ANativeActivity*,ANativeWindow*){redraw658();}
void wd658(ANativeActivity*,ANativeWindow*w){std::lock_guard<std::mutex>l(g_mutex);if(g_window==w&&g_window){ANativeWindow_release(g_window);g_window=nullptr;}}
void destroy658(ANativeActivity*){g_alive=false;if(g_input_queue)AInputQueue_detachLooper(g_input_queue);g_input_queue=nullptr;std::lock_guard<std::mutex>l(g_mutex);if(g_window){ANativeWindow_release(g_window);g_window=nullptr;}}
}
extern "C" const char* ra658_version(){return"65.8";}extern "C" int ra658_core_ready(){return 1;}extern "C" const char* ra658_mode(){return"OFFLINE_FIRST";}extern "C" const char* ra658_safety(){return"LOCKED";}extern "C" int ra658_touch_ready(){return 1;}extern "C" unsigned long ra658_touch_count(){return g_touch_count.load();}
extern "C" int ra658_write_allowed(){return 0;}extern "C" int ra658_delete_allowed(){return 0;}extern "C" int ra658_restore_allowed(){return 0;}extern "C" int ra658_live_orders_allowed(){return 0;}extern "C" int ra658_auto_update_allowed(){return 0;}extern "C" int ra658_production_gate_open(){return 0;}
extern "C" int ra658_write(){return-403;}extern "C" int ra658_apply(){return-403;}extern "C" int ra658_restore(){return-403;}extern "C" int ra658_install(){return-403;}extern "C" int ra658_delete(){return-403;}extern "C" int ra658_repair(){return-403;}extern "C" int ra658_live_order(){return-403;}
extern "C" void ANativeActivity_onCreate(ANativeActivity*a,void*,size_t){if(!a||!a->callbacks)return;a->callbacks->onStart=start658;a->callbacks->onResume=resume658;a->callbacks->onPause=[](ANativeActivity*){};a->callbacks->onStop=[](ANativeActivity*){};a->callbacks->onDestroy=destroy658;a->callbacks->onNativeWindowCreated=wc658;a->callbacks->onNativeWindowRedrawNeeded=wr658;a->callbacks->onNativeWindowDestroyed=wd658;a->callbacks->onInputQueueCreated=input_created;a->callbacks->onInputQueueDestroyed=input_destroyed;}
