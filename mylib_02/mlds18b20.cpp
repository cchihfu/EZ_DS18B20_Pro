#include "mlds18b20.h"

MLDs18b20::MLDs18b20(uint8_t DQ_PIN) {
	//雖然外部都叫翡翠，但可能內部有另外的編號如茶葉001
	//或是內部以A或F等代號作訊息傳達，要於此說明，通常用小寫
	//前面再加"_"底線，以區別，就知道此為外部給予之函數值
	//建構元：外部連接內部的設定
	//將此變數儲存於 private 變數以供其他類別內的函式使用
	_g_dq_pin = DQ_PIN;

	//將此 pin 腳初始化成輸出狀態
	pinMode(_g_dq_pin, INPUT);
}

//:: 符號。這代表此函式是屬於MLDs18b20類別的成員函式

// 函式功能說明
// 1.檢測pin腳是否連接正常（未
// 2.檢測DS18B20是否能正常回應
// 以上狀況，若ok則傳回1，異常就傳回0
uint8_t MLDs18b20::reset(void)
{
	//檢測01：接線狀態
	if(!TestConnect()) {return 0;}

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

uint8_t MLDs18b20::TestConnect(void)
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

