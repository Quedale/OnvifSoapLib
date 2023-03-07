# OnvifSoapLib
Onvif Wsdl Soap Library used in Onvif Device Manager

## Linux Compile
```shell
./bootstrap.sh
```

## Windows Compile
### Prepare
- git clone https://github.com/lus-zzz/OnvifSoapLib.git
- DownLoad https://udomain.dl.sourceforge.net/project/gsoap2/gsoap_2.8.124.zip
- unzip to project root directory  
```
directory struct:  
    OnvifSoapLib  
        example
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