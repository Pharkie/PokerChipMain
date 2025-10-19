// SPDX-License-Identifier: CC-BY-NC-4.0

#include "game_log.hpp"
#include "game_state.hpp"
#include <nvs_flash.h>
#include <nvs.h>
#include <esp_log.h>
#include <cstring>

static const char *TAG = "game_log";

namespace storage {

GameLog& GameLog::instance() {
    static GameLog inst;
    return inst;
}

bool GameLog::save_current_game() {
    // Initialize NVS
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

    // Load existing game count
    uint32_t game_count = 0;
    nvs_get_u32(handle, KEY_GAME_COUNT, &game_count);

    // Load existing records
    GameRecord records[MAX_GAMES];
    memset(records, 0, sizeof(records));
    size_t blob_size = sizeof(records);
    nvs_get_blob(handle, KEY_GAME_BLOB, records, &blob_size);
    int existing_count = blob_size / sizeof(GameRecord);

    // Create new record from current game state
    auto& game = GameState::instance();
    GameRecord new_record;
    new_record.game_number = game_count + 1;
    new_record.game_seconds = game.total_game_seconds();
    new_record.paused_seconds = game.total_paused_seconds();
    new_record.max_round = static_cast<uint16_t>(game.max_round_reached());
    new_record.starting_small_blind = static_cast<uint8_t>(game.small_blind());
    new_record.round_minutes = static_cast<uint8_t>(game.round_minutes());

    // Determine blind mode from multiplier
    float multiplier = game.blind_multiplier();
    if (multiplier >= 1.9f && multiplier <= 2.1f) {
        new_record.blind_mode = 1;  // TURBO
    } else if (multiplier >= 1.2f && multiplier <= 1.3f) {
        new_record.blind_mode = 2;  // RELAXED
    } else {
        new_record.blind_mode = 0;  // STANDARD
    }
    new_record.reserved = 0;

    // Ring buffer: shift if at capacity
    if (existing_count >= MAX_GAMES) {
        memmove(&records[0], &records[1], (MAX_GAMES - 1) * sizeof(GameRecord));
        records[MAX_GAMES - 1] = new_record;
    } else {
        records[existing_count] = new_record;
        existing_count++;
    }

    // Save back to NVS
    nvs_set_u32(handle, KEY_GAME_COUNT, game_count + 1);
    nvs_set_blob(handle, KEY_GAME_BLOB, records, existing_count * sizeof(GameRecord));
    err = nvs_commit(handle);
    nvs_close(handle);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Saved game #%lu: %lus game, %lus paused, round %d",
                 (unsigned long)new_record.game_number, (unsigned long)new_record.game_seconds,
                 (unsigned long)new_record.paused_seconds, (int)new_record.max_round);
        return true;
    } else {
        ESP_LOGE(TAG, "NVS commit failed: %s", esp_err_to_name(err));
        return false;
    }
}

int GameLog::load_games(GameRecord* records, int max_count) {
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "NVS init failed: %s", esp_err_to_name(err));
        return 0;
    }

    nvs_handle_t handle;
    err = nvs_open(NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        ESP_LOGI(TAG, "NVS open failed (no games saved yet): %s", esp_err_to_name(err));
        return 0;
    }

    // Load records blob
    size_t blob_size = max_count * sizeof(GameRecord);
    err = nvs_get_blob(handle, KEY_GAME_BLOB, records, &blob_size);
    nvs_close(handle);

    if (err == ESP_OK) {
        int count = blob_size / sizeof(GameRecord);
        ESP_LOGI(TAG, "Loaded %d game records", count);
        return count;
    } else {
        ESP_LOGI(TAG, "NVS get blob failed: %s", esp_err_to_name(err));
        return 0;
    }
}

uint32_t GameLog::get_total_game_count() {
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "NVS init failed: %s", esp_err_to_name(err));
        return 0;
    }

    nvs_handle_t handle;
    err = nvs_open(NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        return 0;
    }

    uint32_t game_count = 0;
    nvs_get_u32(handle, KEY_GAME_COUNT, &game_count);
    nvs_close(handle);

    return game_count;
}

} // namespace storage
