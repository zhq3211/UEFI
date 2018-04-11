//***************************************************************************************************
// 功能：I/O 遍历所有 PCI Device，列出挂载的 PCI.
// 逻辑：
//   1. (最简易，无效率)遍历 BUS#0-255,DEV#0-32, FUN#0-8，
//   2.1 判断 Verdor ID 是否不为 FFFF_FFFFh. [但，Bus#n Dev#n Fun#0 读取的值都不为0？]
//   2.2 也可以以 class code 作为判断是否存在 PCI 设备.
//   3. (提高效率) 参考 PCI-to-PCI 桥架构，省去不必要的遍历.
//
//一、 I/O 方式 Access:
//    (0x01 << 31 )  // Type-1 PCI Configuration
//	+ (0x00 << 16 )  // Bus#0       (bit<23-16>: 2^8=256)
//	+ (0x00 << 11 )  // Device#0    (bit<15-11>: 2^5=32)
//	+ (0x00 << 8 )   // Function#0  (bit<10-8>: 2^3=8)
//	+ 0x00 			 // Register#00
//
//二、 Memory 方式 Access
//1. PCIe 对应的 Memory 基地址 (PCI Express Base Address)： Bus0; Device0; Function0 的 Offset60h-67h。
//2. Address = PCIEXBAR + Bus Number * 1MB + Device Number * 32KB + Function Number * 4KB
//             
//           = PCIEXBAR + Bus_No.*1MB + Device_No.*32KB + Fun_No.*4KB
//           = PCIEXBAR + Bus*100000h + Device_No*8000h + Fun*1000h
//           = PCIEXBAR + Bus_No.<<20 + Device_No.<<15  + Fun_No.<<12 (//Code)
//
//
//	=== QA ===
//  1. << 打成了 <.
//  2. bus 定义成 UINT8(0-255), 永远<256. >>> for 死循环
//****************************************************************************************************

#include <Uefi.h>
#include <Library/UefiLib.h>	//Print()
#include <Library/UefiApplicationEntryPoint.h>
#include <Library\IoLib.h>		//IoWrite32() & IoRead32()

#define	CFG_ADDRESS	0xCF8	
#define	CFG_DATA	0xCFC
#define BUS_MAX     256
#define DEV_MAX     32
#define FUN_MAX     8

EFI_STATUS
EFIAPI
MyScanPCIMain (
	IN EFI_HANDLE		ImageHandle,
	IN EFI_SYSTEM_TABLE	*SystemTable
	)
{	
	EFI_STATUS	Status	=	EFI_SUCCESS;	
    UINT16 bus; //若定义 UINT8, 0-255，for(bus=0,bus<256,bus++s) 为死循环
	UINT8  device, function;
	UINT32 address, data;
	
	Print(L"=== List the PCI device of current PC ===\n");
	for(bus=0; bus<BUS_MAX; bus++)
	{  
	  for(device=0; device<DEV_MAX; device++)
	  {
		for(function=0; function<FUN_MAX; function++)
		{
		  //计算 Verdor ID, 并判断
		  address = 0x80000000 + (bus<<16) + (device<<11) + (function<<8);
		  
		  IoWrite32(CFG_ADDRESS, address);
		  data = IoRead32(CFG_DATA);
		  
		  //Print(L"Verdor ID = %08X\n", data); //Debug
		  if(data != 0xFFFFFFFF)
		  {
		    Print(L"Bus %02X, Device %02X, Function %02X\n", bus,device,function); //以两位十六进制大写显示
		  } 
		}
	  }
	}
	
    return Status;	
}
