@if "%1"=="-nobuildgen" goto endbuildgen

@pushd %SDXROOT%\testsrc\windowstest\graphics\DirectX\tools\WrapperGenerator
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
@copy %OBJECT_ROOT%\testsrc\windowstest\graphics\DirectX\tools\WrapperGenerator\FrameworkWrapperGenerator\obj%_BuildType%\%ArchDir%\FrameworkWrapperGenerator.exe %temp%

@pushd %SDXROOT%\testsrc\windowstest\graphics\DirectX\Framework\GraphicsTest
@sd edit dxgi*

@%temp%\FrameworkWrapperGenerator.exe DXGILatestConfig.xml

@sd revert -a ...
@popd
