attrib -r C:\dg.TestBin\%build.arch%%build.type%\DXGIGetDispSurfData.exe
attrib -r C:\dg.TestBin\%build.arch%%build.type%\DXGIGetDispSurfData.pdb
attrib -r C:\dg.TestBin\%build.arch%%build.type%\GetDispSurfData.fx

copy /v /y /z C:\dg.binaries.%build.arch%%build.type%\nttest\Windowstest\graphics\d3d\conf\DXGIGetDispSurfData.exe C:\dg.TestBin\%build.arch%%build.type%\*
copy /v /y /z C:\dg.binaries.%build.arch%%build.type%\Symbols.pri\nttest\WINDOWSTEST\graphics\exe\DXGIGetDispSurfData.pdb C:\dg.TestBin\%build.arch%%build.type%\*
copy /v /y /z C:\dg.binaries.%build.arch%%build.type%\nttest\Windowstest\graphics\d3d\conf\GetDispSurfData.fx C:\dg.TestBin\%build.arch%%build.type%\*

