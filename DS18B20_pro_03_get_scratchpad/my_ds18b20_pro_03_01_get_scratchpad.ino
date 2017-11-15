/**********************************************************************
 - 適用狀況：只能用於1台arduino連接1個DS18B20之狀態
 - 功能描述：可重置DS18B20及讀寫命令及取得溫度值
 - 詳細內容：不引用他人函數庫的情況下，讓Arduino與DS18B20於1對1的連接下，
 			能做資料讀寫之功能，順利取得溫度值並回饋訊息至串列埠螢幕中
 - 版本新增：讀取中間暫存器之資料
 - 主機版本：Arduino UNO R3
 *********************************************************************/
const uint8_t  g_dq_pin =7;             //Arduino數位腳位2接到DS18B20，DQ為Data input/output的縮寫
uint8_t scratchpad[9];
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
	PrintScratchpad();
	delay(3000);

}
void PrintScratchpad() {
	Serial.println("DS18b20's scratchpad Data ->");
	for(int i = 0; i < 9; ++i)
	{
		Serial.print(i);
		Serial.print(". 0x");
		Serial.print(scratchpad[i],HEX);
		Serial.println("");
	}
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
	while(! ReadSlot());


	//再次重啟，因為要作通訊作業
	while(!CommandReset());       	  //重啟成功，再進行動作
	SendCommand(Skip_ROM);		      //主機下0xcc命令（1對1的省略ROM確認作業）
	SendCommand(Read_Scratchpad);	  //0xBE溫度暫存器中的訊息（共9個字元）
	SaveData();						  //將讀取的資料放入陣列中
	//暫存器有資料了，可以進行溫度的計算
	return Caculate_Temperature();    //計算溫度並傳回溫度值
}

void SaveData() {
	for(int i = 0; i < 9; ++i)
	{
		noInterrupts();
		scratchpad[i]=ReadByte();
		interrupts();
	}
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
float Caculate_Temperature()
{
	//前兩個字元就是溫度的訊息
	uint8_t temp_LSB = scratchpad[0];	  //第一個讀到的是低位
	uint8_t temp_MSB = scratchpad[1];      //第二個讀到的是高位
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

uint8_t ReadByte()
{
	uint8_t byte_in=0;
	for(uint8_t i = 0; i < 8; i++)
	{
		//此時所測到的電位，就是此位元的資料
		noInterrupts();
		if(ReadSlot()) {
			//看看此時主機線的電位狀況若為高位，就是1
			bitSet(byte_in, i); 		  //將byte_in第i個位元值，設置為1
		}
		else {
			bitClear(byte_in, i); 		//將byte_in第i個位元值，設置為0
		}
		interrupts();
	}
	return (byte_in);
}

uint8_t ReadSlot() {
	//one wire的設定是3,10,53
	//主廠建議：6/9/55（含步驟是4/8/55）亦可
	//me:調整為2/10/48
	//所以位置於12~13時為佳！這個時間點，才能判別0或1
	//讀時隙（Read Time Slot）步驟01：確保與上一個讀時序有1us的間隔
	noInterrupts();
	delayMicroseconds(1);
	//讀時隙（Read Time Slot）步驟02：啟始信號--拉低電位
	pinMode(g_dq_pin, OUTPUT);	      //轉為輸出，可達到高電位
	digitalWrite(g_dq_pin, LOW);	  //將電位拉低告訴DS18B20，主機已準備好了
	//讀時隙（Read Time Slot）步驟03：保持低電位最少1us
	delayMicroseconds(2);
	//讀時隙（Read Time Slot）步驟04：釋放線路電位
	pinMode(g_dq_pin, INPUT);		  //轉為輸入狀態，同時釋放線路
	//讀時隙（Read Time Slot）步驟05：等待時間，
	delayMicroseconds(10);			  //加前面的延時，約於12~13us時取樣
	//讀時隙（Read Time Slot）步驟06：讀取slot的電位值
	uint8_t fp=digitalRead(g_dq_pin);
	//讀時隙（Read Time Slot）步驟07：延時動作至少要60us
	interrupts();
	delayMicroseconds(55);			 //加上延時過渡此段作業時間60us
	return fp;
}

void SendCommand(uint8_t instruction)
{
	for(uint8_t i = 0; i < 8; i++) {
		noInterrupts();
		WriteSolt(bitRead(instruction, i));
		interrupts();
	}
}

void WriteSolt(uint8_t order_bit)
{
	if(order_bit) {   	    //當值為1時的處理，
		noInterrupts();
		pinMode(g_dq_pin, OUTPUT);      //先將pin腳改為輸出狀態
		digitalWrite(g_dq_pin, LOW);    //將電位拉低，等於通知DS18B20要do something
		delayMicroseconds(10); 		    //至少要等待1us，但於15us前轉為高電位
		pinMode(g_dq_pin, INPUT);	    //將接收轉成INPUT狀態，轉為高電位
		interrupts();
		delayMicroseconds(60);		    //加前段的延時至少等待60us過此周期
	}
	else { 							   //當寫入值為'0'時，Tx拉低電位時段60~120us
		noInterrupts();
		pinMode(g_dq_pin, OUTPUT);	   //先轉為輸出狀態
		digitalWrite(g_dq_pin, LOW);   //將電位輸出低電位
		delayMicroseconds(65);         //靜靜的等待DS18B20來讀取資料
		pinMode(g_dq_pin, INPUT);      //釋放電位控制轉回輸入狀態
		interrupts();
		delayMicroseconds(5);		   //等待上拉電阻將電位復位為HIGH
	}
}

uint8_t CommandReset()
{
	TxReset();
	noInterrupts();
	uint8_t f=RxResult();
	interrupts();
	ThroughRx();
	//Serial.print("Reset==>");
	//Serial.println(f);
	return f;
}

void TxReset() {
	uint16_t time_keep_low = 480;
	noInterrupts();
	//Tx階段：Step 1.主機發送重置脈沖
	pinMode(g_dq_pin, OUTPUT);
	//Tx階段：Step 2.主機拉低電位
	digitalWrite(g_dq_pin, LOW);
	interrupts();
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
	//Rx階段：Step 6.延時並讓其超過Rx的480us時間

	//方法1：讓其自然超過Master最長的Rx週期
	uint16_t time_through =410;
	delayMicroseconds(time_through);

	//方法2：
	//當DS18B20仍在回應（低電位）就停於此，等其拉高就返回
	//while(!digitalRead(g_dq_pin));
}


