//***********************************************************************************************************
//	功能： 
//		检测 SIO [IT8617E] Hardware Monitor 中的电压(VIN0~2,VIN4~5). 该实例测试 VIN0.
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
// 程序流程：
//		1. 进入 IT8617E
//		2. 读取 LDN=04h, offset=60h-61h 的值 EC_BaseAddress, 
//		   加上 05/06，得 ECAddressPort 和 ECDataPort
//		3. 设置 BANK_SEL=0
//		4. 读取 VIN0 值(Index=20h)，并计算相应值
//		5. 退出 IT8617E
//***********************************************************************************************************
#include <Uefi.h>
#include <Library\UefiLib.h> 
#include <Library\UefiBootServicesTableLib.h>
#include "Library\BaseIoLibIntrinsic\BaseIoLibIntrinsicInternal.h" //_outp()

//IT8617E Address Port & Data Port
//#define	ITE_AddressPort	0x2E
//#define	ITE_DataPort	0x2F

//Environment Controller Base Address Register
//LDN = 04h 
//MSB Register, Index = 60h
//LSB Register， Index = 61h
//	>>> RW 读取值为 0x0A30
//#define	EC_BaseAddress	0x0A30
//#define	ECAddressPort	EC_BaseAddress+0x05
//#define	ECDataPort		EC_BaseAddress+0x06

//IT8617E Spec 电压的解析系数为 11mV！
//Voltage = ReadingValue*11mV
#define Foctor	11

//HW 电路中 VIN0 外部 Ra 与 Rb 阻值的设定()
//Ra = 6.65千欧姆 -> (QA：怎样使用浮点数？)
//Rb = 10千欧姆
#define	Ra	6650
#define Rb	10000


//Enter MB PnP Mode
void EnterITESIO(){
	_outp(0x2E,0x87);
	_outp(0x2E,0x01);
	_outp(0x2E,0x55);
	_outp(0x2E,0x55);
}

//Exit MB PnP Mode
void ExitITESIO(){
	_outp(0x2E,	0x07);
	_outp(0x2F,	0x07);
  
	//Index=02h bit<1>=1
	_outp(0x2E,	0x02);
	_outp(0x2F,	_inp(0x2F) | 0x02 );
}


EFI_STATUS
EFIAPI
UefiMain(
	IN EFI_HANDLE		ImageHandle,
	IN EFI_SYSTEM_TABLE	*SystemTable
	)
{
  UINT16	EC_BaseAddress;
  UINT8  	MSB;
  UINT8  	LSB;
  UINT8		ReadVIN0; //IT8617E Pin - Register Value
  UINT16	VoltageVIN0; //ReadVIN0*Factor
  UINT16	HWTestVIN0; //HW 电表测量点的模拟电压值：ReadVIN0*11mv*[(Ra+Rb)/Rb]
  
  Print(L"============================================\n");
  Print(L"=== IT8617E's Hardware Monitor - Voltage ===\n");
  Print(L"============================================\n");
  
  //进入 IT8617E
  EnterITESIO();
  
  //读取 LDN = 04h, Offset = 60h-61h
  _outp(0x2E,0x07);
  _outp(0x2F,0x04);
  
  _outp(0x2E, 0x60);
  MSB = (UINT8)_inp(0x2F);
  _outp(0x2E, 0x61);
  LSB = (UINT8)_inp(0x2F);
  //Print(L"MSB = 0x%02x\n", MSB);
  //Print(L"LSB = 0x%02x\n", LSB);
  
  EC_BaseAddress = (UINT16)((MSB<<8) + LSB);
  Print(L"EC_BaseAddress     = 0x%04x\n", EC_BaseAddress);
  Print(L"Address Port of EC = 0x%04x\n", EC_BaseAddress+0x05);
  Print(L"Data Port of EC    = 0x%04x\n", EC_BaseAddress+0x06);

  //IT8617E 需要设置 BANK_SEL=0 才能读取电压值 
  //ECAddressPort & ECDataPort 的 Index = 06h,bit<6-5>=00h
  _outp(EC_BaseAddress+0x05, 0x06);
  _outp(EC_BaseAddress+0x06, _inp(EC_BaseAddress+0x06) & 0x09FF);
	
  //读取 VIN0 值(Index=20h)，并计算相应值
  _outp(EC_BaseAddress+0x05, 0x20);
  ReadVIN0 = (UINT8)_inp(EC_BaseAddress+0x06);
  VoltageVIN0 = ReadVIN0*Foctor;
  HWTestVIN0 = ReadVIN0*Foctor*(Ra+Rb)/Rb; //(mV)
  
  Print(L"VIN0 Voltage Reading Register: 0x%02x\n", ReadVIN0);
  Print(L"VIN0 Voltage Reading Register * Foctor(11mV): %d (mV)\n", VoltageVIN0);
  Print(L"VIN0 Voltage Reading Register * Foctor(11mV) * (Ra+Rb)/Rb: %d (mV)\n", HWTestVIN0);
  
  //退出 IT8617E
  ExitITESIO();
  
  return EFI_SUCCESS;
}