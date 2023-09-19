pkgconfig_DATA = onvifsoap.pc
AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4
GSOAP_SRC_DIR ?= $(shell pwd)/gsoap-2.8

DEPDIR= .deps

lib_LTLIBRARIES = libonvifsoap.la
libonvifsoap_la_SOURCES = $(GSOAP_SRC_DIR)/gsoap/dom.c $(GSOAP_SRC_DIR)/gsoap/stdsoap2.c $(GSOAP_SRC_DIR)/gsoap/plugin/smdevp.c $(GSOAP_SRC_DIR)/gsoap/plugin/mecevp.c $(GSOAP_SRC_DIR)/gsoap/plugin/wsaapi.c $(GSOAP_SRC_DIR)/gsoap/plugin/wsseapi.c $(GSOAP_SRC_DIR)/gsoap/plugin/wsddapi.c $(GSOAP_SRC_DIR)/gsoap/plugin/threads.c $(GSOAP_SRC_DIR)/gsoap/plugin/httpda.c $(GSOAP_SRC_DIR)/gsoap/custom/struct_timeval.c src/generated/soapClient.c src/generated/soapClient.h src/generated/soapC.c src/generated/soapC.h src/ip_match.c src/onvif_base_service.c src/onvif_credentials.c src/onvif_device_capabilities.c src/onvif_device_info.c src/onvif_device_interface.c src/onvif_device_scopes.c src/onvif_device_service.c src/onvif_device.c src/onvif_media_profile.c src/onvif_media_service.c src/onvif_media_snapshot.c
libonvifsoap_la_CFLAGS= -c -Wall -fvisibility=hidden -DWITH_DOM -DWITH_OPENSSL -DWITH_NOEMPTYSTRUCT -DWITH_GZIP -DBUILD_SHARD -I$(GSOAP_SRC_DIR)/gsoap/ -I$(GSOAP_SRC_DIR)/gsoap/import -I$(GSOAP_SRC_DIR)/gsoap/custom -I$(GSOAP_SRC_DIR)/gsoap/plugin -I$(srcdir)/src/generated
libonvifsoap_la_LDFLAGS= -all-static `pkg-config --libs openssl zlib`
include_HEADERS= src/onvif_base_service.h src/onvif_device_capabilities.h src/onvif_credentials.h src/onvif_device.h src/onvif_media_profile.h src/onvif_device_service.h src/onvif_device_info.h src/onvif_device_interface.h src/onvif_media_service.h src/onvif_device_scopes.h src/shard_export.h src/onvif_media_snapshot.h