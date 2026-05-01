#pragma once

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device {
    lgfx::Panel_ST7796   _panel_instance;
    lgfx::Bus_SPI        _bus_instance;
    lgfx::Touch_FT5x06  _touch_instance;
    lgfx::Light_PWM      _light_instance;

public:
    LGFX(void) {

        // ── Bus SPI (affichage) ─────────────────────────────────────────────
        {
            auto cfg = _bus_instance.config();
            cfg.spi_host    = HSPI_HOST;
            cfg.spi_mode    = 0;
            cfg.freq_write  = 40000000;
            cfg.freq_read   = 16000000;
            cfg.spi_3wire   = false;
            cfg.use_lock    = true;
            cfg.dma_channel = SPI_DMA_CH_AUTO;
            cfg.pin_sclk    = 14;   // SCK
            cfg.pin_mosi    = 13;   // SDI (MOSI)
            cfg.pin_miso    = 12;   // SDO (MISO)
            cfg.pin_dc      = 2;    // LCD_RS
            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }

        // ── Panneau ST7796 ─────────────────────────────────────────────────
        {
            auto cfg = _panel_instance.config();
            cfg.pin_cs           = 15;
            cfg.pin_rst          = 4;
            cfg.pin_busy         = -1;
            cfg.memory_width     = 320;
            cfg.memory_height    = 480;
            cfg.panel_width      = 320;
            cfg.panel_height     = 480;
            cfg.offset_x         = 0;
            cfg.offset_y         = 0;
            cfg.offset_rotation  = 0;
            cfg.dummy_read_pixel = 8;
            cfg.dummy_read_bits  = 1;
            cfg.readable         = true;
            cfg.invert           = true;   // ST7796 nécessite inversion
            cfg.rgb_order        = false;
            cfg.dlen_16bit       = false;
            cfg.bus_shared       = false;
            _panel_instance.config(cfg);
        }

        // ── Touch GT911 (I2C) ──────────────────────────────────────────────
        {
            auto cfg = _touch_instance.config();
            cfg.i2c_port    = 0;
            cfg.i2c_addr    = 0x38;   // GT911 : 0x5D ou 0x14
            cfg.pin_sda     = 18;     // CTP_SDA
            cfg.pin_scl     = 19;     // CTP_SCL
            cfg.pin_int     = 23;     // CTP_INT
            cfg.pin_rst     = 22;     // CTP_RST
            cfg.freq        = 400000;
            cfg.x_min       = 0;
            cfg.x_max       = 319;
            cfg.y_min       = 0;
            cfg.y_max       = 479;
            cfg.bus_shared  = false;
            cfg.offset_rotation = 0;
            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }

        // ── Rétroéclairage PWM ─────────────────────────────────────────────
        {
            auto cfg = _light_instance.config();
            cfg.pin_bl      = 21;    // LED
            cfg.invert      = false;
            cfg.freq        = 44100;
            cfg.pwm_channel = 7;
            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }

        setPanel(&_panel_instance);
    }
};