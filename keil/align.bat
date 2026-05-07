@echo off
setlocal enabledelayedexpansion

:: 工程根目录（脚本所在目录）
set SCRIPT_DIR=%~dp0
set OBJECTS_DIR=%SCRIPT_DIR%Objects\

:: 文件名（根据你的工程名修改）
set PROJECT_NAME=empty_LP_MSPM0G3507_nortos_keil

:: 文件路径
set MAPFILE=%SCRIPT_DIR%%PROJECT_NAME%.map
set HEXFILE=%OBJECTS_DIR%%PROJECT_NAME%.hex
set OUTHEX=%OBJECTS_DIR%empty_aligned.hex
set SREC_CAT=%SCRIPT_DIR%bin\srec_cat.exe

:: 检查文件是否存在
if not exist "%MAPFILE%" ( echo 错误：找不到 %MAPFILE% & exit /b 1 )
if not exist "%HEXFILE%" ( echo 错误：找不到 %HEXFILE% & exit /b 1 )
if not exist "%SREC_CAT%" ( echo 错误：找不到 %SREC_CAT% & exit /b 1 )

:: 从 .map 中提取 ER_IROM1 的大小
for /f "delims=" %%a in ('findstr /c:"Execution Region ER_IROM1" "%MAPFILE%"') do set LINE=%%a
set SIZE_PART=%LINE:*Size:=%
for /f "tokens=1 delims=, " %%b in ("%SIZE_PART%") do set SIZE_HEX=%%b

:: 计算十进制大小并向上取整到 8 的倍数
set /a SIZE_DEC=%SIZE_HEX%
set /a ALIGNED_SIZE=((%SIZE_DEC% + 7) / 8) * 8

:: 将十进制转换为十六进制字符串（用于 srec_cat 命令）
set HEX_END_ADDR=
set /a TEMP=%ALIGNED_SIZE%
call :dec2hex %TEMP%
set HEX_END_ADDR=0x%HEX_END_ADDR%

:: 调用 srec_cat
"%SREC_CAT%" "%HEXFILE%" -Intel -fill 0xFF 0x00000000 %HEX_END_ADDR% -o "%OUTHEX%" -Intel
exit /b 0

:: 十进制转十六进制子程序
:dec2hex
set HEX_END_ADDR=
set /a num=%~1
set hexchars=0123456789ABCDEF
:loop
set /a mod=num %% 16
set /a num=num / 16
set HEX_END_ADDR=!hexchars:~%mod%,1!!HEX_END_ADDR!
if %num% gtr 0 goto loop
exit /b