// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2024 mzyy94

#if defined(ESP_PLATFORM)
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/pulse_cnt.h>
#include <driver/gpio.h>
#include "encoder.hpp"

#define PCNT_LOW_LIMIT -32768
#define PCNT_HIGH_LIMIT 32767


Encoder::Encoder()
{
    _pcnt_unit = NULL;
    _pcnt_chan_a = NULL;
    _pcnt_chan_b = NULL;
}

Encoder::~Encoder()
{
    pcnt_unit_stop(_pcnt_unit);
    pcnt_unit_disable(_pcnt_unit);
    pcnt_del_channel(_pcnt_chan_a);
    pcnt_del_channel(_pcnt_chan_b);
    pcnt_del_unit(_pcnt_unit);
}

void Encoder::setup(gpio_num_t pin_a, gpio_num_t pin_b)
{
    pcnt_unit_config_t unit_config = {};
    unit_config.low_limit = PCNT_LOW_LIMIT;
    unit_config.high_limit = PCNT_HIGH_LIMIT;
    unit_config.intr_priority = 0;
    unit_config.flags.accum_count = 0;
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &_pcnt_unit));

    pcnt_glitch_filter_config_t filter_config = {};
    filter_config.max_glitch_ns = 1000;
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(_pcnt_unit, &filter_config));

    pcnt_chan_config_t chan_a_config = {};
    chan_a_config.edge_gpio_num = pin_a;
    chan_a_config.level_gpio_num = pin_b;
    ESP_ERROR_CHECK(pcnt_new_channel(_pcnt_unit, &chan_a_config, &_pcnt_chan_a));

    pcnt_chan_config_t chan_b_config = {};
    chan_b_config.edge_gpio_num = pin_b;
    chan_b_config.level_gpio_num = pin_a;
    ESP_ERROR_CHECK(pcnt_new_channel(_pcnt_unit, &chan_b_config, &_pcnt_chan_b));

    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(_pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(_pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_HOLD, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(_pcnt_chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(_pcnt_chan_b, PCNT_CHANNEL_LEVEL_ACTION_HOLD, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

    ESP_ERROR_CHECK(pcnt_unit_enable(_pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(_pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_start(_pcnt_unit));
}

int Encoder::getCount(bool clear)
{
    int count = 0;
    ESP_ERROR_CHECK(pcnt_unit_get_count(_pcnt_unit, &count));
    if (clear)
    {
        ESP_ERROR_CHECK(pcnt_unit_clear_count(_pcnt_unit));
    }
    return count;
}

void Encoder::reset()
{
    ESP_ERROR_CHECK(pcnt_unit_clear_count(_pcnt_unit));
}
#endif
