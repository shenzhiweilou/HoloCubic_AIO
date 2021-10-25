#include "file_manager_gui.h"
#include "lvgl.h"

lv_obj_t *file_manager_scr = NULL;

lv_obj_t *local_ip_label;
lv_obj_t *ap_ip_label;
lv_obj_t *domain_label;
lv_obj_t *title_label;

static lv_style_t default_style;
static lv_style_t label_style5;

LV_FONT_DECLARE(lv_font_montserrat_24);

void file_maneger_gui_init(void)
{
    file_manager_scr = NULL;
    local_ip_label = NULL;
    ap_ip_label = NULL;
    domain_label = NULL;
    title_label = NULL;

    lv_style_init(&default_style);
    lv_style_set_bg_color(&default_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_bg_color(&default_style, LV_STATE_PRESSED, LV_COLOR_GRAY);
    lv_style_set_bg_color(&default_style, LV_STATE_FOCUSED, LV_COLOR_BLACK);
    lv_style_set_bg_color(&default_style, LV_STATE_FOCUSED | LV_STATE_PRESSED, lv_color_hex(0xf88));
    // 本地的ip地址
    file_manager_scr = lv_obj_create(NULL, NULL);
    lv_obj_add_style(file_manager_scr, LV_BTN_PART_MAIN, &default_style);
    lv_style_init(&label_style5);
    lv_style_set_text_opa(&label_style5, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_text_color(&label_style5, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_text_font(&label_style5, LV_STATE_DEFAULT, &lv_font_montserrat_24);
}

void display_file_manager_init(void)
{
    lv_obj_t *act_obj = lv_scr_act(); // 获取当前活动页
    if (act_obj == file_manager_scr)
        return;
    if (NULL != local_ip_label)
    {
        lv_obj_clean(local_ip_label);
        lv_obj_clean(ap_ip_label);
        lv_obj_clean(domain_label);
        lv_obj_clean(title_label);
    }
    lv_obj_clean(act_obj); // 清空此前页面
    local_ip_label = lv_label_create(file_manager_scr, NULL);
    ap_ip_label = lv_label_create(file_manager_scr, NULL);
    domain_label = lv_label_create(file_manager_scr, NULL);
    title_label = lv_label_create(file_manager_scr, NULL);
}

void display_file_manager(const char *title, const char *domain,
                     const char *info, const char *ap_ip,
                     lv_scr_load_anim_t anim_type)
{
    display_file_manager_init();

    lv_obj_add_style(title_label, LV_LABEL_PART_MAIN, &label_style5);
    lv_label_set_text(title_label, title);
    lv_obj_align(title_label, NULL, LV_ALIGN_OUT_BOTTOM_MID, 0, -220);

    lv_obj_add_style(domain_label, LV_LABEL_PART_MAIN, &label_style5);
    lv_label_set_text(domain_label, domain);
    lv_obj_align(domain_label, NULL, LV_ALIGN_OUT_BOTTOM_LEFT, 5, -120);

    lv_obj_add_style(local_ip_label, LV_LABEL_PART_MAIN, &label_style5);
    lv_label_set_text(local_ip_label, info);
    lv_obj_align(local_ip_label, NULL, LV_ALIGN_OUT_BOTTOM_LEFT, 5, -50);

    lv_obj_add_style(ap_ip_label, LV_LABEL_PART_MAIN, &label_style5);
    lv_label_set_text(ap_ip_label, ap_ip);
    lv_obj_align(ap_ip_label, NULL, LV_ALIGN_OUT_BOTTOM_LEFT, 5, -80);

    // lv_scr_load_anim(main_scr, anim_type, 300, 300, false);
    lv_scr_load(file_manager_scr);
}

void file_manager_gui_del(void)
{

    if (NULL != local_ip_label)
    {
        lv_obj_clean(local_ip_label);
        lv_obj_clean(ap_ip_label);
        lv_obj_clean(domain_label);
        lv_obj_clean(title_label);
        local_ip_label = NULL;
        ap_ip_label = NULL;
        domain_label = NULL;
        title_label = NULL;
    }

    if (NULL != file_manager_scr)
    {
        lv_obj_clean(file_manager_scr); // 清空此前页面
        file_manager_scr = NULL;
    }
}