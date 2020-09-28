@if "%1"=="-nobuildgen" goto endbuildgen

@pushd %SDXROOT%\testsrc\windowstest\graphics\DirectX\tools\WrapperGenerator
@build -cZ
@popd
@pushd %SDXROOT%\testsrc\windowstest\graphics\DirectX\tools\WrapperGenerator\D3D11TestWrapperGenerator
@build -cZ
@popd
@pushd %SDXROOT%\testsrc\windowstest\graphics\DirectX\tools\WrapperGenerator\FrameworkWrapperGenerator
@build -cZ
@popd

@if "%_BuildArch%" == "x86" (
	set ArchDir=i386
) else (
	set ArchDir=%_BuildArch%
)

:endbuildgen

@copy %OBJECT_ROOT%\testsrc\windowstest\graphics\DirectX\tools\WrapperGenerator\obj%_BuildType%\%ArchDir%\WrapperGenerator.dll %temp%
@copy %OBJECT_ROOT%\testsrc\windowstest\graphics\DirectX\tools\WrapperGenerator\D3D11TestWrapperGenerator\obj%_BuildType%\%ArchDir%\D3D11TestWrapperGenerator.exe %temp%
@copy %OBJECT_ROOT%\testsrc\windowstest\graphics\DirectX\tools\WrapperGenerator\FrameworkWrapperGenerator\obj%_BuildType%\%ArchDir%\FrameworkWrapperGenerator.exe %temp%

@pushd %SDXROOT%\testsrc\windowstest\graphics\DirectX\Common\WGFTestFramework\d3d11test
@sd edit *

@%temp%\D3D11TestWrapperGenerator.exe D3D11Test.xml
@REM %temp%\D3D11TestWrapperGenerator.exe D3D11TestGlobals.xml

@sd revert -a ...

@call ..\GraphicsTest\makewrappers.bat %1

@popd
