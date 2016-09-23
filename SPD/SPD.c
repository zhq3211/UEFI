//**************************************
//	Hiwen 2016.04.07 
//	通过 SMBus Protocol 读取 SPD 信息！
//	Byte Read SPD data(256bytes)
//注：（将 *.inf 添加到 MdeModulePkg.dsc）
//	1. 仿真环境下，运行此 *.efi 会卡住！
//	2. 需要在 Shell U盘下执行！！！
//**************************************

#include <Uefi.h>
#include <Library/UefiLib.h>	//Print()
#include <Library/UefiApplicationEntryPoint.h>
#include <Library\IoLib.h>		//IoWrite8() & IoRead8()

// 宏定义
// SMBus Base Address 需要按照实际的南桥EDS Spec.
// SPD slave Address 需要参考 WH 的电路设计.
#define	SMB_BASE	0xF040	//PCI - B00D31F03 0ffset = 20h-23h
#define	SPD_Address	0xA0

EFI_STATUS
EFIAPI
SPDMain (
	IN EFI_HANDLE		ImageHandle,
	IN EFI_SYSTEM_TABLE	*SystemTable
	)
{	
	EFI_STATUS	Status	=	EFI_SUCCESS ;
	UINT8		i,j,SPD_OFFSET,DATA;
    
    for(i=0; i<16; i++)
    {
	    for(j=0;j<16;j++)
	    {
		    SPD_OFFSET = i*16+j;
		    
		    IoWrite8(SMB_BASE+0x00, 0xFF); //Clear to 0x40
		    
			//Wait - Make sure "SMB_BASE+0x00" = "0x40" to continue
		    while( (IoRead8(SMB_BASE+0x00)&0x40) != 0x40); 
			
		    IoWrite8(SMB_BASE+0x04, SPD_Address+1); //(Slave Address + 1) to Read way
		    IoWrite8(SMB_BASE+0x03, SPD_OFFSET); //Read Which SPD offset
		    IoWrite8(SMB_BASE+0x02, 0x48); //Byte Read Command
		    
			//Wait - Make sure "SMB_BASE+0x00" = "0x42" to continue, read DATA
		    while( (IoRead8(SMB_BASE+0x00)&0x42) != 0x42); 
			
		    DATA = IoRead8(SMB_BASE+0x05); //Read the SPD data
		    
		    Print(L" %2X",DATA);
	    }
        
		Print(L"\n");
    } 
	  
    IoWrite8(SMB_BASE+0x00, 0xFF); //Clear to 0x40
    while( (IoRead8(SMB_BASE+0x00)&0x40) != 0x40); //Make sure clear
    return Status;	
}
