#include "Mlds18b20.h"

MLDS18B20::MLDS18B20(uint8_t DQ_PIN) {
	_g_dq_pin = DQ_PIN;

	//設定Pin腳的初始狀態
	pinMode(_g_dq_pin, INPUT);
}

// 函式功能說明
// 1.檢測pin腳是否連接正常（未
// 2.檢測DS18B20是否能正常回應
// 以上狀況，若ok則傳回1，異常就傳回0
uint8_t MLDS18B20::reset(void)
{
	//檢測01：接線狀態
	if(!TestConnect()){return 0;} 

	//檢測02:DS18B20狀態
	noInterrupts();
	//Tx階段：Step 1.主機發送重置脈沖
	pinMode(_g_dq_pin, OUTPUT);
	//Tx階段：Step 2.主機拉低電位
	digitalWrite(_g_dq_pin, LOW);
	interrupts();

	//Tx階段：Step 3.主機持續於低電位
	delayMicroseconds(480);

	//Tx階段：Step 4.主機釋放電位控制，轉為輸入狀態
	noInterrupts();
	pinMode(_g_dq_pin, INPUT);

	//Rx階段：Step 5.延時並讀取DS18B20回應電位值
	delayMicroseconds(70);
	uint8_t f=!digitalRead(_g_dq_pin);
	interrupts();

	//Rx階段：Step 6.延時並讓其超過Rx的480us時間
	delayMicroseconds(410);
	return f;
}

uint8_t MLDS18B20::TestConnect(void)
 {
	//防未正常接腳：arduino接到DQ_pin之線路異常
	uint8_t retries = 60;
	//先拉高電位（轉為讀取狀態）
	noInterrupts();
	pinMode(_g_dq_pin, INPUT);
	interrupts();
	//先觀察線路的狀態，時間約240us每隔4秒查一次
	//若是正常線路應處於高電位（idle state）
	while(!digitalRead(_g_dq_pin))
	{
		retries--;
		if(retries == 0) {return 0;}
		delayMicroseconds(4);
	}
	return 1;
	
}

