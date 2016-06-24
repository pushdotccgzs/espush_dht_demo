/******************************************************************************
			Ģ���ƶ��ҽ̳�
			�����������У�������룺https://item.taobao.com/item.htm?id=534025556644

	��ͥʵʱ��ʪ�ȴ����������������仯���ƶ��ƿء�
*******************************************************************************/

#include <osapi.h>
#include <at_custom.h>		//�м���ذ��� at_custom.h
#include <os_type.h>

#include "driver/dht.h"
#include "driver/espush.h"

//ȡ����ʪ�����ݣ��˴�ֻʹ�����¶�����
void ICACHE_FLASH_ATTR push_temperature(void* param)
{
    char buf[32] = { 0 };
    struct sensor_reading *dht = readDHT(0);
    if(!dht->success) {
        return;
    }
    if(espush_server_connect_status() != STATUS_CONNECTED) {
    	return;
    }

    //�¶����ݳ�����100����Ϊ����SDK��os_sprintf����û��ʵ��float���������͵ĸ�ʽ�������Դ˴��򵥴�����
    uint32 temperature = dht->temperature * 100;
    //�˴�ȡ�˵�ǰϵͳʱ�����ֻ������ƽ̨����Ч
    uint32 timestamp = get_timestamp();
    //�����ݴ���wd ��ǩ����ν��ǩ����������ǰ���ӵ� ���ŷָ����ַ�����Ŀǰ��֧�ֵ���ǩ
    os_sprintf(buf, "wd,%d", temperature);

    //���������͵�ƽ̨��
    espush_msg_plan(buf, os_strlen(buf), timestamp);
}

//��Ƶ������ʼ����������
void ICACHE_FLASH_ATTR user_rf_pre_init(void){}

//��ڳ�ʼ��
void ICACHE_FLASH_ATTR user_init(void) {
    at_init();
    at_port_print("\r\nready\r\n");

    DHTInit(SENSOR_DHT11, 5000);
    //���ö�ʱ����û60�봫��һ���¶�����
    static os_timer_t temperature_timer;
    os_timer_disarm(&temperature_timer);
    os_timer_setfn(&temperature_timer, push_temperature, NULL);
    os_timer_arm(&temperature_timer, 60000, 1);

    //ע�ᵽƽ̨�������APPID��APPKEy������д���Ӧ��ֵ��
    espush_register(15233, "da6ff36c678411e58c10002288fc6d2b", "", VER_SDK, NULL);
}