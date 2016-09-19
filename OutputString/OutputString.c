//******************************************
// Example
//    1. SystemTable->ConOut->OutputString()
//    2. gST->ConOut->OutputString()
//******************************************

#include <Library\UefiBootServicesTableLib.h> //gST->ConOut->OutputString

EFI_STATUS
EFIAPI
UefiMain(
	IN EFI_HANDLE		ImageHandle,
	IN EFI_SYSTEM_TABLE	*SystemTable
	)
{
  SystemTable->ConOut->OutputString(SystemTable->ConOut, L"This is a SystemTable->ConOut->OutputString() test!\n");
  SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Welcome to GitHub!\n");
  
  //UefiBootServicesTableLib.h
  gST->ConOut->OutputString(gST->ConOut, L"This is a gST->ConOut->OutputString() test!\n");
  gST->ConOut->OutputString(gST->ConOut, L"Welcome to zhuxain world!\n");
  
  return EFI_SUCCESS;
}