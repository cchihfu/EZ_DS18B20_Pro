/*SimpleDs18b20.h - Library for 1 pin connect 1 DS18B20.
  Created by Billy Cheng,November 20, 2017.
  Released into the public domain
*/
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation

//#ifndef SIMPLEDS18B20_h
//#define SIMPLEDS18B20_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class SimpleDs18b20 {
public:
	SimpleDs18b20(uint8_t DQ_PIN);

	float GetTemperature(void);

private:
	float Caculate_Temperature(void);

	uint8_t FoundDataWarehouse(void);

	void ShelveData(void);

	uint8_t CheckSratchpadCRC(void);

	uint8_t ReCalculateCRC(uint8_t old_CRC,uint8_t input_byte);

	uint8_t ReadByte(void);

	uint8_t ReadSlot(void);

	void SendCommand(uint8_t instruction);

	void WriteSolt(uint8_t order_bit);

	uint8_t Initialize(void);

	uint8_t TestConnect(void);

	void TxReset(void);

	uint8_t RxResult(void);

	void ThroughRx(void);

	uint8_t _g_dq_pin;

};