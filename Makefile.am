AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4
GSOAP_SRC_DIR ?= $(shell pwd)/gsoap-2.8

DEPDIR= .deps

lib_LTLIBRARIES = libonvifsoap.la
libonvifsoap_la_SOURCES = $(GSOAP_SRC_DIR)/gsoap/dom.c $(GSOAP_SRC_DIR)/gsoap/stdsoap2.c $(GSOAP_SRC_DIR)/build/share/gsoap/plugin/smdevp.c $(GSOAP_SRC_DIR)/build/share/gsoap/plugin/mecevp.c $(GSOAP_SRC_DIR)/build/share/gsoap/plugin/wsaapi.c $(GSOAP_SRC_DIR)/build/share/gsoap/plugin/wsseapi.c $(GSOAP_SRC_DIR)/build/share/gsoap/plugin/wsddapi.c $(GSOAP_SRC_DIR)/build/share/gsoap/custom/struct_timeval.c src/generated/soapClient.c src/generated/soapClient.h src/generated/soapC.c src/generated/soapC.h src/onvif_list.c src/onvif_device.c src/client.c
libonvifsoap_la_CFLAGS= -c -Wall -fpic -fvisibility=hidden `PKG_CONFIG_PATH=$(GSOAP_SRC_DIR)/build/lib/pkgconfig pkg-config --cflags gsoap gsoapssl` -I$(GSOAP_SRC_DIR)/ -I$(GSOAP_SRC_DIR)/build/include/ -I$(GSOAP_SRC_DIR)/build/share/gsoap/import -I$(GSOAP_SRC_DIR)/build/share/gsoap/custom -I$(GSOAP_SRC_DIR)/build/share/gsoap/plugin -I$(srcdir)/src/generated
libonvifsoap_la_LDFLAGS= `pkg-config --libs openssl zlib`
include_HEADERS= src/onvif_device.h src/onvif_list.h src/client.h
