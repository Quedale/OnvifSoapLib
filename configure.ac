AC_INIT([OnvifSoapLib], [0.1], [maintainer@example.com])
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_MACRO_DIRS([m4])
AM_SILENT_RULES([yes])
AC_PROG_CC
PKG_INSTALLDIR(${prefix}/lib/pkgconfig)

AC_ARG_VAR(GSOAP_SRC_DIR,[gSoap source directory location])
AS_VAR_SET_IF(GSOAP_SRC_DIR,[],AC_SUBST(GSOAP_SRC_DIR,'$(abs_srcdir)/subprojects/gsoap-2.8'))

AC_SUBST(PKG_CUTILS_PATH, '$(abs_srcdir)/subprojects/CUtils/build/dist/lib/pkgconfig')
AC_SUBST(PKG_NTLM_PATH, '$(abs_srcdir)/subprojects/libntlm/build/dist/lib/pkgconfig')

AC_SUBST(PKG_FULL_PATH, '$(PKG_CUTILS_PATH):$(PKG_NTLM_PATH)')


AC_CONFIG_FILES([
    Makefile
    src/generated/Makefile
    onvifsoap.pc
])
LT_INIT([static])
AC_OUTPUT