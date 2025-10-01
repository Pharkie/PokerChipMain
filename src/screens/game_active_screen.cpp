#include "game_active_screen.hpp"

#include <M5Unified.hpp>
#include <esp_log.h>
#include "game_state.hpp"

namespace {
constexpr const char* kLogTag = "game_active_screen";

// Musical scale for round transitions (chromatic progression)
constexpr float kTransitionTones[] = {
    2093.0f,  // C7
    2217.0f,  // C#7
    2349.0f,  // D7
    2489.0f   // D#7
};
constexpr size_t kTransitionToneCount = sizeof(kTransitionTones) / sizeof(kTransitionTones[0]);
constexpr uint32_t kTransitionToneDuration = 150;  // milliseconds
}

GameActiveScreen& GameActiveScreen::instance() {
    static GameActiveScreen instance;
    return instance;
}

void GameActiveScreen::on_enter() {
    ESP_LOGI(kLogTag, "Entering screen");

    last_tick_ms_ = M5.millis();

    // Hide configuration widgets
    set_visible(ui().logo, false);
    set_visible(ui().big_number, false);
    set_visible(ui().pushtext_bg, false);
    set_visible(ui().push_text, false);
    set_visible(ui().down_arrow, false);

    // Setup round title
    update_round_title();
    lv_obj_set_style_text_align(ui().page_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(ui().page_title, LV_ALIGN_TOP_MID, 0, 20);
    set_visible(ui().page_title, true);

    // Setup small blind display
    lv_obj_set_pos(ui().active_small_blind_label, 5, 82);
    set_visible(ui().active_small_blind_label, true);

    lv_obj_set_pos(ui().small_blind_active, 88, 58);
    set_visible(ui().small_blind_active, true);

    // Setup big blind display
    lv_obj_set_pos(ui().active_big_blind_label, 5, 143);
    set_visible(ui().active_big_blind_label, true);

    lv_obj_set_pos(ui().big_blind_active, 71, 121);
    set_visible(ui().big_blind_active, true);

    // Setup timer display
    lv_obj_set_pos(ui().elapsed_mins, 94, 191);
    set_visible(ui().elapsed_mins, true);

    lv_label_set_text(ui().mins_label, "mins");
    lv_obj_set_pos(ui().mins_label, 90, 218);
    set_visible(ui().mins_label, true);

    lv_obj_set_pos(ui().elapsed_secs, 137, 202);
    set_visible(ui().elapsed_secs, true);

    lv_label_set_text(ui().secs_label, "secs");
    lv_obj_set_pos(ui().secs_label, 129, 218);
    set_visible(ui().secs_label, true);

    // Update displays with initial values
    update_blind_display();
    update_timer_display();

    ESP_LOGI(kLogTag, "Game started: Round %d, SB=%d, BB=%d, Time=%ds",
             GameState::instance().current_round,
             GameState::instance().small_blind,
             GameState::instance().big_blind,
             GameState::instance().seconds_remaining);
}

void GameActiveScreen::on_exit() {
    ESP_LOGI(kLogTag, "Exiting screen");
}

void GameActiveScreen::handle_encoder(int diff) {
    // Encoder disabled during active game
    (void)diff;
}

void GameActiveScreen::handle_button_click() {
    ESP_LOGI(kLogTag, "Button clicked - powering off device");

    // Play confirmation tones (G7 -> C8)
    play_tone(2960.0f, 120);
    M5.delay(200);
    play_tone(4186.0f, 120);
    M5.delay(150);

    // Power off device
    M5.Power.powerOff();
}

void GameActiveScreen::tick() {
    uint32_t now_ms = M5.millis();

    // Check if 1 second has elapsed
    if (now_ms - last_tick_ms_ < kTickIntervalMs) {
        return;
    }

    last_tick_ms_ = now_ms;

    auto& game = GameState::instance();

    if (game.seconds_remaining > 0) {
        game.seconds_remaining--;
        update_timer_display();

        if (game.seconds_remaining == 0) {
            ESP_LOGI(kLogTag, "Round %d complete", game.current_round);
            advance_round();
        }
    }
}

void GameActiveScreen::update_timer_display() {
    auto& game = GameState::instance();
    int mins = game.seconds_remaining / 60;
    int secs = game.seconds_remaining % 60;

    lv_label_set_text_fmt(ui().elapsed_mins, "%02d", mins);
    lv_label_set_text_fmt(ui().elapsed_secs, "%02d", secs);
}

void GameActiveScreen::update_blind_display() {
    auto& game = GameState::instance();
    lv_label_set_text_fmt(ui().small_blind_active, "%d", game.small_blind);
    lv_label_set_text_fmt(ui().big_blind_active, "%d", game.big_blind);
}

void GameActiveScreen::update_round_title() {
    auto& game = GameState::instance();
    lv_label_set_text_fmt(ui().page_title, "Round %d", game.current_round);
}

void GameActiveScreen::advance_round() {
    auto& game = GameState::instance();

    // Increment round
    game.current_round++;

    // Double blinds with failsafe
    game.small_blind *= 2;
    game.big_blind *= 2;

    if (game.small_blind > kMaxBlind) {
        game.small_blind = kMaxBlind;
    }
    if (game.big_blind > kMaxBlind) {
        game.big_blind = kMaxBlind;
    }

    // Reset timer
    game.seconds_remaining = game.round_minutes * 60;

    ESP_LOGI(kLogTag, "Advanced to Round %d: SB=%d, BB=%d",
             game.current_round, game.small_blind, game.big_blind);

    // Update displays
    update_round_title();
    update_blind_display();
    update_timer_display();

    // Play transition tones
    play_round_transition_tones();
}

void GameActiveScreen::play_round_transition_tones() {
    for (size_t i = 0; i < kTransitionToneCount; i++) {
        play_tone(kTransitionTones[i], kTransitionToneDuration);
        M5.delay(kTransitionToneDuration + 50);  // Small gap between tones
    }
}
