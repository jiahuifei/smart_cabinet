#include <main.h>

// // 超时时间（单位：毫秒），例如 30 秒
// #define INACTIVITY_TIMEOUT_MS 10000
// static lv_timer_t* inactivity_timer = NULL;
// static uint32_t last_activity_time = 0;
// static void timeout_callback(lv_timer_t* timer);

//lv_obj_t *tabview = NULL; // 实际定义位置（在初始化函数中创建）

int borrowing_status = 0;//0状态是领用，1状态是归还，2状态是维护

void super_loop()
{
  //如果在home页则检测按键，不在home页则计时清除状态
  if (lv_tabview_get_tab_act(objects.tabview) == 0){
    if(lv_obj_has_state(objects.home_home_use, LV_STATE_PRESSED)){
      borrowing_status = 0;
      lv_tabview_set_act(objects.tabview, 1, LV_ANIM_ON);
      lv_obj_invalidate(lv_scr_act());  
    }
    if(lv_obj_has_state(objects.home_home_return, LV_STATE_PRESSED)){
      borrowing_status = 1;
      lv_tabview_set_act(objects.tabview, 1, LV_ANIM_ON);
      lv_obj_invalidate(lv_scr_act());
    }
    if(lv_obj_has_state(objects.home_home_maintain, LV_STATE_PRESSED)){
      borrowing_status = 2;
      lv_tabview_set_act(objects.tabview, 1, LV_ANIM_ON);
      lv_obj_invalidate(lv_scr_act());
    }
  }
  else{//无操作30s回到home页
    // if (lv_tick_elaps(last_activity_time) > INACTIVITY_TIMEOUT_MS) {
    //     lv_tabview_set_act(tabview, 0, LV_ANIM_OFF);//切换至home页
    //     // 重置计时
    //     last_activity_time = lv_tick_get();
    // }
    switch(lv_tabview_get_tab_act(objects.tabview)){
      case 1 :
        if(lv_obj_has_state(objects.home_idcheck_ok, LV_STATE_PRESSED)){
          
          //mqtt发送输入框信息
          const char *idcheck_text = lv_textarea_get_text(objects.home_idcheck_textarea);
          //mqtt_publish((char*)idcheck_text);
          js_publish_id((char*)idcheck_text);
          //等待mqtt返回信息，创建弹窗

          lv_tabview_set_act(objects.tabview, 2, LV_ANIM_ON);
          lv_obj_invalidate(lv_scr_act());
          //清空输入框
          lv_textarea_set_text(objects.home_idcheck_textarea, "");
        }
        break;
      case 2 :
        if(lv_obj_has_state(objects.home_select_ok, LV_STATE_PRESSED)){
          lv_tabview_set_act(objects.tabview, 3, LV_ANIM_ON);
          lv_obj_invalidate(lv_scr_act());
        }
        break;
      case 3 :

        break;
    }

  }



}

//跟据mqtt返回用户按键信息点亮或熄灭home_select页的btn，用户通过一个json文件来控制四个按键
void update_select_btn(){
  //读取json文件
  DynamicJsonDocument doc(1024);
  doc["btn1"] = 0;
  doc["btn2"] = 0;
  doc["btn3"] = 0;
  doc["btn4"] = 0;
  doc["btn1"] = 1;
  doc["btn2"] = 1;
  doc["btn3"] = 1;
  doc["btn4"] = 1;
  //更新btn状态
  if(doc["btn1"] == 1){
    lv_obj_add_state(objects.home_select_btn1, LV_STATE_CHECKED);
  }
  else{
    lv_obj_clear_state(objects.home_select_btn1, LV_STATE_CHECKED);
  }
  if(doc["btn2"] == 1){
    lv_obj_add_state(objects.home_select_btn2, LV_STATE_CHECKED);
  }
  else{
    lv_obj_clear_state(objects.home_select_btn2, LV_STATE_CHECKED);
  }
  if(doc["btn3"] == 1){
    lv_obj_add_state(objects.home_select_btn3, LV_STATE_CHECKED);
  }
  else{
    lv_obj_clear_state(objects.home_select_btn3, LV_STATE_CHECKED);
  }
  if(doc["btn4"] == 1){
    lv_obj_add_state(objects.home_select_btn0, LV_STATE_CHECKED);
  }
  else{
    lv_obj_clear_state(objects.home_select_btn0, LV_STATE_CHECKED);
  }
}


