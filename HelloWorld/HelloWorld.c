#include <Uefi.h>

EFI_STATUS
MainHelloWorld(
	IN EFI_HANDLE		ImageHandle,
	IN EFI_SYSTEM_TABLE	*SystemTable
	)
{
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Hello World!\n");
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"BIOS first test...\n");

	return EFI_SUCCESS;
}


