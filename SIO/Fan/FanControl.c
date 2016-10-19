//****************************************************************************
//	功能： 
//		调节 SIO [IT8617E] Hardware Monitor 中风扇转速(Fan Control - Fan's 【PWM】)
//	注：[IT8617E] >>>  four sets of advanced FAN Controllers. 
//		PWM (Pulse Width Modulation)：脉宽调变 
//
//	平台配置：
//		Project: ROSA/APL-DT
//		Platform: APL
//		SIO: IT8617E 
//
//	BIOS：
//		1. 了解 HW 电路中使用到了 IT8617E 中哪些 Fan Pin. 如 HW 只连接了 FAN2
//			a. FAN_TAC2: Fan2 转速读取 pin 
//			b. FAN_CTL2: Fan2 转速调节 pin (此例子用到)
//		2. IT8617E LDN=04h 的 BaseAddress, Fan 转速控制/调节的相关寄存器 Index.
//		3. 编程
//		
//	程序流程：
//		1. 进入 IT8617E
//		2. 通过 LDN = 04h, Offset = 60h-61h, 计算 ECAddressPort & ECDataPort
//		3. 设置 BANK_SEL=0 (IT8617E需求)
//		4. 调节 PWM
//		5. 例子：调节FAN2 PWM 为 0xFF, 并不断读取其 RPM，最后 PWM 还原 0x80
//***************************************************************************

#include <Uefi.h>
#include <Library\UefiLib.h> 
#include <Library\UefiBootServicesTableLib.h>
#include "Library\BaseIoLibIntrinsic\BaseIoLibIntrinsicInternal.h" //_outp()

//IT8617E Address Port & Data Port
//#define	ITE_AddressPort	0x2E
//#define	ITE_DataPort	0x2F

UINT16	ECAddressPort;
UINT16	ECDataPort;


//******************
//Enter MB PnP Mode
//******************
void EnterITESIO(){
	_outp(0x2E,0x87);
	_outp(0x2E,0x01);
	_outp(0x2E,0x55);
	_outp(0x2E,0x55);
}

//******************
//Exit MB PnP Mode
//******************
void ExitITESIO(){
	_outp(0x2E,	0x07);
	_outp(0x2F,	0x07);
  
	//Index=02h bit<1>=1
	_outp(0x2E,	0x02);
	_outp(0x2F,	_inp(0x2F) | 0x02 );
}

//***********************************************************************************
//读取 Envirnment Controller of IT8617E decodes two address: ECAddressPort/ECDataPort
//LDN = 04h, Offset = 60h-61h (EC_BaseAddress)
//	MSB Register(Index = 60h): 0x0A
//	LSB Register(Index = 61h): 0x30
//***********************************************************************************
void GetECInterface(){
	UINT8	MSB;
	UINT8	LSB;
	UINT16	EC_BaseAddress;

	//选择 LDN=04h
	_outp(0x2E,0x07);
	_outp(0x2F,0x04);
 
	//读取 EC_BaseAddress
	_outp(0x2E, 0x60);
	MSB = (UINT8)_inp(0x2F);
	_outp(0x2E, 0x61);
	LSB = (UINT8)_inp(0x2F);
	Print(L"MSB = 0x%02x\n", MSB);
	Print(L"LSB = 0x%02x\n", LSB);
	EC_BaseAddress = (UINT16)((MSB<<8) + LSB);
	Print(L"EC_BaseAddress     = 0x%04x\n", EC_BaseAddress);
	
	//计算 ECAddressPort 和 ECDataPort
	ECAddressPort = EC_BaseAddress + 0x05;
	ECDataPort    = EC_BaseAddress + 0x06;
	Print(L"Address Port of EC = 0x%04x\n", ECAddressPort);
	Print(L"Data Port of EC    = 0x%04x\n", ECDataPort);
}

//******************************************************
//BANK_SEL 设置为 BANK 00h
//ECAddressPort/ECDataPort 的 Index = 06h,bit<6-5>=00h. 
//  >>> 清零 0110_0000b： &~(0x60) 
//******************************************************
void SetBankSel_0(){
	_outp(ECAddressPort, 0x06);
	_outp(ECDataPort, _inp(ECDataPort) & ~(0x60) );
	Print(L"BANK_SEL (06h): 0x%02x\n", _inp(ECDataPort));
}


//*************
// Main 主函数
//*************
EFI_STATUS
EFIAPI
UefiMain(
	IN EFI_HANDLE		ImageHandle,
	IN EFI_SYSTEM_TABLE	*SystemTable
	)
{
  UINT16 ReadFanCounter;
  int delay; //用于更改 PWM 后，延时一会在读取 FAN RPM
   
  Print(L"=============================================================\n");
  Print(L"=== IT8617E's Hardware Monitor - Fan Controller =============\n");
  Print(L"=== Please reference the HW, which pin is used? Like FAN2 ===\n");
  Print(L"=============================================================\n");
  
  //进入 IT8617E
  EnterITESIO();
  
  //读取 LDN = 04h, Offset = 60h-61h, 并计算 ECAddressPort & ECDataPort
  GetECInterface();

  //IT8617E 需要设置 BANK_SEL=0 才能访问 TMPIN 寄存器
  SetBankSel_0();
  
  //FAN_CTL2,3,4,5 PWM Control Registers 
  //FAN_CTL PWM Mode Software/Automatic Operation Selection; 
  //PWM setting
  //		  |	Software  |	PWM Index
  //		  |	Operation |	0-255 steps
  //----------+-----------+---------------------------------------
  //FAN_CTL2  | 16h<7>=0  |	6Bh	(=0xFF 全速；0x80 半速；0x00 停止)
  //FAN_CTL3  |	17h<7>=0  |	73h
  //FAN_CTL4  |	1Eh<7>=0  |	7Bh
  //FAN_CTL5  |	1Fh<7>=0  |	A3h
  Print(L"=== FAN_CTL PWM Mode Software/Automatic Selection: Software ===\n");
  Print(L"=== PWM Control (0x00-0xFF steps) =============================\n");
  
  //FAN_CTL2 (如果存在 FAN2，设置其 FAN2 的 PWM 值)
  //设置为 Software Operation Mode
  _outp(ECAddressPort, 0x16);
  _outp(ECDataPort, _inp(ECDataPort) & ~(0x80) );
  Print(L"FAN_CTL2 PWM Software Mode Selection(16h<7>): 0x%02x\n", (UINT8)_inp(ECDataPort) );
  //PWM Control 来调节 FAN 转速
  _outp(ECAddressPort, 0x6B);
  _outp(ECDataPort, 0xFF);  //0xFF 全速；0x80 半速；0x00 停止(保护机制，不一定停止不转!)
  Print(L"FAN_CTL2 PWM(0x6B): 0x%02x\n", (UINT8)_inp(ECDataPort) );
  
  //>>> --- 插曲 --- 读取 FAN2 的 RPM ---
  //Enable FAN2
  for( delay = 0xFF; delay>0; delay--){	
  _outp(ECAddressPort, 0x13);
  _outp(ECDataPort, _inp(ECDataPort) | 0x20);
  //计算 FAN2 Counter
  _outp(ECAddressPort, 0x19);
  ReadFanCounter = (UINT8)_inp(ECDataPort);
  _outp(ECAddressPort, 0x0E);
  ReadFanCounter = (ReadFanCounter<<8) + (UINT8)_inp(ECDataPort);
  //套用公式，计算 FAN2 RPM. 需要等待一下， 读取的 RPM 才会变化！
  if (ReadFanCounter==0)
    Print(L"Can not find the Fan! or the Fan not work, and please check the HW...\n");
  else
    Print(L"Current Fan2 RPM = %d (r/min)\n", 1350000/(ReadFanCounter*2));   
  }//For
  //最后，设置为半速
  Print(L"\n--- Set FAN2 PWM 0x80 after 0xFF ---\n");
  _outp(ECAddressPort, 0x6B);
  _outp(ECDataPort, 0x80);  //0xFF 全速；0x80 半速；0x00 
  //<<< --- 插曲 --- 读取 FAN2 的 RPM ---
  
  //FAN_CTL3 
  //设置为 Software Operation Mode
  _outp(ECAddressPort, 0x17);
  _outp(ECDataPort, _inp(ECDataPort) & ~(0x80) );
  //PWM Control 来调节 FAN 转速
  _outp(ECAddressPort, 0x73);
  _outp(ECDataPort, 0xFF);  //0xFF 全速；0x80 半速；0x00 停止(保护机制，不一定停止不转!)
  
  //FAN_CTL4
  //设置为 Software Operation Mode
  _outp(ECAddressPort, 0x1E);
  _outp(ECDataPort, _inp(ECDataPort) & ~(0x80) );
  //PWM Control 来调节 FAN 转速
  _outp(ECAddressPort, 0x7B);
  _outp(ECDataPort, 0xFF);  //0xFF 全速；0x80 半速；0x00 停止(保护机制，不一定停止不转!)

  //FAN_CTL5 
  //设置为 Software Operation Mode
  _outp(ECAddressPort, 0x1F);
  _outp(ECDataPort, _inp(ECDataPort) & ~(0x80) );
  //PWM Control 来调节 FAN 转速
  _outp(ECAddressPort, 0xA3);
  _outp(ECDataPort, 0xFF);  //0xFF 全速；0x80 半速；0x00 停止(保护机制，不一定停止不转!)

  //退出 IT8617E
  ExitITESIO();
  
  return EFI_SUCCESS;
}