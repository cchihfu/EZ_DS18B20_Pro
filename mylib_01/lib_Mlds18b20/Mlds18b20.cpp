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
	//Tx���q�GStep 1.�D���o�e���m�ߨR
	pinMode(dqpin, OUTPUT);
	//Tx���q�GStep 2.�D���ԧC�q��
	digitalWrite(dqpin, LOW);
	interrupts();
	//Tx���q�GStep 3.�D�������C�q��
	delayMicroseconds(time_keep_low);
	//Tx���q�GStep 4.�D������q�챱��A�ର��J���A
	pinMode(dqpin, INPUT);
}

uint8_t MLDS18B20::RxResult(void) {
	uint8_t time_wait_read=70;
	//Rx���q�GStep 5.���ɨ�Ū��DS18B20�^���q���
	delayMicroseconds(time_wait_read);
	return !digitalRead(dqpin);
}

void MLDS18B20::ThroughRx(void) {
	//Rx���q�GStep 6.���ɨ�����W�LRx��480us�ɶ�
	
	uint16_t time_through =410;
	delayMicroseconds(time_through);
}
