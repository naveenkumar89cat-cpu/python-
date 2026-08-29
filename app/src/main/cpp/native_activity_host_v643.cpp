#include <android/native_activity.h>
#include <android/native_window.h>
#include <android/log.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <cstdint>

#define LOG_TAG "RobotAdminV643"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

namespace {
std::mutex g_window_mutex;
ANativeWindow* g_window = nullptr;
std::atomic<int> g_backend_state{0}; // 0=checking, 1=connected, -1=disconnected

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

void fill_rect(ANativeWindow_Buffer& b, int x0, int y0, int x1, int y1, uint32_t rgba) {
    if (!b.bits) return;
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > b.width) x1 = b.width;
    if (y1 > b.height) y1 = b.height;
    auto* pixels = static_cast<uint32_t*>(b.bits);
    for (int y = y0; y < y1; ++y) {
        uint32_t* row = pixels + y * b.stride;
        for (int x = x0; x < x1; ++x) row[x] = rgba;
    }
}

void draw_status_screen() {
    ANativeWindow* window = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_window_mutex);
        if (!g_window) return;
        window = g_window;
        ANativeWindow_acquire(window);
    }

    ANativeWindow_setBuffersGeometry(window, 0, 0, WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer buffer{};
    if (ANativeWindow_lock(window, &buffer, nullptr) == 0) {
        const int state = g_backend_state.load();
        const uint32_t bg = state == 1 ? 0xFF1B6E3A : (state == -1 ? 0xFF6E1B2B : 0xFF1E2A3A);
        fill_rect(buffer, 0, 0, buffer.width, buffer.height, bg);

        const int w = buffer.width;
        const int h = buffer.height;
        const int cx = w / 2;
        const int cy = h / 2;

        // Simple C++ native Robot Admin status mark.
        fill_rect(buffer, cx - w/8, cy - h/8, cx + w/8, cy + h/8, 0xFFF2F2F2);
        fill_rect(buffer, cx - w/11, cy - h/14, cx - w/22, cy - h/28, 0xFF203040);
        fill_rect(buffer, cx + w/22, cy - h/14, cx + w/11, cy - h/28, 0xFF203040);
        fill_rect(buffer, cx - w/18, cy + h/30, cx + w/18, cy + h/18, 0xFF203040);

        // Top safety bar: always white = high-risk APIs remain denied.
        fill_rect(buffer, 0, 0, w, h/28, 0xFFF5F5F5);
        // Bottom state bar mirrors backend state.
        const uint32_t bar = state == 1 ? 0xFF37C871 : (state == -1 ? 0xFFD94A5A : 0xFFF0B43C);
        fill_rect(buffer, 0, h - h/18, w, h, bar);

        ANativeWindow_unlockAndPost(window);
    }
    ANativeWindow_release(window);
}

void run_backend_probe() {
    const bool ok = read_backend_status();
    g_backend_state.store(ok ? 1 : -1);
    LOGI("Installed-host localhost backend runtime check: %s", ok ? "PASS" : "NOT_REACHABLE");
    draw_status_screen();
}

void on_start(ANativeActivity*) {
    LOGI("V64.3 NativeActivity start");
    g_backend_state.store(0);
    draw_status_screen();
    std::thread(run_backend_probe).detach();
}
void on_resume(ANativeActivity*) { LOGI("V64.3 resume"); }
void on_pause(ANativeActivity*) { LOGI("V64.3 pause"); }
void on_stop(ANativeActivity*) { LOGI("V64.3 stop"); }
void on_destroy(ANativeActivity*) {
    LOGI("V64.3 destroy");
    std::lock_guard<std::mutex> lock(g_window_mutex);
    if (g_window) {
        ANativeWindow_release(g_window);
        g_window = nullptr;
    }
}
void on_window_created(ANativeActivity*, ANativeWindow* window) {
    {
        std::lock_guard<std::mutex> lock(g_window_mutex);
        if (g_window) ANativeWindow_release(g_window);
        g_window = window;
        if (g_window) ANativeWindow_acquire(g_window);
    }
    draw_status_screen();
}
void on_window_destroyed(ANativeActivity*, ANativeWindow* window) {
    std::lock_guard<std::mutex> lock(g_window_mutex);
    if (g_window == window && g_window) {
        ANativeWindow_release(g_window);
        g_window = nullptr;
    }
}
}

extern "C" const char* ra643_version() { return "64.3"; }
extern "C" const char* ra643_engine() { return "C++"; }
extern "C" const char* ra643_mode() { return "VISIBLE_READ_ONLY_ANDROID_NATIVE_HOST"; }
extern "C" int ra643_read_backend_status() { return read_backend_status() ? 200 : -1; }
extern "C" int ra643_backend_state() { return g_backend_state.load(); }

extern "C" int ra643_write() { return -403; }
extern "C" int ra643_apply() { return -403; }
extern "C" int ra643_restore() { return -403; }
extern "C" int ra643_install() { return -403; }
extern "C" int ra643_delete() { return -403; }
extern "C" int ra643_repair() { return -403; }
extern "C" int ra643_command() { return -403; }
extern "C" int ra643_live_order() { return -403; }

extern "C" void ANativeActivity_onCreate(ANativeActivity* activity, void*, size_t) {
    if (!activity || !activity->callbacks) return;
    LOGI("V64.3 ANativeActivity_onCreate");
    activity->callbacks->onStart = on_start;
    activity->callbacks->onResume = on_resume;
    activity->callbacks->onPause = on_pause;
    activity->callbacks->onStop = on_stop;
    activity->callbacks->onDestroy = on_destroy;
    activity->callbacks->onNativeWindowCreated = on_window_created;
    activity->callbacks->onNativeWindowDestroyed = on_window_destroyed;
}
