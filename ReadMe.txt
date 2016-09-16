UEFI 基本原理中，基本代码。

=== 编写程序 ===
1. EDK2 source 下编写 *.c 和 *.inf 文件
	如：
	D:\UEFI\EDK2-18916\UEFI\BaseCode\BaseCode.c
	D:\UEFI\EDK2-18916\UEFI\BaseCode\BaseCode.inf

=== 仿真环境运行 ===
1. 将 *.inf 的路径添加到 Nt32Pkg.dsc 的 [Components] 部分
2. 编译
	> Edk2Setup.bat
	> build -p Nt32Pkg\Nt32Pkg.dsc -a IA32
	>>>生成 *.efi 的路径：
	D:\UEFI\EDK2-18916\Build\NT32IA32\DEBUG_VS2013x86\IA32\zhqBaseCode.efi
3. 调试
	> build run
	> fs0:
	> *.efi

=== U盘 Shell 环境运行 ===
1. 将 *.inf 的路径添加到 MdeModulePkg.dsc 的 [Components] 部分
2. 编译
	> Edk2Setup.bat
	> build -p MdeModulePkg\MdeModulePkg.dsc -a X64
	> build -p MdeModulePkg\MdeModulePkg.dsc -a X64 -m zhq\BaseCode\BaseCode.inf
	>>>生成 *.efi 的路径：
	D:\UEFI\EDK2-18916\Build\MdeModule\DEBUG_VS2013x86\X64\zhqBaseCode.efi
3. 调试
	将 *.efi 拷贝到 Shell U盘，直接运行！
	> fsX:
	> *.efi
