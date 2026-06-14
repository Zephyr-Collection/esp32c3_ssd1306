#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <lvgl.h>

int main(void)
{
    const struct device *display_dev;
    lv_obj_t *hello_label;

    // 1. Kiểm tra phần cứng màn hình đã sẵn sàng chưa
    display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    if (!device_is_ready(display_dev)) {
        return 0; 
    }

    // 2. Ép màn hình nền hoạt động ở chế độ Đơn Sắc (Xóa bóng ma/nhiễu nền)
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_white(), LV_PART_MAIN);

    // 3. Tạo một Style riêng cho Chữ để ép Font và Màu hiển thị
    static lv_style_t text_style;
    lv_style_init(&text_style);
    
    // Sử dụng font chữ Montserrat 14 đã bật trong prj.conf
    lv_style_set_text_font(&text_style, &lv_font_montserrat_14); 
    
    // Đặt màu chữ là Màu Đen (tương phản với nền Trắng)
    lv_style_set_text_color(&text_style, lv_color_black());

    // 4. Tạo nhãn chữ (Label)
    hello_label = lv_label_create(lv_screen_active());
    
    // Áp dụng Style đơn sắc vừa tạo vào Label này
    lv_obj_add_style(hello_label, &text_style, LV_PART_MAIN);
    
    // Đặt nội dung chữ
    lv_label_set_text(hello_label, "Hello World!");
    
    // Căn chữ nằm chính giữa màn hình
    lv_obj_align(hello_label, LV_ALIGN_CENTER, 0, 0);

    // 5. Bật màn hình
    display_blanking_off(display_dev);

    // 6. Vòng lặp cập nhật giao diện
    while (1) {
        lv_timer_handler(); 
        k_sleep(K_MSEC(10)); 
    }
}