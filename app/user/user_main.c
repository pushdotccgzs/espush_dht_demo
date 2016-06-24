/******************************************************************************
			Ģ���ƶ��ҽ̳�
			�����������У�������룺https://item.taobao.com/item.htm?id=534025556644

	��ͥʵʱ��ʪ�ȴ����������������仯���ƶ��ƿء�
*******************************************************************************/

#include <osapi.h>
#include <os_type.h>
#include <at_custom.h>		//�м���ذ��� at_custom.h
#include "driver/dht.h"		//DHT11��ͷ�ļ����ǵð���
#include "driver/espush.h"	//Ģ����ƽ̨ͷ�ļ�

//ȡ����ʪ�����ݣ��˴�ֻʹ�����¶�����
void ICACHE_FLASH_ATTR push_temperature(void* param) {
    char buf[32] = { 0 };
    //��ȡDHT���ݣ�����0�����ǿ�ƶ�ȡ��ȡ1��ǿ��ͬ��DHT�������˴�����Ҫ��
    struct sensor_reading *dht = readDHT(0);
    //�����ȡDHT����ʧ�ܣ�����δ���ӵ�ƽ̨�������������������͡�
    if(!dht->success) return;
    if(espush_server_connect_status() != STATUS_CONNECTED) return;
    //�¶����ݳ�����100����Ϊ����SDK��os_sprintf����û��ʵ��float���������͵ĸ�ʽ�������Դ˴��򵥴���
    //�����ݴ���wd ��ǩ����ν��ǩ����������ǰ���ӵ� ���ŷָ����ַ�����Ŀǰ��֧�ֵ���ǩ
    os_sprintf(buf, "wd,%d", dht->temperature * 100);
    //���������͵�ƽ̨��
    espush_msg_plan(buf, os_strlen(buf), get_timestamp());
}

//��Ƶ������ʼ����������
void ICACHE_FLASH_ATTR user_rf_pre_init(void){}

//��ڳ�ʼ��
void ICACHE_FLASH_ATTR user_init(void) {
    at_init();
    at_port_print("\r\nready\r\n");
    //��ʼ��DHT11��������������ÿ20�����һ������
    //�˴����ʵ�������������ڲ�Ҫ������ƽ̨����������ʱ��������
    DHTInit(SENSOR_DHT11, 20000);
    //���ö�ʱ����û60�봫��һ���¶�����
    static os_timer_t temperature_timer;
    os_timer_disarm(&temperature_timer);
    os_timer_setfn(&temperature_timer, push_temperature, NULL);
    os_timer_arm(&temperature_timer, 60000, 1);
    //ע�ᵽƽ̨�������APPID��APPKEy������д���Ӧ��ֵ��
    espush_register(15233, "da6ff36c678411e58c10002288fc6d2b", "", VER_SDK, NULL);
}
