AUTOMAKE_OPTIONS = foreign

GSOAP_SRC_DIR ?= $(shell pwd)/gsoap-2.8

DEPDIR= .deps

lib_LTLIBRARIES = libonvifsoap.la
libonvifsoap_la_SOURCES = $(GSOAP_SRC_DIR)/gsoap/dom.c $(GSOAP_SRC_DIR)/gsoap/stdsoap2.c $(GSOAP_SRC_DIR)/build/share/gsoap/plugin/smdevp.c $(GSOAP_SRC_DIR)/build/share/gsoap/plugin/mecevp.c $(GSOAP_SRC_DIR)/build/share/gsoap/plugin/wsaapi.c $(GSOAP_SRC_DIR)/build/share/gsoap/plugin/wsseapi.c $(GSOAP_SRC_DIR)/build/share/gsoap/plugin/wsddapi.c $(GSOAP_SRC_DIR)/build/share/gsoap/custom/struct_timeval.c src/generated/soapClient.c src/generated/soapClient.h src/generated/soapC.c src/generated/soapC.h src/onvif_list.c src/onvif_device.c src/client.c
libonvifsoap_la_CFLAGS= -c -Wall -fpic -fvisibility=hidden `PKG_CONFIG_PATH=$(GSOAP_SRC_DIR)/build/lib/pkgconfig pkg-config --cflags gsoap gsoapssl` -I$(GSOAP_SRC_DIR)/ -I$(GSOAP_SRC_DIR)/build/include/ -I$(GSOAP_SRC_DIR)/build/share/gsoap/import -I$(GSOAP_SRC_DIR)/build/share/gsoap/custom -I$(GSOAP_SRC_DIR)/build/share/gsoap/plugin -I$(top_srcdir)/src/generated
libonvifsoap_la_LDFLAGS= `pkg-config --libs openssl zlib`
include_HEADERS= src/onvif_device.h src/onvif_list.h src/client.h



# clienttest$(EXEEXT):
# 	@echo "Compiling clienttest..."
# 	@mkdir -p build
# 	@gcc `pkg-config --cflags openssl` \
# 		`pkg-config --cflags zlib` \
# 		-Lbuild/lib \
# 		src/client-test.c \
# 		-o build/client-test \
# 			-lonvif \
# 			`pkg-config --libs openssl` \
# 			`pkg-config --libs zlib` 


# onviflib$(EXEEXT):
# 	@mkdir -p build
# 	@mkdir -p build/lib
# 	@echo "Compiling Onvif Objects..."
# 	@gcc -c -Wall -fpic -fvisibility=hidden \
# 		`PKG_CONFIG_PATH=$(GSOAP_SRC_DIR)/build/lib/pkgconfig pkg-config --cflags gsoap` \
# 		`PKG_CONFIG_PATH=$(GSOAP_SRC_DIR)/build/lib/pkgconfig pkg-config --cflags gsoapssl` \
# 		`pkg-config --cflags openssl` \
# 		`pkg-config --cflags zlib` \
# 		-I$(GSOAP_SRC_DIR)/ \
# 		-I$(GSOAP_SRC_DIR)/build/include/ \
# 		-I$(GSOAP_SRC_DIR)/build/share/gsoap/import \
# 		-I$(GSOAP_SRC_DIR)/build/share/gsoap/custom \
# 		-I$(GSOAP_SRC_DIR)/build/share/gsoap/plugin \
# 		-Isrc/generated \
# 		$(GSOAP_SRC_DIR)/gsoap/dom.c \
# 		$(GSOAP_SRC_DIR)/gsoap/stdsoap2.c \
# 		$(GSOAP_SRC_DIR)/build/share/gsoap/plugin/smdevp.c \
# 		$(GSOAP_SRC_DIR)/build/share/gsoap/plugin/mecevp.c \
# 		$(GSOAP_SRC_DIR)/build/share/gsoap/plugin/wsaapi.c \
# 		$(GSOAP_SRC_DIR)/build/share/gsoap/plugin/wsseapi.c \
# 		$(GSOAP_SRC_DIR)/build/share/gsoap/plugin/wsddapi.c \
# 		$(GSOAP_SRC_DIR)/build/share/gsoap/custom/struct_timeval.c \
# 		src/generated/soapClient.c \
# 		src/generated/soapC.c \
# 		src/onvif_list.c \
# 		src/onvif_device.c \
# 		src/client.c \
# 			`pkg-config --libs openssl` \
# 			`pkg-config --libs zlib` \
# 			`PKG_CONFIG_PATH=$(GSOAP_SRC_DIR)/build/lib/pkgconfig pkg-config --libs gsoap` \
# 			`PKG_CONFIG_PATH=$(GSOAP_SRC_DIR)/build/lib/pkgconfig pkg-config --libs gsoapssl`
# 
# 	@echo "Compiling Onvif Shared Library..."
# 	@gcc -shared -o build/lib/libonvif.so wsddapi.o wsaapi.o stdsoap2.o soapClient.o soapC.o dom.o struct_timeval.o wsseapi.o mecevp.o smdevp.o onvif_list.o onvif_device.o client.o
# 	@mkdir -p build/include
# 	@cp src/*.h build/include
# 	@rm -rf *.o