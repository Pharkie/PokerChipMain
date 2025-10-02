#include "game_state.hpp"

GameState& GameState::instance() {
    static GameState instance;
    return instance;
}

void GameState::reset() {
    small_blind = 25;
    big_blind = 50;
    round_minutes = 15;
    blind_multiplier = 1.5f;
    current_round = 1;
    seconds_remaining = 0;
}
