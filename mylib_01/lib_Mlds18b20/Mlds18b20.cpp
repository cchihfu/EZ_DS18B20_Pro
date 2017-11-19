#include "Mlds18b20.h"

MLDS18B20::MLDS18B20(uint8_t DQPIN) {
  dqpin = DQPIN;

  //define pin modes
  pinMode(dqpin, INPUT);
}

uint8_t MLDS18B20::CommandReset(void)
{
	TxReset();
	noInterrupts();
	uint8_t f=RxResult();
	interrupts();
	ThroughRx();
	return f;
}

void MLDS18B20::TxReset(void) {
	uint16_t time_keep_low = 480;
	noInterrupts();
	//Tx階段：Step 1.主機發送重置脈沖
	pinMode(dqpin, OUTPUT);
	//Tx階段：Step 2.主機拉低電位
	digitalWrite(dqpin, LOW);
	interrupts();
	//Tx階段：Step 3.主機持續於低電位
	delayMicroseconds(time_keep_low);
	//Tx階段：Step 4.主機釋放電位控制，轉為輸入狀態
	pinMode(dqpin, INPUT);
}

uint8_t MLDS18B20::RxResult(void) {
	uint8_t time_wait_read=70;
	//Rx階段：Step 5.延時並讀取DS18B20回應電位值
	delayMicroseconds(time_wait_read);
	return !digitalRead(dqpin);
}

void MLDS18B20::ThroughRx(void) {
	//Rx階段：Step 6.延時並讓其超過Rx的480us時間
	
	uint16_t time_through =410;
	delayMicroseconds(time_through);
}
