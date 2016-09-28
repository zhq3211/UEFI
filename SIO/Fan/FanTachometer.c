//****************************************************************************
//	功能： 
//		检测 SIO [IT8617E] Hardware Monitor 中风扇转速(Fan Tachometer - Fan's RPM)
//	注：[IT8617E] >>> four  FAN  Tachometer  inputs	
//
//	平台配置：
//		Project: ROSA/APL-DT
//		Platform: APL
//		SIO: IT8617E 
//
//	BIOS：
//		1. 了解 HW 电路中使用到了 IT8617E 中哪些 Fan Pin. 如 HW 只连接了 FAN2
//			a. FAN_TAC2: Fan2 转速读取 pin (此例子用到)
//			b. FAN_CTL2: Fan2 转速调节 pin
//		2. IT8617E LDN=04h 的 BaseAddress, Fan 转速读取的相关寄存器 Index.
//		3. 编程
//		
//	程序流程：
//		1. 进入 IT8617E
//		2. 通过 LDN = 04h, Offset = 60h-61h, 计算 ECAddressPort & ECDataPort
//		3. 设置 BANK_SEL=0 (IT8617E需求)
//		4. FAN Tachometer Reading (Count)，并通过公式计算： 
//			RPM = 1.35*10^6/(Count*Divisor) ; (Default Divisor = 2) 
//		注： Count=0，公式错误，程序运行不下去，会卡主；所以需要判定 Count 是否为0！
//***************************************************************************

#include <Uefi.h>
#include <Library\UefiLib.h> 
#include <Library\UefiBootServicesTableLib.h>
#include "Library\BaseIoLibIntrinsic\BaseIoLibIntrinsicInternal.h" //_outp()

//IT8617E Address Port & Data Port
//#define	ITE_AddressPort	0x2E
//#define	ITE_DataPort	0x2F

#define Divisor 2

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
	//Print(L"BANK_SEL (06h): 0x%02x\n", _inp(ECDataPort));
}

//***********************************
//Fan Tachometer 2-5 Enable Register
//FAN2	13h<5>  >>> 置1：| 0x20
//FAN3	13h<6>  >>> 置1：| 0x40
//FAN4	0Ch<4>  >>> 置1：| 0x10
//FAN5	0Ch<5>  >>> 置1：| 0x20
//***********************************
void EnableFanTach(){
	//Enable FAN2
	_outp(ECAddressPort, 0x13);
	_outp(ECDataPort, _inp(ECDataPort) | 0x20);
	//Print(L"Enable FAN2 (13h<5>=1): 0x%02x\n", _inp(ECDataPort));

	//Enable FAN3
	_outp(ECAddressPort, 0x13);
	_outp(ECDataPort, _inp(ECDataPort) | 0x40);
	//Print(L"Enable FAN3 (13h<6>=1): 0x%02x\n", _inp(ECDataPort));

	//Enable FAN4
	_outp(ECAddressPort, 0x0C);
	_outp(ECDataPort, _inp(ECDataPort) | 0x10);
	//Print(L"Enable FAN4 (0Ch<4>=1): 0x%02x\n", _inp(ECDataPort));
	
	//Enable FAN5
	_outp(ECAddressPort, 0x0C);
	_outp(ECDataPort, _inp(ECDataPort) | 0x20);
	//Print(L"Enable FAN5 (0Ch<5>=1): 0x%02x\n", _inp(ECDataPort));
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
  UINT8 MSB;
  UINT8 LSB;
  UINT16 ReadFanCounter;
  
  Print(L"==========================================================\n");
  Print(L"=== IT8617E's Hardware Monitor - Fan Tachometer ==========\n");
  Print(L"=== Four FAN Tachometer inputs: Fan2,3,4,5 ===============\n");
  Print(L"=== RPM = 1.35*10^6 / (Count X Divisor) ==================\n");
  Print(L"==========================================================\n");
  
  //进入 IT8617E
  EnterITESIO();
  
  //读取 LDN = 04h, Offset = 60h-61h, 并计算 ECAddressPort & ECDataPort
  GetECInterface();

  //IT8617E 需要设置 BANK_SEL=0 才能访问 TMPIN 寄存器
  SetBankSel_0();
  
  //使能 Fan Tachometer 2-5 Register
  EnableFanTach();
  
  //读取相应的风扇转速 RPM, 即 r/min
  //Fan2 Tachometer Reading Register (19h+0Eh)
  Print(L"\n=== Fan2 RPM Test ===\n");
  _outp(ECAddressPort, 0x19);
  MSB = (UINT8)_inp(ECDataPort);
  Print(L"  MSB (19h) = 0x%02x\n", MSB);
  _outp(ECAddressPort, 0x0E);
  LSB = (UINT8)_inp(ECDataPort);
  Print(L"  LSB (0Eh) = 0x%02x\n", LSB);
  ReadFanCounter = (MSB<<8) + LSB;
  Print(L"ReadFanCounter (Fan2) = 0x%04x\n", ReadFanCounter);
  if (ReadFanCounter==0)
    Print(L"Can not find the Fan! or the Fan not work, and please check the HW...\n");
  else
    Print(L"Fan2 RPM = %d (r/min)\n", 1350000/(ReadFanCounter*Divisor));    

  //Fan3 Tachometer Reading Register (1Ah+0Fh)
  Print(L"\n=== Fan3 RPM Test ===\n");
  _outp(ECAddressPort, 0x1A);
  MSB = (UINT8)_inp(ECDataPort);
  _outp(ECAddressPort, 0x0F);
  LSB = (UINT8)_inp(ECDataPort);
  ReadFanCounter = (MSB<<8) + LSB;
  Print(L"ReadFanCounter (Fan3) = 0x%04x\n", ReadFanCounter);
  if (ReadFanCounter==0)
    Print(L"Can not find the Fan! or the Fan not work, and please check the HW...\n");
  else
    Print(L"Fan2 RPM = %d (r/min)\n", 1350000/(ReadFanCounter*Divisor)); 
  
  //Fan4 Tachometer Reading Register (81h+80h)
  Print(L"\n=== Fan4 RPM Test ===\n");
  _outp(ECAddressPort, 0x81);
  MSB = (UINT8)_inp(ECDataPort);
  _outp(ECAddressPort, 0x80);
  LSB = (UINT8)_inp(ECDataPort);
  ReadFanCounter = (MSB<<8) + LSB;
  Print(L"ReadFanCounter (Fan4) = 0x%04x\n", ReadFanCounter);
  if (ReadFanCounter==0)
    Print(L"Can not find the Fan! or the Fan not work, and please check the HW...\n");
  else
    Print(L"Fan2 RPM = %d (r/min)\n", 1350000/(ReadFanCounter*Divisor)); 
  
  //Fan5 Tachometer Reading Register (83h+82h)
  Print(L"\n=== Fan5 RPM Test ===\n");
  _outp(ECAddressPort, 0x83);
  MSB = (UINT8)_inp(ECDataPort);
  _outp(ECAddressPort, 0x82);
  LSB = (UINT8)_inp(ECDataPort);
  ReadFanCounter = (MSB<<8) + LSB;
  Print(L"ReadFanCounter (Fan5) = 0x%04x\n", ReadFanCounter);
  if (ReadFanCounter==0)
    Print(L"Can not find the Fan! or the Fan not work, and please check the HW...\n");
  else
    Print(L"Fan2 RPM = %d (r/min)\n", 1350000/(ReadFanCounter*Divisor)); 
  
  //退出 IT8617E
  ExitITESIO();
  
  return EFI_SUCCESS;
}