#include "game_active_screen.hpp"

#include <M5Unified.hpp>
#include <esp_log.h>
#include "game_state.hpp"
#include "screen_manager.hpp"
#include "small_blind_screen.hpp"

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
    lv_obj_align(ui().page_title, LV_ALIGN_TOP_MID, 0, 10);
    set_visible(ui().page_title, true);

    // Setup small blind display (large font)
    lv_obj_set_style_text_font(ui().small_blind_active, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_align(ui().small_blind_active, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(ui().small_blind_active, LV_ALIGN_CENTER, -60, -30);
    set_visible(ui().small_blind_active, true);

    lv_label_set_text(ui().active_small_blind_label, "SB");
    lv_obj_set_style_text_align(ui().active_small_blind_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(ui().active_small_blind_label, LV_ALIGN_CENTER, -60, -60);
    set_visible(ui().active_small_blind_label, true);

    // Setup big blind display (large font)
    lv_obj_set_style_text_font(ui().big_blind_active, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_align(ui().big_blind_active, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(ui().big_blind_active, LV_ALIGN_CENTER, 60, -30);
    set_visible(ui().big_blind_active, true);

    lv_label_set_text(ui().active_big_blind_label, "BB");
    lv_obj_set_style_text_align(ui().active_big_blind_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(ui().active_big_blind_label, LV_ALIGN_CENTER, 60, -60);
    set_visible(ui().active_big_blind_label, true);

    // Setup timer display (large font, MM:SS format)
    lv_obj_set_style_text_font(ui().elapsed_mins, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_align(ui().elapsed_mins, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN);
    lv_obj_align(ui().elapsed_mins, LV_ALIGN_CENTER, -15, 50);
    set_visible(ui().elapsed_mins, true);

    lv_obj_set_style_text_font(ui().elapsed_secs, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_align(ui().elapsed_secs, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN);
    lv_obj_align(ui().elapsed_secs, LV_ALIGN_CENTER, 15, 50);
    set_visible(ui().elapsed_secs, true);

    // Hide the "mins" and "secs" labels (not needed with large font)
    set_visible(ui().mins_label, false);
    set_visible(ui().secs_label, false);

    // Show menu button
    set_visible(ui().menu_button, true);

    // Hide menu overlay initially
    hide_menu();

    // Update displays with initial values
    update_blind_display();
    update_timer_display();

    // Reset state
    paused_ = false;
    menu_selection_ = 0;

    ESP_LOGI(kLogTag, "Game started: Round %d, SB=%d, BB=%d, Time=%ds",
             GameState::instance().current_round,
             GameState::instance().small_blind,
             GameState::instance().big_blind,
             GameState::instance().seconds_remaining);
}

void GameActiveScreen::on_exit() {
    ESP_LOGI(kLogTag, "Exiting screen");
    set_visible(ui().menu_button, false);
    hide_menu();
}

void GameActiveScreen::handle_encoder(int diff) {
    if (!paused_) {
        return;  // Encoder disabled during active game
    }

    // Navigate menu
    int step = (diff > 0) ? 1 : ((diff < 0) ? -1 : 0);
    if (step != 0) {
        menu_selection_ += step;

        // Wrap around
        if (menu_selection_ < 0) {
            menu_selection_ = kMenuItemCount - 1;
        } else if (menu_selection_ >= kMenuItemCount) {
            menu_selection_ = 0;
        }

        update_menu_selection();
        play_tone(2000.0f, 50);  // Selection beep
    }
}

void GameActiveScreen::handle_button_click() {
    if (paused_) {
        // Execute menu action
        execute_menu_action();
    } else {
        // Show menu and pause
        paused_ = true;
        show_menu();
        play_tone(1500.0f, 100);
    }
}

void GameActiveScreen::tick() {
    if (paused_) {
        return;  // Don't tick timer when paused
    }

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

void GameActiveScreen::show_menu() {
    set_visible(ui().menu_overlay, true);
    menu_selection_ = 0;  // Default to Resume
    update_menu_selection();
}

void GameActiveScreen::hide_menu() {
    set_visible(ui().menu_overlay, false);
}

void GameActiveScreen::update_menu_selection() {
    // Update menu item styles based on selection
    lv_obj_t* items[] = {
        ui().menu_item_resume,
        ui().menu_item_reset,
        ui().menu_item_skip,
        ui().menu_item_settings,
        ui().menu_item_poweroff
    };

    const char* labels[] = {
        "Resume",
        "Reset",
        "Skip Round",
        "Settings",
        "Power Off"
    };

    for (int i = 0; i < kMenuItemCount; i++) {
        if (i == menu_selection_) {
            lv_label_set_text_fmt(items[i], "> %s", labels[i]);
            lv_obj_set_style_text_color(items[i], lv_color_hex(0x00FF00), LV_PART_MAIN);
        } else {
            lv_label_set_text_fmt(items[i], "  %s", labels[i]);

            // Power Off stays red
            if (i == 4) {
                lv_obj_set_style_text_color(items[i], lv_color_hex(0xFF0000), LV_PART_MAIN);
            } else {
                lv_obj_set_style_text_color(items[i], lv_color_hex(0xFFFFFF), LV_PART_MAIN);
            }
        }
    }
}

void GameActiveScreen::execute_menu_action() {
    ESP_LOGI(kLogTag, "Menu action: %d", menu_selection_);
    play_tone(2500.0f, 100);

    switch (menu_selection_) {
        case 0:  // Resume
            paused_ = false;
            hide_menu();
            break;

        case 1:  // Reset
            ESP_LOGI(kLogTag, "Resetting to small blind screen");
            ScreenManager::instance().transition_to(&SmallBlindScreen::instance());
            break;

        case 2:  // Skip Round
            ESP_LOGI(kLogTag, "Skipping to next round");
            paused_ = false;
            hide_menu();
            advance_round();
            break;

        case 3:  // Settings
            ESP_LOGI(kLogTag, "TODO: Settings screen");
            // TODO: Implement settings screen
            break;

        case 4:  // Power Off
            ESP_LOGI(kLogTag, "Powering off");
            M5.Power.powerOff();
            break;
    }
}
