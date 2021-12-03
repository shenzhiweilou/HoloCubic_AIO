#include "file_manager.h"
#include "file_manager_gui.h"
#include "sys/app_contorller.h"
#include "network.h"
#include "common.h"
#include "ESP32FtpServer.h"

#define FILE_MANAGER_REFLUSH_INTERVAL 2000UL // 配置界面重新刷新时间(2s)

FtpServer ftpSrv;               // 定义FTP服务端
bool connected = false;
const char *ftpuser = "Holo";
const char *ftppass = "12345678";

void file_maneger_init(void)
{
    
    file_maneger_gui_init();
    // 初始化运行时参数
    // 预显示
    display_file_manager(
        "File Manager",
        WiFi.softAPIP().toString().c_str(),
        "21",
        "Wait connect ....",
        LV_SCR_LOAD_ANIM_NONE);
    //SPI.begin(14, 26, 13, 15); //SCK, MISO, MOSI,SS
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

    if(!connected){
        sys->req_event(&file_manager_app, APP_EVENT_WIFI_CONN, 0);
        sys->req_event(&file_manager_app, APP_EVENT_WIFI_AP, 0);
        connected = true;
    }
    ftpSrv.handleFTP();
    
    sys->req_event(&file_manager_app, APP_EVENT_WIFI_ALIVE, 0);
}

void file_maneger_exit_callback(void)
{
    connected = false;
    file_manager_gui_del();
}

void file_maneger_event_notification(APP_EVENT_TYPE type, int event_id)
{
    switch (type)
    {
    case APP_EVENT_WIFI_CONN:
    {
        Serial.print(F("APP_EVENT_WIFI_AP enable\n"));
        display_file_manager(
            "File Manager",
            WiFi.localIP().toString().c_str(),
            "21",
            "Connect succ",
            LV_SCR_LOAD_ANIM_NONE);
        // ftpSrv.begin("esp32", "esp32");
    }
    break;
    case APP_EVENT_WIFI_AP:
    {
        Serial.print(F("APP_EVENT_WIFI_AP enable\n"));
        display_file_manager(
            "File Manager",
            WiFi.localIP().toString().c_str(),
            "21",
            "Connect succ",
            LV_SCR_LOAD_ANIM_NONE);
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