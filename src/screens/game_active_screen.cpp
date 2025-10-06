#include "game_active_screen.hpp"

#include <M5Unified.hpp>
#include <esp_log.h>
#include "game_state.hpp"
#include "screen_manager.hpp"
#include "small_blind_screen.hpp"
#include "volume_screen.hpp"
#include "ui/ui_styles.hpp"

namespace {
constexpr const char* kLogTag = "game_active_screen";

// Round transitions - ascending tension with tritone resolution!
constexpr float kTransitionTones[] = {
    2093.0f,  // C7   (base)
    2489.0f,  // D#7  (minor 3rd up - tense)
    2960.0f,  // G7   (tritone from C# - maximum tension!)
    3520.0f   // A7   (resolution - higher climax!)
};
constexpr size_t kTransitionToneCount = sizeof(kTransitionTones) / sizeof(kTransitionTones[0]);
constexpr uint32_t kTransitionToneDuration = 130;  // milliseconds
}

GameActiveScreen& GameActiveScreen::instance() {
    static GameActiveScreen instance;
    return instance;
}

void GameActiveScreen::create_widgets() {
    ESP_LOGI(kLogTag, "Creating widgets");
    lv_obj_t* scr = lv_scr_act();

    // Title background
    title_bg_ = lv_obj_create(scr);
    ui::styles::apply_title_bg(title_bg_);
    lv_obj_set_pos(title_bg_, -20, -10);

    // Title (Round 1, Round 2, etc.)
    title_ = lv_label_create(scr);
    ui::styles::apply_title_text(title_);
    lv_obj_align(title_, LV_ALIGN_TOP_MID, 0, 28);

    // Small blind value and label
    small_blind_active_ = lv_label_create(scr);
    lv_obj_set_style_text_font(small_blind_active_, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_color(small_blind_active_, lv_color_hex(0x00FF46), LV_PART_MAIN);
    lv_obj_set_style_text_align(small_blind_active_, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(small_blind_active_, LV_ALIGN_CENTER, -60, -10);

    active_small_blind_label_ = lv_label_create(scr);
    lv_label_set_text(active_small_blind_label_, "Small\nBlind");
    lv_obj_set_style_text_color(active_small_blind_label_, lv_color_hex(0x00FF46), LV_PART_MAIN);
    lv_obj_set_style_text_align(active_small_blind_label_, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(active_small_blind_label_, LV_ALIGN_CENTER, -60, -50);

    // Big blind value and label
    big_blind_active_ = lv_label_create(scr);
    lv_obj_set_style_text_font(big_blind_active_, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_color(big_blind_active_, lv_color_hex(0x00FBFF), LV_PART_MAIN);
    lv_obj_set_style_text_align(big_blind_active_, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(big_blind_active_, LV_ALIGN_CENTER, 60, -10);

    active_big_blind_label_ = lv_label_create(scr);
    lv_label_set_text(active_big_blind_label_, "Big\nBlind");
    lv_obj_set_style_text_color(active_big_blind_label_, lv_color_hex(0x00FBFF), LV_PART_MAIN);
    lv_obj_set_style_text_align(active_big_blind_label_, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(active_big_blind_label_, LV_ALIGN_CENTER, 60, -50);

    // Timer (MM:SS)
    elapsed_mins_ = lv_label_create(scr);
    lv_obj_set_style_text_font(elapsed_mins_, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_color(elapsed_mins_, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_align(elapsed_mins_, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN);
    lv_obj_align(elapsed_mins_, LV_ALIGN_CENTER, -40, 40);

    timer_colon_ = lv_label_create(scr);
    lv_label_set_text(timer_colon_, ":");
    lv_obj_set_style_text_font(timer_colon_, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_color(timer_colon_, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_align(timer_colon_, LV_ALIGN_CENTER, 0, 40);

    elapsed_secs_ = lv_label_create(scr);
    lv_obj_set_style_text_font(elapsed_secs_, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_color(elapsed_secs_, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_align(elapsed_secs_, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN);
    lv_obj_align(elapsed_secs_, LV_ALIGN_CENTER, 40, 40);

    // Bottom Menu button
    bottom_button_ = lv_button_create(scr);
    ui::styles::apply_bottom_button(bottom_button_);
    lv_obj_set_pos(bottom_button_, 0, 200);

    menu_label_ = lv_label_create(bottom_button_);
    ui::styles::apply_bottom_button_label(menu_label_);
    lv_label_set_text(menu_label_, "Menu");
    lv_obj_align(menu_label_, LV_ALIGN_CENTER, 0, -12);

    // Menu overlay
    menu_overlay_ = lv_obj_create(scr);
    ui::styles::apply_overlay_bg(menu_overlay_);
    lv_obj_set_pos(menu_overlay_, 0, 0);

    menu_paused_note_ = lv_label_create(menu_overlay_);
    lv_label_set_text(menu_paused_note_, "Paused");
    lv_obj_set_style_text_color(menu_paused_note_, lv_color_hex(0xCCCCCC), LV_PART_MAIN);
    lv_obj_set_style_text_align(menu_paused_note_, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(menu_paused_note_, LV_ALIGN_TOP_MID, 0, 20);

    // Menu items
    menu_item_resume_ = lv_button_create(menu_overlay_);
    lv_obj_set_size(menu_item_resume_, 280, 28);
    lv_obj_align(menu_item_resume_, LV_ALIGN_TOP_MID, 0, 55);
    lv_obj_set_style_bg_color(menu_item_resume_, lv_color_hex(0x555555), LV_PART_MAIN);
    lv_obj_set_style_radius(menu_item_resume_, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(menu_item_resume_, 0, LV_PART_MAIN);
    lv_obj_t* resume_label = lv_label_create(menu_item_resume_);
    lv_label_set_text(resume_label, "Resume");
    lv_obj_center(resume_label);

    menu_item_skip_ = lv_button_create(menu_overlay_);
    lv_obj_set_size(menu_item_skip_, 280, 28);
    lv_obj_align(menu_item_skip_, LV_ALIGN_TOP_MID, 0, 87);
    lv_obj_set_style_bg_color(menu_item_skip_, lv_color_hex(0x555555), LV_PART_MAIN);
    lv_obj_set_style_radius(menu_item_skip_, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(menu_item_skip_, 0, LV_PART_MAIN);
    lv_obj_t* skip_label = lv_label_create(menu_item_skip_);
    lv_label_set_text(skip_label, "Skip Round");
    lv_obj_center(skip_label);

    menu_item_volume_ = lv_button_create(menu_overlay_);
    lv_obj_set_size(menu_item_volume_, 280, 28);
    lv_obj_align(menu_item_volume_, LV_ALIGN_TOP_MID, 0, 119);
    lv_obj_set_style_bg_color(menu_item_volume_, lv_color_hex(0x555555), LV_PART_MAIN);
    lv_obj_set_style_radius(menu_item_volume_, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(menu_item_volume_, 0, LV_PART_MAIN);
    lv_obj_t* volume_label = lv_label_create(menu_item_volume_);
    lv_label_set_text(volume_label, "Volume");
    lv_obj_center(volume_label);

    menu_item_reset_ = lv_button_create(menu_overlay_);
    lv_obj_set_size(menu_item_reset_, 280, 28);
    lv_obj_align(menu_item_reset_, LV_ALIGN_TOP_MID, 0, 151);
    lv_obj_set_style_bg_color(menu_item_reset_, lv_color_hex(0x555555), LV_PART_MAIN);
    lv_obj_set_style_radius(menu_item_reset_, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(menu_item_reset_, 0, LV_PART_MAIN);
    lv_obj_t* reset_label = lv_label_create(menu_item_reset_);
    lv_label_set_text(reset_label, "New Game");
    lv_obj_center(reset_label);

    menu_item_poweroff_ = lv_button_create(menu_overlay_);
    lv_obj_set_size(menu_item_poweroff_, 280, 90);
    lv_obj_align(menu_item_poweroff_, LV_ALIGN_TOP_MID, 0, 183);
    lv_obj_set_style_bg_color(menu_item_poweroff_, lv_color_hex(0x555555), LV_PART_MAIN);
    lv_obj_set_style_radius(menu_item_poweroff_, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(menu_item_poweroff_, 0, LV_PART_MAIN);
    lv_obj_t* poweroff_label = lv_label_create(menu_item_poweroff_);
    lv_label_set_text(poweroff_label, "Power Off");
    lv_obj_set_style_text_align(poweroff_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(poweroff_label, LV_ALIGN_TOP_MID, 0, 4);

    // Hide menu initially
    lv_obj_add_flag(menu_overlay_, LV_OBJ_FLAG_HIDDEN);
}

void GameActiveScreen::destroy_widgets() {
    ESP_LOGI(kLogTag, "Destroying widgets");

    // Delete all game widgets
    if (title_bg_) { lv_obj_del(title_bg_); title_bg_ = nullptr; }
    if (title_) { lv_obj_del(title_); title_ = nullptr; }
    if (small_blind_active_) { lv_obj_del(small_blind_active_); small_blind_active_ = nullptr; }
    if (active_small_blind_label_) { lv_obj_del(active_small_blind_label_); active_small_blind_label_ = nullptr; }
    if (big_blind_active_) { lv_obj_del(big_blind_active_); big_blind_active_ = nullptr; }
    if (active_big_blind_label_) { lv_obj_del(active_big_blind_label_); active_big_blind_label_ = nullptr; }
    if (elapsed_mins_) { lv_obj_del(elapsed_mins_); elapsed_mins_ = nullptr; }
    if (elapsed_secs_) { lv_obj_del(elapsed_secs_); elapsed_secs_ = nullptr; }
    if (timer_colon_) { lv_obj_del(timer_colon_); timer_colon_ = nullptr; }
    if (bottom_button_) { lv_obj_del(bottom_button_); bottom_button_ = nullptr; }
    if (menu_label_) { menu_label_ = nullptr; }  // Child of bottom_button_

    // Delete menu overlay (deletes all menu children automatically)
    if (menu_overlay_) { lv_obj_del(menu_overlay_); menu_overlay_ = nullptr; }
    menu_paused_note_ = nullptr;
    menu_item_resume_ = nullptr;
    menu_item_skip_ = nullptr;
    menu_item_volume_ = nullptr;
    menu_item_reset_ = nullptr;
    menu_item_poweroff_ = nullptr;
}

void GameActiveScreen::on_enter() {
    ESP_LOGI(kLogTag, "Entering screen");

    last_tick_ms_ = M5.millis();

    // Register touch events for menu items
    lv_obj_add_event_cb(bottom_button_, bottom_button_bg_clicked_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(menu_item_resume_, menu_item_clicked_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(menu_item_reset_, menu_item_clicked_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(menu_item_skip_, menu_item_clicked_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(menu_item_volume_, menu_item_clicked_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(menu_item_poweroff_, menu_item_clicked_cb, LV_EVENT_CLICKED, this);

    // Update displays with initial values
    update_round_title();
    update_blind_display();
    update_timer_display();

    // Reset state
    paused_ = false;
    menu_selection_ = 0;

    ESP_LOGI(kLogTag, "Game started: Round %d, SB=%d, BB=%d, Time=%ds",
             GameState::instance().current_round(),
             GameState::instance().small_blind(),
             GameState::instance().big_blind(),
             GameState::instance().seconds_remaining());
}

void GameActiveScreen::on_exit() {
    ESP_LOGI(kLogTag, "Exiting screen");
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
        play_tone(2960.0f, 40);  // G7 - retro blip
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
        // D6 → A6 arpeggio (retro menu open)
        play_tone(1175.0f, 50);  // D6
        M5.delay(40);
        play_tone(1760.0f, 70);  // A6
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

    if (game.seconds_remaining() > 0) {
        int new_seconds = game.decrement_seconds();
        update_timer_display();

        if (new_seconds == 0) {
            ESP_LOGI(kLogTag, "Round %d complete", game.current_round());
            advance_round();
        }
    }
}

void GameActiveScreen::update_timer_display() {
    auto& game = GameState::instance();
    int mins = game.seconds_remaining() / 60;
    int secs = game.seconds_remaining() % 60;

    lv_label_set_text_fmt(elapsed_mins_, "%02d", mins);
    lv_label_set_text_fmt(elapsed_secs_, "%02d", secs);

    // Flash colon: show on even seconds, hide on odd seconds (standard digital clock)
    bool colon_visible = (secs % 2) == 0;
    set_visible(timer_colon_, colon_visible);
}

void GameActiveScreen::update_blind_display() {
    auto& game = GameState::instance();
    lv_label_set_text_fmt(small_blind_active_, "%d", game.small_blind());
    lv_label_set_text_fmt(big_blind_active_, "%d", game.big_blind());
}

void GameActiveScreen::update_round_title() {
    auto& game = GameState::instance();
    lv_label_set_text_fmt(title_, "Round %d", game.current_round());
}

void GameActiveScreen::advance_round() {
    auto& game = GameState::instance();

    // Increment round
    game.set_current_round(game.current_round() + 1);

    // Store previous blind for minimum increase check
    int prev_small_blind = game.small_blind();

    // Apply multiplier to small blind
    float new_sb = game.small_blind() * game.blind_multiplier();

    // Round to nearest 25
    int new_small_blind = static_cast<int>((new_sb + 12.5f) / 25) * 25;

    // Ensure minimum increase of 25
    if (new_small_blind <= prev_small_blind) {
        new_small_blind = prev_small_blind + 25;
    }

    // Apply failsafe
    if (new_small_blind > kMaxBlind) {
        new_small_blind = kMaxBlind;
    }

    // Update blinds (big_blind automatically set to 2x small_blind)
    game.update_blinds(new_small_blind);

    // Reset timer
    game.set_seconds_remaining(game.round_minutes() * 60);

    ESP_LOGI(kLogTag, "Advanced to Round %d: SB=%d, BB=%d (multiplier=%.2fx)",
             game.current_round(), game.small_blind(), game.big_blind(), game.blind_multiplier());

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
    set_visible(menu_overlay_, true);
    menu_selection_ = 0;  // Default to Resume
    update_menu_selection();
    update_paused_note();
}

void GameActiveScreen::hide_menu() {
    set_visible(menu_overlay_, false);
}

void GameActiveScreen::update_menu_selection() {
    // Update menu item styles based on selection
    // Order: Resume, Skip, Volume, New Game, Power Off
    lv_obj_t* items[] = {
        menu_item_resume_,
        menu_item_skip_,
        menu_item_volume_,
        menu_item_reset_,
        menu_item_poweroff_
    };

    int current_round = GameState::instance().current_round();
    int next_round = current_round + 1;

    char resume_label[32];
    char skip_label[32];
    snprintf(resume_label, sizeof(resume_label), "Resume round %d", current_round);
    snprintf(skip_label, sizeof(skip_label), "Skip to round %d", next_round);

    const char* labels[] = {
        resume_label,
        skip_label,
        "Volume",
        "New Game",
        "Power Off"
    };

    for (int i = 0; i < kMenuItemCount; i++) {
        // Get the label child from the button
        lv_obj_t* label = lv_obj_get_child(items[i], 0);
        if (!label) continue;

        // Update label text
        lv_label_set_text(label, labels[i]);
        lv_obj_center(label);  // Re-center after text change

        // Update colors based on selection
        if (i == menu_selection_) {
            lv_obj_set_style_bg_color(items[i], lv_color_hex(0x0088FF), LV_PART_MAIN);
        } else {
            lv_obj_set_style_bg_color(items[i], lv_color_hex(0x555555), LV_PART_MAIN);
        }
    }
}

void GameActiveScreen::update_paused_note() {
    auto& game = GameState::instance();
    int mins = game.seconds_remaining() / 60;
    int secs = game.seconds_remaining() % 60;
    lv_label_set_text_fmt(menu_paused_note_, "Paused %02d:%02d", mins, secs);
}

void GameActiveScreen::execute_menu_action() {
    ESP_LOGI(kLogTag, "Menu action: %d", menu_selection_);

    switch (menu_selection_) {
        case 0:  // Resume
            ESP_LOGI(kLogTag, "Resuming game");
            // A6 → F6 downward chirp (dismiss menu)
            play_tone(1760.0f, 40);
            M5.delay(30);
            play_tone(1397.0f, 60);
            paused_ = false;
            hide_menu();
            break;

        case 1:  // Skip Round
            ESP_LOGI(kLogTag, "Skipping to next round");
            // No confirmation sound - round transition tones will play
            paused_ = false;
            hide_menu();
            advance_round();
            break;

        case 2:  // Volume
            ESP_LOGI(kLogTag, "Opening volume screen");
            // C7 → E7 → G7 arpeggio (action executed)
            play_tone(2093.0f, 50);  // C7
            M5.delay(40);
            play_tone(2637.0f, 50);  // E7
            M5.delay(40);
            play_tone(2960.0f, 70);  // G7
            paused_ = false;
            hide_menu();
            ScreenManager::instance().transition_to(&VolumeScreen::instance());
            break;

        case 3:  // New Game
            ESP_LOGI(kLogTag, "Resetting to small blind screen");
            // C7 → E7 → G7 arpeggio (action executed)
            play_tone(2093.0f, 50);  // C7
            M5.delay(40);
            play_tone(2637.0f, 50);  // E7
            M5.delay(40);
            play_tone(2960.0f, 70);  // G7
            GameState::instance().reset();
            ScreenManager::instance().transition_to(&SmallBlindScreen::instance());
            break;

        case 4:  // Power Off
            ESP_LOGI(kLogTag, "Powering off");
            M5.Power.powerOff();
            break;
    }
}

void GameActiveScreen::bottom_button_bg_clicked_cb(lv_event_t* e) {
    GameActiveScreen* screen = static_cast<GameActiveScreen*>(lv_event_get_user_data(e));
    if (screen && !screen->paused_) {
        screen->paused_ = true;
        screen->show_menu();
        // D6 → A6 arpeggio (retro menu open)
        screen->play_tone(1175.0f, 50);  // D6
        M5.delay(40);
        screen->play_tone(1760.0f, 70);  // A6
    }
}

void GameActiveScreen::menu_item_clicked_cb(lv_event_t* e) {
    GameActiveScreen* screen = static_cast<GameActiveScreen*>(lv_event_get_user_data(e));
    if (!screen || !screen->paused_) {
        return;  // Only process when menu is visible
    }

    lv_obj_t* clicked_item = static_cast<lv_obj_t*>(lv_event_get_target(e));

    // Determine which menu item was clicked
    // Order: Resume, Skip, Volume, New Game, Power Off
    lv_obj_t* items[] = {
        screen->menu_item_resume_,
        screen->menu_item_skip_,
        screen->menu_item_volume_,
        screen->menu_item_reset_,
        screen->menu_item_poweroff_
    };

    for (int i = 0; i < kMenuItemCount; i++) {
        if (clicked_item == items[i]) {
            screen->menu_selection_ = i;
            screen->execute_menu_action();
            break;
        }
    }
}

bool GameActiveScreen::is_modal_blocking() const {
    return menu_overlay_ && !lv_obj_has_flag(menu_overlay_, LV_OBJ_FLAG_HIDDEN);
}
