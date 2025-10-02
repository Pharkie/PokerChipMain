#pragma once

#include <cstdint>

namespace hardware {

/// Rotary encoder interface using hardware PCNT peripheral.
/// Provides high-level API for encoder rotation events.
class Encoder {
public:
    /// Callback type for rotation events
    /// @param delta Rotation delta (positive = clockwise, negative = counter-clockwise)
    using Callback = void(*)(int delta);

    /// Get the singleton instance
    static Encoder& instance();

    /// Set the callback for rotation events
    void on_rotation(Callback cb) { rotation_cb_ = cb; }

    /// Called by low-level driver when encoder rotates
    /// (This is invoked by the C callback encoder_notify_diff)
    void notify_rotation(int delta);

private:
    Encoder() = default;
    Callback rotation_cb_ = nullptr;
};

} // namespace hardware
