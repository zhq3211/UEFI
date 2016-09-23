//***********************************************************************************************************
//	功能： (Sample Demo)
//		检测 SIO [IT8617E] Hardware Monitor 中的电压 VIN0.
//	BIOS：
//		1. 确认 HW 电路中，SIO 所使用的 VIN pin(如：VIN0), 以及外围电路的组成(Ra & Rb)
//		2. 确认 IT8617E Spec 中 VIN pin(VIN0) 的定义，以及解析系数(Foctor=11mV)
//		3. 读取 VIN0 的值: ReadVIN0
//		4. 计算 VIN0 电压: ReadVIN0*Foctor
//		5. 推算 HW 待测点的电压: V待测 = ReadVIN0*Foctor*(Ra +Rb)/Rb (mV)
//	注：某些值，是直接由 RW 读取后代入程序。
//***********************************************************************************************************
#include <Uefi.h>
#include <Library\UefiLib.h> 
#include <Library\UefiBootServicesTableLib.h>
#include "Library\BaseIoLibIntrinsic\BaseIoLibIntrinsicInternal.h" //_outp()

EFI_STATUS
EFIAPI
UefiMain(
	IN EFI_HANDLE		ImageHandle,
	IN EFI_SYSTEM_TABLE	*SystemTable
	)
{
  int ReadVIN0; //IT8617E VIN0 Pin
  int Foctor = 11;
  int Ra = 6650; //6.65千欧姆
  int Rb = 10000; //10千欧姆
  
  //进入 IT8617E
  _outp(0x2E,0x87);
  _outp(0x2E,0x01);
  _outp(0x2E,0x55);
  _outp(0x2E,0x55);
  
  //IT8617E 规定需要设置 BANK_SLE=0, 才能读取电压值 
  _outp(0x0A35,	0x06);
  _outp(0x0A36,	_inp(0x0A36) & 0x09FF);
  //Print(L"After set BANK_SLE: 0x%02x\n", _inp(0x0A36));
  
  //读取 VIN0 值(Index=20h)
  _outp(0x0A35,	0x20);
  ReadVIN0 = _inp(0x0A36)&0xFF; //不&0xFF, 职为0xFFFFFFE0
  
  //打印 VIN0读取值；VIN0电压值；HW待测点电压值
  Print(L"VIN0 Voltage Reading Register: 0x%02x\n", ReadVIN0); //0xE0 or 0xE1
  Print(L"VIN0 Voltage Reading Register * Foctor(11mV): %d (mV)\n", ReadVIN0*Foctor);
  Print(L"VIN0 Voltage Reading Register * Foctor(11mV) * (Ra+Rb)/Rb: %d (mV)\n", ReadVIN0*Foctor*(Ra+Rb)/Rb);
  
  //退出 IT8617E
  _outp(0x2E, 0x07);
  _outp(0x2F, 0x07);
  _outp(0x2E, 0x02);
  _outp(0x2F, _inp(0x2F) | 0x02 );

  return EFI_SUCCESS;
}