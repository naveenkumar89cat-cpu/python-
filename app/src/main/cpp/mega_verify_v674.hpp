#pragma once
#include <android/asset_manager.h>
#include <string>
#include <algorithm>

// V67.4 trusted verification module. READ ONLY.
namespace ra674 {
static constexpr const char* kVerifyAsset = "v673/verify.txt";
static constexpr const char* kExpectedSha256 = "9c79aec8f8f34beb04ddebf23054f30d265b1dc6e6cd762c53b2a5204f85f4fc";

enum class VerifyState { ASSET_MISSING=0, HASH_MISMATCH=1, MATCH=2 };

inline VerifyState verify_asset(AAssetManager* mgr, std::string (*sha256_fn)(const unsigned char*, size_t), std::string& actual) {
    actual.clear();
    if (!mgr || !sha256_fn) return VerifyState::ASSET_MISSING;
    AAsset* a = AAssetManager_open(mgr, kVerifyAsset, AASSET_MODE_BUFFER);
    if (!a) return VerifyState::ASSET_MISSING;
    const off_t n = AAsset_getLength(a);
    const void* p = AAsset_getBuffer(a);
    if (!p || n < 0) { AAsset_close(a); return VerifyState::ASSET_MISSING; }
    actual = sha256_fn(static_cast<const unsigned char*>(p), static_cast<size_t>(n));
    AAsset_close(a);
    std::string expected(kExpectedSha256);
    std::transform(actual.begin(), actual.end(), actual.begin(), ::tolower);
    return actual == expected ? VerifyState::MATCH : VerifyState::HASH_MISMATCH;
}

inline const char* label(VerifyState s) {
    switch (s) {
        case VerifyState::MATCH: return "MATCH";
        case VerifyState::HASH_MISMATCH: return "FAIL";
        default: return "MISSING";
    }
}

// High-risk operations remain unavailable in this module.
inline int write_allowed() { return 0; }
inline int delete_allowed() { return 0; }
inline int restore_allowed() { return 0; }
inline int apply_allowed() { return 0; }
}
