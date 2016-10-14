//---------------------------------------------------------------------------------
//	平台配置：
//		Project: ROSA/APL-DT
//		Platform: APL
//		SIO: IT8617E 
//
//	功能:
//		设置 Power Button LED 为黄色/橘色/琥珀色。
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
//							 |【黄】|	灭	|	白	|	灭	||  白/灭闪烁	|  黄/灭	|  白/灭	|  黄/灭
//		---------------------+------+-------+-------+-------++--------------+-----------+-----------+----------
//
//	IT8617E Spec (LDN=07h)：
//		1. GP45:
//				GPIO/Native:	28h<5>=1 (GPIO Function)
//				GPI/GPO:		CBh<5>=1 (GPO)
//				Simple IO Function:	C3h<5>=1 (否则，=0 为Alternate，>>> blinking)
//				High/Low:		BaseAddress=62h-63h，Index=03h<5>=1	(High)
//		2. GP41:
//				GPIO/Native:	28h<1>=1 (GPIO Function)
//				GPI/GPO:		CBh<1>=1 (GPO)
//				Simple IO Function:	C3h<1>=1 (否则，=0 为Alternate，>>> blinking)
//				High/Low:		BaseAddress=62h-63h，Index=03h<1>=1	(High)
//		
//	程序流程：
//		1. 访问 ITE, 并选择 LDN=07h
//		2. 读取/设置相应的寄存器 (GP45;GP41)：
//			a. GPIO/Native; 
//			b. Simple_I/O_Function or Alternate; 
//		    c. GPI/GPO;
//			d. High/Low
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
	Print(L"LDN=07h,Index=Index=62h(M)-63h(L), Simple I/O BaseAddress = 0x%04x\n", BaseAddress);
	
	return BaseAddress;
}
	

//----------------------
//设置 GP45 为 GPIO; GPO
//----------------------
void GP45(UINT16 BaseAddress){

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
	
	//4. High: BaseAddress+0x03，bit<5>=1
	_outp(BaseAddress + 0x03, _inp(BaseAddress+0x03) | 0x20);
}

//----------------------
//设置 GP41 为 GPIO; GPO
//----------------------
void GP41(UINT16 BaseAddress){

	//选择 LDN=07h
	_outp(0x2E, 0x07);
	_outp(0x2F, 0x07);
  
	//1. GPIO Function: 28h<1>=1
	_outp(0x2E, 0x28);
	_outp(0x2F, _inp(0x2F) | 0x02);
  
	//2. Simple IO Function： C3h<1>=1
	_outp(0x2E, 0xC3);
	_outp(0x2F, _inp(0x2F) | 0x02); 
  
	//3. GPO：CBh<1>=1
	_outp(0x2E, 0xCB);
	_outp(0x2F, _inp(0x2F) | 0x02);
	
	//4. High：BaseAddress+0x03，bit<1>=1
	_outp(BaseAddress + 0x03, _inp(BaseAddress+0x03) | 0x02);
}


EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE			ImageHandle,
  IN EFI_SYSTEM_TABLE	*SystemTable
  )
{
  UINT16 SimpleIOBaseAddress;
  
  Print(L"=== \"Power Button LED Colour\" Change to Yellow/Amber, Please check it! ===\n");
  Print(L"=== Set IT8617E GP45 & GP41: GPO_H =======================================\n");
  
  //进入 IT8617E
  EnterITESIO();
  
  //获取 Simple I/O BaseAddress
  SimpleIOBaseAddress = GetBaseAddress();
  
  //GP45 setting: GPO_H
  GP45(SimpleIOBaseAddress);

  //GP41 setting: GPO_H
  GP41(SimpleIOBaseAddress);

  //退出 IT8617E
  ExitITESIO();

  return EFI_SUCCESS;
}
