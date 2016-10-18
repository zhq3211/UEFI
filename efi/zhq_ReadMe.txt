=== 2016.10.18 制作 UEFI启动盘 ===

1. 制作 U盘 的 “EFI/BOOT/bootX64.efi” or “EFI/BOOT/bootia32.efi”
	cmd 进入 EDK2 目录 
	> build -a X64 -p DuetPkg\DuetPkgX64.dsc
	> cd DuetPkg
	> PostBuild.bat X64
	> CreateBootDisk.bat usb H: FAT32 X64 (注：H为U盘盘符名)
	拔出并重新插入U盘，继续执行如下命令
	> CreateBootDisk.bat usb H: FAT32 X64 step2
	OK!
PS: IA32 类似！



=== cmd ===
D:\UEFI\EDK2-18916> build -a X64 -p DuetPkg\DuetPkgX64.dsc
	......
	Generating DUETEFIMAINFV FV
	########################################
	########################################
	########################################
	###############
	GUID cross reference file can be found at d:\uefi\edk2-18916\Build\DuetPkgX64\DE
	BUG_VS2013x86\FV\Guid.xref

	FV Space Information
	DUETEFIMAINFV [95%Full] 917504 total, 878248 used, 39256 free

	- Done -
	Build end time: 15:17:52, Oct.18 2016
	Build total time: 00:01:29

D:\UEFI\EDK2-18916> cd DuetPkg

D:\UEFI\EDK2-18916\DuetPkg>PostBuild.bat X64
	Compressing DUETEFIMainFv.FV ...
	系统找不到指定的路径。
	Compressing DxeMain.efi ...
	系统找不到指定的路径。
	Compressing DxeIpl.efi ...
	系统找不到指定的路径。
	Generate Loader Image ...
	系统找不到指定的路径。
	D:\UEFI\EDK2-18916\DuetPkg\BootSector\bin\Start64.com
	D:\UEFI\EDK2-18916\DuetPkg\BootSector\bin\efi64.com2
	已复制         1 个文件。
	系统找不到指定的路径。
	D:\UEFI\EDK2-18916\DuetPkg\BootSector\bin\St16_64.com
	D:\UEFI\EDK2-18916\DuetPkg\BootSector\bin\efi64.com2
	已复制         1 个文件。
	系统找不到指定的路径。
	D:\UEFI\EDK2-18916\DuetPkg\BootSector\bin\St32_64.com
	D:\UEFI\EDK2-18916\DuetPkg\BootSector\bin\efi64.com2
	已复制         1 个文件。
	系统找不到指定的路径。

D:\UEFI\EDK2-18916\DuetPkg>CreateBootDisk.bat usb H: FAT32 X64
	Start to create usb boot disk ...
	Format H: ...
	Create boot sector ...
	系统找不到指定的路径。
	已复制         1 个文件。
	系统找不到指定的路径。
	找不到 D:\UEFI\EDK2-18916\DuetPkg\UsbBs32.com
	系统找不到指定的路径。
	系统找不到指定的路径。
	Done.
	PLEASE UNPLUG USB, THEN PLUG IT AGAIN!
	
(拔出并重新插入U盘，继续执行如下命令)

D:\UEFI\EDK2-18916\DuetPkg>CreateBootDisk.bat usb H: FAT32 X64 step2
	Start to create usb boot disk ...
	系统找不到指定的文件。

	D:\UEFI\EDK2-18916\DuetPkg>copy D:\UEFI\EDK2-18916\ShellBinPkg\UefiShell\X64\She
	ll.efi H:\efi\boot\bootX64.efi /y
	已复制         1 个文件。

(OK! U盘生成 “EFI/BOOT/bootX64.efi”)

