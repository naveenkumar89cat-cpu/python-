#include <android/native_activity.h>
#include <android/native_window.h>
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

#define LOG_TAG "RobotAdminV646"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
namespace {
std::mutex g_mutex; ANativeWindow* g_window=nullptr; std::atomic<int> g_state{0}; std::atomic<int> g_diag{0}; std::atomic<bool> g_alive{true}; std::atomic<bool> g_probe_started{false};
// diag: 0 checking, 1 socket, 2 connect, 3 send, 4 recv, 5 validate, 200 connected
bool backend_check(){
 int fd=socket(AF_INET,SOCK_STREAM,0); if(fd<0){g_diag=1;return false;}
 int flags=fcntl(fd,F_GETFL,0); if(flags>=0) fcntl(fd,F_SETFL,flags|O_NONBLOCK);
 sockaddr_in a{};a.sin_family=AF_INET;a.sin_port=htons(8080);inet_pton(AF_INET,"127.0.0.1",&a.sin_addr);
 int rc=connect(fd,reinterpret_cast<sockaddr*>(&a),sizeof(a));
 if(rc!=0 && errno!=EINPROGRESS){g_diag=2;close(fd);return false;}
 if(rc!=0){pollfd p{fd,POLLOUT,0}; if(poll(&p,1,2500)<=0){g_diag=2;close(fd);return false;} int e=0;socklen_t l=sizeof(e);getsockopt(fd,SOL_SOCKET,SO_ERROR,&e,&l);if(e){g_diag=2;close(fd);return false;}}
 if(flags>=0) fcntl(fd,F_SETFL,flags);
 timeval tv{};tv.tv_sec=3;setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv));
 const char* q="GET /native/foundation/summary HTTP/1.1\r\nHost: 127.0.0.1:8080\r\nConnection: close\r\n\r\n";
 if(send(fd,q,strlen(q),0)<=0){g_diag=3;close(fd);return false;}
 std::string s;char b[2048];for(;;){ssize_t n=recv(fd,b,sizeof(b),0);if(n<=0)break;s.append(b,(size_t)n);if(s.size()>65536)break;}close(fd);
 if(s.empty()){g_diag=4;return false;}
 bool ok=s.find("200")!=std::string::npos&&s.find("PASS")!=std::string::npos&&s.find("38.0")!=std::string::npos;g_diag=ok?200:5;return ok;
}
const unsigned char* glyph(char c){static const unsigned char z[7]={0,0,0,0,0,0,0};
#define G(n,a,b,c,d,e,f,g) static const unsigned char n[7]={a,b,c,d,e,f,g}
G(A,14,17,17,31,17,17,17);G(B,30,17,17,30,17,17,30);G(C,14,17,16,16,16,17,14);G(D,30,17,17,17,17,17,30);G(E,31,16,16,30,16,16,31);G(F,31,16,16,30,16,16,16);G(G,14,17,16,23,17,17,15);G(H,17,17,17,31,17,17,17);G(I,31,4,4,4,4,4,31);G(J,7,2,2,2,18,18,12);G(K,17,18,20,24,20,18,17);G(L,16,16,16,16,16,16,31);G(M,17,27,21,21,17,17,17);G(N,17,25,21,19,17,17,17);G(O,14,17,17,17,17,17,14);G(P,30,17,17,30,16,16,16);G(Q,14,17,17,17,21,18,13);G(R,30,17,17,30,20,18,17);G(S,15,16,16,14,1,1,30);G(T,31,4,4,4,4,4,4);G(U,17,17,17,17,17,17,14);G(V,17,17,17,17,17,10,4);G(W,17,17,17,21,21,21,10);G(X,17,17,10,4,10,17,17);G(Y,17,17,10,4,4,4,4);G(Z,31,1,2,4,8,16,31);G(N0,14,17,19,21,25,17,14);G(N1,4,12,4,4,4,4,14);G(N2,14,17,1,2,4,8,31);G(N3,30,1,1,14,1,1,30);G(N4,2,6,10,18,31,2,2);G(N5,31,16,16,30,1,1,30);G(N6,14,16,16,30,17,17,14);G(N7,31,1,2,4,8,8,8);G(N8,14,17,17,14,17,17,14);G(N9,14,17,17,15,1,1,14);G(COL,0,4,4,0,4,4,0);G(DOT,0,0,0,0,0,4,4);G(DASH,0,0,0,31,0,0,0);
#undef G
 switch(c){case'A':return A;case'B':return B;case'C':return C;case'D':return D;case'E':return E;case'F':return F;case'G':return G;case'H':return H;case'I':return I;case'J':return J;case'K':return K;case'L':return L;case'M':return M;case'N':return N;case'O':return O;case'P':return P;case'Q':return Q;case'R':return R;case'S':return S;case'T':return T;case'U':return U;case'V':return V;case'W':return W;case'X':return X;case'Y':return Y;case'Z':return Z;case'0':return N0;case'1':return N1;case'2':return N2;case'3':return N3;case'4':return N4;case'5':return N5;case'6':return N6;case'7':return N7;case'8':return N8;case'9':return N9;case':':return COL;case'.':return DOT;case'-':return DASH;default:return z;}}
void rect(int x,int y,int w,int h,float r,float g,float b){if(w<=0||h<=0)return;glEnable(GL_SCISSOR_TEST);glScissor(x,y,w,h);glClearColor(r,g,b,1);glClear(GL_COLOR_BUFFER_BIT);glDisable(GL_SCISSOR_TEST);}void text(const std::string&t,int x,int y,int s,float r,float g,float b){int p=x;for(char c:t){auto q=glyph(c);for(int a=0;a<7;a++)for(int d=0;d<5;d++)if(q[a]&(1u<<(4-d)))rect(p+d*s,y+(6-a)*s,s,s,r,g,b);p+=6*s;}}
bool render(ANativeWindow*w){if(!w)return false;EGLDisplay d=eglGetDisplay(EGL_DEFAULT_DISPLAY);if(d==EGL_NO_DISPLAY||!eglInitialize(d,nullptr,nullptr))return false;const EGLint ca[]={EGL_SURFACE_TYPE,EGL_WINDOW_BIT,EGL_RENDERABLE_TYPE,EGL_OPENGL_ES2_BIT,EGL_RED_SIZE,8,EGL_GREEN_SIZE,8,EGL_BLUE_SIZE,8,EGL_NONE};EGLConfig c=nullptr;EGLint n=0;if(!eglChooseConfig(d,ca,&c,1,&n)||n<1){eglTerminate(d);return false;}EGLint f=0;eglGetConfigAttrib(d,c,EGL_NATIVE_VISUAL_ID,&f);ANativeWindow_setBuffersGeometry(w,0,0,f);EGLSurface su=eglCreateWindowSurface(d,c,w,nullptr);const EGLint xa[]={EGL_CONTEXT_CLIENT_VERSION,2,EGL_NONE};EGLContext x=eglCreateContext(d,c,EGL_NO_CONTEXT,xa);if(su==EGL_NO_SURFACE||x==EGL_NO_CONTEXT||!eglMakeCurrent(d,su,su,x)){if(x!=EGL_NO_CONTEXT)eglDestroyContext(d,x);if(su!=EGL_NO_SURFACE)eglDestroySurface(d,su);eglTerminate(d);return false;}EGLint W=0,H=0;eglQuerySurface(d,su,EGL_WIDTH,&W);eglQuerySurface(d,su,EGL_HEIGHT,&H);glViewport(0,0,W,H);glClearColor(.025f,.07f,.15f,1);glClear(GL_COLOR_BUFFER_BIT);int s=W/220;if(s<2)s=2;if(s>6)s=6;int st=g_state.load(),dg=g_diag.load();float r=st==1?.2f:(st==-1?.9f:.95f),g=st==1?.9f:(st==-1?.25f:.7f),b=st==1?.35f:(st==-1?.25f:.2f);text("ROBOT ADMIN AI OS",8*s,H-25*s,s,.92f,.96f,1);text("VERSION: 64.6",8*s,H-39*s,s,.6f,.75f,.95f);text("BACKEND:",8*s,H-57*s,s,.92f,.96f,1);text(st==1?"CONNECTED":(st==-1?"DISCONNECTED":"CHECKING"),28*s,H-57*s,s,r,g,b);text("DIAG: "+std::to_string(dg),8*s,H-71*s,s,.8f,.8f,.9f);text("MODE: READ ONLY",8*s,H-85*s,s,.75f,.85f,.95f);text("SAFETY: LOCKED",8*s,H-99*s,s,.35f,.9f,.55f);text("LIVE ORDERS: OFF",8*s,H-113*s,s,.95f,.55f,.3f);text("AUTO UPDATE: OFF",8*s,H-127*s,s,.95f,.55f,.3f);text("PRODUCTION GATE: CLOSED",8*s,H-141*s,s,.95f,.55f,.3f);text("EMERGENCY STOP: READY",8*s,H-155*s,s,.35f,.9f,.55f);bool ok=eglSwapBuffers(d,su)==EGL_TRUE;eglMakeCurrent(d,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);eglDestroyContext(d,x);eglDestroySurface(d,su);eglTerminate(d);return ok;}
void redraw(){std::lock_guard<std::mutex>l(g_mutex);if(g_window)render(g_window);}void probe(){for(int i=0;i<8&&g_alive;i++){g_diag=0;bool ok=backend_check();g_state=ok?1:-1;LOGI("probe %d state=%d diag=%d",i+1,g_state.load(),g_diag.load());redraw();if(ok)return;std::this_thread::sleep_for(std::chrono::seconds(2));}}
void start_probe_once(){bool expected=false;if(g_probe_started.compare_exchange_strong(expected,true))std::thread(probe).detach();}
void on_start(ANativeActivity*){g_alive=true;g_state=0;g_diag=0;}void on_resume(ANativeActivity*){redraw();start_probe_once();}void on_pause(ANativeActivity*){}void on_stop(ANativeActivity*){}void wc(ANativeActivity*,ANativeWindow*w){{std::lock_guard<std::mutex>l(g_mutex);if(g_window)ANativeWindow_release(g_window);g_window=w;if(w)ANativeWindow_acquire(w);}redraw();start_probe_once();}void wr(ANativeActivity*,ANativeWindow*){redraw();}void wd(ANativeActivity*,ANativeWindow*w){std::lock_guard<std::mutex>l(g_mutex);if(g_window==w&&g_window){ANativeWindow_release(g_window);g_window=nullptr;}}void destroy(ANativeActivity*){g_alive=false;std::lock_guard<std::mutex>l(g_mutex);if(g_window){ANativeWindow_release(g_window);g_window=nullptr;}}
}
extern "C" const char* ra646_version(){return"64.6";}extern "C" int ra646_write(){return-403;}extern "C" int ra646_apply(){return-403;}extern "C" int ra646_restore(){return-403;}extern "C" int ra646_install(){return-403;}extern "C" int ra646_delete(){return-403;}extern "C" int ra646_repair(){return-403;}extern "C" int ra646_live_order(){return-403;}
extern "C" void ANativeActivity_onCreate(ANativeActivity*a,void*,size_t){if(!a||!a->callbacks)return;a->callbacks->onStart=on_start;a->callbacks->onResume=on_resume;a->callbacks->onPause=on_pause;a->callbacks->onStop=on_stop;a->callbacks->onDestroy=destroy;a->callbacks->onNativeWindowCreated=wc;a->callbacks->onNativeWindowRedrawNeeded=wr;a->callbacks->onNativeWindowDestroyed=wd;}
