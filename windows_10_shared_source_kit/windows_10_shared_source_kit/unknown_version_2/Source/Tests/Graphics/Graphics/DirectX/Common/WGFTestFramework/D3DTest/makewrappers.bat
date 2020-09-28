if "%1"=="-nobuildgen" goto endbuildgen

@pushd %SDXROOT%\testsrc\windowstest\graphics\DirectX\tools\WrapperGenerator
@build -cZ
@popd
@pushd %SDXROOT%\testsrc\windowstest\graphics\DirectX\tools\WrapperGenerator\D3DTestWrapperGenerator
@build -cZ
@popd
@pushd %SDXROOT%\testsrc\windowstest\graphics\DirectX\tools\WrapperGenerator\D3D10on11TestWrapperGenerator
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
@copy %OBJECT_ROOT%\testsrc\windowstest\graphics\DirectX\tools\WrapperGenerator\D3DTestWrapperGenerator\obj%_BuildType%\%ArchDir%\D3DTestWrapperGenerator.exe %temp%
@copy %OBJECT_ROOT%\testsrc\windowstest\graphics\DirectX\tools\WrapperGenerator\D3D10on11TestWrapperGenerator\obj%_BuildType%\%ArchDir%\D3D10on11TestWrapperGenerator.exe %temp%
@copy %OBJECT_ROOT%\testsrc\windowstest\graphics\DirectX\tools\WrapperGenerator\FrameworkWrapperGenerator\obj%_BuildType%\%ArchDir%\FrameworkWrapperGenerator.exe %temp%

@pushd %SDXROOT%\testsrc\windowstest\graphics\DirectX\Common\WGFTestFramework\d3dtest
@sd edit D3D*

@%temp%\D3D10on11TestWrapperGenerator.exe D3D10on11Test.xml
@%temp%\D3DTestWrapperGenerator.exe D3DTest.xml
@%temp%\D3DTestWrapperGenerator.exe D3DTestGlobals.xml

@sd revert -a ...

@call ..\GraphicsTest\makewrappers.bat %1

@popd