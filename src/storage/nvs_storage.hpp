// SPDX-License-Identifier: CC-BY-NC-4.0
// Persistent storage for app settings using ESP-IDF NVS

#pragma once

#include <cstdint>

namespace storage {

class NVSStorage {
public:
    static NVSStorage& instance();

    // Volume storage (0-10 scale)
    bool save_volume(uint8_t volume);
    uint8_t load_volume(uint8_t default_value = 5);

private:
    NVSStorage() = default;
    ~NVSStorage() = default;
    NVSStorage(const NVSStorage&) = delete;
    NVSStorage& operator=(const NVSStorage&) = delete;

    static constexpr const char* NAMESPACE = "poker_chip";
    static constexpr const char* KEY_VOLUME = "volume";
};

} // namespace storage
