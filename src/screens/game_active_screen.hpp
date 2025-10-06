#pragma once

#include "screen.hpp"
#include <cstdint>

/// Active game screen showing timer countdown and current blinds.
/// Displays round number, small/big blind values, and countdown timer.
/// Auto-advances rounds when timer expires, doubling blinds each time.
class GameActiveScreen : public Screen {
public:
    /// Get the singleton instance.
    static GameActiveScreen& instance();

    // Screen interface implementation
    void create_widgets() override;
    void destroy_widgets() override;
    void on_enter() override;
    void on_exit() override;
    void handle_encoder(int diff) override;
    void handle_button_click() override;
    void tick() override;
    bool is_modal_blocking() const override;

private:
    GameActiveScreen() = default;

    // Widget handles - game screen
    lv_obj_t* title_bg_ = nullptr;
    lv_obj_t* title_ = nullptr;
    lv_obj_t* small_blind_active_ = nullptr;
    lv_obj_t* active_small_blind_label_ = nullptr;
    lv_obj_t* big_blind_active_ = nullptr;
    lv_obj_t* active_big_blind_label_ = nullptr;
    lv_obj_t* elapsed_mins_ = nullptr;
    lv_obj_t* elapsed_secs_ = nullptr;
    lv_obj_t* timer_colon_ = nullptr;
    lv_obj_t* bottom_button_ = nullptr;
    lv_obj_t* menu_label_ = nullptr;

    // Widget handles - menu overlay
    lv_obj_t* menu_overlay_ = nullptr;
    lv_obj_t* menu_paused_note_ = nullptr;
    lv_obj_t* menu_item_resume_ = nullptr;
    lv_obj_t* menu_item_skip_ = nullptr;
    lv_obj_t* menu_item_volume_ = nullptr;
    lv_obj_t* menu_item_reset_ = nullptr;
    lv_obj_t* menu_item_poweroff_ = nullptr;

    uint32_t last_tick_ms_ = 0;
    bool paused_ = false;
    int menu_selection_ = 0;  // 0=Resume, 1=Skip, 2=NewGame, 3=Volume, 4=PowerOff

    static constexpr uint32_t kTickIntervalMs = 1000;  // 1 second
    static constexpr int kMaxBlind = 9999;             // Failsafe
    static constexpr int kMenuItemCount = 5;

    void update_timer_display();
    void update_blind_display();
    void update_round_title();
    void advance_round();
    void play_round_transition_tones();

    void show_menu();
    void hide_menu();
    void update_menu_selection();
    void update_paused_note();
    void execute_menu_action();

    static void bottom_button_bg_clicked_cb(lv_event_t* e);
    static void menu_item_clicked_cb(lv_event_t* e);
};
