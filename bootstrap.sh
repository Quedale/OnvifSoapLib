sudo apt install automake autoconf gcc make pkg-config
sudo apt install unzip

echo "-- Building gsoap libgsoap-dev --"
#WS-Security depends on OpenSSL library 3.0 or 1.1
wget https://sourceforge.net/projects/gsoap2/files/gsoap_2.8.123.zip/download
unzip download
rm download
cd gsoap-2.8
mkdir build
./configure --with-openssl=/usr/lib/ssl --prefix=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd)/build
LIBRARY_PATH="$(pkg-config --variable=libdir openssl):$LIBRARY_PATH" \
LD_LIBRARY_PATH="$(pkg-config --variable=libdir openssl):$LD_LIBRARY_PATH" \
    make -j$(nproc)
make install
cd ..

aclocal
autoconf
automake --add-missing

echo "Generating WSDL gsoap files..."
rm -rf src/generated
mkdir src/generated
gsoap-2.8/build/bin/wsdl2h -x -t wsdl/typemap.dat -o src/generated/common_service.h -c \
http://www.onvif.org/onvif/ver10/device/wsdl/devicemgmt.wsdl \
http://www.onvif.org/onvif/ver10/event/wsdl/event.wsdl \
http://www.onvif.org/onvif/ver10/display.wsdl \
http://www.onvif.org/onvif/ver10/deviceio.wsdl \
http://www.onvif.org/onvif/ver20/imaging/wsdl/imaging.wsdl \
http://www.onvif.org/onvif/ver10/media/wsdl/media.wsdl \
http://www.onvif.org/onvif/ver20/ptz/wsdl/ptz.wsdl \
http://www.onvif.org/onvif/ver10/receiver.wsdl \
http://www.onvif.org/onvif/ver10/recording.wsdl \
http://www.onvif.org/onvif/ver10/search.wsdl \
http://www.onvif.org/onvif/ver10/replay.wsdl \
http://www.onvif.org/onvif/ver20/analytics/wsdl/analytics.wsdl \
http://www.onvif.org/onvif/ver10/analyticsdevice.wsdl \
http://www.onvif.org/onvif/ver10/schema/onvif.xsd 
gsoap-2.8/build/bin/soapcpp2 -CL -x -Igsoap-2.8/gsoap/import:gsoap-2.8/gsoap src/generated/common_service.h -dsrc/generated

