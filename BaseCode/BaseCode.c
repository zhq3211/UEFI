//************************
// 最基本的一个 UEFI 实例
//************************

#include <Uefi.h>
#include <Library\UefiLib.h>  //Print()  

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE			ImageHandle,
  IN EFI_SYSTEM_TABLE	*SystemTable
  )
{
  //Use UefiLib Print API to print string to UEFI console
  Print(L"Hi,everyone. This is the most basic UEFI APP!\n");
  Print(L"Are OK?\n");
  //SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Hello\n");
  //SystemTable->ConOut->OutputString(SystemTable->ConOut, L"world!\n");
 
  return EFI_SUCCESS;
}
