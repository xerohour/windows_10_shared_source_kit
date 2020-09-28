rem The following are prerequisitive steps for running the script
rem cd /d %SystemDrive%\
rem mkdir match
rem cd %SystemDrive%\match
rem \\winbuilds\release\FBL_GRFX_DEV\9298.0.130113-1800\amd64fre\bin\NTTEST\WINDOWSTEST\dxaml\xcp\SetupDirectUIDir.cmd
rem GetAppXTools.cmd
rem cd AppXCerts
rem InstallAppxTestLabRootCert.cmd
rem copy StandaloneTest\*.* %SystemDrive%\windows\system32
rem 

cd /d %SystemDrive%\match
XCOPY /FIVERY \\davis\scratch\marmel\vidtest\modern c:\modern\D3D
del DXGIHybridPresentModern.appx
MakeAppx.exe pack /o /d c:\Modern\D3D /p DXGIHybridPresentModern.appx
signtool.exe sign /a /v /fd SHA256 /f AppXCerts\APPX_TEST_0.pfx DXGIHybridPresentModern.appx 2>&1
DeployAppX.exe /remove /packagepath:"DXGIHybridPresentModern.appx"
DeployAppX.exe /install /packagepath:"DXGIHybridPresentModern.appx"
