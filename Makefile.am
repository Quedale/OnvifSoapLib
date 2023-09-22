pkgconfig_DATA = onvifsoap.pc
AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4

DEPDIR= .deps
SUBDIRS = src/generated

lib_LTLIBRARIES = libonvifsoap.la
libonvifsoap_la_SOURCES =   src/ip_match.c \
                            src/onvif_base_service.c \
                            src/onvif_credentials.c \
                            src/onvif_device_capabilities.c \
                            src/onvif_device_info.c \
                            src/onvif_device_interface.c \
                            src/onvif_device_scopes.c \
                            src/onvif_device_service.c \
                            src/onvif_device.c \
                            src/onvif_media_profile.c \
                            src/onvif_media_service.c \
                            src/onvif_media_snapshot.c
libonvifsoap_la_CFLAGS= -c -Wall -fvisibility=hidden -DWITH_DOM -DWITH_OPENSSL -DWITH_GZIP -DBUILD_SHARD -I$(GSOAP_SRC_DIR)/gsoap/ -I$(GSOAP_SRC_DIR)/gsoap/import -I$(GSOAP_SRC_DIR)/gsoap/custom -I$(GSOAP_SRC_DIR)/gsoap/plugin -I$(srcdir)/src/generated
libonvifsoap_la_LDFLAGS= -all-static `pkg-config --libs openssl zlib`
libonvifsoap_la_LIBADD = src/generated/libgeneratedsoap.la

include_HEADERS= src/onvif_base_service.h src/onvif_device_capabilities.h src/onvif_credentials.h src/onvif_device.h src/onvif_media_profile.h src/onvif_device_service.h src/onvif_device_info.h src/onvif_device_interface.h src/onvif_media_service.h src/onvif_device_scopes.h src/shard_export.h src/onvif_media_snapshot.h