=== 常识 ===
1. Print(L"XXX\n"); 的头文件位于 "MdePkg\Include\Library\UefiLib.h"
	>>> 
	#include <Library\UefiLib.h>	//OK! 
	//#include "UefiLib.h"	//Error
	//#include <UefiLib.h>	//Error

