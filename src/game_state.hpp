#pragma once

#include <cstdint>

/// Global game state shared across screens.
/// Implements singleton pattern for easy access.
/// Provides encapsulated access to prevent invalid state modifications.
class GameState {
public:
    /// Get the singleton instance.
    static GameState& instance();

    /// Reset to initial state (useful for restarting game).
    void reset();

    // Configuration getters
    int small_blind() const { return small_blind_; }
    int big_blind() const { return big_blind_; }
    int round_minutes() const { return round_minutes_; }
    float blind_multiplier() const { return blind_multiplier_; }

    // Active game state getters
    int current_round() const { return current_round_; }
    int seconds_remaining() const { return seconds_remaining_; }

    // Game timer getters
    uint32_t total_game_seconds() const { return total_game_seconds_; }
    uint32_t total_paused_seconds() const { return total_paused_seconds_; }
    uint32_t total_overall_seconds() const { return total_game_seconds_ + total_paused_seconds_; }
    int max_round_reached() const { return max_round_reached_; }

    // Configuration setters with validation
    /// Set small blind value (automatically updates big blind to 2x)
    /// @param value Small blind amount (must be positive)
    void set_small_blind(int value);

    /// Set round duration in minutes
    /// @param minutes Duration (must be positive)
    void set_round_minutes(int minutes);

    /// Set blind progression multiplier
    /// @param multiplier Blind increase rate (1.25=RELAXED, 1.5=STANDARD, 2.0=TURBO)
    void set_blind_multiplier(float multiplier);

    // Active game state setters
    /// Set current round number
    /// @param round Round number (must be positive)
    void set_current_round(int round);

    /// Set remaining seconds in current round
    /// @param seconds Remaining time (must be non-negative)
    void set_seconds_remaining(int seconds);

    /// Decrement seconds remaining (used by game timer)
    /// @return New seconds_remaining value
    int decrement_seconds();

    /// Update blind values for next round (maintains small_blind/big_blind invariant)
    /// @param new_small_blind New small blind value
    void update_blinds(int new_small_blind);

    // Game timer control
    /// Start/reset game timer (called when round 1 begins)
    void start_game_timer();

    /// Increment game time by 1 second (called every tick during active play)
    void tick_game_timer();

    /// Record pause start time (called when pausing)
    void pause_game_timer();

    /// Accumulate paused time and resume (called when unpausing)
    void resume_game_timer();

    /// Update max round if current round is higher
    /// @param round Current round number
    void record_max_round(int round);

private:
    GameState() = default;
    GameState(const GameState&) = delete;
    GameState& operator=(const GameState&) = delete;

    // Configuration (set by setup screens)
    int small_blind_ = 25;
    int big_blind_ = 50;
    int round_minutes_ = 15;
    float blind_multiplier_ = 1.5f;  // STANDARD progression (1.25=RELAXED, 1.5=STANDARD, 2.0=TURBO)

    // Active game state
    int current_round_ = 1;
    int seconds_remaining_ = 0;

    // Game session timing
    uint32_t total_game_seconds_ = 0;    // In-game time (playing)
    uint32_t total_paused_seconds_ = 0;  // Time spent paused
    uint32_t pause_start_ms_ = 0;        // Timestamp when pause started (0 if not paused)
    int max_round_reached_ = 1;          // Highest round number achieved
};
