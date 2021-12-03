#include "file_manager.h"
#include "file_manager_gui.h"
#include "sys/app_contorller.h"
#include "network.h"
#include "common.h"
#include "ESP32FtpServer.h"

#define FILE_MANAGER_REFLUSH_INTERVAL 5000UL // 配置界面重新刷新时间(5s)

FtpServer ftpSrv; // 定义FTP服务端
bool connected = false;
const char *ftpuser = "Holo";
const char *ftppass = "12345678";
unsigned long last_reflush_time;

void file_maneger_init(void)
{
    last_reflush_time = millis();
    connected = false;

    file_maneger_gui_init();
    // 初始化运行时参数
    // 预显示
    display_file_manager(
        "File Manager",
        WiFi.softAPIP().toString().c_str(),
        "0000",
        "Wait connect ....",
        LV_SCR_LOAD_ANIM_NONE);

    SD.begin();
    ftpSrv.begin(ftpuser, ftppass);
}

void file_maneger_process(AppController *sys,
                          const Imu_Action *action)
{
    if (RETURN == action->active)
    {
        sys->app_exit();
        return;
    }

    // 启动STA/AP
    if (!connected)
    {
        sys->req_event(&file_manager_app, APP_EVENT_WIFI_CONN, 0);
        sys->req_event(&file_manager_app, APP_EVENT_WIFI_AP, 0);
        connected = true;
    }

    // 定时刷新IP地址
    if (millis() - last_reflush_time > FILE_MANAGER_REFLUSH_INTERVAL)
    {

        if (WiFi.softAPgetStationNum() == 0)
        {
            display_file_manager(
                "File Manager",
                WiFi.localIP().toString().c_str(),
                "21",
                "Connect succ",
                LV_SCR_LOAD_ANIM_NONE);
        }
        else
        {
            display_file_manager(
                "File Manager",
                WiFi.softAPIP().toString().c_str(),
                "21",
                "Connect succ",
                LV_SCR_LOAD_ANIM_NONE);
        }
        last_reflush_time = millis();
    }

    // 需要放在循环中
    ftpSrv.handleFTP();

    sys->req_event(&file_manager_app, APP_EVENT_WIFI_ALIVE, 0);
}

void file_maneger_exit_callback(void)
{
    connected = NULL;
    file_manager_gui_del();
    last_reflush_time = NULL;
}

void file_maneger_event_notification(APP_EVENT_TYPE type, int event_id)
{
    switch (type)
    {
    case APP_EVENT_WIFI_CONN:
    {
    }
    break;
    case APP_EVENT_WIFI_AP:
    {
    }
    break;
    case APP_EVENT_WIFI_ALIVE:
    {
        // wifi心跳维持的响应 可以不做任何处理
    }
    break;
    default:
        break;
    }
}

APP_OBJ file_manager_app = {"文件管理", &app_file_manager, "", file_maneger_init,
                            file_maneger_process, file_maneger_exit_callback,
                            file_maneger_event_notification};