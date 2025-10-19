// SPDX-License-Identifier: CC-BY-NC-4.0
// Game session logging using ESP-IDF NVS

#pragma once

#include <cstdint>

namespace storage {

/// Single game record (20 bytes per entry)
struct GameRecord {
    uint32_t game_number;         // Sequential game ID
    uint32_t game_seconds;        // In-game time
    uint32_t paused_seconds;      // Paused time
    uint16_t max_round;           // Highest round reached
    uint8_t starting_small_blind; // Starting SB value
    uint8_t round_minutes;        // Round duration
    uint8_t blind_mode;           // 0=STANDARD, 1=TURBO, 2=RELAXED
    uint8_t reserved;             // Padding for alignment
};

/// Game log persistence manager using NVS ring buffer
class GameLog {
public:
    static GameLog& instance();

    /// Save current game session to NVS
    /// @return true if saved successfully
    bool save_current_game();

    /// Load all game records from NVS
    /// @param records Output array to fill
    /// @param max_count Maximum number of records to load
    /// @return Number of records actually loaded
    int load_games(GameRecord* records, int max_count);

    /// Get total number of games played (lifetime counter)
    /// @return Total game count
    uint32_t get_total_game_count();

private:
    GameLog() = default;
    ~GameLog() = default;
    GameLog(const GameLog&) = delete;
    GameLog& operator=(const GameLog&) = delete;

    static constexpr const char* NAMESPACE = "poker_chip";
    static constexpr const char* KEY_GAME_COUNT = "game_count";
    static constexpr const char* KEY_GAME_BLOB = "game_blob";
    static constexpr int MAX_GAMES = 50;
};

} // namespace storage
