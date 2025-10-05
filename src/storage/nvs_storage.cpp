// SPDX-License-Identifier: CC-BY-NC-4.0

#include "nvs_storage.hpp"
#include <nvs_flash.h>
#include <nvs.h>
#include <esp_log.h>

static const char *TAG = "nvs_storage";

namespace storage {

NVSStorage& NVSStorage::instance() {
    static NVSStorage inst;
    return inst;
}

bool NVSStorage::save_volume(uint8_t volume) {
    // Initialize NVS if needed
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "NVS init failed: %s", esp_err_to_name(err));
        return false;
    }

    nvs_handle_t handle;
    err = nvs_open(NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "NVS open failed: %s", esp_err_to_name(err));
        return false;
    }

    err = nvs_set_u8(handle, KEY_VOLUME, volume);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "NVS set failed: %s", esp_err_to_name(err));
        nvs_close(handle);
        return false;
    }

    err = nvs_commit(handle);
    nvs_close(handle);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Saved volume: %d", volume);
        return true;
    } else {
        ESP_LOGE(TAG, "NVS commit failed: %s", esp_err_to_name(err));
        return false;
    }
}

uint8_t NVSStorage::load_volume(uint8_t default_value) {
    // Initialize NVS if needed
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "NVS init failed: %s", esp_err_to_name(err));
        return default_value;
    }

    nvs_handle_t handle;
    err = nvs_open(NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        ESP_LOGI(TAG, "NVS open failed (using default): %s", esp_err_to_name(err));
        return default_value;
    }

    uint8_t volume = default_value;
    err = nvs_get_u8(handle, KEY_VOLUME, &volume);
    nvs_close(handle);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Loaded volume: %d", volume);
        return volume;
    } else {
        ESP_LOGI(TAG, "NVS get failed (using default): %s", esp_err_to_name(err));
        return default_value;
    }
}

} // namespace storage
