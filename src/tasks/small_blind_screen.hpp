#pragma once

namespace screens::small_blind
{
// Initialize the LVGL view and reset the active value.
void show();

// Adjust the small blind value using rotary steps (+/-1).
void handle_encoder_delta(int step);
bool handle_raw_encoder_diff(int diff);
}

