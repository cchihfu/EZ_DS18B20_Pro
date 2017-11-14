/**********************************************************************
 - 適用狀況：只能用於1台arduino連接1個DS18B20之狀態
 - 功能描述：可重置DS18B20及讀寫命令及取得溫度值
 - 詳細內容：不引用他人函數庫的情況下，讓Arduino與DS18B20於1對1的連接下，
 			能做資料讀寫之功能，順利取得溫度值並回饋訊息至串列埠螢幕中
 - 主機版本：Arduino UNO R3
 *********************************************************************/
const uint8_t  g_dq_pin =2;             //Arduino數位腳位2接到DS18B20，DQ為Data input/output的縮寫

//ROM命令
#define Skip_ROM 			0xcc	//用於1對1時，省略每次作ROM序號確認程序

//RAM命令
#define Convert_T			0x44	//啟動溫度轉換
#define Read_Scratchpad		0xbe	//讀取暫存器值，有9個字元


void setup() {
	Serial.begin(9600);
	Serial.println("<<Get The Temperature from DS18B20>>");
}

void loop() {
	Serial.print("DS18b20's Temperature ->");
	Serial.println(getTempC());
	delay(2000);
}

float getTempC()
{
	while(!CommandReset());      	//重啟成功，再進行動作

	SendCommand(Skip_ROM);		//主機下0xcc命令（1對1的省略ROM確認作業）
	SendCommand(Convert_T);		//0x44啟動溫度轉換
	//方法1：
	//由於位元精度差異，其轉換時間93.75/187.5/375/750ms等四種變化，
	//先以最大值作延時設定，可以保證時間的充足
	//delay(750);

	//方法2：讀忙控制：參考Datasheet當DS18B20於轉換溫度資料時
	//若主機對它作讀取slot的動作，它將會持續於低電位(0)
	//直到它轉換完畢後，就能得到1
	while(! CatchSlot());
	//實際上，會有點問題，就是要再delay一下，不然會有奇怪的增值出現
	//例如：MSB從1變成10001，但只要加點延時既可解決，若要保險時
	//將MSB的前4位皆強迫去除，使用(MSB&0x0f)
	delay(50);

	//再次重啟，因為要作通訊作業
	while(!CommandReset());       	  //重啟成功，再進行動作
	SendCommand(Skip_ROM);		      //主機下0xcc命令（1對1的省略ROM確認作業）
	SendCommand(Read_Scratchpad);	  //0xBE溫度暫存器中的訊息（共9個字元）
	//前兩個字元就是溫度的訊息
	uint8_t LSB = ReceiveData();	  //第一個讀到的是低位
	uint8_t MSB = ReceiveData();      //第二個讀到的是高位
	return Caculate_Temperature(LSB,MSB);
}

/**************************************************************************
 * [溫度值轉換程序]
 * 1、正負值判定：當MSB>7，就是負值（將負值指標值設置為true）
 * 2、若為負值就進行補數運算：反相（~）後，再加1
 * 3、結合MSB及LSB成一個十六位元，並算出其整數
 * 4、乘上單位比例值0.0625
 * 5、若為負數時，加負數符號
 * 6、將溫度值傳回
 ************************************************************************/
float Caculate_Temperature(uint8_t temp_LSB , uint8_t temp_MSB)
{
	boolean  fp_minus = false;	  	    //温度正負標誌：預設為false，因為通常為零度以上
	if(temp_MSB > 0x7f)  //當temp_MSB>7代表此溫度為負數值時，測到的數值需要先反相再加 1
	{
		fp_minus = true;					//溫度正負指標，負數時fg_Minus=0
		temp_MSB = ~temp_MSB;			 	//將temp_MSB中的每一位元反相（0、1互換）
		temp_LSB = ~temp_LSB + 1;
		//將temp_LSB中的每一位元反相（0、1互換），要記得加一，才能正確的反應其值
	}
	//以十六位元空箱來結合MSB及LSB
	uint16_t raw_temp =
	    (((uint16_t)temp_MSB) << 8) |     	//將MSB先用空的十六位元左移8個位元，等於乘256
	    (((uint16_t)temp_LSB));
	/*
	 	//底下是輸出LSB及MSB資料以作為檢核用，若不顯示可整段mark起來
		Serial.println("<<DS18b20's Temperature LSB&MSB >>");

		Serial.print("LSB BIN->");
		Serial.println(temp_LSB, BIN);

		Serial.print("MSB BIN->");
		Serial.println(B00000000|temp_MSB, BIN);

		Serial.print("MSB+LSB BIN->");
		Serial.print(raw_temp, BIN);
		Serial.print("  int MSB+LSB -->");
		Serial.println(raw_temp);

		//以上是輸出LSB及MSB資料以作為檢核用，若不顯示可整段mark起來
	*/
	float fp_temp = raw_temp * 0.0625;
	//將十六位元的整數值再乘於0.0625的單位值，既得溫度值

	if(fp_minus) {fp_temp = -fp_temp;}	//當fp_minus是1，代表是負數，將溫度加上負號
	return fp_temp;
}

uint8_t ReceiveData()
{
	uint8_t byte_in=0;
	for(uint8_t i = 0; i < 8; i++)
	{
		//此時所測到的電位，就是此位元的資料
		if(CatchSlot()) {
			//看看此時主機線的電位狀況若為高位，就是1
			bitSet(byte_in, i); 		  //將byte_in第i個位元值，設置為1
		}
		else {
			bitClear(byte_in, i); 		//將byte_in第i個位元值，設置為0
		}
	}
	return (byte_in);
}

uint8_t CatchSlot() {
	pinMode(g_dq_pin, OUTPUT);	      //轉為輸出，可達到高電位
	digitalWrite(g_dq_pin, LOW);	  //將電位拉低告訴DS18B20，主機已準備好了
	delayMicroseconds(2);
	pinMode(g_dq_pin, INPUT);		  //轉為輸入狀態，這樣就不會輸出電壓與DB18B20相衝突，同時位於高電位
	delayMicroseconds(10);			  //約於12us時取樣
	uint8_t fp=digitalRead(g_dq_pin);
	delayMicroseconds(53);		//加上延時過渡此段作業時間
	return fp;
}

void SendCommand(uint8_t instruction)
{
	for(uint8_t i = 0; i < 8; i++) {
		WriteSolt(bitRead(instruction, i));
	}
}

void WriteSolt(uint8_t order_bit)
{
	if(order_bit) {   	    //當值為1時的處理
		pinMode(g_dq_pin, OUTPUT);       //先將pin腳改為輸出狀態
		digitalWrite(g_dq_pin, LOW);     //將電位拉低，等於通知DS18B20要do something
		delayMicroseconds(1); 		   //至少要等待1us
		pinMode(g_dq_pin, INPUT);	       //將接收轉成INPUT狀態，就是高電位
		delayMicroseconds(59);		   //基本等待60ms過此周期
	}
	else { 							   //當寫入值為'0'時
		pinMode(g_dq_pin, OUTPUT);	   //先轉為輸出狀態
		digitalWrite(g_dq_pin, LOW);	   //將電位輸出低電位
		delayMicroseconds(59);         //靜靜的等待DS18B20來讀取資料
		pinMode(g_dq_pin, INPUT);        //轉回輸入狀態，可達到高電位的狀態，並可準備好下一個周期
		delayMicroseconds(1);
	}
}

uint8_t CommandReset()
{
	TxReset();
	uint8_t f=RxResult();
	ThroughRx();
	//Serial.print("Reset==>");
	//Serial.println(f);
	return f;
}

void TxReset() {
	uint16_t time_keep_low = 720;
	//Tx階段：Step 1.主機發送重置脈沖
	pinMode(g_dq_pin, OUTPUT);
	//Tx階段：Step 2.主機拉低電位
	digitalWrite(g_dq_pin, LOW);
	//Tx階段：Step 3.主機持續於低電位
	delayMicroseconds(time_keep_low);
	//Tx階段：Step 4.主機釋放電位控制，轉為輸入狀態
	pinMode(g_dq_pin, INPUT);
}

uint8_t RxResult() {
	uint8_t time_wait_read=70;
	//Rx階段：Step 5.延時並讀取DS18B20回應電位值
	delayMicroseconds(time_wait_read);
	return !digitalRead(g_dq_pin);
}

void ThroughRx() {
	//方法1：讓其自然超過Master最長的Rx週期
	//uint16_t time_through =420;
	//Rx階段：Step 6.延時並讓其超過Rx的480us時間
	//delayMicroseconds(time_through);

	//方法2：
	//當DS18B20仍在回應（低電位）就停於此，等其拉高就返回
	while(!digitalRead(g_dq_pin));
}


