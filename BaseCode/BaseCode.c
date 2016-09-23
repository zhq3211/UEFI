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

  return EFI_SUCCESS;
}
