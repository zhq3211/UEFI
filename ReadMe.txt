UEFI 基本原理 & 操作
	程序编写
	编译&运行
	

=== 程序编写 ===
1. EDK2 source 下编写 *.c 和 *.inf 文件
	如：
	D:\WorkSpace\UEFI\edk2\UEFI\BaseCode\BaseCode.c
	D:\WorkSpace\UEFI\edk2\UEFI\BaseCode\BaseCode.inf

	注：
    a. 将.dsc存储格式改为“UTF-8 without BOM”，防止出现中文乱码！
	b. 将 .inf 路径添加到 "edk2\Nt32Pkg.dsc" or "edk2\MdeModulePkg.dsc" 的 [Components] 部分



=== 编译&运行*.efi [仿真环境] ===
1. 将 *.inf 的路径添加到 Nt32Pkg.dsc 的 [Components] 部分

2. 编译
	> edksetup.bat --nt32
	> build -p Nt32Pkg\Nt32Pkg.dsc -a IA32
	=> 生成 *.efi 的路径：
	edk2\Build\NT32IA32\DEBUG_VS2013x86\IA32\MyBASE.efi

3. 调试
	> build run
	> fs0:
	> *.efi



=== 编译&运行*.efi [shell U盘] ===
1. 将 *.inf 的路径添加到 MdeModulePkg.dsc 的 [Components] 部分

2. 编译(cmd)
	> edksetup.bat --nt32

	[X64]
	> build -p MdeModulePkg\MdeModulePkg.dsc -a X64
	> build -p MdeModulePkg\MdeModulePkg.dsc -a X64 -m UEFI\BaseCode\BaseCode.inf
	=> 生成 *.efi 的路径：
	edk2\Build\MdeModule\DEBUG_VS2013x86\X64\MyBASE.efi
	
	[IA32]
	> build -p MdeModulePkg\MdeModulePkg.dsc
	=> 生成 *.efi 的路径：
	edk2\Build\MdeModule\DEBUG_VS2013x86\IA32

3. 调试
	将 *.efi 拷贝到 Shell U盘，直接运行！
	> fsX:
	> *.efi


