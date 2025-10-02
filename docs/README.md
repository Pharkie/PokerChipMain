# M5Stack Dial Docs

To see

- What works and what doesn't with M5Unified on the Dial
- My solution with hardware abstraction for Button, Encoder, Buzzer

See **[M5Unified For M5Stack Dial](m5unified-architecture.md)**

## Hardware and pins

- **MCU**: ESP32-S3 (240MHz, dual-core Xtensa)
- **Display**: 240x240 GC9A01 round LCD (via M5GFX)
- **Touch**: FT3267 capacitive touchscreen (I2C)
- **Encoder**: Hardware PCNT on dedicated pins
- **Button A**: GPIO 42 (physical button) - **M5.BtnA API broken, use GPIO polling**
- **Buzzer**: GPIO 3 (LEDC PWM)
- **Power on**: controlled by GPIO 46

