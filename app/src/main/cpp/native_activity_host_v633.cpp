#include <android/native_activity.h>
#include <android/log.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>

#define LOG_TAG "RobotAdminV633"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static bool read_backend_status() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return false;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

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

extern "C" const char* ra633_version() { return "63.3"; }
extern "C" const char* ra633_engine() { return "C++"; }
extern "C" const char* ra633_mode() { return "READ_ONLY_ANDROID_NATIVE_HOST"; }
extern "C" int ra633_read_backend_status() { return read_backend_status() ? 200 : -1; }

extern "C" int ra633_write() { return -403; }
extern "C" int ra633_apply() { return -403; }
extern "C" int ra633_restore() { return -403; }
extern "C" int ra633_install() { return -403; }
extern "C" int ra633_delete() { return -403; }
extern "C" int ra633_repair() { return -403; }
extern "C" int ra633_command() { return -403; }
extern "C" int ra633_live_order() { return -403; }

static void on_start(ANativeActivity*) {
    LOGI("V63.3 NativeActivity start");
    const bool ok = read_backend_status();
    LOGI("Read-only localhost backend check: %s", ok ? "PASS" : "NOT_REACHABLE");
}
static void on_resume(ANativeActivity*) { LOGI("V63.3 resume"); }
static void on_pause(ANativeActivity*) { LOGI("V63.3 pause"); }
static void on_stop(ANativeActivity*) { LOGI("V63.3 stop"); }
static void on_destroy(ANativeActivity*) { LOGI("V63.3 destroy"); }

extern "C" void ANativeActivity_onCreate(ANativeActivity* activity, void*, size_t) {
    if (!activity || !activity->callbacks) return;
    LOGI("V63.3 ANativeActivity_onCreate");
    activity->callbacks->onStart = on_start;
    activity->callbacks->onResume = on_resume;
    activity->callbacks->onPause = on_pause;
    activity->callbacks->onStop = on_stop;
    activity->callbacks->onDestroy = on_destroy;
}
