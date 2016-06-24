/******************************************************************************
			蘑菇云独家教程
			开发板热卖中，点击进入：https://item.taobao.com/item.htm?id=534025556644

	家庭实时温湿度传感器，把握天气变化，云端掌控。
*******************************************************************************/

#include <osapi.h>
#include <os_type.h>
#include <at_custom.h>		//切记务必包含 at_custom.h
#include "driver/dht.h"		//DHT11的头文件，记得包含
#include "driver/espush.h"	//蘑菇云平台头文件

//取出温湿度数据，此处只使用了温度数据
void ICACHE_FLASH_ATTR push_temperature(void* param) {
    char buf[32] = { 0 };
    //读取DHT数据，参数0代表非强制读取，取1则强制同步DHT读数，此处不需要。
    struct sensor_reading *dht = readDHT(0);
    //如果读取DHT数据失败，或者未连接到平台，都放弃本次数据推送。
    if(!dht->success) return;
    if(espush_server_connect_status() != STATUS_CONNECTED) return;
    //温度数据乘以了100，因为乐鑫SDK的os_sprintf函数没有实现float浮点数类型的格式化，所以此处简单处理。
    //将数据打上wd 标签，所谓标签即是在数居前增加的 逗号分隔的字符串，目前仅支持单标签
    os_sprintf(buf, "wd,%d", dht->temperature * 100);
    //将数据推送到平台。
    espush_msg_plan(buf, os_strlen(buf), get_timestamp());
}

//射频参数初始化，请留空
void ICACHE_FLASH_ATTR user_rf_pre_init(void){}

//入口初始化
void ICACHE_FLASH_ATTR user_init(void) {
    at_init();
    at_port_print("\r\nready\r\n");
    //初始化DHT11传感器，并配置每20秒更新一下数据
    //此处可适当调整，间隔周期不要高于向平台的数据推送时间间隔即可
    DHTInit(SENSOR_DHT11, 20000);
    //配置定时器，没60秒传输一次温度数据
    static os_timer_t temperature_timer;
    os_timer_disarm(&temperature_timer);
    os_timer_setfn(&temperature_timer, push_temperature, NULL);
    os_timer_arm(&temperature_timer, 60000, 1);
    //注册到平台，下面的APPID与APPKEy，请填写你对应的值。
    espush_register(15233, "da6ff36c678411e58c10002288fc6d2b", "", VER_SDK, NULL);
}
