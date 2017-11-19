// this library just used in 1 pin by 1 DS18B20 situation!

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

class MLDS18B20 {
 public:
  MLDS18B20(uint8_t DQPIN);			
  uint8_t CommandReset(void);

 private:
 	void TxReset(void);
 	uint8_t RxResult(void);
 	void ThroughRx(void);
 	
 	uint8_t dqpin;
};
