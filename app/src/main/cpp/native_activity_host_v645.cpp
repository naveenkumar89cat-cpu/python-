#include <android/native_activity.h>
#include <android/native_window.h>
#include <android/log.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

#define LOG_TAG "RobotAdminV645"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

namespace {
std::mutex g_mutex;
ANativeWindow* g_window = nullptr;
std::atomic<int> g_backend_state{0}; // 0 checking, 1 connected, -1 disconnected
std::atomic<bool> g_alive{true};

bool read_backend_status() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return false;

    timeval tv{};
    tv.tv_sec = 3;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) != 1) {
        close(fd);
        return false;
    }
    if (connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
        close(fd);
        return false;
    }

    const char* req =
        "GET /native/foundation/summary HTTP/1.1\r\n"
        "Host: 127.0.0.1:8080\r\n"
        "Connection: close\r\n\r\n";
    if (send(fd, req, std::strlen(req), 0) <= 0) {
        close(fd);
        return false;
    }

    std::string response;
    char buf[2048];
    for (;;) {
        const ssize_t n = recv(fd, buf, sizeof(buf), 0);
        if (n <= 0) break;
        response.append(buf, static_cast<size_t>(n));
        if (response.size() > 65536) break;
    }
    close(fd);

    return response.find("200") != std::string::npos &&
           response.find("PASS") != std::string::npos &&
           response.find("38.0") != std::string::npos;
}

const unsigned char* glyph(char c) {
    static const unsigned char blank[7]={0,0,0,0,0,0,0};
#define G(name,a,b,c,d,e,f,g) static const unsigned char name[7]={a,b,c,d,e,f,g}
    G(A,14,17,17,31,17,17,17); G(B,30,17,17,30,17,17,30); G(C,14,17,16,16,16,17,14);
    G(D,30,17,17,17,17,17,30); G(E,31,16,16,30,16,16,31); G(F,31,16,16,30,16,16,16);
    G(G,14,17,16,23,17,17,15); G(H,17,17,17,31,17,17,17); G(I,31,4,4,4,4,4,31);
    G(J,7,2,2,2,18,18,12); G(K,17,18,20,24,20,18,17); G(L,16,16,16,16,16,16,31);
    G(M,17,27,21,21,17,17,17); G(N,17,25,21,19,17,17,17); G(O,14,17,17,17,17,17,14);
    G(P,30,17,17,30,16,16,16); G(Q,14,17,17,17,21,18,13); G(R,30,17,17,30,20,18,17);
    G(S,15,16,16,14,1,1,30); G(T,31,4,4,4,4,4,4); G(U,17,17,17,17,17,17,14);
    G(V,17,17,17,17,17,10,4); G(W,17,17,17,21,21,21,10); G(X,17,17,10,4,10,17,17);
    G(Y,17,17,10,4,4,4,4); G(Z,31,1,2,4,8,16,31);
    G(N0,14,17,19,21,25,17,14); G(N1,4,12,4,4,4,4,14); G(N2,14,17,1,2,4,8,31);
    G(N3,30,1,1,14,1,1,30); G(N4,2,6,10,18,31,2,2); G(N5,31,16,16,30,1,1,30);
    G(N6,14,16,16,30,17,17,14); G(N7,31,1,2,4,8,8,8); G(N8,14,17,17,14,17,17,14);
    G(N9,14,17,17,15,1,1,14); G(COLON,0,4,4,0,4,4,0); G(DOT,0,0,0,0,0,4,4); G(DASH,0,0,0,31,0,0,0);
#undef G
    switch(c){
        case 'A':return A;case 'B':return B;case 'C':return C;case 'D':return D;case 'E':return E;case 'F':return F;
        case 'G':return G;case 'H':return H;case 'I':return I;case 'J':return J;case 'K':return K;case 'L':return L;
        case 'M':return M;case 'N':return N;case 'O':return O;case 'P':return P;case 'Q':return Q;case 'R':return R;
        case 'S':return S;case 'T':return T;case 'U':return U;case 'V':return V;case 'W':return W;case 'X':return X;
        case 'Y':return Y;case 'Z':return Z;case '0':return N0;case '1':return N1;case '2':return N2;case '3':return N3;
        case '4':return N4;case '5':return N5;case '6':return N6;case '7':return N7;case '8':return N8;case '9':return N9;
        case ':':return COLON;case '.':return DOT;case '-':return DASH;default:return blank;
    }
}

void clear_rect(int x, int y, int w, int h, float r, float g, float b) {
    if (w <= 0 || h <= 0) return;
    glEnable(GL_SCISSOR_TEST);
    glScissor(x,y,w,h);
    glClearColor(r,g,b,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}

void draw_text(const std::string& text, int x, int y, int scale, float r, float g, float b) {
    int pen=x;
    for(char ch:text){
        const unsigned char* rows=glyph(ch);
        for(int row=0;row<7;++row){
            for(int col=0;col<5;++col){
                if(rows[row] & (1u << (4-col))) {
                    const int py = y + (6-row)*scale;
                    clear_rect(pen+col*scale, py, scale, scale, r,g,b);
                }
            }
        }
        pen += 6*scale;
    }
}

bool render_frame_locked(ANativeWindow* window, int state) {
    if (!window) return false;
    EGLDisplay display=eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(display==EGL_NO_DISPLAY || !eglInitialize(display,nullptr,nullptr)) return false;
    const EGLint cfgAttrs[]={EGL_SURFACE_TYPE,EGL_WINDOW_BIT,EGL_RENDERABLE_TYPE,EGL_OPENGL_ES2_BIT,
        EGL_RED_SIZE,8,EGL_GREEN_SIZE,8,EGL_BLUE_SIZE,8,EGL_ALPHA_SIZE,8,EGL_NONE};
    EGLConfig cfg=nullptr; EGLint n=0;
    if(!eglChooseConfig(display,cfgAttrs,&cfg,1,&n)||n<1){eglTerminate(display);return false;}
    EGLint fmt=0; eglGetConfigAttrib(display,cfg,EGL_NATIVE_VISUAL_ID,&fmt);
    ANativeWindow_setBuffersGeometry(window,0,0,fmt);
    EGLSurface surface=eglCreateWindowSurface(display,cfg,window,nullptr);
    if(surface==EGL_NO_SURFACE){eglTerminate(display);return false;}
    const EGLint ctxAttrs[]={EGL_CONTEXT_CLIENT_VERSION,2,EGL_NONE};
    EGLContext ctx=eglCreateContext(display,cfg,EGL_NO_CONTEXT,ctxAttrs);
    if(ctx==EGL_NO_CONTEXT || !eglMakeCurrent(display,surface,surface,ctx)){
        if(ctx!=EGL_NO_CONTEXT) eglDestroyContext(display,ctx); eglDestroySurface(display,surface); eglTerminate(display); return false;
    }
    EGLint w=0,h=0; eglQuerySurface(display,surface,EGL_WIDTH,&w); eglQuerySurface(display,surface,EGL_HEIGHT,&h);
    glViewport(0,0,w,h);
    glClearColor(0.025f,0.07f,0.15f,1.0f); glClear(GL_COLOR_BUFFER_BIT);

    int scale = w/220; if(scale<2) scale=2; if(scale>6) scale=6;
    const float sr = state==1?0.20f:(state==-1?0.90f:0.95f);
    const float sg = state==1?0.90f:(state==-1?0.25f:0.70f);
    const float sb = state==1?0.35f:(state==-1?0.25f:0.20f);

    clear_rect(0,h-10*scale,w,10*scale,0.05f,0.15f,0.30f);
    draw_text("ROBOT ADMIN AI OS", 8*scale, h-25*scale, scale, 0.92f,0.96f,1.0f);
    draw_text("VERSION: 64.5", 8*scale, h-39*scale, scale, 0.60f,0.75f,0.95f);
    draw_text("BACKEND:", 8*scale, h-57*scale, scale, 0.92f,0.96f,1.0f);
    draw_text(state==1?"CONNECTED":(state==-1?"DISCONNECTED":"CHECKING"), 28*scale, h-57*scale, scale, sr,sg,sb);
    draw_text("MODE: READ ONLY", 8*scale, h-75*scale, scale, 0.75f,0.85f,0.95f);
    draw_text("SAFETY: LOCKED", 8*scale, h-89*scale, scale, 0.35f,0.90f,0.55f);
    draw_text("LIVE ORDERS: OFF", 8*scale, h-103*scale, scale, 0.95f,0.55f,0.30f);
    draw_text("AUTO UPDATE: OFF", 8*scale, h-117*scale, scale, 0.95f,0.55f,0.30f);
    draw_text("PRODUCTION GATE: CLOSED", 8*scale, h-131*scale, scale, 0.95f,0.55f,0.30f);
    draw_text("EMERGENCY STOP: READY", 8*scale, h-145*scale, scale, 0.35f,0.90f,0.55f);

    const bool swapped=eglSwapBuffers(display,surface)==EGL_TRUE;
    glFinish(); eglMakeCurrent(display,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);
    eglDestroyContext(display,ctx); eglDestroySurface(display,surface); eglTerminate(display);
    return swapped;
}

void render_current_state(){
    std::lock_guard<std::mutex> lock(g_mutex);
    if(!g_window) return;
    render_frame_locked(g_window,g_backend_state.load());
}

void backend_probe_loop(){
    for(int i=0;i<5 && g_alive.load();++i){
        const bool ok=read_backend_status();
        g_backend_state.store(ok?1:-1);
        LOGI("Backend probe %d: %s",i+1,ok?"PASS":"NOT_REACHABLE");
        render_current_state();
        if(ok) return;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

void on_start(ANativeActivity*){g_alive.store(true);g_backend_state.store(0);render_current_state();std::thread(backend_probe_loop).detach();}
void on_resume(ANativeActivity*){render_current_state();}
void on_pause(ANativeActivity*){}
void on_stop(ANativeActivity*){}
void on_window_created(ANativeActivity*,ANativeWindow* window){
    {std::lock_guard<std::mutex> lock(g_mutex);if(g_window)ANativeWindow_release(g_window);g_window=window;if(g_window)ANativeWindow_acquire(g_window);} render_current_state();
}
void on_window_redraw_needed(ANativeActivity*,ANativeWindow*){render_current_state();}
void on_window_destroyed(ANativeActivity*,ANativeWindow* window){std::lock_guard<std::mutex> lock(g_mutex);if(g_window==window&&g_window){ANativeWindow_release(g_window);g_window=nullptr;}}
void on_destroy(ANativeActivity*){g_alive.store(false);std::lock_guard<std::mutex> lock(g_mutex);if(g_window){ANativeWindow_release(g_window);g_window=nullptr;}}
}

extern "C" const char* ra645_version(){return "64.5";}
extern "C" int ra645_backend_state(){return g_backend_state.load();}
extern "C" int ra645_write(){return -403;} extern "C" int ra645_apply(){return -403;} extern "C" int ra645_restore(){return -403;}
extern "C" int ra645_install(){return -403;} extern "C" int ra645_delete(){return -403;} extern "C" int ra645_repair(){return -403;}
extern "C" int ra645_command(){return -403;} extern "C" int ra645_live_order(){return -403;}

extern "C" void ANativeActivity_onCreate(ANativeActivity* activity,void*,size_t){
    if(!activity||!activity->callbacks)return;
    activity->callbacks->onStart=on_start; activity->callbacks->onResume=on_resume; activity->callbacks->onPause=on_pause;
    activity->callbacks->onStop=on_stop; activity->callbacks->onDestroy=on_destroy;
    activity->callbacks->onNativeWindowCreated=on_window_created; activity->callbacks->onNativeWindowRedrawNeeded=on_window_redraw_needed;
    activity->callbacks->onNativeWindowDestroyed=on_window_destroyed;
}
