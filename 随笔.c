QA：
1. UEFI 中是否有输入函数？如 scanf
2. float 的定义头文件？因为直接使用，报错



=== 常识 ===
1. Print(L"XXX\n"); 的头文件位于 "EDK2-18916\MdePkg\Include\Library\UefiLib.h"
	>>> 
	#include <Library\UefiLib.h>	//OK! 
	//#include "UefiLib.h"	//Error
	//#include <UefiLib.h>	//Error

2. IoWrite8() & IoRead8() 的头文件位于 "EDK2-18916\MdePkg\Include\Library\IoLib.h"
	>>>
	#include <Library\IoLib.h> //OK!
	//#include <MdePkg\Include\Library\IoLib.h>  // Error: No such file or directory
	//#include "MdePkg\Include\Library\IoLib.h"  // Error: No such file or directory
	

