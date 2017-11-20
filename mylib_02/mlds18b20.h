/*mlds18b20.h - Library for 1 pin connect 1 DS18B20.
  Created by Billy Cheng,November 20, 2017.
  Released into the public domain
*/

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

//ML是MinimaList 極簡的縮寫
//Google命名規則：類型名稱的每個單詞首字母均大寫, 不包含底線
class MLDs18b20 {
public:
	//DQ_PIN 要用大寫，等同於大家都這麼叫（外部可調用）
	//如人人都知道要點翡翠（全省清心統一都有）
	MLDs18b20(uint8_t DQ_PIN);

	//為了無縫接軌，直接使用大品牌Onewire的函式名稱
	uint8_t reset(void);

private:
	//內部函式，外部無法調用
	uint8_t TestConnect(void);

	//內部變數，只有在此類別中可用
	//於此之義，我們都在一條線上
	uint8_t _g_dq_pin;
};
