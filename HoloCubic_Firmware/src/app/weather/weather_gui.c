#include "weather_gui.h"
#include "weather_image.h"

#include "lvgl.h"

LV_FONT_DECLARE(lv_font_ibmplex_115);
LV_FONT_DECLARE(lv_font_ibmplex_64);
LV_FONT_DECLARE(weather_font_20);
static lv_style_t default_style;
static lv_style_t chFont_style;
static lv_style_t numberSmall_style;
static lv_style_t numberBig_style;
static lv_style_t btn_style;
static lv_style_t bar_style;

static lv_obj_t *scr_1 = NULL;
static lv_obj_t *scr_2 = NULL;
static lv_obj_t *chart, *titleLabel;

static lv_obj_t *weatherImg = NULL;
static lv_obj_t *cityLabel = NULL;
static lv_obj_t *btn = NULL, *btnLabel = NULL;
static lv_obj_t *txtLabel = NULL;
static lv_obj_t *clockLabel_1 = NULL, *clockLabel_2 = NULL;
static lv_obj_t *dateLabel = NULL;
static lv_obj_t *tempImg = NULL, *tempBar = NULL, *tempLabel = NULL;
static lv_obj_t *humiImg = NULL, *humiBar = NULL, *humiLabel = NULL;

static lv_obj_t *tip1 = NULL, *tip2 = NULL, *tip3 = NULL;

static lv_chart_series_t *ser1, *ser2;

// 天气图标路径的映射关系(已修改为从SD卡读取)
const void *weaImage_map[] = {"S:/weather/1.bin","S:/weather/2.bin","S:/weather/3.bin","S:/weather/4.bin","S:/weather/5.bin","S:/weather/6.bin",
                            "S:/weather/7.bin","S:/weather/8.bin","S:/weather/9.bin"};

static const char weekDayCh[7][4] = {"日", "一", "二", "三", "四", "五", "六"};
static const char airQualityCh[6][10] = {"优", "良", "轻度", "中度", "重度", "严重"};

void weather_gui_init(void)
{
    lv_style_init(&default_style);
    lv_style_set_bg_color(&default_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_bg_color(&default_style, LV_STATE_PRESSED, LV_COLOR_GRAY);
    lv_style_set_bg_color(&default_style, LV_STATE_FOCUSED, LV_COLOR_BLACK);
    lv_style_set_bg_color(&default_style, LV_STATE_FOCUSED | LV_STATE_PRESSED, lv_color_hex(0xf88));

    lv_style_init(&chFont_style);
    lv_style_set_text_opa(&chFont_style, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_text_color(&chFont_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_text_font(&chFont_style, LV_STATE_DEFAULT, &weather_font_20);
    lv_style_init(&numberSmall_style);
    lv_style_set_text_opa(&numberSmall_style, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_text_color(&numberSmall_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_text_font(&numberSmall_style, LV_STATE_DEFAULT, &lv_font_ibmplex_64);
    lv_style_init(&numberBig_style);
    lv_style_set_text_opa(&numberBig_style, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_text_color(&numberBig_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_text_font(&numberBig_style, LV_STATE_DEFAULT, &lv_font_ibmplex_115);
    lv_style_init(&btn_style);
    lv_style_set_border_width(&btn_style, LV_STATE_DEFAULT, 0);
    lv_style_init(&bar_style);
    lv_style_set_bg_color(&bar_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_border_width(&bar_style, LV_STATE_DEFAULT, 2);
    lv_style_set_border_color(&bar_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_pad_top(&bar_style, LV_STATE_DEFAULT, 1); //指示器到背景四周的距离
    lv_style_set_pad_bottom(&bar_style, LV_STATE_DEFAULT, 1);
    lv_style_set_pad_left(&bar_style, LV_STATE_DEFAULT, 1);
    lv_style_set_pad_right(&bar_style, LV_STATE_DEFAULT, 1);
}

void display_curve_init(lv_scr_load_anim_t anim_type)
{
    lv_obj_t *act_obj = lv_scr_act(); // 获取当前活动页
    if (act_obj == scr_2)
        return;
    weather_gui_del();
    lv_obj_clean(act_obj); // 清空此前页面

    scr_2 = lv_obj_create(NULL, NULL);
    lv_obj_add_style(scr_2, LV_BTN_PART_MAIN, &default_style);

    titleLabel = lv_label_create(scr_2, NULL);
    lv_obj_add_style(titleLabel, LV_LABEL_PART_MAIN, &chFont_style);
    lv_label_set_recolor(titleLabel, true);
    lv_label_set_text(titleLabel, "查看更多天气");

    chart = lv_chart_create(scr_2, NULL);
    lv_obj_set_size(chart, 220, 180);
    lv_chart_set_range(chart, -20, 40);//修改下限为-20
    lv_chart_set_point_count(chart, 7);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE); /*Show lines and points too*/
    ser1 = lv_chart_add_series(chart, LV_COLOR_RED);
    ser2 = lv_chart_add_series(chart, LV_COLOR_BLUE);
    lv_obj_set_style_local_pad_left(chart, LV_CHART_PART_BG, LV_STATE_DEFAULT, 40);
    lv_chart_set_y_tick_texts(chart, "40\n30\n20\n10\n0\n-10\n-20", 0, LV_CHART_AXIS_DRAW_LAST_TICK);//修改下限为-20
    
    // 绘制
    lv_obj_align(titleLabel, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);
    lv_obj_align(chart, NULL, LV_ALIGN_CENTER, 0, 10);

    if (LV_SCR_LOAD_ANIM_NONE != anim_type)
    {
        lv_scr_load_anim(scr_2, anim_type, 300, 300, false);
    }
    else
    {
        lv_scr_load(scr_2);
    }
}

void display_curve(short maxT[], short minT[], lv_scr_load_anim_t anim_type)
{
    display_curve_init(anim_type);
    for (int Ti = 0; Ti < 7; ++Ti)
    {
        ser1->points[Ti] = maxT[Ti];
    }
    for (int Ti = 0; Ti < 7; ++Ti)
    {
        ser2->points[Ti] = minT[Ti];
    }
    lv_chart_refresh(chart);
}

void display_weather_init(lv_scr_load_anim_t anim_type)
{
    lv_obj_t *act_obj = lv_scr_act(); // 获取当前活动页
    if (act_obj == scr_1)
        return;
    weather_gui_del();
    lv_obj_clean(act_obj); // 清空此前页面

    scr_1 = lv_obj_create(NULL, NULL);
    lv_obj_add_style(scr_1, LV_BTN_PART_MAIN, &default_style);

    weatherImg = lv_img_create(scr_1, NULL);
    lv_img_set_src(weatherImg, weaImage_map[0]);

    cityLabel = lv_label_create(scr_1, NULL);
    lv_obj_add_style(cityLabel, LV_LABEL_PART_MAIN, &chFont_style);
    lv_label_set_recolor(cityLabel, true);
    lv_label_set_text(cityLabel, "上海");

    btn = lv_btn_create(scr_1, NULL);
    lv_obj_add_style(btn, LV_BTN_PART_MAIN, &btn_style);
    lv_obj_set_pos(btn, 75, 15);
    lv_obj_set_size(btn, 50, 25);
    lv_obj_set_style_local_bg_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
    btnLabel = lv_label_create(btn, NULL);
    lv_obj_add_style(btnLabel, LV_LABEL_PART_MAIN, &chFont_style);
    lv_label_set_text(btnLabel, airQualityCh[0]);

    txtLabel = lv_label_create(scr_1, NULL);
    lv_obj_add_style(txtLabel, LV_LABEL_PART_MAIN, &chFont_style);
    // LV_LABEL_LONG_SROLL_CIRC 模式一旦设置 宽度恒定等于当前文本的长度，所以下面先设置以下长度
    lv_label_set_text(txtLabel, "最低气温12°C, ");
    lv_label_set_long_mode(txtLabel, LV_LABEL_LONG_SROLL_CIRC);
    lv_label_set_text_fmt(txtLabel, "最低气温%d°C, 最高气温%d°C, %s%d 级.   ", 15, 20, "西北风", 0);

    clockLabel_1 = lv_label_create(scr_1, NULL);
    lv_obj_add_style(clockLabel_1, LV_LABEL_PART_MAIN, &numberBig_style);
    lv_label_set_recolor(clockLabel_1, true);
    lv_label_set_text_fmt(clockLabel_1, "%02d#ffa500 %02d#", 00, 00);//改变默认显示
    clockLabel_2 = lv_label_create(scr_1, NULL);
    lv_obj_add_style(clockLabel_2, LV_LABEL_PART_MAIN, &numberSmall_style);
    lv_label_set_recolor(clockLabel_2, true);
    lv_label_set_text_fmt(clockLabel_2, "%02d", 00);

    dateLabel = lv_label_create(scr_1, NULL);
    lv_obj_add_style(dateLabel, LV_LABEL_PART_MAIN, &chFont_style);
    lv_label_set_text_fmt(dateLabel, "%2d月%2d日   周%s", 1, 1, weekDayCh[1]);//改变默认显示

    tempImg = lv_img_create(scr_1, NULL);
    lv_img_set_src(tempImg, &temp);
    lv_img_set_zoom(tempImg, 180);
    tempBar = lv_bar_create(scr_1, NULL);
    lv_obj_add_style(tempBar, LV_BAR_TYPE_NORMAL, &bar_style);
    lv_bar_set_range(tempBar, -20, 40);//修改下限
    lv_obj_set_size(tempBar, 60, 12);
    lv_obj_set_style_local_bg_color(tempBar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_RED);
    lv_bar_set_value(tempBar, 10, LV_ANIM_OFF);
    tempLabel = lv_label_create(scr_1, NULL);
    lv_obj_add_style(tempLabel, LV_LABEL_PART_MAIN, &chFont_style);
    lv_label_set_text_fmt(tempLabel, "%2d°C", 18);

    humiImg = lv_img_create(scr_1, NULL);
    lv_img_set_src(humiImg, &humi);
    lv_img_set_zoom(humiImg, 180);
    humiBar = lv_bar_create(scr_1, NULL);
    lv_obj_add_style(humiBar, LV_BAR_TYPE_NORMAL, &bar_style);
    lv_bar_set_range(humiBar, 0, 100);
    lv_obj_set_size(humiBar, 60, 12);
    lv_obj_set_style_local_bg_color(humiBar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    lv_bar_set_value(humiBar, 50, LV_ANIM_OFF);
    humiLabel = lv_label_create(scr_1, NULL);
    lv_obj_add_style(humiLabel, LV_LABEL_PART_MAIN, &chFont_style);
    //修复湿度固定显示
    lv_label_set_text_fmt(humiLabel, "%2d%%",50);

    //提示信息
    tip1 = lv_label_create(scr_1, NULL);
    lv_obj_add_style(tip1, LV_LABEL_PART_MAIN, &chFont_style);
    lv_label_set_text(tip1, "初始化中");
    lv_label_set_long_mode(tip1, LV_LABEL_LONG_SROLL_CIRC);
    lv_label_set_text(tip1, "暂时没有新消息  ");

    tip2 = lv_label_create(scr_1, NULL);
    lv_obj_add_style(tip2, LV_LABEL_PART_MAIN, &chFont_style);
    lv_label_set_text(tip2, "初始化中");
    lv_label_set_long_mode(tip2, LV_LABEL_LONG_SROLL_CIRC);
    lv_label_set_text(tip2, "暂时没有新消息  ");

    tip3 = lv_label_create(scr_1, NULL);
    lv_obj_add_style(tip3, LV_LABEL_PART_MAIN, &chFont_style);
    lv_label_set_text(tip3, "初始化中");
    lv_label_set_long_mode(tip3, LV_LABEL_LONG_SROLL);
    lv_label_set_recolor(tip3, true); 


    // 绘制图形
    lv_obj_align(weatherImg, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, -10);// 使天气图标更靠近角落
    lv_obj_align(cityLabel, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 15);
    lv_obj_align(txtLabel, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 50);
    lv_obj_align(tempImg, NULL, LV_ALIGN_IN_LEFT_MID, 10, 70);
    lv_obj_align(tempBar, NULL, LV_ALIGN_IN_LEFT_MID, 35, 70);
    lv_obj_align(tempLabel, NULL, LV_ALIGN_IN_LEFT_MID, 100, 70);
    lv_obj_align(humiImg, NULL, LV_ALIGN_IN_LEFT_MID, 0, 100);
    lv_obj_align(humiBar, NULL, LV_ALIGN_IN_LEFT_MID, 35, 100);
    lv_obj_align(humiLabel, NULL, LV_ALIGN_IN_LEFT_MID, 100, 100);
    // lv_obj_align(spaceImg, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10);

    lv_obj_align(tip1, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -8, -68);
    lv_obj_align(tip2, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -8, -38);
    lv_obj_align(tip3, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -8, -8);

    lv_obj_align(clockLabel_1, NULL, LV_ALIGN_IN_LEFT_MID, 0, 10);
    lv_obj_align(clockLabel_2, NULL, LV_ALIGN_IN_LEFT_MID, 165, 9);
    lv_obj_align(dateLabel, NULL, LV_ALIGN_IN_LEFT_MID, 10, 32);

    // if (LV_SCR_LOAD_ANIM_NONE != anim_type)
    // {
    //     lv_scr_load_anim(scr_1, anim_type, 300, 300, false);
    // }
    // else
    // {
        // lv_scr_load(scr_1);
    // }
}

void display_weather(struct Weather weaInfo, lv_scr_load_anim_t anim_type)
{
    display_weather_init(anim_type);

    lv_label_set_text(cityLabel, weaInfo.cityname);
    lv_label_set_text(btnLabel, airQualityCh[weaInfo.airQulity]);
    lv_label_set_text_fmt(txtLabel, "最低气温%d°C, 最高气温%d°C, %s%d 级.   ",
                          weaInfo.minTemp, weaInfo.maxTmep, weaInfo.windDir, weaInfo.windLevel);
    lv_bar_set_value(tempBar, weaInfo.temperature, LV_ANIM_OFF);
    lv_label_set_text_fmt(tempLabel, "%2d°C", weaInfo.temperature);
    lv_img_set_src(weatherImg, weaImage_map[weaInfo.weather_code]);

    //修复湿度固定显示
    lv_label_set_text_fmt(humiLabel, "%2d%%",weaInfo.humidity);
    lv_bar_set_value(humiBar, weaInfo.humidity, LV_ANIM_OFF);

    // // 绘制图形
    // lv_obj_align(weatherImg, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, 10);
    // lv_obj_align(cityLabel, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 15);
    // lv_obj_align(txtLabel, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 50);
    // lv_obj_align(tempImg, NULL, LV_ALIGN_IN_LEFT_MID, 10, 70);
    // lv_obj_align(tempBar, NULL, LV_ALIGN_IN_LEFT_MID, 35, 70);
    // lv_obj_align(tempLabel, NULL, LV_ALIGN_IN_LEFT_MID, 100, 70);
    // lv_obj_align(humiImg, NULL, LV_ALIGN_IN_LEFT_MID, 0, 100);
    // lv_obj_align(humiBar, NULL, LV_ALIGN_IN_LEFT_MID, 35, 100);
    // lv_obj_align(humiLabel, NULL, LV_ALIGN_IN_LEFT_MID, 100, 100);
    // lv_obj_align(spaceImg, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10);

    if (LV_SCR_LOAD_ANIM_NONE != anim_type)
    {
        lv_scr_load_anim(scr_1, anim_type, 300, 300, false);
    }
    else
    {
        lv_scr_load(scr_1);
    }
}

void display_time(struct TimeStr timeInfo, lv_scr_load_anim_t anim_type)
{
    display_weather_init(anim_type);
    lv_label_set_text_fmt(clockLabel_1, "%02d#ffa500 %02d#", timeInfo.hour, timeInfo.minute);
    lv_label_set_text_fmt(clockLabel_2, "%02d", timeInfo.second);
    lv_label_set_text_fmt(dateLabel, "%2d月%2d日   周%s", timeInfo.month, timeInfo.day,
                          weekDayCh[timeInfo.weekday]);

    // lv_obj_align(clockLabel_1, NULL, LV_ALIGN_IN_LEFT_MID, 0, 10);
    // lv_obj_align(clockLabel_2, NULL, LV_ALIGN_IN_LEFT_MID, 165, 9);
    // lv_obj_align(dateLabel, NULL, LV_ALIGN_IN_LEFT_MID, 10, 32);

    // if (LV_SCR_LOAD_ANIM_NONE != anim_type)
    // {
    //     lv_scr_load_anim(scr_1, anim_type, 300, 300, false);
    // }
    // else
    // {
    //     lv_scr_load(scr_1);
    // }
}

void display_tips(bool connected, struct Tips *tips)
{
    if (connected == true)
    {
        lv_label_set_text(tip3, "网络已连接");
    }else 
    {
        lv_label_set_text(tip3, "#ff0000 网络未连接#");
    }

    lv_label_set_text(tip1, tips->msg);
    lv_label_set_text(tip2, tips->date);
    
}

void weather_obj_del(void)
{
    if (weatherImg != NULL)
    {
        lv_obj_clean(weatherImg);
        lv_obj_clean(cityLabel);
        lv_obj_clean(btn);
        lv_obj_clean(btnLabel);
        lv_obj_clean(txtLabel);
        lv_obj_clean(clockLabel_1);
        lv_obj_clean(clockLabel_2);
        lv_obj_clean(dateLabel);
        lv_obj_clean(tempImg);
        lv_obj_clean(tempBar);
        lv_obj_clean(tempLabel);
        lv_obj_clean(humiImg);
        lv_obj_clean(humiBar);
        lv_obj_clean(humiLabel);
        weatherImg = NULL;
        cityLabel = NULL;
        btn = NULL;
        btnLabel = NULL;
        txtLabel = NULL;
        clockLabel_1 = NULL;
        clockLabel_2 = NULL;
        dateLabel = NULL;
        tempImg = NULL;
        tempBar = NULL;
        tempLabel = NULL;
        humiImg = NULL;
        humiBar = NULL;
        humiLabel = NULL;
    }
    if (chart != NULL)
    {
        lv_obj_clean(chart);
        lv_obj_clean(titleLabel);
        chart = NULL;
        titleLabel = NULL;
        ser1 = NULL;
        ser2 = NULL;
    }
}

void weather_gui_del(void)
{
    weather_obj_del();
    if (scr_1 != NULL)
    {
        lv_obj_clean(scr_1);
        scr_1 = NULL;
    }
    
    if (scr_2 != NULL)
    {
        lv_obj_clean(scr_2);
        scr_2 = NULL;
    }
}

int airQulityLevel(int q)
{
    if (q < 50)
    {
        return 0;
    }
    else if (q < 100)
    {
        return 1;
    }
    else if (q < 150)
    {
        return 2;
    }
    else if (q < 200)
    {
        return 3;
    }
    else if (q < 300)
    {
        return 4;
    }
    return 5;
}