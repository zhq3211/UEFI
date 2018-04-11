//************************
// 最基本的一个 UEFI 实例
//************************

#include <Uefi.h>
#include <Library\UefiLib.h>  //Print()
#include <Library/IoLib.h> //IO

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE			ImageHandle,
  IN EFI_SYSTEM_TABLE	*SystemTable
  )
{
	//UINT32 CFG_ADDRESS = 0xCF8;
	//UINT32 CFG_DATA = 0xCFC;
	//UINT32 i;

	//IoWrite32(CFG_ADDRESS, 0x80000000); //B0<<16 + D0<<11 + F0<<8 + Reg  
	//i = IoRead32(CFG_DATA);
	//Print(L"B0D0F0 offset[00-04] = %0X\n", i);




  /*
  //Print
  //Use UefiLib Print API to print string to UEFI console
  Print(L"Hi,everyone. This is the most basic UEFI APP!\n");
  Print(L"Are OK?\n");

  //ConOut
  //顶格输出
  //换行后，下一行依旧是紧跟前一行的字符追加！
  SystemTable->ConOut->OutputString(SystemTable->ConOut, L"\n");
  SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Hello\n");
  SystemTable->ConOut->OutputString(SystemTable->ConOut, L"world!\n");
  SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Are");
  SystemTable->ConOut->OutputString(SystemTable->ConOut, L"you");
  SystemTable->ConOut->OutputString(SystemTable->ConOut, L"OK?");
  */
  return EFI_SUCCESS;
}
