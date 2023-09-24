@ECHO OFF
SET MYPATH=%~dp0
SET SUBPROJ_PATH=%MYPATH%subprojects
SET CACHE_PATH=%MYPATH%subprojects\.cache
SET CMAKE_PATH=%SUBPROJ_PATH%\vcpkg\scripts\buildsystems\vcpkg.cmake
SET VSCODE_PATH=%MYPATH%.vscode
SET ESC_CMAKE_PATH=%CMAKE_PATH:\=\\%

if NOT EXIST %SUBPROJ_PATH% (
    ECHO "Making directory %SUBPROJ_PATH%"
    mkdir %SUBPROJ_PATH%
)

if NOT EXIST %CACHE_PATH% (
    ECHO "Making directory %CACHE_PATH%"
    mkdir %CACHE_PATH%
)

IF NOT EXIST %SUBPROJ_PATH%\gsoap-2.8\ (
    IF NOT EXIST %CACHE_PATH%\gsoap_2.8.zip (
        echo "Downloading gsoap archive..."
        curl.exe -L --output %CACHE_PATH%\gsoap_2.8.zip --url https://sourceforge.net/projects/gsoap2/files/gsoap_2.8.129.zip/download
    )
    echo "Extracting gsoap archive..."
    tar -xf %CACHE_PATH%\gsoap_2.8.zip -C %SUBPROJ_PATH%
)

IF NOT EXIST %SUBPROJ_PATH%\vcpkg\ (
    git clone https://github.com/Microsoft/vcpkg.git %SUBPROJ_PATH%\vcpkg
    %SUBPROJ_PATH%\vcpkg\bootstrap-vcpkg.sh
) else (
    git -C %SUBPROJ_PATH%\vcpkg pull
)

%SUBPROJ_PATH%\vcpkg\vcpkg install openssl:x64-windows openssl:x86-windows zlib:x64-windows zlib:x86-windows

rd /s/q %cd%\src\generated  
md %cd%\src\generated  
%cd%\subprojects\gsoap-2.8\gsoap\bin\win64\wsdl2h.exe -x -t %cd%\wsdl\typemap.dat -o %cd%\src\generated\common_service.h -c ^
    http://www.onvif.org/onvif/ver10/device/wsdl/devicemgmt.wsdl ^
    http://www.onvif.org/onvif/ver10/media/wsdl/media.wsdl ^
    http://www.onvif.org/onvif/ver10/schema/onvif.xsd

%cd%\subprojects\gsoap-2.8\gsoap\bin\win64\soapcpp2.exe -f100 -CL -x -I "%cd%\subprojects\gsoap-2.8\gsoap\import";"%cd%\subprojects\gsoap-2.8\gsoap" %cd%\src\generated\common_service.h -d%cd%\src\generated

IF NOT EXIST %VSCODE_PATH%\ (
    mkdir %VSCODE_PATH%\
)

IF NOT EXIST %VSCODE_PATH%\settings.json (
    echo { > %VSCODE_PATH%\settings.json
    echo     "cmake.configureSettings": { >> %VSCODE_PATH%\settings.json
    echo         "CMAKE_TOOLCHAIN_FILE":"%ESC_CMAKE_PATH%" >> %VSCODE_PATH%\settings.json
    echo     } >> %VSCODE_PATH%\settings.json
    echo } >> %VSCODE_PATH%\settings.json
) else (
    ECHO ******************************************************************************************************
    ECHO *  
    ECHO *  Successfully generated OnvifSoapLib build environment.
    ECHO *  To configure vscode to use vcpkg dependencies,
    ECHO *      make sure the following exist in your settings.json
    ECHO *
    ECHO *          }
    ECHO *              "cmake.configureSettings": {
    ECHO *                  "CMAKE_TOOLCHAIN_FILE": "%SUBPROJ_PATH%\vcpkg\scripts\buildsystems\vcpkg.cmake"
    ECHO *              }
    ECHO *          }
    ECHO *
    ECHO *
    ECHO ******************************************************************************************************
)