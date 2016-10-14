//---------------------------------------------------------------------------------
//	平台配置：
//		Project: ROSA/APL-DT
//		Platform: APL
//		SIO: IT8617E 
//
//	功能:
//		设置 Power Button LED 按指定频率闪烁（白/灭）。
//
//	HW 电路：
//		电路图上，表面上控制黄/白的是 GP10 和 GP41;
//		其实，使用了 “Power LED Backup Plan”，实际是由 GP45 和 GP41 控制！
//		---------------------+------+-------+-------+-------++--------------+-----------+-----------+----------
//							 |		|		|	S0	| S4/S5	||	  S3		|			|			|
//		---------------------+------+-------+-------+-------++--------------+-----------+-----------+----------	
//		GP45 (Pin57,RTC_DET#)|	H	|	H	|	L	|	L	||	  L			|	 H		| Blingking	| Blingking
//		---------------------+------+-------+---------------++--------------+-----------+-----------+----------
//		GP41 (Pin59,5V_LED)	 |	H	|	L	|	L	|	H	||  Blingking	| Blingking	|     L		|    H
//		---------------------+------+-------+-------+-------++--------------+-----------------------+----------
//							 |	黄	|	灭	|	白	|	灭	||【白/灭闪烁】	|  黄/灭	|  白/灭	|  黄/灭
//		---------------------+------+-------+-------+-------++--------------+-----------+-----------+----------
//
//	IT8617E Spec (LDN=07h)：
//		1. 在 GPO 模式下，开启 Alternate Function 后，IT8617E提供了2组寄存器(Bliking 1 、2)，来实现Blinking！
//		2. GP45 设置:
//				GPIO/Native:		28h<5>=1 (GPIO Function)
//				GPI/GPO:			CBh<5>=1 (GPO)
//				[Simple IO Function]:	C3h<5>=1 (否则，=0 为Alternate，>>> blinking)
//				High/Low:			BaseAddress=62h-63h，Index=03h<5>=0	(Low)
//		3. GP41 blinking 设置:
//				GPIO/Native:		28h<1>=1 (GPIO Function)
//				GPI/GPO:			CBh<1>=1 (GPO)
//				[Alternate Function]:	C3h<1>=0 (否则，=1 为 Simple IO Function)
//				在 Bliking 1 的 Mapping Register 选择 GP41！ (F8h = 00_100_001b)；
//				在 Bliking 1 的 Control Register 设置 Blinking 频率！ (如： F9h = 10_00_XXX_0b, >>> 1s OFF，1s ON)；
//
//	程序流程：
//		1. 访问 ITE, 并选择 LDN=07h
//		2. 读取/设置相应的寄存器 (GP45;GP41)：
//			a. GPIO/Native; 
//			b. Simple_I/O_Function or Alternate; 
//		    c. GPI/GPO;
//			d. 指定GPIO High/Low
//			e. 指定GPIO blinking
//		3. 退出 ITE
//---------------------------------------------------------------------------------

#include <Uefi.h>
#include <Library\UefiLib.h>  //Print()

//------------------
//Enter MB PnP Mode
//------------------
void EnterITESIO(){
	_outp(0x2E,0x87);
	_outp(0x2E,0x01);
	_outp(0x2E,0x55);
	_outp(0x2E,0x55);
}


//------------------
//Exit MB PnP Mode
//------------------
void ExitITESIO(){
	_outp(0x2E,	0x07);
	_outp(0x2F,	0x07);
  
	//Index=02h bit<1>=1
	_outp(0x2E,	0x02);
	_outp(0x2F,	_inp(0x2F) | 0x02 );
}


//---------------------------------------------------------
//LDN=07h，Index=62h(M)-63h(L) 获取 Simple I/O BaseAddress
//---------------------------------------------------------
UINT16 GetBaseAddress(){
	UINT8 	MSB;
	UINT8 	LSB;
	UINT16 	BaseAddress;
	
	//选择 LDN=07h
	_outp(0x2E, 0x07);
	_outp(0x2F, 0x07);
	
	//获取 BaseAddress
	_outp(0x2E, 0x62);
	MSB = (UINT8)_inp(0x2F);
	_outp(0x2E, 0x63);
	LSB = (UINT8)_inp(0x2F);
	//Print(L"MSB = 0x%02x\n", MSB);
	//Print(L"LSB = 0x%02x\n", LSB);
	BaseAddress = (UINT16)((MSB<<8) + LSB);
	Print(L"LDN=07h,Index=Index=62h(M)-63h(L), Simple I/O BaseAddress = 0x%04x\n\n", BaseAddress);
	
	return BaseAddress;
}
	

//----------------------
//设置 GP45 为 GPO_L
//----------------------
void GP45_GPO_L(UINT16 BaseAddress){

	//选择 LDN=07h
	_outp(0x2E, 0x07);
	_outp(0x2F, 0x07);
  
	//1. GPIO Function: 28h<5>=1
	_outp(0x2E, 0x28);
	_outp(0x2F, _inp(0x2F) | 0x20);
  
	//2. Simple IO Function： C3h<5>=1
	_outp(0x2E, 0xC3);
	_outp(0x2F, _inp(0x2F) | 0x20); 
  
	//3. GPO：CBh<5>=1
	_outp(0x2E, 0xCB);
	_outp(0x2F, _inp(0x2F) | 0x20);
	
	//4. Low: BaseAddress+0x03，bit<5>=0
	_outp(BaseAddress + 0x03, _inp(BaseAddress+0x03) & (~0x20) );
}


//----------------------
//设置 GP45 为 GPO_H
//----------------------
void GP45_GPO_H(UINT16 BaseAddress){

	//选择 LDN=07h
	_outp(0x2E, 0x07);
	_outp(0x2F, 0x07);
  
	//1. GPIO Function: 28h<5>=1
	_outp(0x2E, 0x28);
	_outp(0x2F, _inp(0x2F) | 0x20);
  
	//2. ★ Simple IO Function： C3h<5>=1
	_outp(0x2E, 0xC3);
	_outp(0x2F, _inp(0x2F) | 0x20); 
  
	//3. GPO：CBh<5>=1
	_outp(0x2E, 0xCB);
	_outp(0x2F, _inp(0x2F) | 0x20);
	
	//4. High: BaseAddress+0x03，bit<5>=1
	_outp(BaseAddress + 0x03, _inp(BaseAddress+0x03) | 0x20);
}

//--------------------------------------------------------------------------
//设置 GP41 为 Blingking (这里使用 Binking 1 这一组寄存器,参考 IT8617E Spec)
//	GP LED 1 or 2 Frequency Control:
//	--------+-----------+---------------------------+------------------+---------------
//	<7-6>	|	<3-1>	|	Blink Frequency & Duty	|	On/Off Time
//	--------+-----------+---------------------------+------------------+---------------
//	00		|	000		|			4Hz,	50%		|	0.125s, 0.125s	(Try1 - Quick)
//	00		|	001		|			1HZ,	50%		|	0.5s,	0.5s
//	00		|	010		|			0.25Hz,	50%		|	2s,		2s
//	00		|	011		|			2Hz,	50%		|	0.25s,	0.25s
//	00		|	100		|			0.25Hz,	25%		|	3s,		1s		(Try4 - NonStep)
//	00		|	101		|			0.25Hz,	75%		|	1s,		3s
//	00		|	110		|			0.125Hz,25%		|	6s,		2s
//	00		|	111		|			0.125Hz,75%		|	2s,		6s
//	01		|	XXX		|			0.4Hz,	20%		|	2s,		0.5s
//	10		|	XXX		|			0.5Hz,	50%		|	1s,		1s		(Try2 - Normal)
//	11		|	XXX		|			0.125Hz,50%		|	4s,		4s		(Try3 - Slow)
//	--------+-----------+---------------------------+------------------+---------------
//--------------------------------------------------------------------------------------
void GP41_Blinking(UINT8 Frequency_Duty){

	//选择 LDN=07h
	_outp(0x2E, 0x07);
	_outp(0x2F, 0x07);
  
	//1. GPIO Function: 28h<1>=1
	_outp(0x2E, 0x28);
	_outp(0x2F, _inp(0x2F) | 0x02);
  
	//2. ★ Alternate Function： C3h<1>=0
	_outp(0x2E, 0xC3);
	_outp(0x2F, _inp(0x2F) & (~0x02) ); 
  
  	//3. GPO：CBh<1>=1
	_outp(0x2E, 0xCB);
	_outp(0x2F, _inp(0x2F) | 0x02);
	
	//4. 将 GP41 添加到 Blinking 1 的 Mapping Register (Index=F8h): = 00_100_001b，GP41
	_outp(0x2E, 0xF8);
	_outp(0x2F, 0x21); //GP41 
	
	//5. 设置 Blinking 1 的 Control Register (Index=F9h): 设置闪烁频率&周期: XX00_XXX0b
	if(Frequency_Duty == 1){
		_outp(0x2E, 0xF9);
		_outp(0x2F, 0x00); //00XX_000Xb
		
		Print(L"\n\nF9h = 0x%02x\n", (UINT8)_inp(0x2F) );
		Print(L"Blink Frequency & Duty: 4Hz, 50%\n");
		Print(L"Quick: 0.125s ON, 0.125s OFF\n\n");
	}//0.125s ON, 0.125s OFF (Try1 - Quick)
		
	else if(Frequency_Duty == 2){
		_outp(0x2E, 0xF9);
		_outp(0x2F, 0x80); //00XX_000Xb
		
		Print(L"\n\nF9h = 0x%02x\n", (UINT8)_inp(0x2F) );
		Print(L"Blink Frequency & Duty: 0.5Hz, 50%\n");
		Print(L"Normal: 1s ON, 1s OFF\n\n");
	}//1s ON, 1s OFF(Try2 - Normal)
	
	else if(Frequency_Duty == 3){
		_outp(0x2E, 0xF9);
		_outp(0x2F, 0xC0); //11XX_xxxXb
		
		Print(L"\n\nF9h = 0x%02x\n", (UINT8)_inp(0x2F) );
		Print(L"Blink Frequency & Duty: 0.125Hz, 50%\n");
		Print(L"Slow: 4s ON, 4s OFF\n\n");
	}//4s ON, 4s OFF (Try3 - Slow)
		
	else if(Frequency_Duty == 4){
		_outp(0x2E, 0xF9);
		_outp(0x2F, 0x08); //00XX_100Xb
		
		Print(L"\n\nF9h = 0x%02x\n", (UINT8)_inp(0x2F) );
		Print(L"Blink Frequency & Duty: 0.25Hz, 25%\n");
		Print(L"NonStep: 3s ON, 1s OFF\n\n");
	}//3s ON, 1s OFF (Try4 - NonStep)
}


void Delay(){
	UINT16 i;
	
	for(i=0x4FFF; i>0; i-- ){
		//; //无任何语句,则没有延时效果！ Why???
		//Print(L"%04x ", i); //有延时
		//Print(L""); //无延时
		Print(L" "); //有延时(0x3FFF-18s左右; 0x4FFF-22s; 0x8FFF-40s左右)
	}
}



EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE			ImageHandle,
  IN EFI_SYSTEM_TABLE	*SystemTable
  )
{
  UINT16 SimpleIOBaseAddress;
  //UINT8 i;
  
  Print(L"=== \"Power Button LED Blinking\" Please check it! =================\n");
  Print(L"=== Set IT8617E GP45(GPO_L) & GP41(Blinking): White Blinking =====\n");
  Print(L"=== Set IT8617E GP45(GPO_H) & GP41(Blinking): Yellow Blingking ===\n\n");
  
  //进入 IT8617E
  EnterITESIO();
  
  //获取 Simple I/O BaseAddress
  SimpleIOBaseAddress = GetBaseAddress();
  
  //白/灭闪烁 ----------
  //GP45 setting: GPO_L
  GP45_GPO_L(SimpleIOBaseAddress);
  Print(L"=== White Blinking: 4 Types ===\n");
  
  GP41_Blinking(1);
  Delay();
  
  GP41_Blinking(2);
  Delay();
  
  GP41_Blinking(3);
  Delay();
  
  GP41_Blinking(4);
  Delay();
  
  
  //黄/灭闪烁 ----------
  //GP45 setting: GPO_H
  GP45_GPO_H(SimpleIOBaseAddress);
  Print(L"\n=== Yellow Blinking:  4 Types ===\n");
  
  GP41_Blinking(1);
  Delay();
  
  GP41_Blinking(2);
  Delay();
  
  GP41_Blinking(3);
  Delay();
  
  GP41_Blinking(4);
  Delay();

  //退出 IT8617E
  ExitITESIO();

  return EFI_SUCCESS;
}
