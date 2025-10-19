#include "blind_progression_screen.hpp"

#include <cmath>
#include <M5Unified.hpp>
#include <esp_log.h>
#include "game_state.hpp"
#include "screen_manager.hpp"
#include "game_active_screen.hpp"
#include "ui/ui_helpers.hpp"
#include "ui/ui_styles.hpp"

namespace {
constexpr const char* kLogTag = "blind_progression_screen";
}

BlindProgressionScreen& BlindProgressionScreen::instance() {
    static BlindProgressionScreen instance;
    return instance;
}

void BlindProgressionScreen::create_widgets() {
    ESP_LOGI(kLogTag, "Creating widgets");

    lv_obj_t* scr = lv_scr_act();

    // Title background banner
    title_bg_ = lv_obj_create(scr);
    ui::styles::apply_title_bg(title_bg_);
    lv_obj_set_pos(title_bg_, -20, -10);

    // Title
    title_ = lv_label_create(scr);
    ui::styles::apply_title_text(title_);
    lv_obj_align(title_, LV_ALIGN_TOP_MID, 0, 28);
    lv_label_set_text(title_, "Mode");

    // Mode name (replaces big_number for this screen)
    mode_name_ = lv_label_create(scr);
    lv_obj_set_style_text_font(mode_name_, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_obj_set_style_text_color(mode_name_, lv_color_hex(0xFF00DC), LV_PART_MAIN);
    lv_obj_set_style_text_align(mode_name_, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(mode_name_, LV_ALIGN_CENTER, 0, -25);

    // Mode description
    mode_description_ = lv_label_create(scr);
    lv_obj_set_style_text_color(mode_description_, lv_color_hex(0xAAAAAA), LV_PART_MAIN);
    lv_obj_set_style_text_align(mode_description_, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(mode_description_, LV_ALIGN_CENTER, 0, -5);

    // Mode game time estimate
    mode_game_time_ = lv_label_create(scr);
    lv_obj_set_style_text_color(mode_game_time_, lv_color_hex(0x777777), LV_PART_MAIN);
    lv_obj_set_style_text_align(mode_game_time_, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(mode_game_time_, LV_ALIGN_CENTER, 0, 25);

    // Bottom button
    bottom_button_ = ui::helpers::create_button(scr);
    lv_obj_clear_flag(bottom_button_, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(bottom_button_, LV_OBJ_FLAG_CLICKABLE);
    ui::styles::apply_bottom_button(bottom_button_);
    lv_obj_set_pos(bottom_button_, 0, 200);

    confirm_label_ = lv_label_create(bottom_button_);
    ui::styles::apply_bottom_button_label(confirm_label_);
    lv_label_set_text(confirm_label_, "Confirm");
    lv_obj_align(confirm_label_, LV_ALIGN_CENTER, 0, -12);

    // Info overlay (no info button on mode screen)
    info_overlay_ = lv_obj_create(scr);
    ui::styles::apply_overlay_bg(info_overlay_);
    lv_obj_set_pos(info_overlay_, 0, 0);
    lv_obj_add_flag(info_overlay_, LV_OBJ_FLAG_CLICKABLE);

    info_title_bg_ = lv_obj_create(info_overlay_);
    ui::styles::apply_title_bg(info_title_bg_);
    lv_obj_set_pos(info_title_bg_, -20, -10);

    info_title_ = lv_label_create(info_overlay_);
    ui::styles::apply_title_text(info_title_);
    lv_obj_align(info_title_, LV_ALIGN_TOP_MID, 0, 28);
    lv_label_set_text(info_title_, "Chip Breakdown");

    info_blue_ = lv_label_create(info_overlay_);
    lv_label_set_text(info_blue_, "16 x Blue (25) = 400");
    lv_obj_set_style_text_color(info_blue_, lv_color_hex(0x4488FF), LV_PART_MAIN);
    lv_obj_set_style_text_align(info_blue_, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(info_blue_, LV_ALIGN_CENTER, 0, -35);

    info_white_ = lv_label_create(info_overlay_);
    lv_label_set_text(info_white_, "20 x White (50) = 1000");
    lv_obj_set_style_text_color(info_white_, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_align(info_white_, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(info_white_, LV_ALIGN_CENTER, 0, -10);

    info_red_ = lv_label_create(info_overlay_);
    lv_label_set_text(info_red_, "6 x Red (100) = 600");
    lv_obj_set_style_text_color(info_red_, lv_color_hex(0xFF4444), LV_PART_MAIN);
    lv_obj_set_style_text_align(info_red_, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(info_red_, LV_ALIGN_CENTER, 0, 15);

    info_stack_ = lv_label_create(info_overlay_);
    lv_label_set_text(info_stack_, "Total stack: 2000");
    lv_obj_set_style_text_color(info_stack_, lv_color_hex(0xCCCCCC), LV_PART_MAIN);
    lv_obj_set_style_text_align(info_stack_, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(info_stack_, LV_ALIGN_CENTER, 0, 45);

    info_close_button_ = ui::helpers::create_button(info_overlay_);
    ui::styles::apply_bottom_button(info_close_button_);
    lv_obj_set_pos(info_close_button_, 0, 200);

    info_close_label_ = lv_label_create(info_close_button_);
    ui::styles::apply_bottom_button_label(info_close_label_);
    lv_label_set_text(info_close_label_, "Close");
    lv_obj_align(info_close_label_, LV_ALIGN_CENTER, 0, -12);

    lv_obj_add_flag(info_overlay_, LV_OBJ_FLAG_HIDDEN);
}

void BlindProgressionScreen::destroy_widgets() {
    ESP_LOGI(kLogTag, "Destroying widgets");

    if (info_overlay_) lv_obj_del(info_overlay_);
    if (bottom_button_) lv_obj_del(bottom_button_);
    if (mode_game_time_) lv_obj_del(mode_game_time_);
    if (mode_description_) lv_obj_del(mode_description_);
    if (mode_name_) lv_obj_del(mode_name_);
    if (title_) lv_obj_del(title_);
    if (title_bg_) lv_obj_del(title_bg_);

    title_bg_ = nullptr;
    title_ = nullptr;
    mode_name_ = nullptr;
    mode_description_ = nullptr;
    mode_game_time_ = nullptr;
    bottom_button_ = nullptr;
    confirm_label_ = nullptr;
    info_overlay_ = nullptr;
    info_title_bg_ = nullptr;
    info_title_ = nullptr;
    info_blue_ = nullptr;
    info_white_ = nullptr;
    info_red_ = nullptr;
    info_stack_ = nullptr;
    info_close_button_ = nullptr;
    info_close_label_ = nullptr;
}

void BlindProgressionScreen::on_enter() {
    ESP_LOGI(kLogTag, "Entering screen");

    // Default to STANDARD (first option)
    selection_ = 0;
    update_display();

    // Setup event callbacks
    lv_obj_add_event_cb(info_overlay_, info_overlay_clicked_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(info_close_button_, info_overlay_clicked_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(bottom_button_, push_button_clicked_cb, LV_EVENT_CLICKED, this);

    ESP_LOGI(kLogTag, "Initial selection: %s", kNames[selection_]);
}

void BlindProgressionScreen::on_exit() {
    // Event callbacks will be destroyed with widgets
}

void BlindProgressionScreen::handle_encoder(int diff) {
    if (diff == 0 || is_modal_blocking()) {
        return;  // Ignore encoder when modal overlay is shown
    }

    int prev_selection = selection_;
    selection_ += diff;

    // Wrap around (all choices are valid, no boundaries)
    if (selection_ < 0) {
        selection_ = kOptionCount - 1;
    } else if (selection_ >= kOptionCount) {
        selection_ = 0;
    }

    if (selection_ != prev_selection) {
        // Play consistent tone for any valid selection change
        play_tone(diff > 0 ? kToneUp : kToneDown, kToneDuration);
        update_display();
        ESP_LOGI(kLogTag, "Selection changed to: %s", kNames[selection_]);
    }
}

void BlindProgressionScreen::handle_button_click() {
    // If info overlay is visible, close it instead of confirming
    if (is_modal_blocking()) {
        hide_info();
        return;
    }

    ESP_LOGI(kLogTag, "Button clicked, selected: %s (multiplier: %.2f)",
             kNames[selection_], kMultipliers[selection_]);

    // Store selected multiplier in game state
    GameState& game = GameState::instance();
    game.set_blind_multiplier(kMultipliers[selection_]);

    // Initialize timer for first round
    game.set_seconds_remaining(game.round_minutes() * 60);

    // Play confirmation tone (A7 → A7 double beep - climax, game starting!)
    play_tone(3520.0f, 90);
    M5.delay(60);
    play_tone(3520.0f, 90);

    // Transition to game active screen
    ScreenManager::instance().transition_to(&GameActiveScreen::instance());
}

int BlindProgressionScreen::calculate_estimated_rounds(float multiplier) const {
    const GameState& game = GameState::instance();
    int small_blind = game.small_blind();

    // Structural fix: Calculate rounds based on multiplier growth until endgame
    // Game ends when blinds force all-in play (push/fold territory)
    // Key insight: endgame is when average player has ~2-4 big blinds left
    // At this point, players are in push/fold mode and games conclude quickly
    //
    // For 2000 stack: endgame SB ≈ 333 (BB=666, players have ~3 BB average)
    // This gives realistic game durations across all starting blind levels
    //
    // Formula: starting_sb × (multiplier ^ rounds) = endgame_sb
    // Therefore: rounds = log(endgame_sb / starting_sb) / log(multiplier)

    int endgame_small_blind = kStartingStack / 6;  // ~333 for 2000 stack

    // Calculate number of rounds using logarithmic formula
    // This gives consistent estimates regardless of starting blind level
    float growth_factor = static_cast<float>(endgame_small_blind) / static_cast<float>(small_blind);
    float exact_rounds = std::log(growth_factor) / std::log(multiplier);
    int rounds = static_cast<int>(exact_rounds + 0.5f);  // Round to nearest

    // Sanity bounds: at least 3 rounds, at most 15 rounds
    if (rounds < 3) rounds = 3;
    if (rounds > 15) rounds = 15;

    return rounds;
}

void BlindProgressionScreen::update_display() {
    // Show option name (TURBO/STANDARD/RELAXED)
    lv_label_set_text(mode_name_, kNames[selection_]);

    // Show description
    lv_label_set_text(mode_description_, kDescriptions[selection_]);

    // Calculate dynamic game time estimate
    const GameState& game = GameState::instance();
    int estimated_rounds = calculate_estimated_rounds(kMultipliers[selection_]);
    int total_minutes = estimated_rounds * game.round_minutes();

    snprintf(game_time_buffer_, sizeof(game_time_buffer_),
             "Game time: ~%d mins", total_minutes);

    lv_label_set_text(mode_game_time_, game_time_buffer_);
}

void BlindProgressionScreen::push_button_clicked_cb(lv_event_t* e) {
    BlindProgressionScreen* screen = static_cast<BlindProgressionScreen*>(lv_event_get_user_data(e));
    screen->handle_button_click();
}

void BlindProgressionScreen::info_overlay_clicked_cb(lv_event_t* e) {
    BlindProgressionScreen* screen = static_cast<BlindProgressionScreen*>(lv_event_get_user_data(e));
    screen->hide_info();
}

void BlindProgressionScreen::show_info() {
    ESP_LOGI(kLogTag, "Showing info overlay");
    set_visible(info_overlay_, true);
    // F#7 → A7 chirp (playful upward)
    play_tone(2794.0f, 40);  // F#7
    M5.delay(40);
    play_tone(3520.0f, 60);  // A7
}

void BlindProgressionScreen::hide_info() {
    ESP_LOGI(kLogTag, "Hiding info overlay");
    set_visible(info_overlay_, false);
    // A6 → F6 chirp (playful downward)
    play_tone(1760.0f, 40);  // A6
    M5.delay(40);
    play_tone(1397.0f, 60);  // F6
}

bool BlindProgressionScreen::is_modal_blocking() const {
    return info_overlay_ && !lv_obj_has_flag(info_overlay_, LV_OBJ_FLAG_HIDDEN);
}
