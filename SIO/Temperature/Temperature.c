//****************************************************************************
//	功能： 
//		检测 SIO [IT8617E] Hardware Monitor 中的温度。 TM_PIN1-6
//		
//	平台配置：
//		Project: ROSA/APL-DT
//		Platform: APL
//		SIO: IT8617E 
//
//	BIOS：
//		1. 了解 HW 电路中使用到了 IT8617E 中的哪些 TMPIN Pin.
//		2. IT8617E LDN=04h 的 BaseAddress, TMPIN 相关的寄存器 Index.
//		3. 编程
//		
//	程序流程：
//		1. 进入 IT8617E
//		2. 通过 LDN = 04h, Offset = 60h-61h, 计算 ECAddressPort & ECDataPort
//		3. 设置 BANK_SEL=0 (IT8617E需求)
//		4. 读取相应的温度寄存器 TMPIN1-6
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
  //温度值是有正负值的，需要定义为有符号类型！
  INT8 ReadTMPIN1;
  INT8 ReadTMPIN2;
  INT8 ReadTMPIN3;
  INT8 ReadTMPIN4;
  INT8 ReadTMPIN5;
  INT8 ReadTMPIN6;
  
  Print(L"==========================================================\n");
  Print(L"=== IT8617E's Hardware Monitor - Temperature: TMPIN1-4 ===\n");
  Print(L"==========================================================\n");
  
  //进入 IT8617E
  EnterITESIO();
  
  //读取 LDN = 04h, Offset = 60h-61h, 并计算 ECAddressPort & ECDataPort
  GetECInterface();

  //IT8617E 需要设置 BANK_SEL=0 才能访问 TMPIN 寄存器
  SetBankSel_0();
	
  //读取相应的温度寄存器
  Print(L"=== TMPIN1-6 ==============================================\n");
  //TMPIN1 (29h): HW 上对应 CPU_THERMDA+
  _outp(ECAddressPort, 0x29);
  ReadTMPIN1 = (INT8)_inp(ECDataPort);
  Print(L"Read TMPIN1(29h): 0x%02x -> %d\n", ReadTMPIN1,ReadTMPIN1);
  
  //TMPIN2 (2Ah): HW 上对应 SYS_THERMDA+
  _outp(ECAddressPort, 0x2A);
  ReadTMPIN2 = (INT8)_inp(ECDataPort);
  Print(L"Read TMPIN2(2Ah): 0x%02x -> %d\n", ReadTMPIN2,ReadTMPIN2);
  
  //TMPIN3 (2Bh)
  _outp(ECAddressPort, 0x2B);
  ReadTMPIN3 = (INT8)_inp(ECDataPort);
  Print(L"Read TMPIN3(2Bh): 0x%02x -> %d\n", ReadTMPIN3,ReadTMPIN3);
  
  //TMPIN4 (2Ch)
  _outp(ECAddressPort, 0x2C);
  ReadTMPIN4 = (INT8)_inp(ECDataPort);
  Print(L"Read TMPIN4(2Ch): 0x%02x -> %d\n", ReadTMPIN4,ReadTMPIN4);

  //TMPIN5 (2Dh)
  _outp(ECAddressPort, 0x2D);
  ReadTMPIN5 = (INT8)_inp(ECDataPort);
  Print(L"Read TMPIN5(2Dh): 0x%02x -> %d\n", ReadTMPIN5,ReadTMPIN5);
  
  //TMPIN6 (2Eh)
  _outp(ECAddressPort, 0x2E);
  ReadTMPIN6 = (INT8)_inp(ECDataPort);
  Print(L"Read TMPIN6(2Eh): 0x%02x -> %d\n", ReadTMPIN6,ReadTMPIN6);
  
  //退出 IT8617E
  ExitITESIO();
  
  return EFI_SUCCESS;
}