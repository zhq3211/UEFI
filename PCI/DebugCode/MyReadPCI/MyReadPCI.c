//******************************************************************************************
// 【时间】2017.08.23 by zhq3211 
//
// 【功能】以I/O方式 遍历所有系统上所有 PCI Device，并输出各个 PCI 前256byte
// 
// 【逻辑】(采用最无效率的判断方式) Verdor ID 为 FFFF_FFFFh(非法厂商), 则 PCI device 不存在.
//
// 【算法】I/O 方式 Access:
//   IoAddress = (0x01<<31) + (Bus<<16)   + (Dev<<11) + (Fun<<8) + Register [Code]
//             = 8000_0000h + Bus*1_0000h + Dev*8000h + Fun*100h + Register [RW]
//
// 【注意事项】
// 1. bus 定义类型
// 2. << 与 + 优先级
// 3. 排版
//******************************************************************************************

#include <Uefi.h>
#include <Library/UefiLib.h>	//Print()
#include <Library/UefiApplicationEntryPoint.h>
#include <Library\IoLib.h>		//IoWrite32() & IoRead32()

//IO Address & DATA Port
#define	CFG_ADDRESS	0xCF8	
#define	CFG_DATA	0xCFC

//BUS-DEV-FUN 取值范围
#define BUS_MAX     256
#define DEV_MAX     32
#define FUN_MAX     8

EFI_STATUS
EFIAPI
MyReadPCIMain (
	IN EFI_HANDLE		ImageHandle,
	IN EFI_SYSTEM_TABLE	*SystemTable
	)
{	
	EFI_STATUS	Status = EFI_SUCCESS;	
    UINT16 bus; //若定义 UINT8, 0-255，for(bus=0,bus<256,bus++s) 为死循环
	UINT8  device;
	UINT8  function;
	UINT32 address, data;
	UINT8  i;
	UINT8  j;
	UINT8  reg =0;
	
	//[打印提示] 无实际作用
	Print(L"***********************************************\n");
	Print(L"**           2017.08.23 by zhq3211           **\n");
	Print(L"**   List System's PCI Device & 256 bytes    **\n");
	Print(L"***********************************************\n");
	
	for(bus=0; bus<BUS_MAX; bus++)
	{  
	  for(device=0; device<DEV_MAX; device++)
	  {
		for(function=0; function<FUN_MAX; function++)
		{
		  //计算 Verdor ID 地址
		  address = 0x80000000 + (bus<<16) + (device<<11) + (function<<8);
		  
		  IoWrite32(CFG_ADDRESS, address);
		  data = IoRead32(CFG_DATA);
		  
		  //Print(L"Verdor ID = %08X\n", data); //Debug
		 
		  if(data != 0xFFFFFFFF)
		  {
		    //[打印提示] 排版，以两位十六进制大写显示 当前所属 BUS-DEV-FUN 
			Print(L"\n");
		    Print(L"-----------------------------------------------\n");
			Print(L"===     Bus#%02X，Device#%02X, Function#%02X      ===\n",bus, device,function);
	        Print(L"===       Register [00h-FFh] as below       ===\n");
			Print(L"-----------------------------------------------\n");
	        Print(L" [offset] | 03020100 07060504 0B0A0908 0F0E0D0C\n"); //装饰而已
			Print(L"----------+------------------------------------\n"); //装饰而已
			
            for(i=0; i<16; i++)
	        {
	          Print(L"    %0X0    | ", i); //装饰而已
	          for(j=0; j<4; j++)
	          {
	            address = 0x80000000 + (bus<<16) + (device<<11) + (function<<8)+ reg; //注意优先级
	            IoWrite32(CFG_ADDRESS, address);
		        data = IoRead32(CFG_DATA);
                Print(L"%08X ", data); //以8个大写十六进制字符(补零)显示
	            reg += 4;
              }
	          Print(L"\n");
	        }//for 打印当前 PCI Device 的 256 bytes.
		  }//for 判断 Verdor ID 不是非法厂商
		}
	  }
	}////遍历 PCI list
	
    return Status;	
}
