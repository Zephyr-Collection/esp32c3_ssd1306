#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <lvgl.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include "display.h"
#include "time/time.h"

// --- QUẢN LÝ MÀN HÌNH ---
#define TOTAL_SCREENS 4
static lv_obj_t *screens[TOTAL_SCREENS]; // Mảng quản lý 4 màn hình
static uint8_t current_screen_idx = 0;   // Lưu index màn hình hiện tại (0 đến 3)

// --- WIDGETS & STYLES ---
static lv_obj_t *time_label;
static lv_obj_t *msg_label_1;
static lv_obj_t *msg_label_2;
static lv_obj_t *msg_label_3;
static lv_style_t text_style;

// --- TIMER ---
static lv_timer_t *screen_timer;

void update_lvgl_clock(void)
{
    struct timeval tv;
    struct tm tm;
    char time_str[32];

    if (gettimeofday(&tv, NULL) < 0) {
        return;
    }
    time_t local_time = tv.tv_sec + TIMEZONE_OFFSET_SEC;
    localtime_r(&local_time, &tm);

    snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d\n%02d/%02d/%04d",
             tm.tm_hour, tm.tm_min, tm.tm_sec,
             tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);

    if (time_label != NULL) {
        lv_label_set_text(time_label, time_str);
    }
}

/**
 * @brief Hàm callback của LVGL Timer. Cứ sau mỗi 5000ms tự động trượt sang màn hình kế tiếp.
 */
static void screen_switch_timer_cb(lv_timer_t * timer)
{
    (void) timer; 

    // Tính toán index của màn hình tiếp theo
    uint8_t next_screen_idx = (current_screen_idx + 1) % TOTAL_SCREENS;

    // Kích hoạt hiệu ứng trượt sang trái (MOVE_LEFT) từ màn hình cũ sang màn hình mới
    lv_screen_load_anim(screens[next_screen_idx], LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 0, false);

    // Cập nhật lại trạng thái màn hình hiện tại
    current_screen_idx = next_screen_idx;
}

int display_init(void)
{
    const struct device *display_dev;

    display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    if (!device_is_ready(display_dev)) {
        return -1; 
    }

    // Khởi tạo Style chữ dùng chung cho tất cả màn hình để tiết kiệm RAM
    lv_style_init(&text_style);
    lv_style_set_text_font(&text_style, &lv_font_montserrat_14); 
    lv_style_set_text_color(&text_style, lv_color_black());
    lv_style_set_text_align(&text_style, LV_TEXT_ALIGN_CENTER);

    // ==========================================
    // MÀN HÌNH 1 (Index 0): ĐỒNG HỒ HỆ THỐNG
    // ==========================================
    screens[0] = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screens[0], lv_color_white(), LV_PART_MAIN);
    
    time_label = lv_label_create(screens[0]);
    lv_obj_add_style(time_label, &text_style, LV_PART_MAIN);
    lv_label_set_text(time_label, "Waiting NTP...");
    lv_obj_align(time_label, LV_ALIGN_CENTER, 0, 0);

    // ==========================================
    // MÀN HÌNH 2 (Index 1): THÔNG ĐIỆP YÊU THƯƠNG 1
    // ==========================================
    screens[1] = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screens[1], lv_color_white(), LV_PART_MAIN);
    
    msg_label_1 = lv_label_create(screens[1]);
    lv_obj_add_style(msg_label_1, &text_style, LV_PART_MAIN);
    lv_label_set_text(msg_label_1, "Hello\neveryone\n!!!!!!");
    lv_obj_align(msg_label_1, LV_ALIGN_CENTER, 0, 0);

    // ==========================================
    // MÀN HÌNH 3 (Index 2): THÔNG ĐIỆP 2 (MỚI THÊM)
    // ==========================================
    screens[2] = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screens[2], lv_color_white(), LV_PART_MAIN);
    
    msg_label_2 = lv_label_create(screens[2]);
    lv_obj_add_style(msg_label_2, &text_style, LV_PART_MAIN);
    lv_label_set_text(msg_label_2, "This demo\nruns on\nZephyr OS");
    lv_obj_align(msg_label_2, LV_ALIGN_CENTER, 0, 0);

    // ==========================================
    // MÀN HÌNH 4 (Index 3): THÔNG ĐIỆP 3 (MỚI THÊM)
    // ==========================================
    screens[3] = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screens[3], lv_color_white(), LV_PART_MAIN);
    
    msg_label_3 = lv_label_create(screens[3]);
    lv_obj_add_style(msg_label_3, &text_style, LV_PART_MAIN);
    lv_label_set_text(msg_label_3, "Bye Bye\nSee you\nnext time!");
    lv_obj_align(msg_label_3, LV_ALIGN_CENTER, 0, 0);

    // Load màn hình đầu tiên (Màn hình Đồng hồ) làm mặc định lúc khởi động
    current_screen_idx = 0;
    lv_screen_load(screens[current_screen_idx]);

    // =================================================================
    // KHỞI TẠO LVGL TIMER: Gọi hàm callback định kỳ mỗi 5 giây (5000ms)
    // =================================================================
    screen_timer = lv_timer_create(screen_switch_timer_cb, 5000, NULL);
    if (screen_timer == NULL) {
        return -1; 
    }

    display_blanking_off(display_dev);
    return 0;
}

void display_task_handler(void)
{
    // Chỉ cập nhật đồng hồ khi màn hình Đồng hồ (Index 0) đang hiển thị thực tế
    if (wifi_connected && current_screen_idx == 0) {
        update_lvgl_clock();
    }

    // Thực thi hệ thống quản lý timer và sự kiện đồ họa của LVGL
    lv_timer_handler();
}