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

#define LOG_TAG "RobotAdminV644"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace {
std::mutex g_mutex;
ANativeWindow* g_window = nullptr;
std::atomic<int> g_backend_state{0}; // 0 checking, 1 connected, -1 disconnected

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

    return response.find("HTTP/1.1 200") != std::string::npos &&
           response.find("PASS") != std::string::npos &&
           response.find("38.0") != std::string::npos;
}

bool render_frame_locked(ANativeWindow* window, int state) {
    if (!window) return false;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) return false;
    if (!eglInitialize(display, nullptr, nullptr)) return false;

    const EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_NONE
    };

    EGLConfig config = nullptr;
    EGLint num_configs = 0;
    if (!eglChooseConfig(display, config_attribs, &config, 1, &num_configs) || num_configs < 1) {
        eglTerminate(display);
        return false;
    }

    EGLint format = 0;
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(window, 0, 0, format);

    EGLSurface surface = eglCreateWindowSurface(display, config, window, nullptr);
    if (surface == EGL_NO_SURFACE) {
        eglTerminate(display);
        return false;
    }

    const EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attribs);
    if (context == EGL_NO_CONTEXT) {
        eglDestroySurface(display, surface);
        eglTerminate(display);
        return false;
    }

    if (!eglMakeCurrent(display, surface, surface, context)) {
        eglDestroyContext(display, context);
        eglDestroySurface(display, surface);
        eglTerminate(display);
        return false;
    }

    EGLint width = 0, height = 0;
    eglQuerySurface(display, surface, EGL_WIDTH, &width);
    eglQuerySurface(display, surface, EGL_HEIGHT, &height);
    glViewport(0, 0, width, height);

    // V64.4 visual proof colors:
    // green = connected, red = disconnected, blue = checking.
    if (state == 1) glClearColor(0.035f, 0.34f, 0.16f, 1.0f);
    else if (state == -1) glClearColor(0.42f, 0.05f, 0.08f, 1.0f);
    else glClearColor(0.04f, 0.12f, 0.28f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    const bool swapped = eglSwapBuffers(display, surface) == EGL_TRUE;
    glFinish();

    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(display, context);
    eglDestroySurface(display, surface);
    eglTerminate(display);
    return swapped;
}

void render_current_state() {
    std::lock_guard<std::mutex> lock(g_mutex);
    if (!g_window) return;
    const int state = g_backend_state.load();
    const bool ok = render_frame_locked(g_window, state);
    LOGI("EGL first-frame render state=%d result=%s", state, ok ? "PASS" : "FAIL");
}

void backend_probe() {
    const bool ok = read_backend_status();
    g_backend_state.store(ok ? 1 : -1);
    LOGI("Read-only localhost backend check: %s", ok ? "PASS" : "NOT_REACHABLE");
    render_current_state();
}

void on_start(ANativeActivity*) {
    LOGI("V64.4 start");
    g_backend_state.store(0);
    render_current_state();
    std::thread(backend_probe).detach();
}
void on_resume(ANativeActivity*) { LOGI("V64.4 resume"); }
void on_pause(ANativeActivity*) { LOGI("V64.4 pause"); }
void on_stop(ANativeActivity*) { LOGI("V64.4 stop"); }

void on_window_created(ANativeActivity*, ANativeWindow* window) {
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (g_window) ANativeWindow_release(g_window);
        g_window = window;
        if (g_window) ANativeWindow_acquire(g_window);
    }
    LOGI("V64.4 native window created");
    render_current_state();
}

void on_window_redraw_needed(ANativeActivity*, ANativeWindow*) {
    render_current_state();
}

void on_window_destroyed(ANativeActivity*, ANativeWindow* window) {
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_window == window && g_window) {
        ANativeWindow_release(g_window);
        g_window = nullptr;
    }
}

void on_destroy(ANativeActivity*) {
    LOGI("V64.4 destroy");
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_window) {
        ANativeWindow_release(g_window);
        g_window = nullptr;
    }
}
}

extern "C" const char* ra644_version() { return "64.4"; }
extern "C" const char* ra644_engine() { return "C++"; }
extern "C" const char* ra644_mode() { return "EGL_READ_ONLY_ANDROID_NATIVE_HOST"; }
extern "C" int ra644_read_backend_status() { return read_backend_status() ? 200 : -1; }
extern "C" int ra644_backend_state() { return g_backend_state.load(); }

extern "C" int ra644_write() { return -403; }
extern "C" int ra644_apply() { return -403; }
extern "C" int ra644_restore() { return -403; }
extern "C" int ra644_install() { return -403; }
extern "C" int ra644_delete() { return -403; }
extern "C" int ra644_repair() { return -403; }
extern "C" int ra644_command() { return -403; }
extern "C" int ra644_live_order() { return -403; }

extern "C" void ANativeActivity_onCreate(ANativeActivity* activity, void*, size_t) {
    if (!activity || !activity->callbacks) return;
    LOGI("V64.4 ANativeActivity_onCreate");
    activity->callbacks->onStart = on_start;
    activity->callbacks->onResume = on_resume;
    activity->callbacks->onPause = on_pause;
    activity->callbacks->onStop = on_stop;
    activity->callbacks->onDestroy = on_destroy;
    activity->callbacks->onNativeWindowCreated = on_window_created;
    activity->callbacks->onNativeWindowRedrawNeeded = on_window_redraw_needed;
    activity->callbacks->onNativeWindowDestroyed = on_window_destroyed;
}
