//*********************************************************************
// [ʱ��] 2016.04.07 by zhq3211
// [����] ��֪ SMB_BASE ��ͨ�� SMBus Protocol�� 
//        �� Byte Read ��ʽ��ȡ SPD(256bytes) ��Ϣ��
// [�߼�] ������ Spec - PCI ���� SMBus �Ļ���ַλ�ã�RW��ȡ��ֵ��
//        �� HW ��·ͼ��ȡ SPD slave Address;
//        ͸�� I/O, �� Byte Read ��ȡ SPD 256 bytes.
// [���] ��д MySPD.inf & MySPD.c;
//        �� MySPD.inf ��ӵ� MdeModulePkg.dsc �� [Components];
//        build ���� *.efi;
//        Shell U ��������.
// [ע������]
//    1. ���� *.inf ��ӵ� Nt32Pkg.dsc,���滷�������д� *.efi �Ῠס��
//    2. ����������, Debug ���� HST_STS �ı仯�����
//      - HST_STS д�� FFh �� �仯�� 
//        �� xxh -> 00h -> 40h
//      - SMB_BASE+0x02 д�� ByteRead ָ��(48h)�� HST_STS �仯:
//        �� xxh -> 41h -> 42h or 44h
//*********************************************************************

#include <Uefi.h>
#include <Library/UefiLib.h> //Print()
#include <Library/UefiApplicationEntryPoint.h>
#include <Library\IoLib.h> //IoWrite8() & IoRead8()
//#include <MdePkg\Include\Library\IoLib.h>  // Error: No such file or directory
//#include "MdePkg\Include\Library\IoLib.h"  // Error: No such file or directory

// �궨��
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
		    
		    //Print(L" "); //(1. Print- ��Ч, hang��)
	        //Print(L" \n"); //�ṩ��ʱ��ȡHST_STS (2.PrintN ��Ч�����OK)
         	//Print(L"[Debug] HST_STS\n"); //(6.PrintDebug ��Ч�����OK)
			Print(L"\n[Debug]1: HST_STS= %02Xh\n",IoRead8(SMB_BASE+0x00)); 
			Print(L"[Debug]2: HST_STS= %02Xh\n",IoRead8(SMB_BASE+0x00)); 
		    /*
			while( (IoRead8(SMB_BASE+0x00)&0x40) != 0x40)
			{
			  //�ȴ� INUSE_STS bit<6>=1, �����
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
			  //�ȴ� INTR bit<1>=1, �����
			  //HST_STS: xxh -> 41h -> 42h
			}*/
			
		    DATA = IoRead8(SMB_BASE+0x05); //Read the SPD data
		    
		    Print(L" %2X",DATA);
	    }//��ӡ16bytes
        
		Print(L"\n");
    }//��ӡ16*16bytes
	
    return Status;	
}
