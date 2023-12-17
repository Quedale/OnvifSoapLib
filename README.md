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
### Note
Windows build supported through VSCode and CMake plugin for simplicity.
This assumes you already have an operational VSCode MVSC or Mingw64 environment.

### Prepare
Simply run `autogen.bat` from the project directory.
autogen.bat will:
    - Download gsoap-2.8
    - Download vcpkg
    - Download openssl and zlip using vcpkg

If the file '.vscode/settings.json' doesn't exists, it will be generated automatically.
If the file already exists, the configuration to append will be printed on the console.

```
directory struct:  
    OnvifSoapLib  
        .vscode
        example
        subprojects
            cutils
            gsoap-2.8
            vcpkg  
        src  
        wsdl  
```
### Build
Simply open this project with vscode and build it using CMake's plugin.
