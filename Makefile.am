AUTOMAKE_OPTIONS = foreign
bin_PROGRAMS = clienttest
clienttest_SOURCES = client-test.c

clienttest$(EXEEXT):
	@echo "Compiling clienttest..."
	@mkdir -p build
	@gcc `pkg-config --cflags openssl` \
		`pkg-config --cflags zlib` \
		-Lbuild/lib \
		src/client-test.c \
		-o build/client-test \
			-lonvif \
			`pkg-config --libs openssl` \
			`pkg-config --libs zlib` 


onviflib$(EXEEXT):
	@mkdir -p build
	@mkdir -p build/lib
	@echo "Compiling Onvif Objects..."
	@gcc -c -Wall -fpic -fvisibility=hidden \
		`PKG_CONFIG_PATH=$(shell pwd)/gsoap-2.8/build/lib/pkgconfig pkg-config --cflags gsoap` \
		`PKG_CONFIG_PATH=$(shell pwd)/gsoap-2.8/build/lib/pkgconfig pkg-config --cflags gsoapssl` \
		`pkg-config --cflags openssl` \
		`pkg-config --cflags zlib` \
		-Igsoap-2.8/ \
		-Igsoap-2.8/build/include/ \
		-Igsoap-2.8/build/share/gsoap/import \
		-Igsoap-2.8/build/share/gsoap/custom \
		-Igsoap-2.8/build/share/gsoap/plugin \
		-Isrc/generated \
		gsoap-2.8/gsoap/dom.c \
		gsoap-2.8/gsoap/stdsoap2.c \
		gsoap-2.8/build/share/gsoap/plugin/smdevp.c \
		gsoap-2.8/build/share/gsoap/plugin/mecevp.c \
		gsoap-2.8/build/share/gsoap/plugin/wsaapi.c \
		gsoap-2.8/build/share/gsoap/plugin/wsseapi.c \
		gsoap-2.8/build/share/gsoap/plugin/wsddapi.c \
  		gsoap-2.8/build/share/gsoap/custom/struct_timeval.c \
		src/generated/soapClient.c \
		src/generated/soapC.c \
		src/onvif_list.c \
		src/onvif_device.c \
		src/client.c \
			`pkg-config --libs openssl` \
			`pkg-config --libs zlib` \
			`PKG_CONFIG_PATH=$(shell pwd)/gsoap-2.8/build/lib/pkgconfig pkg-config --libs gsoap` \
			`PKG_CONFIG_PATH=$(shell pwd)/gsoap-2.8/build/lib/pkgconfig pkg-config --libs gsoapssl`
	@echo "Compiling Onvif Shared Library..."
	@gcc -shared -o build/lib/libonvif.so wsddapi.o wsaapi.o stdsoap2.o soapClient.o soapC.o dom.o struct_timeval.o wsseapi.o mecevp.o smdevp.o onvif_list.o onvif_device.o client.o
	@mkdir -p build/include
	@cp src/*.h build/include
	@rm -rf *.o