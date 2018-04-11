【常识】
【QA】


【常识】
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
	
3. _outp(x,x); inp(x); 的头文件位于 "EDK2-18916\MdePkg\Library\BaseIoLibIntrinsic\BaseIoLibIntrinsicInternal.h"
	>>>
	#include "Library\BaseIoLibIntrinsic\BaseIoLibIntrinsicInternal.h" 
	
1. shell 下，按 F7 >>> 显示历史输出命令

2. 不修改*.c 和 *.inf, build 是不会重新生成 *.efi 的。

3. *.efi > *.txt 类似于 DOS，可用！


【QA】
1. UEFI 中是否有输入函数？如 scanf
2. float 的定义头文件？因为直接使用，报错
3. *.dsc 的[Components]中引用工程目录外的路径，报错
4. UEFI 中没有 %c, %#x 的输出格式？Error
5. _input(); _outp() 为 int 格式！
6. 调试窗口的调出：两个 or 一个？
7. 定义类型时，最好优先使用无符号类型：UINT8... 而不是 int,INT8...
8. Print(L"XXX\n"); 不能输出 TAB，中文？
9. 延时程序：0xFFFF_FFFF！
	void Delay(){
	UINT16 i; //INT16
		for(i=0x4FFF; i>0; i-- ){
		//; //无任何语句,则没有延时效果！ Why???
		//Print(L"%04x ", i); //有延时
		//Print(L""); //无延时
		Print(L" "); //有延时(0x3FFF-18s左右; 0x4FFF-22s; 0x8FFF-40s左右)
		}
	}



