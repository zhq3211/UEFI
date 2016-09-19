//*********************************************************************
//	Demo
//		1. 尝试 Print 不同的输出格式
//		2. Print(L“XX\n”)；XX 不能是中文，不然运行 *.efi 时不显示！！！
//		3. Print 不能打印 TAB
//		4. Print 不能打印字符型；且定义 float 型报错
//*********************************************************************

#include <Library\UefiLib.h> //Print()

EFI_STATUS
EFIAPI
UefiMain(
	IN EFI_HANDLE		ImageHandle,
	IN EFI_SYSTEM_TABLE	*SystemTable
	)
{
  int i = 0x0F;
  //float f = 0.1; //Error

  Print(L"int i=0x0F, and print it through different type!\n");
  Print(L"[Type] | [Input]\n");
  Print(L"c   >>> %c\n", i);
  Print(L"d   >>> %d\n", i);
  Print(L"x   >>> %x\n", i);
  Print(L"#x  >>> %#x\n", i);
  Print(L"02x >>> %02x\n", i);
  Print(L"2x  >>> %2x\n", i);
  Print(L"08x >>> %08x\n", i);
  Print(L"8x  >>> %8x\n", i);
  //Print(L"%c\n", f);
  
  return EFI_SUCCESS;
}

























/*
//SMBUS
#include <Library\UefiLib.h> //Print()


EFI_STATUS
EFIAPI
main(
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
outp(0xF040,0xFF);
Print(L"%#x\n",inp(0xF040));

outp(0xF040+0X04,0xA1);
outp(0xF040+0X03,0x00);
outp(0xF040+0X02,0x48);

Print(L"%#x",inp(0xF040+0X05));

  return EFI_SUCCESS;
}
*/




/*
#include <Library\UefiLib.h> 
//#include "D:\EDK2\MyWorkSpace\CryptoPkg\Include\stdio.h"
#include <stdio.h>
#include "D:\EDK2\MyWorkSpace\BaseTools\Source\C\VfrCompile\Pccts\h\pcctscfg.h"
#include "D:\EDK2\MyWorkSpace\MdePkg\Include\Protocol\hash.h"
//#include <float.h>

EFI_STATUS
EFIAPI
main(
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{

  //float a = 10;
  //char b = 17;
  //double b = 17.8;
  //int b = 17;
  //float b = 1.0;
  //UINT32 a = (*((UINT32*) &b));
  //a = a + b;
  //Print(L"Test! %d\n",a);
  //fprintf(stderr,"%f\n",b);
  //printf("%f\n",b);
  return EFI_SUCCESS;
}
*/


/*
#include <Library\UefiLib.h> 
//#include "D:\EDK2\MyWorkSpace\CryptoPkg\Include\stdio.h"
#include <stdio.h>
#include "D:\EDK2\MyWorkSpace\BaseTools\Source\C\VfrCompile\Pccts\h\pcctscfg.h"
#include "D:\EDK2\MyWorkSpace\MdePkg\Include\Protocol\hash.h"
//#include <float.h>

EFI_STATUS
EFIAPI
main(
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{

  //float a = 10;
  //char b = 17;
  //double b = 17.8;
  //int b = 17;
  //float b = 1.0;
  //UINT32 a = (*((UINT32*) &b));
  //a = a + b;
  //Print(L"Test! %d\n",a);
  //fprintf(stderr,"%f\n",b);
  //printf("%f\n",b);
  return EFI_SUCCESS;
}
*/





//////////////////////////////////////////////////
/*
#include <Library\UefiLib.h> 
#include <Library\UefiBootServicesTableLib.h>
//_output()
//#include "Library\BaseIoLibIntrinsic\BaseIoLibIntrinsicInternal.h"


//进入 ITE SIO PnP Mode
void EntryITESIO()
{
  _outp(0x2E,0x87);
  _outp(0x2E,0x01);
  _outp(0x2E,0x55);
  _outp(0x2E,0x55);
}

//退出 ITE SIO
void ExitITESIO()
{
  //进入 LDN =07h
  _outp(0x2E,0x07);
  _outp(0x2F,0x07);
  
  //Index=02h bit<1>=1
  _outp(0x2E,0x02);
  _outp(0x2F, _inp(0x2F) | 0x20 );
}



EFI_STATUS
EFIAPI
UefiMain(
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  
  EntryITESIO();

  Print(L"Test!\n");
  return EFI_SUCCESS;
}
*/



////////////////////////////////////////////////////
/*
#include <Library\UefiLib.h> 
#include <Library\UefiBootServicesTableLib.h>

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE            ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
	int i;
	for (i = 0; i < 0x10; i++)
  //_asm int 3
  // Use UefiLib Print API to print string to UEFI console
  Print(L"Test!\n");
  
  //for (i = 0; i < 0x10; i++)
  //_asm int 3
  //UefiBootServicesTableLib.h
  gST-> ConOut -> OutputString(gST -> ConOut, L"zhq_test\n");
  
  return EFI_SUCCESS;
}
*/
