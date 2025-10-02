## M5Unified For M5Stack Dial

If you're building M5Stack Dial projects with PIO and ESP-IDF, here are my findings:

| Component | API | Status | Notes |
|-----------|-----|--------|-------|
| **Display** | `M5.Display.*` | ✅ Works | M5GFX handles GC9A01 LCD |
| **Touch** | `M5.Touch.*` | ✅ Works | FT3267 I2C driver  |
| **Speaker/Buzzer** | `M5.Speaker.tone()` | ✅ Works | LEDC PWM on GPIO3 |
| **Power** | `M5.Power.powerOff()` | ✅ Works | GPIO46 control |
| **Button A** | `M5.BtnA.*` | ❌ **BROKEN** | Use GPIO polling |
| **Encoder** | M5 encoder API | ⚠️ Inefficient | Use PCNT instead |

**Suggest**: Use M5Unified where it works, extend with custom modules where needed.

---

## Hardware Abstraction Pattern

### Problem: M5.BtnA Doesn't Work on Dial

```cpp
// ❌ DOESN'T WORK
if (M5.BtnA.wasClicked()) {
    // Never fires on M5Stack Dial
}
```

### Solution: Custom Button Module

**[src/hardware/button.hpp](../src/hardware/button.hpp)**

```cpp
// ✅ WORKS
hardware::Button btnA(GPIO_NUM_42, 100, 2000);  // 100ms debounce, 2s long press
btnA.on_short_press([]() { /* handle click */ });
btnA.on_long_press([]() { M5.Power.powerOff(); });

// In loop:
btnA.update();
```

**Features:**
- 100ms software debounce
- Short press / long press callbacks
- GPIO polling (the only method that works)
- Reusable for any GPIO button

---

### Problem: Encoder API is Inefficient

### Solution: Hardware PCNT Module

**[src/hardware/encoder.hpp](../src/hardware/encoder.hpp)**

```cpp
// ✅ Hardware counting, zero CPU overhead
hardware::Encoder::instance().on_rotation([](int delta) {
    // delta = rotation amount (positive = CW, negative = CCW)
});
```

**Benefits:**
- ESP32-S3 hardware pulse counter (PCNT peripheral)
- 1000ns glitch filter in hardware
- Zero CPU overhead
- Bridges C callback → C++ OOP

---

## Quick Start Template

```cpp
#include <M5Unified.hpp>
#include "hardware/button.hpp"
#include "hardware/encoder.hpp"

// Initialize hardware modules
hardware::Button btnA(GPIO_NUM_42, 100, 2000);
btnA.on_short_press([]() {
    M5.Speaker.tone(3520.0f, 100);  // A7 tone
});
btnA.on_long_press([]() {
    M5.Power.powerOff();
});

hardware::Encoder::instance().on_rotation([](int delta) {
    ESP_LOGI("ENCODER", "Delta: %d", delta);
});

// Main loop
void loop() {
    M5.update();           // Update M5Unified (display, touch)
    btnA.update();         // Poll button with debounce
    // Encoder updates automatically via PCNT interrupt
    delay(5);
}
```

