# OnvifSoapLib
Onvif Wsdl Soap Library used in Onvif Device Manager

## Linux Compile
Prepare the environment
```shell
./bootstrap.sh
```

Configure, Compile and install
```shell
./configure --prefix=$(pwd)/build
make
make install
```

## Windows Compile [Mingw64 and MSVC tested]
### Prepare
- git clone https://github.com/Quedale/OnvifSoapLib.git
- DownLoad https://udomain.dl.sourceforge.net/project/gsoap2/gsoap_2.8.124.zip
- unzip to project root directory  
```
directory struct:  
    OnvifSoapLib  
        example
        subprojects
            gsoap-2.8  
        src  
        wsdl  
```
### Build
- in the project root directory open powershell
```cmd
.\wsdl\generated.bat
```
- install OpenSSL and ZLIB with vcpkg  
- config vscode [vcpkg Overview](https://github.com/microsoft/vcpkg/blob/master/README_zh_CN.md)
- open this project with vscode and build
