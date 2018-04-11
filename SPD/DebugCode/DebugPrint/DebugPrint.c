//*********************************************************************
// [时间] 2016.04.07 by zhq3211
// [功能] 已知 SMB_BASE 后，通过 SMBus Protocol， 
//        以 Byte Read 方式读取 SPD(256bytes) 信息！
// [逻辑] 由南桥 Spec - PCI 查找 SMBus 的基地址位置，RW读取其值；
//        由 HW 电路图获取 SPD slave Address;
//        透过 I/O, 以 Byte Read 读取 SPD 256 bytes.
// [编程] 编写 MySPD.inf & MySPD.c;
//        将 MySPD.inf 添加到 MdeModulePkg.dsc 的 [Components];
//        build 生成 *.efi;
//        Shell U 盘中运行.
// [注意事项]
//    1. 若将 *.inf 添加到 Nt32Pkg.dsc,仿真环境下运行此 *.efi 会卡住！
//    2. 程序运行中, Debug 跟踪 HST_STS 的变化情况：
//      - HST_STS 写入 FFh 后， 变化： 
//        由 xxh -> 00h -> 40h
//      - SMB_BASE+0x02 写入 ByteRead 指令(48h)后， HST_STS 变化:
//        由 xxh -> 41h -> 42h or 44h
//*********************************************************************

#include <Uefi.h>
#include <Library/UefiLib.h> //Print()
#include <Library/UefiApplicationEntryPoint.h>
#include <Library\IoLib.h> //IoWrite8() & IoRead8()
//#include <MdePkg\Include\Library\IoLib.h>  // Error: No such file or directory
//#include "MdePkg\Include\Library\IoLib.h"  // Error: No such file or directory

// 宏定义
#define	SMB_BASE	 0xF040	//PCI(B00D31F03) [0ffset]20h-23h
#define SPD_Address  0xA0   //HW

EFI_STATUS
EFIAPI
MySPDMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{	
    EFI_STATUS        Status =  EFI_SUCCESS ;
    UINT8             i,j,SPD_OFFSET,DATA;
    
    for(i=0; i<16; i++)
    {
	    for(j=0;j<16;j++)
	    {
		    SPD_OFFSET = i*16+j;
		    
		    IoWrite8(SMB_BASE+0x00, 0xFF);
		    
		    //Print(L" "); //(1. Print- 无效, hang机)
	        //Print(L" \n"); //提供延时读取HST_STS (2.PrintN 有效，结果OK)
         	//Print(L"[Debug] HST_STS\n"); //(6.PrintDebug 有效，结果OK)
			Print(L"\n[Debug]1: HST_STS= %02Xh\n",IoRead8(SMB_BASE+0x00)); 
			Print(L"[Debug]2: HST_STS= %02Xh\n",IoRead8(SMB_BASE+0x00)); 
		    /*
			while( (IoRead8(SMB_BASE+0x00)&0x40) != 0x40)
			{
			  //等待 INUSE_STS bit<6>=1, 则继续
			  //HST_STS: xxh -> 00h -> 40h
			} */
			
		    IoWrite8(SMB_BASE+0x04, SPD_Address+1); //(Slave Address + 1) to Read way
		    IoWrite8(SMB_BASE+0x03, SPD_OFFSET); //Read Which SPD offset
		    IoWrite8(SMB_BASE+0x02, 0x48); //Byte Read Command
		    
		    Print(L"[Debug]3: HST_STS= %02Xh\n",IoRead8(SMB_BASE+0x00)); 
			Print(L"[Debug]4: HST_STS= %02Xh\n",IoRead8(SMB_BASE+0x00)); 
		    /*
			while( (IoRead8(SMB_BASE+0x00)&0x42) != 0x42)
			{
			  //等待 INTR bit<1>=1, 则继续
			  //HST_STS: xxh -> 41h -> 42h
			}*/
			
		    DATA = IoRead8(SMB_BASE+0x05); //Read the SPD data
		    
		    Print(L" %2X",DATA);
	    }//打印16bytes
        
		Print(L"\n");
    }//打印16*16bytes
	
    return Status;	
}
