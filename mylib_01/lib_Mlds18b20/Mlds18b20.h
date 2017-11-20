/*Mlds18b20.h - Library for 1 pin connect 1 DS18B20.
  Created by Billy Cheng,November 20, 2017.
  Released into the public domain
*/

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
 
//ML是MinimaList 極簡的縮寫
class MLDS18B20 {
public:
	MLDS18B20(uint8_t DQ_PIN);

	//為了無縫接軌，直接使用大品牌Onewire的函式名稱
	// Perform a 1-Wire reset cycle. Returns 1 if a device responds
	// with a presence pulse.  Returns 0 if there is no device or the
	// bus is shorted or otherwise held low for more than 250uS
	uint8_t reset(void);

private:
	uint8_t TestConnect(void);
	
	uint8_t _g_dq_pin;
};
