#include "apps.h"

using namespace fs;

int current_app = 0;

void draw_page_indicator();

void next_page()
{
    if (current_page < total_pages - 1)
    {
        current_page++;
        draw_home();
    }
}

void previous_page()
{
    if (current_page > 0)
    {
        current_page--;
        draw_home();
    }
}

void go_to_page(int page)
{
    if (page >= 0 && page < total_pages)
    {
        current_page = page;
        draw_home();
    }
}

void home_handler()
{
    int max_apps_per_row = max_apps_per_rows;
    int y_start = margin;
    int visible_index = 0;

    int max_apps_per_page = max_apps_per_row * max_apps_per_columns;

    int start_index = current_page * max_apps_per_page;
    int end_index = min((current_page + 1) * max_apps_per_page, app_list_size);

    TouchPoint p = get_pos();

    if (total_pages > 1)
    {
        if (swipe && swipe_direction == 2)
        {
            previous_page();
        }
        else if (swipe && swipe_direction == 4)
        {
            next_page();
        }
    }

    if (touch)
    {

        for (int i = start_index; i < end_index; i++)
        {
            if (!app_list[i].on_home && !op_mode)
                continue;

            int row = visible_index / max_apps_per_row;
            int col = visible_index % max_apps_per_row;
            int apps_in_row = min(max_apps_per_row, (end_index - start_index) - row * max_apps_per_row);

            int total_width = apps_in_row * app_icon_size + (apps_in_row - 1) * app_margin;
            int startX = (tft.width() - total_width) / 2;

            int x = startX + col * (app_icon_size + app_margin);
            int y = y_start + row * (app_icon_size + app_margin);

            if (p.x >= x && p.x <= x + app_icon_size && p.y >= y && p.y <= y + app_icon_size)
            {
                if (debug)
                {
                    Serial.print("Selected App : ");
                    Serial.println(app_list[i].name);
                    if (app_list[i].beta)
                    {
                        Serial.println("/!\\ App in beta");
                    }
                }

                tft.fillRoundRect(x, y, app_icon_size, app_icon_size, 10, changeBrightness(app_list[i].color, 0.5));
                delay(150);

                if (app_list[i].beta)
                {
                    notification = true;
                    notification_text = "/!\\ This app is still in beta";
                    notification_time = 2.0;
                    notification_handler();
                }
                launch_app(app_list[i]);
                return;
            }

            visible_index++;
        }
    }
}

void draw_settings()
{

    char time_text[14];
    strcpy(time_text, "Time : ");
    strcat(time_text, current_time);

    char chip_text[20];
    strcpy(chip_text, "Chip : ");
    strcat(chip_text, cpu_name);

    char chip_speed_text[20];
    strcpy(chip_speed_text, "Chip speed : ");
    strcat(chip_speed_text, cpu_speed);

    char dev_mode_text[15];
    strcpy(dev_mode_text, "DEV MODE : ");
    if (debug)
    {
        strcat(dev_mode_text, "on");
    }
    else
    {
        strcat(dev_mode_text, "off");
    }

    char os_name[30] = "OS name : ZackOS mobile (CYD)";

    char os_version_text[17];

    strcpy(os_version_text, "OS version : ");
    strcat(os_version_text, os_version);

    tft.fillScreen(color565(91, 91, 91));
    tft.setTextColor(color565(0, 0, 0), color565(91, 91, 91));
    tft.setTextDatum(TL_DATUM);
    tft.drawCentreString("Settings", tft.width() / 2, 30, 2);
    tft.drawString(dev_mode_text, margin, 90, 2);
    tft.drawString("Infos :", margin, 115, 2);
    tft.drawString(time_text, margin + list_margin, 140, 2);
    tft.drawString(chip_text, margin + list_margin, 165, 2);
    tft.drawString(chip_speed_text, margin + list_margin, 190, 2);
    tft.drawString(os_name, margin + list_margin, 215, 2);
    tft.drawString(os_version_text, margin + list_margin, 235, 2);

    draw_home_indicator();
    draw_top_bar();

    if (debug)
        Serial.println("SETTINGS draw");
}

void settings_handler()
{

    if (need_to_be_redrawn)
    {
        draw_settings();
        need_to_be_redrawn = false;
        need_to_be_refreshed = false;
    }

    if (debug)
        Serial.println("SETTINGS handler");

    TouchPoint p = get_pos();

    if (touch)
    {
        if (p.y < (tft.fontHeight(2) + 90) && p.y > 90)
        {
            if (debug)
                Serial.println("DEV MODE CLICKED FROM SETTINGS");
            debug = !debug;
            draw_settings();
            delay(250);
        }
    }

    if (home_indicator_touched)
    {
        if (debug)
            Serial.println("HI TOUCHED FROM SETTINGS");

        for (int i = 0; i < app_list_size; i++)
        {
            if (strcmp(app_list[i].name, "home") == 0)
            {
                current_app = i;
            }
        }
    }
}

void draw_reboot_menu()
{
    int boxX = tft.width() / 4;
    int boxY = tft.height() / 2;
    int boxW = tft.width() / 2;
    int boxH = 100;

    tft.fillRect(boxX, boxY, boxW, boxH, color565(91, 91, 91));

    tft.setTextColor(color565(0, 0, 0));
    tft.drawCentreString("Restart", tft.width() / 2, boxY + (boxH / 2) - 8, 2);
}

void reboot_menu_handler()
{
    TouchPoint p = get_pos();
    if (touch)
    {
        int boxX = tft.width() / 4;
        int boxY = tft.height() / 2;
        int boxW = tft.width() / 2;
        int boxH = 100;

        if (debug)
        {
            Serial.print("boxX ");
            Serial.println(boxX);
            Serial.print("boxY ");
            Serial.println(boxY);
            Serial.print("boxW ");
            Serial.println(boxW);
            Serial.print("boxH ");
            Serial.println(boxH);

            Serial.println("==========================");
            Serial.println(p.x);
            Serial.println(p.y);
        }

        if (p.x >= boxX && p.x <= boxX + boxW &&
            p.y >= boxY && p.y <= boxY + boxH)
        {

            tft.fillRect(boxX, boxY, boxW, boxH, color565(50, 200, 50));
            tft.setTextColor(TFT_BLACK);
            tft.drawCentreString("Restarting...", tft.width() / 2, boxY + (boxH / 2) - 8, 2);
            need_to_be_redrawn = true;

            for (int i = 0; i < 255; i += 15)
            {
                tft.fillScreen(color565(i, i, i));
                delay(20);
            }
            ESP.restart();
        }
    }
    if (debug)
        Serial.println("HEY from reboot_menu_handler");
    need_to_be_refreshed = true;
}

void handle_apps()
{
    app_list[current_app].run();
    need_to_be_refreshed = true;
    need_to_be_redrawn = true;
}

void startup()
{
    tft.fillScreen(color565(0, 0, 0));
    delay(500);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("ZackOS", tft.width() / 2, tft.height() / 2, 4);
    tft.drawCentreString("By Zack", tft.width() / 2, tft.height() / 2 + 30, 2);
    for (int i = 0; i < 255; i++)
    {
        tft.setBrightness(i);
        delay(10);
    }
    delay(500);
    for (int i = 255; i > 0; i--)
    {
        tft.setBrightness(i);
        delay(5);
    }
    launch_app(lock);
    lock.drawner();
    for (int i = 0; i < 255; i++)
    {
        tft.setBrightness(i);
        delay(1);
    }
}

void draw_sleep()
{
    tft.fillScreen(color565(0, 0, 0));
    tft.setTextColor(color565(255, 255, 255), color565(0, 0, 0));

    bool screen_on = false;
    int idle = 0;

    while (1)
    {
        TouchPoint pos = get_pos();
        uint16_t tx = 0, ty = 0;
        bool touched = tft.getTouch(&tx, &ty);
        if (touched)
            remap_pos(tx, ty);
        else
            screen_touched = false;

        if (pos.touched)
        {
            idle = 0;
            if (screen_on)
            {
                launch_app(home);
                break;
            }
            else
            {
                screen_on = true;
                tft.setBrightness(255);
            }
        }

        if (screen_on)
        {
            tft.drawCentreString(current_time, tft.width() / 2, tft.height() / 2, 4);
            idle++;
            if (idle > 500)
            {
                screen_on = false;
                idle = 0;
                tft.fillScreen(color565(0, 0, 0));
                tft.setBrightness(0);
            }
        }

        delay(10);
    }

    tft.setBrightness(255);
}

void draw_lock_screen()
{
    tft.fillScreen(lock_screen_background_color);
    // draw_top_bar();
    tft.setTextColor(color565(0, 0, 0), lock_screen_background_color);
    tft.drawCentreString(current_time, tft.width() / 2, tft.height() / 4, 6);
    draw_home_indicator();
}

void lock_screen_handler()
{
    if (swipe && swipe_direction == 1)
    {
        double last = millis();
        double speed = 600.0;
        double pos = tft.height();

        tft.setTextColor(TFT_BLACK, lock_screen_background_color);

        tft.fillScreen(lock_screen_background_color);
        // tft.drawCentreString(current_time, tft.width() / 2, tft.height() / 4, 6);

        while (pos > 0)
        {
            double now = millis();
            double deltaTime = (now - last) / 1000.0;
            last = now;

            pos -= speed * deltaTime;
            if (pos < 0)
                pos = 0;

            int i = (int)pos;
            tft.fillRect(0, i, tft.width(), tft.height() - i, background_color);
            int textY = tft.height() / 4 + (i - tft.height());
            if (textY < tft.height())
            {
                tft.drawCentreString(current_time, tft.width() / 2, textY, 6);
            }
        }

        draw_home_indicator();
        swipe = false;
        touch = false;

        launch_app(home);
    }
}

void draw_keyboard()
{
    const int rows = 4;
    const int cols = 10;
    const int key_w = tft.width() / cols - 4;
    const int key_h = (tft.height() / 3) / rows - 4;
    const int corner_radius = 3;

    const uint16_t keyboard_color = color565(40, 40, 40);
    const uint16_t key_color = color565(60, 60, 60);
    const uint16_t text_color = color565(255, 255, 255);

    tft.fillRoundRect(0, tft.height() / 2, tft.width(), tft.height() / 2, 10, keyboard_color);
    tft.fillRect(0, tft.height() / 2 - 30, tft.width(), tft.height() / 2, keyboard_color);
    tft.setTextDatum(ML_DATUM);
    tft.setTextColor(color565(255, 255, 255), keyboard_color);
    tft.drawString(typed_text, margin, (tft.height() / 2) - 15, 2);

    const char *keyboard_keys[rows][cols] = {
        {"a", "z", "e", "r", "t", "y", "u", "i", "o", "p"},
        {"q", "s", "d", "f", "g", "h", "j", "k", "l", "m"},
        {"w", "x", "c", "v", "b", "n", "-", "_", "SC", "BK"},
        {"RRN", "", "", "", "", "", "", "", "", ""}};

    int start_y = tft.height() / 2 + 10;
    int font_h = tft.fontHeight();

    for (int row = 0; row < rows; row++)
    {
        int start_x = 2;
        for (int col = 0; col < cols; col++)
        {
            int x = start_x + col * (key_w + 4);
            int y = start_y + row * (key_h + 4);

            tft.fillRoundRect(x, y, key_w, key_h, corner_radius, key_color);

            tft.setTextDatum(MC_DATUM);
            tft.setTextColor(text_color, key_color);
            tft.drawString(keyboard_keys[row][col], x + key_w / 2, y + key_h / 2 - font_h / 2, 2);
        }
    }
}

void keyboard_handler()
{
    const int rows = 4;
    const int cols = 10;
    const int key_w = tft.width() / cols - 4;
    const int key_h = (tft.height() / 3) / rows - 4;

    const char *keyboard_keys[rows][cols] = {
        {"a", "z", "e", "r", "t", "y", "u", "i", "o", "p"},
        {"q", "s", "d", "f", "g", "h", "j", "k", "l", "m"},
        {"w", "x", "c", "v", "b", "n", "-", "_", "SPACE", "BACK"},
        {"RETURN", "", "", "", "", "", "", "", "", ""}};

    int start_y = tft.height() / 2 + 10;
    TouchPoint p = get_pos();

    if (!touch)
        return;

    if (debug)
        Serial.println("Touched on keyboard (Hello from keyboard handler btw)");

    for (int row = 0; row < rows; row++)
    {
        int start_x = 2;
        for (int col = 0; col < cols; col++)
        {
            int x = start_x + col * (key_w + 4);
            int y = start_y + row * (key_h + 4);

            if (p.x >= x && p.x <= x + key_w && p.y >= y && p.y <= y + key_h)
            {
                const char *key = keyboard_keys[row][col];
                if (strcmp(key, "SPACE") == 0)
                {
                    typed_text += " ";
                    if (debug)
                        Serial.println("ADDED SPACE TO TYPED TEXT");
                }
                else if (strcmp(key, "BACK") == 0)
                {
                    if (typed_text.length() > 0)
                        typed_text.remove(typed_text.length() - 1);
                    if (debug)
                        Serial.println("REMOVED THE LAST CHARACTER FROM TYPED TEXT");
                }
                else if (strcmp(key, "RETURN") == 0)
                {
                    keyboard_active = false;
                    if (debug)
                        Serial.println("KEYBOARD STOPPED LAUNCHING NEXT APP");
                    launch_app(*next_app_after_keyboard);
                    return;
                    if (next_app_after_keyboard != nullptr)
                        launch_app(*next_app_after_keyboard);
                }
                else if (strlen(key) > 0)
                {
                    typed_text += key;
                    if (debug)
                    {
                        Serial.print("ADDED : ");
                        Serial.print(key);
                        Serial.println("TO TYPED TEXT");
                    }
                }
                draw_keyboard();
                delay(100);
                return;
            }
        }
    }
}

void draw_themes()
{
    tft.fillScreen(color565(91, 91, 91));

    tft.setTextDatum(MC_DATUM);
    int margin = 20;
    tft.drawCentreString("Change wallpaper", tft.width() / 2, margin, 4);
    tft.drawCentreString("Color", tft.width() / 2, margin + 25, 4);

    uint16_t colors[8] = {
        color565(255, 0, 0),
        color565(0, 255, 0),
        color565(0, 0, 255),
        color565(255, 255, 0),
        color565(255, 0, 255),
        color565(0, 255, 255),
        color565(255, 128, 0),
        color565(128, 0, 255)};

    int squareSize = 40;
    int padding = 10;
    int startX = (tft.width() - (2 * squareSize + 3 * padding)) / 2;
    int startY = margin + 75;

    for (int i = 0; i < 8; i++)
    {
        int row = i % 4;
        int col = i / 4;
        int x = startX + col * (squareSize + padding);
        int y = startY + row * (squareSize + padding);

        tft.fillRoundRect(x, y, squareSize, squareSize, 5, colors[i]);
    }
}

void themes_handler()
{
    uint16_t colors[8] = {
        color565(255, 0, 0),
        color565(0, 255, 0),
        color565(0, 0, 255),
        color565(255, 255, 0),
        color565(255, 0, 255),
        color565(0, 255, 255),
        color565(255, 128, 0),
        color565(128, 0, 255)};

    int squareSize = 40;
    int padding = 10;
    int margin = 20;
    int startX = (tft.width() - (2 * squareSize + 3 * padding)) / 2;
    int startY = margin + 75;
    if (true)
    {
        TouchPoint p = get_pos();

        if (!touch)
            return;

        for (int i = 0; i < 8; i++)
        {
            int row = i % 4;
            int col = i / 4;
            int x = startX + col * (squareSize + padding);
            int y = startY + row * (squareSize + padding);

            if (p.x >= x && p.x <= x + squareSize && p.y >= y && p.y <= y + squareSize)
            {

                uint16_t selected_color = colors[i];

                background_color = selected_color;

                int row = i % 4;
                int col = i / 4;
                int x = startX + col * (squareSize + padding);
                int y = startY + row * (squareSize + padding);

                tft.fillRoundRect(x, y, squareSize, squareSize, 5, changeBrightness(colors[i], 0.5));

                if (debug)
                {
                    Serial.print("Selected wallpaper : ");
                    Serial.println(i);
                }

                break;
            }
        }
    }
}

void draw_zackpay()
{
    uint16_t zackpay_background_color = color565(125, 129, 203);
    uint16_t zackpacy_pay_button_color = color565(255, 30, 30);
    tft.fillScreen(zackpay_background_color);
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(color565(0, 0, 0), zackpay_background_color);
    draw_top_bar();
    draw_home_indicator();
    tft.drawCentreString("ZackPay", tft.width() / 2, 20, 4);
    char balance_text[50];
    strcpy(balance_text, "Balance : ");
    char balance_str[20];
    sprintf(balance_str, "%ld", balance);
    strcat(balance_text, balance_str);
    strcat(balance_text, "$");
    tft.drawCentreString(balance_text, tft.width() / 2, 50, 2);
    tft.fillRect(tft.width() / 4, tft.height() / 2 - 30, tft.width() / 2, 35, zackpacy_pay_button_color);
    tft.setTextColor(color565(0, 0, 0), zackpacy_pay_button_color);
    tft.drawCentreString("pay", tft.width() / 2, tft.height() / 2 - 15, 2);
}

void zackpay_handler()
{
    TouchPoint p = get_pos();
    if (touch)
    {
        tft.fillRect(tft.width() / 4, tft.height() / 2 - 30, tft.width() / 2, 35, color565(255, 30, 30));
        if (p.x > tft.width() / 2 && p.x < tft.width() / 4 * 3 && p.y > tft.height() / 2 - 35 && p.y < tft.height() / 2)
        {
            if (debug)
                Serial.println("Pay button pressed on zackpay");
            balance -= random(10, 123);
            draw_zackpay();
            delay(100);
        }
    }
}

void torch_on()
{
    if (debug)
        Serial.println("Set torch state to 1");
    torch_enabled = true;
    digitalWrite(led_pin_r, LOW);
    digitalWrite(led_pin_g, LOW);
    digitalWrite(led_pin_b, LOW);
    if (debug)
        Serial.println("Torch tuned on");
}

void torch_off()
{
    torch_enabled = false;
    if (debug)
        Serial.println("Set torch state to 0");
    digitalWrite(led_pin_r, HIGH);
    digitalWrite(led_pin_g, HIGH);
    digitalWrite(led_pin_b, HIGH);
    if (debug)
        Serial.println("Torch tuned off");
}

void setup_torch()
{
    torch_enabled = false;
    if (debug)
        Serial.println("Set torch state to 0 (from setup torch)");
    pinMode(led_pin_r, OUTPUT);
    pinMode(led_pin_g, OUTPUT);
    pinMode(led_pin_b, OUTPUT);
    torch_off();
    if (debug)
        Serial.println("Setup torch");
}

void torch_handler()
{
    if (torch_enabled)
    {
        torch_off();
    }
    else
    {
        torch_on();
    }
    if (debug)
    {
        Serial.println("Launching home from torch");
        Serial.print("Current touch state : ");
        Serial.println(torch_enabled);
    }
    touch = false;
    launch_app(home);
}

void draw_torch()
{
}

void draw_calculator()
{
    tft.fillScreen(color565(30, 30, 30));

    tft.fillRect(10, 50, tft.width() - 20, 60, color565(20, 20, 20));
    tft.setTextColor(TFT_WHITE, color565(20, 20, 20));
    tft.setTextDatum(MR_DATUM);
    tft.drawString("0", tft.width() - 15, 80, 4);

    for (int i = 0; i <= 9; i++)
        calculator_digit_buttons[i].draw();
    calculator_plus_button.draw();
    calculator_minus_button.draw();
    calculator_multiply_button.draw();
    calculator_divide_button.draw();
    calculator_equal_button.draw();
    calculator_clear_button.draw();
}

void calculator_handler() {
    if (debug) Serial.printf("calculator_handler: x=%d y=%d touched=%d\n", global_pos_x, global_pos_y, screen_touched);
    if (!touch) return;

    for (int i = 0; i <= 9; i++) {
        if (calculator_digit_buttons[i].collide(global_pos_x, global_pos_y)) {
            char digit[2] = {(char)('0' + i), '\0'};
            if (strlen(calculator_calcul) < 29)
                strncat(calculator_calcul, digit, 1);
            draw_calculator_display();
            return;
        }
    }

    if (calculator_plus_button.collide(global_pos_x, global_pos_y)) {
        if (strlen(calculator_calcul) < 29)
            strncat(calculator_calcul, "+", 1);
        draw_calculator_display();
        return;
    }

    if (calculator_minus_button.collide(global_pos_x, global_pos_y)) {
        if (strlen(calculator_calcul) < 29)
            strncat(calculator_calcul, "-", 1);
        draw_calculator_display();
        return;
    }

    if (calculator_multiply_button.collide(global_pos_x, global_pos_y)) {
        if (strlen(calculator_calcul) < 29)
            strncat(calculator_calcul, "*", 1);
        draw_calculator_display();
        return;
    }

    if (calculator_divide_button.collide(global_pos_x, global_pos_y)) {
        if (strlen(calculator_calcul) < 29)
            strncat(calculator_calcul, "/", 1);
        draw_calculator_display();
        return;
    }

    if (calculator_clear_button.collide(global_pos_x, global_pos_y)) {
        memset(calculator_calcul, 0, sizeof(calculator_calcul));
        draw_calculator_display();
        return;
    }

    if (calculator_equal_button.collide(global_pos_x, global_pos_y)) {
        if (!strcmp(calculator_calcul, "//00")) launch_app(hidden_menu);
        long result = 0;
        long current = 0;
        char op = '+';
        for (int i = 0; i <= (int)strlen(calculator_calcul); i++) {
            char c = calculator_calcul[i];
            if (c >= '0' && c <= '9') {
                current = current * 10 + (c - '0');
            } else {
                if (op == '+') result += current;
                else if (op == '-') result -= current;
                else if (op == '*') result *= current;
                else if (op == '/') result = (current != 0) ? result / current : 0;
                op = c;
                current = 0;
            }
        }
        memset(calculator_calcul, 0, sizeof(calculator_calcul));
        snprintf(calculator_calcul, 29, "%ld", result);
        draw_calculator_display();
        return;
    }
}

void draw_calculator_display() {
    tft.fillRect(10, 50, tft.width() - 20, 60, color565(20, 20, 20));
    tft.setTextColor(TFT_WHITE, color565(20, 20, 20));
    tft.setTextDatum(MR_DATUM);
    if (strlen(calculator_calcul) == 0)
        tft.drawString("0", tft.width() - 15, 80, 4);
    else
        tft.drawString(calculator_calcul, tft.width() - 15, 80, 4);
}

void init_calculator()
{

    const int btn_w = (tft.width() - 10) / 4;
    const int btn_h = 50;
    const int margin = 2;
    const int grid_y = 130;

    uint16_t digit_color = color565(60, 60, 60);
    uint16_t digit_outline = color565(80, 80, 80);
    uint16_t op_color = color565(200, 120, 0);
    uint16_t op_outline = color565(235, 171, 52);
    uint16_t eq_color = color565(235, 171, 52);
    uint16_t eq_outline = color565(255, 200, 80);
    uint16_t clr_color = color565(180, 30, 30);
    uint16_t clr_outline = color565(220, 60, 60);

    static char digit_labels[10][2] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};

    calculator_digit_buttons[7] = Button(0 * btn_w + margin, grid_y + 0 * (btn_h + margin), btn_w - margin, btn_h, digit_color, digit_labels[7], digit_outline);
    calculator_digit_buttons[8] = Button(1 * btn_w + margin, grid_y + 0 * (btn_h + margin), btn_w - margin, btn_h, digit_color, digit_labels[8], digit_outline);
    calculator_digit_buttons[9] = Button(2 * btn_w + margin, grid_y + 0 * (btn_h + margin), btn_w - margin, btn_h, digit_color, digit_labels[9], digit_outline);
    calculator_divide_button = Button(3 * btn_w + margin, grid_y + 0 * (btn_h + margin), btn_w - margin, btn_h, op_color, (char *)"/", op_outline);

    calculator_digit_buttons[4] = Button(0 * btn_w + margin, grid_y + 1 * (btn_h + margin), btn_w - margin, btn_h, digit_color, digit_labels[4], digit_outline);
    calculator_digit_buttons[5] = Button(1 * btn_w + margin, grid_y + 1 * (btn_h + margin), btn_w - margin, btn_h, digit_color, digit_labels[5], digit_outline);
    calculator_digit_buttons[6] = Button(2 * btn_w + margin, grid_y + 1 * (btn_h + margin), btn_w - margin, btn_h, digit_color, digit_labels[6], digit_outline);
    calculator_multiply_button = Button(3 * btn_w + margin, grid_y + 1 * (btn_h + margin), btn_w - margin, btn_h, op_color, (char *)"*", op_outline);

    calculator_digit_buttons[1] = Button(0 * btn_w + margin, grid_y + 2 * (btn_h + margin), btn_w - margin, btn_h, digit_color, digit_labels[1], digit_outline);
    calculator_digit_buttons[2] = Button(1 * btn_w + margin, grid_y + 2 * (btn_h + margin), btn_w - margin, btn_h, digit_color, digit_labels[2], digit_outline);
    calculator_digit_buttons[3] = Button(2 * btn_w + margin, grid_y + 2 * (btn_h + margin), btn_w - margin, btn_h, digit_color, digit_labels[3], digit_outline);
    calculator_minus_button = Button(3 * btn_w + margin, grid_y + 2 * (btn_h + margin), btn_w - margin, btn_h, op_color, (char *)"-", op_outline);

    calculator_clear_button = Button(0 * btn_w + margin, grid_y + 3 * (btn_h + margin), btn_w - margin, btn_h, clr_color, (char *)"C", clr_outline);
    calculator_digit_buttons[0] = Button(1 * btn_w + margin, grid_y + 3 * (btn_h + margin), btn_w - margin, btn_h, digit_color, digit_labels[0], digit_outline);
    calculator_equal_button = Button(2 * btn_w + margin, grid_y + 3 * (btn_h + margin), btn_w - margin, btn_h, eq_color, (char *)"=", eq_outline);
    calculator_plus_button = Button(3 * btn_w + margin, grid_y + 3 * (btn_h + margin), btn_w - margin, btn_h, op_color, (char *)"+", op_outline);
}

void init_hidden_menu() {

    for (int i = 0; i < 3; i++) {
        hidden_menu_buttons[i] = Button(30, 30 + 30 * (i+1), tft.width() - (30 * 2), 30 + 3, color565(255, 0, 0), "", color565(0, 0, 0));
    }

    hidden_menu_buttons[0].text = (char*)"show all";
    hidden_menu_buttons[1].text = (char*)"boost freq (240)";
    hidden_menu_buttons[2].text = (char*)"normal freq (160)";
}

void draw_hidden_menu() {
    tft.fillScreen(color565(20, 20, 20));
    for (int i = 0; i < 3; i++) {
        hidden_menu_buttons[i].draw();
    }
}

void hidden_menu_handler() {
    for (int i = 0; i < 3; i++) {
        if (hidden_menu_buttons[i].collide(global_pos_x, global_pos_y)) {
            if (i == 0) {
                launch_app(home);
                op_mode = !op_mode;
            }
            else if (i == 1) {
                setCpuFrequencyMhz(240);
            } else if (i == 2) {
                setCpuFrequencyMhz(160);
            }
        }
    }
}

App settings = {
    color565(91, 91, 91), settings_handler, draw_settings, "Settings", true, false};

App reboot_menu = {
    color565(203, 249, 94), reboot_menu_handler, draw_reboot_menu, "Restart", true, false};

App sleep_app = {
    color565(255, 0, 0), draw_sleep, draw_sleep, "Sleep", true, false};

App lock = {
    color565(0, 0, 0), lock_screen_handler, draw_lock_screen, "Lock", true, false};

App themes = {
    color565(126, 252, 42), themes_handler, draw_themes, "Themes", true, false};

App keyboard = {
    color565(0, 0, 0), keyboard_handler, draw_keyboard, "keyboard", true, false};

App zackpay = {
    color565(125, 129, 203), zackpay_handler, draw_zackpay, "ZackPay", true, true};

App torch = {
    color565(0, 0, 0), torch_handler, draw_torch, "Torch", true, true};

App calculator = {
    color565(200, 50, 50), calculator_handler, draw_calculator, "Calculator", true, false};

App hidden_menu {
    color565(0, 0, 0), hidden_menu_handler, draw_hidden_menu, "Hidden menu", false, false
};

App app_list[] = {home, settings, reboot_menu, sleep_app, lock, themes, zackpay, calculator, hidden_menu, keyboard};
int app_list_size = sizeof(app_list) / sizeof(app_list[0]);

void draw_home()
{
    tft.fillScreen(background_color);

    int max_apps_per_row = max_apps_per_rows;
    int y_start = margin;
    int visible_index = 0;

    int max_apps_per_page = max_apps_per_row * max_apps_per_columns;

    int start_index = current_page * max_apps_per_page;
    int end_index = min((current_page + 1) * max_apps_per_page, app_list_size);

    for (int i = start_index; i < end_index; i++)
    {

        if (!app_list[i].on_home && !op_mode)
            continue;

        int row = visible_index / max_apps_per_row;
        int col = visible_index % max_apps_per_row;

        int apps_in_row = min(max_apps_per_row, (end_index - start_index) - row * max_apps_per_row);
        int total_width = apps_in_row * app_icon_size + (apps_in_row - 1) * app_margin;
        int startX = (tft.width() - total_width) / 2;

        int x = startX + col * (app_icon_size + app_margin);
        int y = y_start + row * (app_icon_size + app_margin);

        tft.fillRoundRect(x, y, app_icon_size, app_icon_size, 15, app_list[i].color);

        if (debug)
        {
            char text[100];
            sprintf(text, "Application: %s, index: %d, page: %d/%d",
                    app_list[i].name, i, current_page + 1, total_pages);
            Serial.println(text);
        }

        tft.setTextColor(TFT_WHITE);
        tft.setTextDatum(MC_DATUM);
        tft.drawString(app_list[i].name, x + app_icon_size / 2, y + app_icon_size + 10, 1);

        visible_index++;
    }

    std::vector<String> paths;
    File root_app = SD.open("/apps");
    if (root_app)
    {
        list_files_non_recursive(root_app, "", paths);
        root_app.close();
    }
    else
    {
        if (debug)
            Serial.println("SD: /apps folder not found or SD not ready");
    }

    start_index = end_index;

    end_index = min((current_page + 1) * max_apps_per_page, (int)paths.size());

    for (int i = visible_index; i < end_index; i++)
    {
        // if (!app_list[i].on_home) continue;

        int row = visible_index / max_apps_per_row;
        int col = visible_index % max_apps_per_row;

        int apps_in_row = min(max_apps_per_row, (end_index - start_index) - row * max_apps_per_row);
        int total_width = apps_in_row * app_icon_size + (apps_in_row - 1) * app_margin;
        int startX = (tft.width() - total_width) / 2;

        int x = startX + col * (app_icon_size + app_margin);
        int y = y_start + row * (app_icon_size + app_margin);

        uint16_t logo_w;
        uint16_t logo_h;

        /* if (SD.exists(paths[i] + "/assets/logo.bmp")) {
            read_bmp_dimensions(paths[i].c_str(), logo_w, logo_h);
            if (logo_w == app_icon_size && logo_h == app_icon_size) {
                if (!tft.drawBitmap(paths[i] + "/assets/logo.png"), x, y) {
                    tft.fillRoundRect(x, y, app_icon_size, app_icon_size, 10, color565(0, 0, 0));
                }
            }
        } */

        if (debug)
        {
            Serial.println("Showing zlua app");
        }

        tft.setTextColor(TFT_WHITE);
        tft.setTextDatum(MC_DATUM);
        tft.drawString(paths[i], x + app_icon_size / 2, y + app_icon_size + 10, 1);

        visible_index++;
    }

    draw_page_indicator();
    draw_top_bar();
}

void draw_page_indicator()
{
    if (total_pages > 1)
    {
        int dot_radius = 3;
        int dot_spacing = 10;
        int total_width = total_pages * dot_spacing;
        int start_x = (tft.width() - total_width) / 2;
        int y_pos = tft.height() - 20;

        for (int i = 0; i < total_pages; i++)
        {
            if (i == current_page)
            {
                tft.fillCircle(start_x + i * dot_spacing, y_pos, dot_radius, TFT_WHITE);
            }
            else
            {
                tft.fillCircle(start_x + i * dot_spacing, y_pos, dot_radius, TFT_DARKGREY);
            }
        }
    }
}

App home = {
    background_color, home_handler, run_home, "home", false};