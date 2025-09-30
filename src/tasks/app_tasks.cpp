#include "app_tasks.hpp"

#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <algorithm>

#include <M5Unified.hpp>
#include <lvgl.h>
#include <esp_log.h>

#include "ui/ui_root.hpp"
#include "ui/ui_assets.hpp"
#include "input/encoder_input.hpp"

namespace app_tasks
{
namespace
{
struct ToneStep
{
    int frequency_hz;
    uint16_t duration_ms;
    uint16_t delay_after_ms;
};

struct AppState
{
    enum class Screen
    {
        kNone,
        kSmallBlindSetup,
        kRoundMinutes,
        kRoundUnderway
    };

    Screen current_screen = Screen::kNone;
    int small_blind_value = 25;
    int minutes_between_rounds = 10;
    int current_round = 0;
    int round_secs_remaining = 0;
    int sound_round = 1;

    lv_timer_t *countdown_timer = nullptr;
    lv_timer_t *tone_timer = nullptr;

    const ToneStep *tone_sequence = nullptr;
    size_t tone_sequence_length = 0;
    size_t tone_sequence_index = 0;
    void (*sequence_complete_cb)() = nullptr;

    std::array<ToneStep, 4> round_sequence{};
};

AppState g_state;
constexpr const char *kLogTag = "app_tasks";

struct NoteEntry
{
    const char *name;
    int frequency;
};

// Source: UIFlow2Micropython.py:L25-L30
constexpr std::array<NoteEntry, 14> kNoteTable = {{{"G#6", 1661},
                                                   {"C7", 2093},
                                                   {"C#7", 2217},
                                                   {"D7", 2349},
                                                   {"D#7", 2489},
                                                   {"E7", 2637},
                                                   {"F7", 2793},
                                                   {"F#7", 2959},
                                                   {"G7", 3135},
                                                   {"G#7", 3322},
                                                   {"A7", 3520},
                                                   {"A#7", 3729},
                                                   {"B7", 3951},
                                                   {"C8", 4186}}};

constexpr std::array<int, 13> kSoundNotesList = {2093, 2217, 2349, 2489, 2637, 2793, 2959, 3135, 3322, 3520, 3729, 3951, 4186};

constexpr ToneStep kBootSequence[] = {
    {3135, 200, 150},  // Speaker.tone(sound_notes['G7'], 200); sleep_ms(150)
    {4186, 200, 3000}  // Speaker.tone(sound_notes['C8'], 200); sleep(3s)
};

constexpr ToneStep kAffirmSequence[] = {
    {3135, 150, 150},  // Speaker.tone(sound_notes['G7'], 150); sleep_ms(150)
    {4186, 150, 1000}  // Speaker.tone(sound_notes['C8'], 150); sleep_ms(1000)
};

int frequency_for_name(const char *name)
{
    for (const auto &entry : kNoteTable)
    {
        if (std::strcmp(entry.name, name) == 0)
        {
            return entry.frequency;
        }
    }
    return 0;
}

int frequency_from_round_list(size_t index)
{
    if (index >= kSoundNotesList.size())
    {
        return kSoundNotesList.back();
    }
    return kSoundNotesList[index];
}

void set_visible(lv_obj_t *obj, bool visible)
{
    if (!obj)
    {
        return;
    }
    if (visible)
    {
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
}

void label_set_text_int(lv_obj_t *label, int value)
{
    if (!label)
    {
        return;
    }
    lv_label_set_text_fmt(label, "%d", value);
}


void stop_tone_timer()
{
    if (g_state.tone_timer != nullptr)
    {
        lv_timer_pause(g_state.tone_timer);
    }
    g_state.tone_sequence = nullptr;
    g_state.tone_sequence_length = 0;
    g_state.tone_sequence_index = 0;
    g_state.sequence_complete_cb = nullptr;
}

void play_tone_step(size_t index)
{
    if (g_state.tone_sequence == nullptr || index >= g_state.tone_sequence_length)
    {
        stop_tone_timer();
        return;
    }

    const ToneStep &step = g_state.tone_sequence[index];
    if (step.frequency_hz > 0)
    {
        M5.Speaker.tone(static_cast<float>(step.frequency_hz), step.duration_ms);
    }

    g_state.tone_sequence_index = index + 1;

    if (g_state.tone_sequence_index >= g_state.tone_sequence_length)
    {
        stop_tone_timer();
        if (g_state.sequence_complete_cb)
        {
            g_state.sequence_complete_cb();
        }
        return;
    }

    const ToneStep &next_delay_source = step;
    if (g_state.tone_timer != nullptr)
    {
        lv_timer_set_period(g_state.tone_timer, next_delay_source.duration_ms + next_delay_source.delay_after_ms);
        lv_timer_resume(g_state.tone_timer);
    }
}

void tone_timer_cb(lv_timer_t *timer)
{
    LV_UNUSED(timer);
    play_tone_step(g_state.tone_sequence_index);
}

void start_tone_sequence(const ToneStep *sequence, size_t length, void (*complete_cb)())
{
    if (g_state.tone_timer == nullptr)
    {
        g_state.tone_timer = lv_timer_create(tone_timer_cb, 1, nullptr);
        lv_timer_pause(g_state.tone_timer);
    }

    g_state.tone_sequence = sequence;
    g_state.tone_sequence_length = length;
    g_state.tone_sequence_index = 0;
    g_state.sequence_complete_cb = complete_cb;

    if (length == 0)
    {
        stop_tone_timer();
        if (complete_cb)
        {
            complete_cb();
        }
        return;
    }

    play_tone_step(0);
}

void play_single_tone(int frequency, uint16_t duration_ms)
{
    static ToneStep single_step; // reused, safe because start_tone_sequence copies pointer immediately
    single_step.frequency_hz = frequency;
    single_step.duration_ms = duration_ms;
    single_step.delay_after_ms = 0;
    start_tone_sequence(&single_step, 1, nullptr);
}

void update_elapsed_labels()
{
    const auto &handles = ui::get();
    int remaining = g_state.round_secs_remaining;
    if (remaining < 0)
    {
        remaining = 0;
    }
    int minutes = remaining / 60;
    int seconds = remaining % 60;
    label_set_text_int(handles.elapsed_mins, minutes);
    if (handles.elapsed_secs)
    {
        lv_label_set_text_fmt(handles.elapsed_secs, "%02d", seconds);
    }
}

// Source: UIFlow2Micropython.py:L35-L46
void show_small_blind_screen()
{
    const auto &handles = ui::get();
    g_state.current_screen = AppState::Screen::kSmallBlindSetup;
    g_state.small_blind_value = 25;
    ESP_LOGI(kLogTag, "Show small blind setup screen");

    if (g_state.countdown_timer)
    {
        lv_timer_pause(g_state.countdown_timer);
    }

    set_visible(handles.logo, false);
    lv_label_set_text(handles.page_title, "Starting small blinds");
    set_visible(handles.page_title, true);

    label_set_text_int(handles.big_number, g_state.small_blind_value);
    set_visible(handles.big_number, true);

    set_visible(handles.pushtext_bg, true);
    set_visible(handles.push_text, true);
    set_visible(handles.down_arrow, true);

    set_visible(handles.small_blind_active, false);
    set_visible(handles.big_blind_active, false);
    set_visible(handles.active_small_blind_label, false);
    set_visible(handles.active_big_blind_label, false);
    set_visible(handles.elapsed_mins, false);
    set_visible(handles.elapsed_secs, false);
    set_visible(handles.mins_label, false);
    set_visible(handles.secs_label, false);
}

// Source: UIFlow2Micropython.py:L48-L58
void show_round_minutes_screen()
{
    const auto &handles = ui::get();
    g_state.current_screen = AppState::Screen::kRoundMinutes;
    g_state.minutes_between_rounds = 10;
    ESP_LOGI(kLogTag, "Show minutes-between-rounds screen");

    if (g_state.countdown_timer)
    {
        lv_timer_pause(g_state.countdown_timer);
    }

    lv_label_set_text(handles.page_title, "Mins between rounds");
    set_visible(handles.page_title, true);

    label_set_text_int(handles.big_number, g_state.minutes_between_rounds);
    set_visible(handles.big_number, true);

    set_visible(handles.pushtext_bg, true);
    set_visible(handles.push_text, true);
    set_visible(handles.down_arrow, true);

    set_visible(handles.small_blind_active, false);
    set_visible(handles.big_blind_active, false);
    set_visible(handles.active_small_blind_label, false);
    set_visible(handles.active_big_blind_label, false);
    set_visible(handles.elapsed_mins, false);
    set_visible(handles.elapsed_secs, false);
    set_visible(handles.mins_label, false);
    set_visible(handles.secs_label, false);
}

// Source: UIFlow2Micropython.py:L60-L90
void show_round_underway_screen()
{
    const auto &handles = ui::get();
    g_state.current_screen = AppState::Screen::kRoundUnderway;
    ESP_LOGI(kLogTag, "Round %d underway (small blind %d)", g_state.current_round, g_state.small_blind_value);

    lv_label_set_text_fmt(handles.page_title, "Round %d", g_state.current_round);
    lv_obj_set_pos(handles.page_title, 77, 25);
    set_visible(handles.page_title, true);

    label_set_text_int(handles.small_blind_active, g_state.small_blind_value);
    label_set_text_int(handles.big_blind_active, g_state.small_blind_value * 2);

    set_visible(handles.small_blind_active, true);
    set_visible(handles.big_blind_active, true);
    set_visible(handles.active_small_blind_label, true);
    set_visible(handles.active_big_blind_label, true);

    set_visible(handles.mins_label, true);
    set_visible(handles.secs_label, true);
    set_visible(handles.elapsed_mins, true);
    set_visible(handles.elapsed_secs, true);

    set_visible(handles.big_number, false);
    set_visible(handles.pushtext_bg, false);
    set_visible(handles.push_text, false);
    set_visible(handles.down_arrow, false);

    update_elapsed_labels();

    if (g_state.countdown_timer)
    {
        lv_timer_set_period(g_state.countdown_timer, 1000);
        lv_timer_reset(g_state.countdown_timer);
        lv_timer_resume(g_state.countdown_timer);
    }
}

// Source: UIFlow2Micropython.py:L93-L113
void check_round_end()
{
    if (g_state.round_secs_remaining > 0)
    {
        return;
    }

    g_state.current_round += 1;
    ESP_LOGI(kLogTag, "Round complete -> advancing to round %d", g_state.current_round);
    g_state.sound_round += 1;
    if (g_state.sound_round > 10)
    {
        g_state.sound_round = 10;
    }

    lv_label_set_text_fmt(ui::get().page_title, "Round %d", g_state.current_round);

    g_state.small_blind_value *= 2;
    if (g_state.small_blind_value > 9999)
    {
        g_state.small_blind_value = 9999;
    }

    label_set_text_int(ui::get().small_blind_active, g_state.small_blind_value);
    label_set_text_int(ui::get().big_blind_active, g_state.small_blind_value * 2);

    const size_t max_base = (kSoundNotesList.size() > g_state.round_sequence.size()) ? kSoundNotesList.size() - g_state.round_sequence.size() : 0UL;
    size_t base_index = static_cast<size_t>(g_state.sound_round - 1);
    base_index = std::min(base_index, max_base);
    for (size_t i = 0; i < g_state.round_sequence.size(); ++i)
    {
        uint16_t delay_after = (i < g_state.round_sequence.size() - 1) ? static_cast<uint16_t>(600) : static_cast<uint16_t>(0);
        g_state.round_sequence[i] = {frequency_from_round_list(base_index + i), static_cast<uint16_t>(500), delay_after};
    }

    start_tone_sequence(g_state.round_sequence.data(), g_state.round_sequence.size(), nullptr);

    g_state.round_secs_remaining = 60 * g_state.minutes_between_rounds;
    update_elapsed_labels();
}

// Source: UIFlow2Micropython.py:L114-L120
void play_affirm_sound()
{
    start_tone_sequence(kAffirmSequence, std::size(kAffirmSequence), nullptr);
}

// Source: UIFlow2Micropython.py:L183-L233
void handle_rotation(int delta)
{
    if (delta == 0)
    {
        return;
    }

    switch (g_state.current_screen)
    {
    case AppState::Screen::kSmallBlindSetup:
    {
        int tentative = g_state.small_blind_value + (25 * delta);
        bool boundary = false;
        if (tentative == 225)
        {
            tentative = 200;
            boundary = true;
        }
        if (tentative <= 0)
        {
            tentative = 25;
            boundary = true;
        }
        g_state.small_blind_value = tentative;
        label_set_text_int(ui::get().big_number, g_state.small_blind_value);
        if (boundary)
        {
            play_single_tone(frequency_for_name("G#6"), 400);
        }
        else
        {
            const char *note = (delta > 0) ? "A7" : "F7";
            play_single_tone(frequency_for_name(note), 150);
        }
        break;
    }
    case AppState::Screen::kRoundMinutes:
    {
        int tentative = g_state.minutes_between_rounds + (5 * delta);
        bool boundary = false;
        if (tentative == 50)
        {
            tentative = 45;
            boundary = true;
        }
        if (tentative <= 0)
        {
            tentative = 5;
            boundary = true;
        }
        g_state.minutes_between_rounds = tentative;
        label_set_text_int(ui::get().big_number, g_state.minutes_between_rounds);
        if (boundary)
        {
            play_single_tone(frequency_for_name("G#6"), 400);
        }
        else
        {
            const char *note = (delta > 0) ? "A7" : "F7";
            play_single_tone(frequency_for_name(note), 150);
        }
        break;
    }
    case AppState::Screen::kRoundUnderway:
    default:
        break;
    }
}

// Source: UIFlow2Micropython.py:L122-L143
void handle_button_click()
{
    ESP_LOGI(kLogTag, "Button click on screen %d", static_cast<int>(g_state.current_screen));
    switch (g_state.current_screen)
    {
    case AppState::Screen::kSmallBlindSetup:
        show_round_minutes_screen();
        play_affirm_sound();
        break;
    case AppState::Screen::kRoundMinutes:
        g_state.round_secs_remaining = 60 * g_state.minutes_between_rounds;
        g_state.current_round = 1;
        g_state.sound_round = 1;
        show_round_underway_screen();
        play_affirm_sound();
        break;
    case AppState::Screen::kRoundUnderway:
        break;
    default:
        break;
    }
}

// Source: UIFlow2Micropython.py:L226-L232
void countdown_timer_cb(lv_timer_t *timer)
{
    LV_UNUSED(timer);
    if (g_state.current_screen != AppState::Screen::kRoundUnderway)
    {
        return;
    }

    g_state.round_secs_remaining -= 1;
    update_elapsed_labels();
    if (g_state.round_secs_remaining <= 0)
    {
        check_round_end();
    }
}

void encoder_key_event_cb(lv_event_t *event)
{
    const uint32_t key = lv_event_get_key(event);
    if (key == LV_KEY_RIGHT || key == LV_KEY_NEXT)
    {
        handle_rotation(1);
    }
    else if (key == LV_KEY_LEFT || key == LV_KEY_PREV)
    {
        handle_rotation(-1);
    }
    else if (key == LV_KEY_ENTER)
    {
        handle_button_click();
    }
    else if (key == LV_KEY_ESC)
    {
    }
}

void boot_sequence_complete()
{
    ESP_LOGI(kLogTag, "Boot sequence complete");
    show_small_blind_screen();
}

} // namespace

void init()
{
    ui::assets::init();

    ESP_LOGI(kLogTag, "Initialising tasks");

    M5.Speaker.setVolume(128);

    const auto &handles = ui::get();
    set_visible(handles.logo, true);

    if (handles.focus_proxy)
    {
        lv_obj_add_event_cb(handles.focus_proxy, encoder_key_event_cb, LV_EVENT_KEY, nullptr);
    }

    if (encoder_input::group() && handles.focus_proxy)
    {
        lv_group_focus_obj(handles.focus_proxy);
    }

    if (g_state.countdown_timer == nullptr)
    {
        g_state.countdown_timer = lv_timer_create(countdown_timer_cb, 1000, nullptr);
        lv_timer_pause(g_state.countdown_timer);
    }

    ESP_LOGI(kLogTag, "Starting boot chime sequence");
    start_tone_sequence(kBootSequence, std::size(kBootSequence), boot_sequence_complete);
}

void tick()
{
    if (M5.BtnA.wasClicked())
    {
        handle_button_click();
    }
}

} // namespace app_tasks

