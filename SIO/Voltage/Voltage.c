//*************************************************************************************
//	功能： 
//		检测 SIO [IT8617E] Hardware Monitor 中的电压(VIN0~6,3VSB,VBAT,AVCC3).
//
//	平台配置：
//		Project: ROSA/APL-DT
//		Platform: APL
//		SIO: IT8617E 
//
//	BIOS：
//		1. 确认 HW 电路中，SIO 所使用的 VIN pin(如：VIN0), 以及外围电路的组成(Ra & Rb)
//		2. 确认 IT8617E Spec 中 VIN pin(VIN0) 的定义，以及解析系数(如：Foctor=11mV)
//		3. 读取 VIN pin(VIN0) 的值：ReadVIN0
//		4. 计算 VIN0 电压: ReadVIN0*Foctor
//		5. 推算 HW 待测点的电压: V待测 = ReadVIN0*Foctor*(Ra +Rb)/Rb (mV)
//		6. HW 使用电表检测待测点X的值，与V待测比较，看是否在波动范围！
//
//	程序流程：
//		1. 进入 IT8617E
//		2. 读取 LDN=04h, offset=60h-61h 的值 EC_BaseAddress, 
//		   加上 05/06，得 ECAddressPort 和 ECDataPort
//		3. 设置 BANK_SEL=0
//		4. 读取 VIN0~6,3VSB,VBAT,AVCC3 值(Index=20h...)，并计算相应值
//		5. 退出 IT8617E
//**************************************************************************************

#include <Uefi.h>
#include <Library\UefiLib.h> 
#include <Library\UefiBootServicesTableLib.h>
#include "Library\BaseIoLibIntrinsic\BaseIoLibIntrinsicInternal.h" //_outp()

//IT8617E Address Port & Data Port
//#define	ITE_AddressPort	0x2E
//#define	ITE_DataPort	0x2F

UINT16	ECAddressPort;
UINT16	ECDataPort;

//IT8617E Spec 电压的解析系数为 11mV！不同 Spec 定义值不一样！
//Voltage = ReadValue*11mV
#define Foctor	11

//HW 电路中 VIN0 外部 Ra 与 Rb 阻值的设定
//Ra = 6.65千欧姆 -> (QA：怎样使用浮点数？)
//Rb = 10千欧姆 
#define	Ra	6650
#define Rb	10000


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
//ECAddressPort/ECDataPort 的 Index = 06h,bit<6-5>=00h
//******************************************************
void SetBankSel_0(){
	_outp(ECAddressPort, 0x06);
	_outp(ECDataPort, _inp(ECDataPort) & 0x09FF);
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
  UINT8		ReadVIN0; //IT8617E Pin - Register Value (十六进制)
  
  UINT16	VoltageVIN0; //ReadVIN0*Factor (该点的实际电压值！)
  UINT16	VoltageVIN1;
  UINT16	VoltageVIN2;
  UINT16	VoltageVIN3;
  UINT16	VoltageVIN4;
  UINT16	VoltageVIN5;
  UINT16	VoltageVIN6;
  UINT16	Voltage3VSB;
  UINT16	VoltageVBAT;
  UINT16	VoltageAVCC3;
  
  //由于HW电路中 VIN 的外围电路都不一样，这里仅仅选择 VIN0，计算HW待测点的电压值。
  UINT16	HWTestVIN0; //ReadVIN0*11mv*[(Ra+Rb)/Rb] (电路图中，对应的 HW-电表测量点的模拟电压值)
  
  Print(L"============================================\n");
  Print(L"=== IT8617E's Hardware Monitor - Voltage ===\n");
  Print(L"============================================\n");
  
  //进入 IT8617E
  EnterITESIO();
  
  //读取 LDN = 04h, Offset = 60h-61h, 并计算 ECAddressPort & ECDataPort
  GetECInterface();

  //IT8617E 需要设置 BANK_SEL=0 才能访问电压寄存器
  SetBankSel_0();
	
  //读取 VIN0 值(Index=20h)，并计算相应值
  Print(L"=== VIN0 ===================================\n");
  _outp(ECAddressPort, 0x20);
  ReadVIN0		= (UINT8)_inp(ECDataPort);
  VoltageVIN0 	= ReadVIN0*Foctor;
  HWTestVIN0 	= ReadVIN0*Foctor*(Ra+Rb)/Rb; //(mV)
  
  Print(L"VIN0 Voltage Reading Register: 0x%02x\n", ReadVIN0);
  Print(L"VIN0 Voltage Reading Register * Foctor(11mV): %d (mV)\n", VoltageVIN0);
  Print(L"VIN0 Voltage Reading Register * Foctor(11mV) * (Ra+Rb)/Rb: %d (mV)\n", HWTestVIN0);
  
  Print(L"=== VIN0~6, 3VSB, VBAT, AVCC3 ==============\n");
  //VIN0 (20h)
  _outp(ECAddressPort, 0x20);
  VoltageVIN0 = (UINT8)_inp(ECDataPort)*Foctor;
  Print(L"Voltage VIN0 (20h): %d (mV)\n", VoltageVIN0);
  
  //VIN1 (21h)
  _outp(ECAddressPort, 0x21);
  VoltageVIN1 = (UINT8)_inp(ECDataPort)*Foctor;
  Print(L"Voltage VIN1 (21h): %d (mV)\n", VoltageVIN1);
  
  //VIN2 (22h)
  _outp(ECAddressPort, 0x22);
  VoltageVIN2 = (UINT8)_inp(ECDataPort)*Foctor;
  Print(L"Voltage VIN2 (22h): %d (mV)\n", VoltageVIN2);
  
  //VIN3 (23h) Note: IT8617E 没有定义 VIN3，参考往常 ITE Spec
  _outp(ECAddressPort, 0x23);
  VoltageVIN3 = (UINT8)_inp(ECDataPort)*Foctor;
  Print(L"Voltage VIN3 (23h): %d (mV)\n", VoltageVIN3);
  
  //VIN4 (24h)
  _outp(ECAddressPort, 0x24);
  VoltageVIN4 = (UINT8)_inp(ECDataPort)*Foctor;
  Print(L"Voltage VIN4 (24h): %d (mV)\n", VoltageVIN4);
  
  //VIN5 (25h)
  _outp(ECAddressPort, 0x25);
  VoltageVIN5 = (UINT8)_inp(ECDataPort)*Foctor;
  Print(L"Voltage VIN5 (25h): %d (mV)\n", VoltageVIN5);
  
  //VIN6 (26h) Note: IT8617E 没有定义 VIN3，参考往常 ITE Spec
  _outp(ECAddressPort, 0x26);
  VoltageVIN6 = (UINT8)_inp(ECDataPort)*Foctor;
  Print(L"Voltage VIN6 (26h): %d (mV)\n", VoltageVIN6);

  //3VSB (27h)
  _outp(ECAddressPort, 0x27);
  Voltage3VSB = (UINT8)_inp(ECDataPort)*Foctor;
  Print(L"Voltage 3VSB (27h): %d (mV);  Voltage 3VSB *2: %d (mV)\n", Voltage3VSB, Voltage3VSB*2 );
  
  //VBAT (28h)
  _outp(ECAddressPort, 0x28);
  VoltageVBAT = (UINT8)_inp(ECDataPort)*Foctor;
  Print(L"Voltage VBAT (28h): %d (mV);  Voltage VBAT *2: %d (mV)\n", VoltageVBAT, VoltageVBAT*2);
  
  //AVCC3 (2Fh)
  _outp(ECAddressPort, 0x2F);
  VoltageAVCC3 = (UINT8)_inp(ECDataPort)*Foctor;
  Print(L"Voltage AVCC3 (2Fh): %d (mV);  Voltage AVCC3 *2: %d (mV)\n", VoltageAVCC3, VoltageAVCC3*2);
  
  //退出 IT8617E
  ExitITESIO();
  
  return EFI_SUCCESS;
}