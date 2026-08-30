// ROBOT ADMIN AI OS — V69.0 MITRA AI
// Android-only, offline-first, deterministic human-style assistant core.

#include <algorithm>
#include <array>
#include <cctype>
#include <mutex>
#include <string>

namespace ra690 {

enum class Intent { GREETING, STATUS, TASKS, BACKUP, SAFETY, UNKNOWN };

struct ConversationMemory {
    std::array<Intent, 8> recent{};
    std::size_t count = 0;
    Intent last = Intent::UNKNOWN;
};

static ConversationMemory memory;
static std::mutex memory_mutex;

static std::string lower_ascii(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

Intent classify(const std::string& input) {
    const std::string text = lower_ascii(input);
    if (text.find("hello") != std::string::npos || text.find("hi") != std::string::npos ||
        input.find("ನಮಸ್ಕಾರ") != std::string::npos) return Intent::GREETING;
    if (text.find("status") != std::string::npos || input.find("ಸ್ಥಿತಿ") != std::string::npos)
        return Intent::STATUS;
    if (text.find("task") != std::string::npos || input.find("ಕೆಲಸ") != std::string::npos)
        return Intent::TASKS;
    if (text.find("backup") != std::string::npos || input.find("ಬ್ಯಾಕಪ್") != std::string::npos)
        return Intent::BACKUP;
    if (text.find("safety") != std::string::npos || input.find("ಸುರಕ್ಷ") != std::string::npos)
        return Intent::SAFETY;
    return Intent::UNKNOWN;
}

const char* reply(Intent intent) {
    switch (intent) {
        case Intent::GREETING: return "Namaskara. MITRA AI ready to help.";
        case Intent::STATUS: return "Robot Admin is ready, offline-first and read-only.";
        case Intent::TASKS: return "I can review tasks. Execute remains approval-gated.";
        case Intent::BACKUP: return "I can verify backups. Restore and apply remain blocked.";
        case Intent::SAFETY: return "Safety is locked. Write, delete and auto-apply are off.";
        default: return "Please ask about status, tasks, backup or safety.";
    }
}

static bool is_kannada(const std::string& input) {
    return input.find("ನಮಸ್ಕಾರ") != std::string::npos ||
        input.find("ಸ್ಥಿತಿ") != std::string::npos ||
        input.find("ಕೆಲಸ") != std::string::npos ||
        input.find("ಬ್ಯಾಕಪ್") != std::string::npos ||
        input.find("ಸುರಕ್ಷ") != std::string::npos;
}

const char* reply_kannada(Intent intent) {
    switch (intent) {
        case Intent::GREETING: return "ನಮಸ್ಕಾರ. ಮಿತ್ರ AI ಸಹಾಯಕ್ಕೆ ಸಿದ್ಧವಾಗಿದೆ.";
        case Intent::STATUS: return "ರೋಬೋಟ್ ಅಡ್ಮಿನ್ ಸಿದ್ಧವಾಗಿದೆ; ಆಫ್‌ಲೈನ್ ಮತ್ತು ಓದಲು ಮಾತ್ರ.";
        case Intent::TASKS: return "ಕಾರ್ಯಗಳನ್ನು ಪರಿಶೀಲಿಸಬಹುದು; ಕಾರ್ಯಗತಗೊಳಿಸಲು ಅನುಮೋದನೆ ಅಗತ್ಯ.";
        case Intent::BACKUP: return "ಬ್ಯಾಕಪ್ ಪರಿಶೀಲಿಸಬಹುದು; ರಿಸ್ಟೋರ್ ಮತ್ತು ಅಪ್ಲೈ ನಿರ್ಬಂಧಿತ.";
        case Intent::SAFETY: return "ಸುರಕ್ಷತೆ ಲಾಕ್ ಆಗಿದೆ; ಬರೆಯುವುದು ಮತ್ತು ಅಳಿಸುವುದು ಆಫ್.";
        default: return "ಸ್ಥಿತಿ, ಕಾರ್ಯ, ಬ್ಯಾಕಪ್ ಅಥವಾ ಸುರಕ್ಷತೆ ಕುರಿತು ಕೇಳಿ.";
    }
}

static void remember(Intent intent) {
    if (intent == Intent::UNKNOWN) return;
    std::lock_guard<std::mutex> guard(memory_mutex);
    if (memory.count > 0 && memory.last == intent) return;
    if (memory.count < memory.recent.size()) {
        memory.recent[memory.count++] = intent;
    } else {
        std::move(memory.recent.begin() + 1, memory.recent.end(), memory.recent.begin());
        memory.recent.back() = intent;
    }
    memory.last = intent;
}

const char* intent_name(Intent intent) {
    switch (intent) {
        case Intent::GREETING: return "GREETING";
        case Intent::STATUS: return "STATUS";
        case Intent::TASKS: return "TASKS";
        case Intent::BACKUP: return "BACKUP";
        case Intent::SAFETY: return "SAFETY";
        default: return "UNKNOWN";
    }
}

} // namespace ra690

extern "C" const char* ra690_mitra_version() {
    return "69.0-mitra-ai-offline-readonly";
}

extern "C" int ra690_mitra_selftest() {
    using ra690::Intent;
    return ra690::classify("hello") == Intent::GREETING &&
           ra690::classify("system status") == Intent::STATUS &&
           ra690::classify("tasks") == Intent::TASKS &&
           ra690::classify("backup") == Intent::BACKUP &&
           ra690::classify("safety") == Intent::SAFETY &&
           std::string(ra690::reply(Intent::STATUS)).find("read-only") != std::string::npos
        ? 0 : 1;
}

extern "C" const char* ra690_mitra_respond(const char* input) {
    thread_local std::string response;
    const std::string request = input == nullptr ? "" : input;
    const auto intent = ra690::classify(request);
    ra690::remember(intent);
    response = ra690::is_kannada(request) ? ra690::reply_kannada(intent) : ra690::reply(intent);
    return response.c_str();
}

extern "C" int ra690_mitra_memory_count() {
    std::lock_guard<std::mutex> guard(ra690::memory_mutex);
    return static_cast<int>(ra690::memory.count);
}

extern "C" const char* ra690_mitra_last_intent() {
    std::lock_guard<std::mutex> guard(ra690::memory_mutex);
    return ra690::intent_name(ra690::memory.last);
}

extern "C" int ra690_mitra_write_blocked() { return -403; }
extern "C" int ra690_mitra_delete_blocked() { return -403; }
extern "C" int ra690_mitra_apply_blocked() { return -403; }
