#ifndef MAIN_H
#define MAIN_H
//开发板基础库
#include <Arduino.h>
#include "rgb_lcd_port.h"    // Header for Waveshare RGB LCD driver
#include "gt911.h"           // Header for touch screen operations (GT911)

#include "lvgl_port.h"       // LVGL porting functions for integration
#include "misc/lv_log.h"



//下载的库
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

//lvgl的库
#include <ui.h>
#include <screens.h>
#include <vars.h>

//自己写的库
#include <mqtt.h>
#include <myloop.h>
#include <js.h>





/*
 若要使用LVGL内置示例和演示，请取消下方对应的include注释
*/
// #include <demos/lv_demos.h>
// #include <examples/lv_examples.h>

#endif /*MAIN_H*/