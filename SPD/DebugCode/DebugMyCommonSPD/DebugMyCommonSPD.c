//*********************************************************************************
// [时间] 2017.08.24
// [功能] 读取 Intel 8系列平台，系统中所有的 SPD 信息.
// [逻辑] 1. 参考南桥 EDS，获取 SMBus Controller 对应的 PCI 及 SMB_BASE;
//           - PCI(B00,D31,F03), 0ffset[20h-23h], bit<15-5>
//        2. 以 SMBus Protocol 方式遍历 SPD Slave Address(A0/A2/A4/A6);
//        3. 判断系统上是否挂载对应 SPD，有，则显示 256bytes.
//
// [注意事项]
//   1. 此程序只适应类似 Intel 8系列平台, SMBus 位于 PCI(B00,D31,F03)
//   2. SMB_BASE 位于 offset[20h-30h] bit<15-5>
//   3. SMBus Protocol 读取数据的条件, 注意 HST_STS bit<6> 和 bit<2>
//   4. 读取 HST_STS 时，★ (有无 while 无差异)
//        while( (IoRead8(SMB_BASE+0x00)&0x40) != 0x40); 
//      前后有无 Print 是有差别的！！！
//      - 前后均 1个Print, 可以看到执行 Read(0x48)后，HST_STS 由 41h 变成 42h 或者 44h
//      - 仅前面 1个Print, 结果正确，HST_STS = 41h
//      - 仅后面 1个Print, 结果虽正确，但 HST_STS = 41h (理论上应该为 42h or 44h)
//      - 前面 2个 Print, 结果虽正确， 但 HST_STS 都是 42h or 44h (理论上 while 前为 41h)
//**********************************************************************************

#include <Uefi.h>
#include <Library/UefiLib.h>	//Print()
#include <Library/UefiApplicationEntryPoint.h>
#include <Library\IoLib.h>		//IoWrite8/32() & IoRead8/32()

//PCI - I/O Address/Data Port
#define	CFG_ADDRESS	0xCF8	
#define	CFG_DATA	0xCFC

//PCI - SMbus Controller, SMB_BASE 所在偏移
#define BUS 0x00
#define DEV 0x1F
#define FUN 0x03
#define REG 0x20


EFI_STATUS
EFIAPI
MyCommonSPDMain (
	IN EFI_HANDLE		ImageHandle,
	IN EFI_SYSTEM_TABLE	*SystemTable
	)
{	
	EFI_STATUS Status = EFI_SUCCESS ;

	UINT32 PCIAddr;
	UINT32 SMB_BASE;
	UINT8  SPDAddress;
	UINT8  i,j;
	UINT8  SPD_OFFSET;
	UINT8  data;
	
	//[打印] Title 无实际作用
    Print(L"********************************************\n");
	Print(L"**          2017.08.24 by zhq3211         **\n");
	Print(L"**       Base on Intel 8 Series PCH       **\n");
	Print(L"** Read SPD Information by SMBus Protocol **\n");
	Print(L"********************************************\n");
	
	Print(L"\n");
	Print(L"===  Get SMB_BASE from PCI(B00,D1F,F03)  ===\n");
	Print(L"===      offset[20H-30h] bit<15-5>       ===\n");
	PCIAddr = 0x80000000 + (BUS<<16) + (DEV<<11) + (FUN<<8) + REG;
	IoWrite32(CFG_ADDRESS, PCIAddr);
    SMB_BASE = IoRead32(CFG_DATA) & 0xFFE0;
	Print(L"SMB_BASE = %08Xh\n\n", SMB_BASE);
	
	//获取系统上有哪几个 SPD device(A0/A2/A4/A6), 并输出相应 256bytes.
	for(SPDAddress = 0xA0; SPDAddress <= 0xA6 ; SPDAddress=SPDAddress+2)
	{
	  IoWrite8(SMB_BASE+0x00, 0xFF);
	  
	  //Print(L"[Debug] Before while() HST_STS = %02Xh\n", IoRead8(SMB_BASE+0x00));
      while( (IoRead8(SMB_BASE+0x00)&0x40) != 0x40)
	  {
	    //(INUSE_STS - bit6) =1, 则写 0xFF 生效！
	  }
	  //Print(L"[Debug] After while() HST_STS = %02Xh\n", IoRead8(SMB_BASE+0x00));
	  
	  IoWrite8(SMB_BASE+0x04, SPDAddress+1); //bit<0>=1 -> Read
      IoWrite8(SMB_BASE+0x03, 0x00); //SPD offset[00h]
      IoWrite8(SMB_BASE+0x02, 0x48); //Byte Read Command
	  
	  Print(L"[Debug]Before while(): HST_STS= %02Xh\n",IoRead8(SMB_BASE+0x00)); //(41h - 1.PrintBefore 有效)
      //while( (IoRead8(SMB_BASE+0x00)&0x40) != 0x40)
	  //{
	    //(INUSE_STS - bit6) =1, 则写 0xFF 生效！
	  //}
      Print(L"[Debug]After while(): HST_STS= %02Xh\n",IoRead8(SMB_BASE+0x00)); //(42h 0r 44h - 2.PrintAfter 有效)
	  
	  //Print(L" "); //(3.Print- 无效, hang机)
	  //Print(L" \n"); //提供延时读取HST_STS (4.PrintN 有效，结果OK)
	  //Print(L"[Debug] HST_STS\n"); //(5.PrintDebug 有效，结果OK)
      if( (IoRead8(SMB_BASE+0x00) & 0x04) == 0x04 ) //bit<2>=1, 则为非法/无设备!
	  {
		Print(L"The SPD Address %02Xh is absent.\n", SPDAddress);
		Print(L"\n");
	  }
	  else
	  { 
	    Print(L"The SPD Address %02Xh is OK!\n", SPDAddress);
	    Print(L"===   The SPD Information [00h-FFh] 256bytes as below   ===\n");
	    Print(L"-----------------------------------------------------------\n");
	    Print(L" [offset] | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n"); //装饰而已
	    Print(L"----------+------------------------------------------------\n"); //装饰而已
		for(i=0; i<16; i++)
        {
		  Print(L"    %0X0    | ", i); //装饰而已
	      for(j=0;j<16;j++)
	      {
		    SPD_OFFSET = i*16+j;
		    
		    IoWrite8(SMB_BASE+0x00, 0xFF);
		    while( (IoRead8(SMB_BASE+0x00)&0x40) != 0x40)
			{
			  //等待 INUSE_STS bit<6>=1, 则继续
			} 
		    IoWrite8(SMB_BASE+0x04, SPDAddress+1);
		    IoWrite8(SMB_BASE+0x03, SPD_OFFSET);
		    IoWrite8(SMB_BASE+0x02, 0x48); //Byte Read Command
		    while( (IoRead8(SMB_BASE+0x00)&0x42) != 0x42)
			{
			  //等待 INTR bit<1>=1, 则继续
			}
		    data = IoRead8(SMB_BASE+0x05); //Read SPD data
		    
		    Print(L"%2X ",data);
	      }
		  Print(L"\n"); //排版
        }//打印16*16bytes
		Print(L"\n");
	  }//else
	}
	
    return Status;	
}
