//***************************************************************************************************
// 功能：I/O 方式访问 B0D31F3 (SMBus Controller) 前256bytes
//
//一、 I/O 方式 Access:
//    (0x01 << 31 )  // Type-1 PCI Configuration
//	+ (0x00 << 16 )  // Bus#0
//	+ (0x00 << 11 )  // Device#0
//	+ (0x00 << 8 )   // Function#0
//	+ 0x00 			 // Register#00
//
//二、 Memory 方式 Access
//1. PCIe 对应的 Memory 基地址 (PCI Express Base Address)： Bus0; Device0; Function0 的 Offset60h-67h。
//2. Address = PCIEXBAR + Bus Number * 1MB + Device Number * 32KB + Function Number * 4KB
//             
//           = PCIEXBAR + Bus_No.*1MB + Device_No.*32KB + Fun_No.*4KB
//           = PCIEXBAR + Bus*100000h + Device_No*8000h + Fun*1000h
//           = PCIEXBAR + Bus_No.<<20 + Device_No.<<15  + Fun_No.<<12 (//Code)
//****************************************************************************************************

#include <Uefi.h>
#include <Library/UefiLib.h>	//Print()
#include <Library/UefiApplicationEntryPoint.h>
#include <Library\IoLib.h>		//IoWrite32() & IoRead32()

#define	CFG_ADDRESS	0xCF8	
#define	CFG_DATA	0xCFC

//Bus#00 Device#1F Function#03 - SMBus Controller
#define BUS       0x00
#define DEVICE    0x1F
#define FUNCTION  0x03

EFI_STATUS
EFIAPI
MySMBusPCIMain (
	IN EFI_HANDLE		ImageHandle,
	IN EFI_SYSTEM_TABLE	*SystemTable
	)
{	
	EFI_STATUS	Status	=	EFI_SUCCESS ;	

	UINT32 Address; //IO 方式访问的基地址
	UINT32 Data;
	UINT8  Register = 0; //BusDevFun 中的偏移量
	UINT8  i,j;
   
    //*********************************************************************
	//I/O 方式访问 Bus#0 Device#1F Function#3 - (SMBus Controller) 前256bytes
    //*********************************************************************
	Print(L"=== Bus#00，Device#1F, Function#03 - (SMBus Controller) ===\n");
	Print(L"-----------------------------------------------------------\n");
	Print(L"   03020100 07060504 0B0A0908 0F0E0D0C\n"); //装饰而已
    for(i=0; i<16; i++)
	{
	  Print(L"%0X0 ", i); //装饰而已
	  for(j=0; j<4; j++)
	  {
	    Address = (0x01<<31) + (BUS<<16) + (DEVICE<<11) + (FUNCTION<<8)+ Register; //注意优先级
	    IoWrite32(CFG_ADDRESS, Address);
		Data = IoRead32(CFG_DATA);
		Print(L"%08X ", Data); //以8个大写十六进制字符(补零)显示
		Register += 4;
 	  }
	  Print(L"\n");
	}
   
    return Status;	
}
