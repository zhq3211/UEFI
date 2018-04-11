//******************************************************************************************
// ��ʱ�䡿2017.08.23 by zhq3211 
//
// �����ܡ���I/O��ʽ ��������ϵͳ������ PCI Device����������� PCI ǰ256byte
// 
// ���߼���(��������Ч�ʵ��жϷ�ʽ) Verdor ID Ϊ FFFF_FFFFh(�Ƿ�����), �� PCI device ������.
//
// ���㷨��I/O ��ʽ Access:
//   IoAddress = (0x01<<31) + (Bus<<16)   + (Dev<<11) + (Fun<<8) + Register [Code]
//             = 8000_0000h + Bus*1_0000h + Dev*8000h + Fun*100h + Register [RW]
//
// ��ע�����
// 1. bus ��������
// 2. << �� + ���ȼ�
// 3. �Ű�
//******************************************************************************************

#include <Uefi.h>
#include <Library/UefiLib.h>	//Print()
#include <Library/UefiApplicationEntryPoint.h>
#include <Library\IoLib.h>		//IoWrite32() & IoRead32()

//IO Address & DATA Port
#define	CFG_ADDRESS	0xCF8	
#define	CFG_DATA	0xCFC

//BUS-DEV-FUN ȡֵ��Χ
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
    UINT16 bus; //������ UINT8, 0-255��for(bus=0,bus<256,bus++s) Ϊ��ѭ��
	UINT8  device;
	UINT8  function;
	UINT32 address, data;
	UINT8  i;
	UINT8  j;
	UINT8  reg =0;
	
	//[��ӡ��ʾ] ��ʵ������
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
		  //���� Verdor ID ��ַ
		  address = 0x80000000 + (bus<<16) + (device<<11) + (function<<8);
		  
		  IoWrite32(CFG_ADDRESS, address);
		  data = IoRead32(CFG_DATA);
		  
		  //Print(L"Verdor ID = %08X\n", data); //Debug
		 
		  if(data != 0xFFFFFFFF)
		  {
		    //[��ӡ��ʾ] �Ű棬����λʮ�����ƴ�д��ʾ ��ǰ���� BUS-DEV-FUN 
			Print(L"\n");
		    Print(L"-----------------------------------------------\n");
			Print(L"===     Bus#%02X��Device#%02X, Function#%02X      ===\n",bus, device,function);
	        Print(L"===       Register [00h-FFh] as below       ===\n");
			Print(L"-----------------------------------------------\n");
	        Print(L" [offset] | 03020100 07060504 0B0A0908 0F0E0D0C\n"); //װ�ζ���
			Print(L"----------+------------------------------------\n"); //װ�ζ���
			
            for(i=0; i<16; i++)
	        {
	          Print(L"    %0X0    | ", i); //װ�ζ���
	          for(j=0; j<4; j++)
	          {
	            address = 0x80000000 + (bus<<16) + (device<<11) + (function<<8)+ reg; //ע�����ȼ�
	            IoWrite32(CFG_ADDRESS, address);
		        data = IoRead32(CFG_DATA);
                Print(L"%08X ", data); //��8����дʮ�������ַ�(����)��ʾ
	            reg += 4;
              }
	          Print(L"\n");
	        }//for ��ӡ��ǰ PCI Device �� 256 bytes.
		  }//for �ж� Verdor ID ���ǷǷ�����
		}
	  }
	}////���� PCI list
	
    return Status;	
}
