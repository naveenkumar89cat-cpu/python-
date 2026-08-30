// ROBOT ADMIN AI OS — V69.0 MITRA AI
// Android-only, offline-first, deterministic human-style assistant core.

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <mutex>
#include <string>

namespace ra690 {

enum class Intent { GREETING, STATUS, TASKS, BACKUP, SAFETY, APPROVAL_REQUIRED, UNKNOWN };
enum class RiskAction { NONE, WRITE, DELETE_ACTION, RESTORE, APPLY, EXECUTE, LIVE_ORDER };
enum class GateState { IDLE, PREVIEW_PENDING, APPROVED_LOCKED, CANCELLED };

struct ConversationMemory {
    std::array<Intent, 8> recent{};
    std::size_t count = 0;
    Intent last = Intent::UNKNOWN;
};

static ConversationMemory memory;
static std::mutex memory_mutex;

struct ChatEntry {
    std::string input;
    std::string reply;
};

static std::array<ChatEntry, 8> chat_history;
static std::size_t chat_history_count = 0;
static std::mutex chat_history_mutex;

struct ApprovalGate {
    GateState state = GateState::IDLE;
    RiskAction action = RiskAction::NONE;
    std::uint64_t request_id = 0;
    std::string preview;
};

static ApprovalGate approval_gate;
static std::mutex approval_mutex;

static std::string lower_ascii(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

static std::string trim_ascii(std::string value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return {};
    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

static std::string utf8_prefix(const std::string& value, std::size_t max_codepoints) {
    std::size_t offset = 0;
    std::size_t count = 0;
    while (offset < value.size() && count < max_codepoints) {
        const unsigned char lead = static_cast<unsigned char>(value[offset]);
        std::size_t width = 1;
        if ((lead & 0xE0U) == 0xC0U) width = 2;
        else if ((lead & 0xF0U) == 0xE0U) width = 3;
        else if ((lead & 0xF8U) == 0xF0U) width = 4;
        if (offset + width > value.size()) break;
        bool valid = true;
        for (std::size_t i = 1; i < width; ++i) {
            if ((static_cast<unsigned char>(value[offset + i]) & 0xC0U) != 0x80U) {
                valid = false;
                break;
            }
        }
        offset += valid ? width : 1;
        ++count;
    }
    return value.substr(0, offset);
}

static void record_chat(const std::string& input, const std::string& response) {
    ChatEntry entry{utf8_prefix(input, 96), utf8_prefix(response, 160)};
    std::lock_guard<std::mutex> guard(chat_history_mutex);
    if (chat_history_count < chat_history.size()) {
        chat_history[chat_history_count++] = std::move(entry);
        return;
    }
    for (std::size_t i = 1; i < chat_history.size(); ++i)
        chat_history[i - 1] = std::move(chat_history[i]);
    chat_history.back() = std::move(entry);
}

static RiskAction risk_action(const std::string& input) {
    const std::string text = lower_ascii(input);
    if (text.find("live order") != std::string::npos) return RiskAction::LIVE_ORDER;
    if (text.find("delete") != std::string::npos || input.find("ಅಳಿಸ") != std::string::npos)
        return RiskAction::DELETE_ACTION;
    if (text.find("restore") != std::string::npos || input.find("ರಿಸ್ಟೋರ್") != std::string::npos)
        return RiskAction::RESTORE;
    if (text.find("apply") != std::string::npos || input.find("ಅಪ್ಲೈ") != std::string::npos)
        return RiskAction::APPLY;
    if (text.find("execute") != std::string::npos) return RiskAction::EXECUTE;
    if (text.find("write") != std::string::npos || input.find("ಬರೆಯ") != std::string::npos)
        return RiskAction::WRITE;
    return RiskAction::NONE;
}

static const char* risk_name(RiskAction action) {
    switch (action) {
        case RiskAction::WRITE: return "WRITE";
        case RiskAction::DELETE_ACTION: return "DELETE";
        case RiskAction::RESTORE: return "RESTORE";
        case RiskAction::APPLY: return "APPLY";
        case RiskAction::EXECUTE: return "EXECUTE";
        case RiskAction::LIVE_ORDER: return "LIVE_ORDER";
        default: return "NONE";
    }
}

static bool is_approve_command(const std::string& input) {
    const std::string command = lower_ascii(trim_ascii(input));
    return command == "approve" || command == "ಅನುಮೋದಿಸು" || command == "ಒಪ್ಪಿಗೆ";
}

static bool is_cancel_command(const std::string& input) {
    const std::string command = lower_ascii(trim_ascii(input));
    return command == "cancel" || command == "ರದ್ದು" || command == "ರದ್ದುಮಾಡು";
}

Intent classify(const std::string& input) {
    const std::string text = lower_ascii(input);
    if (risk_action(input) != RiskAction::NONE) return Intent::APPROVAL_REQUIRED;
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
        case Intent::APPROVAL_REQUIRED: return "Approval required. No high-risk action was executed.";
        default: return "Please ask about status, tasks, backup or safety.";
    }
}

static bool is_kannada(const std::string& input) {
    return input.find("ನಮಸ್ಕಾರ") != std::string::npos ||
        input.find("ಸ್ಥಿತಿ") != std::string::npos ||
        input.find("ಕೆಲಸ") != std::string::npos ||
        input.find("ಬ್ಯಾಕಪ್") != std::string::npos ||
        input.find("ಸುರಕ್ಷ") != std::string::npos ||
        input.find("ಅಳಿಸ") != std::string::npos ||
        input.find("ಬರೆಯ") != std::string::npos ||
        input.find("ರಿಸ್ಟೋರ್") != std::string::npos ||
        input.find("ಅಪ್ಲೈ") != std::string::npos ||
        input.find("ಅನುಮೋದಿಸು") != std::string::npos ||
        input.find("ಒಪ್ಪಿಗೆ") != std::string::npos ||
        input.find("ರದ್ದು") != std::string::npos;
}

const char* reply_kannada(Intent intent) {
    switch (intent) {
        case Intent::GREETING: return "ನಮಸ್ಕಾರ. ಮಿತ್ರ AI ಸಹಾಯಕ್ಕೆ ಸಿದ್ಧವಾಗಿದೆ.";
        case Intent::STATUS: return "ರೋಬೋಟ್ ಅಡ್ಮಿನ್ ಸಿದ್ಧವಾಗಿದೆ; ಆಫ್‌ಲೈನ್ ಮತ್ತು ಓದಲು ಮಾತ್ರ.";
        case Intent::TASKS: return "ಕಾರ್ಯಗಳನ್ನು ಪರಿಶೀಲಿಸಬಹುದು; ಕಾರ್ಯಗತಗೊಳಿಸಲು ಅನುಮೋದನೆ ಅಗತ್ಯ.";
        case Intent::BACKUP: return "ಬ್ಯಾಕಪ್ ಪರಿಶೀಲಿಸಬಹುದು; ರಿಸ್ಟೋರ್ ಮತ್ತು ಅಪ್ಲೈ ನಿರ್ಬಂಧಿತ.";
        case Intent::SAFETY: return "ಸುರಕ್ಷತೆ ಲಾಕ್ ಆಗಿದೆ; ಬರೆಯುವುದು ಮತ್ತು ಅಳಿಸುವುದು ಆಫ್.";
        case Intent::APPROVAL_REQUIRED: return "ಅನುಮೋದನೆ ಅಗತ್ಯ. ಯಾವುದೇ ಅಪಾಯಕಾರಿ ಕಾರ್ಯ ನಡೆದಿಲ್ಲ.";
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
        case Intent::APPROVAL_REQUIRED: return "APPROVAL_REQUIRED";
        default: return "UNKNOWN";
    }
}

static bool is_follow_up(const std::string& input) {
    const std::string text = lower_ascii(input);
    return text == "more" || text == "details" || text == "continue" || text == "why" ||
        input.find("ಇನ್ನಷ್ಟು") != std::string::npos ||
        input.find("ವಿವರ") != std::string::npos ||
        input.find("ಮುಂದೆ") != std::string::npos ||
        input.find("ಏಕೆ") != std::string::npos;
}

static Intent contextual_intent(const std::string& input) {
    Intent intent = classify(input);
    if (intent != Intent::UNKNOWN || !is_follow_up(input)) return intent;
    std::lock_guard<std::mutex> guard(memory_mutex);
    return memory.last;
}

static std::string preview_command(const std::string& input, bool kannada) {
    const RiskAction action = risk_action(input);
    if (action == RiskAction::NONE) return {};
    std::lock_guard<std::mutex> guard(approval_mutex);
    if (approval_gate.state == GateState::PREVIEW_PENDING) {
        return kannada
            ? "ಒಂದು ಅನುಮೋದನೆ ಬಾಕಿಯಿದೆ. ಮೊದಲು APPROVE ಅಥವಾ CANCEL ಎಂದು ಉತ್ತರಿಸಿ."
            : "An approval is pending. Reply APPROVE or CANCEL first.";
    }
    approval_gate.state = GateState::PREVIEW_PENDING;
    approval_gate.action = action;
    ++approval_gate.request_id;
    approval_gate.preview = "PREVIEW #" + std::to_string(approval_gate.request_id) + ": " +
        risk_name(action) + " | READ-ONLY | NO ACTION EXECUTED";
    return kannada
        ? approval_gate.preview + " | ಮುಂದುವರಿಸಲು APPROVE, ನಿಲ್ಲಿಸಲು CANCEL."
        : approval_gate.preview + " | Reply APPROVE or CANCEL.";
}

static std::string approve_command(bool kannada) {
    std::lock_guard<std::mutex> guard(approval_mutex);
    if (approval_gate.state != GateState::PREVIEW_PENDING) {
        return kannada ? "ಅನುಮೋದನೆಗೆ ಯಾವುದೇ ಬಾಕಿ ಆದೇಶವಿಲ್ಲ."
                       : "No command is pending approval.";
    }
    const std::string action = risk_name(approval_gate.action);
    approval_gate.state = GateState::APPROVED_LOCKED;
    approval_gate.preview.clear();
    return kannada
        ? "APPROVED: " + action + " | ಓದಲು-ಮಾತ್ರ ಸುರಕ್ಷತಾ ಲಾಕ್ ಕಾರಣ ಕಾರ್ಯಗತವಾಗಿಲ್ಲ."
        : "APPROVED: " + action + " | Execution remains blocked by the read-only safety lock.";
}

static std::string cancel_command(bool kannada) {
    std::lock_guard<std::mutex> guard(approval_mutex);
    if (approval_gate.state != GateState::PREVIEW_PENDING) {
        return kannada ? "ರದ್ದು ಮಾಡಲು ಯಾವುದೇ ಬಾಕಿ ಆದೇಶವಿಲ್ಲ."
                       : "No command is pending cancellation.";
    }
    const std::string action = risk_name(approval_gate.action);
    approval_gate.state = GateState::CANCELLED;
    approval_gate.preview.clear();
    return kannada
        ? "CANCELLED: " + action + " | ಯಾವುದೇ ಕಾರ್ಯ ನಡೆದಿಲ್ಲ."
        : "CANCELLED: " + action + " | No action was executed.";
}

static const char* gate_state_name(GateState state) {
    switch (state) {
        case GateState::PREVIEW_PENDING: return "PREVIEW_PENDING";
        case GateState::APPROVED_LOCKED: return "APPROVED_LOCKED";
        case GateState::CANCELLED: return "CANCELLED";
        default: return "IDLE";
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
    const bool kannada = ra690::is_kannada(request);
    if (ra690::is_approve_command(request)) {
        response = ra690::approve_command(kannada);
    } else if (ra690::is_cancel_command(request)) {
        response = ra690::cancel_command(kannada);
    } else if (ra690::risk_action(request) != ra690::RiskAction::NONE) {
        response = ra690::preview_command(request, kannada);
    } else {
        const auto intent = ra690::contextual_intent(request);
        ra690::remember(intent);
        response = kannada ? ra690::reply_kannada(intent) : ra690::reply(intent);
    }
    ra690::record_chat(request, response);
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

extern "C" int ra690_mitra_approval_required(const char* input) {
    return ra690::classify(input == nullptr ? "" : input) == ra690::Intent::APPROVAL_REQUIRED ? 1 : 0;
}

extern "C" const char* ra690_mitra_gate_state() {
    std::lock_guard<std::mutex> guard(ra690::approval_mutex);
    return ra690::gate_state_name(ra690::approval_gate.state);
}

extern "C" const char* ra690_mitra_pending_action() {
    std::lock_guard<std::mutex> guard(ra690::approval_mutex);
    return ra690::approval_gate.state == ra690::GateState::PREVIEW_PENDING
        ? ra690::risk_name(ra690::approval_gate.action) : "NONE";
}

extern "C" const char* ra690_mitra_pending_preview() {
    thread_local std::string preview;
    std::lock_guard<std::mutex> guard(ra690::approval_mutex);
    preview = ra690::approval_gate.state == ra690::GateState::PREVIEW_PENDING
        ? ra690::approval_gate.preview : "";
    return preview.c_str();
}

extern "C" int ra690_mitra_has_pending_approval() {
    std::lock_guard<std::mutex> guard(ra690::approval_mutex);
    return ra690::approval_gate.state == ra690::GateState::PREVIEW_PENDING ? 1 : 0;
}

extern "C" int ra690_mitra_executed_actions() { return 0; }

extern "C" int ra690_mitra_history_count() {
    std::lock_guard<std::mutex> guard(ra690::chat_history_mutex);
    return static_cast<int>(ra690::chat_history_count);
}

extern "C" const char* ra690_mitra_history_input(int index) {
    thread_local std::string value;
    std::lock_guard<std::mutex> guard(ra690::chat_history_mutex);
    value = index >= 0 && static_cast<std::size_t>(index) < ra690::chat_history_count
        ? ra690::chat_history[static_cast<std::size_t>(index)].input : "";
    return value.c_str();
}

extern "C" const char* ra690_mitra_history_reply(int index) {
    thread_local std::string value;
    std::lock_guard<std::mutex> guard(ra690::chat_history_mutex);
    value = index >= 0 && static_cast<std::size_t>(index) < ra690::chat_history_count
        ? ra690::chat_history[static_cast<std::size_t>(index)].reply : "";
    return value.c_str();
}

extern "C" int ra690_mitra_write_blocked() { return -403; }
extern "C" int ra690_mitra_delete_blocked() { return -403; }
extern "C" int ra690_mitra_apply_blocked() { return -403; }
extern "C" int ra690_mitra_execute_blocked() { return -403; }
