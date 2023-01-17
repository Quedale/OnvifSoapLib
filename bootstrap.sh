SKIP_GSOAP=0
GSOAP_SRC_DIR="${GSOAP_SRC_DIR:=gsoap-2.8}" 
i=1;
for arg in "$@" 
do
    if [ $arg == "--skip-gsoap" ]; then
        SKIP_GSOAP=1
    fi
    i=$((i + 1));
done

sudo apt install automake autoconf gcc make pkg-config
sudo apt install unzip

if [ $SKIP_GSOAP -eq 0 ]; then
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
fi

aclocal
autoconf
automake --add-missing
autoreconf -i

echo "Generating WSDL gsoap files..."
rm -rf $(cd $(dirname "${BASH_SOURCE[0]}") && pwd)/src/generated
mkdir $(cd $(dirname "${BASH_SOURCE[0]}") && pwd)/src/generated
$GSOAP_SRC_DIR/build/bin/wsdl2h -x -t $(cd $(dirname "${BASH_SOURCE[0]}") && pwd)/wsdl/typemap.dat -o $(cd $(dirname "${BASH_SOURCE[0]}") && pwd)/src/generated/common_service.h -c \
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
$GSOAP_SRC_DIR/build/bin/soapcpp2 -CL -x -I$GSOAP_SRC_DIR/gsoap/import:$GSOAP_SRC_DIR/gsoap $(cd $(dirname "${BASH_SOURCE[0]}") && pwd)/src/generated/common_service.h -d$(cd $(dirname "${BASH_SOURCE[0]}") && pwd)/src/generated

