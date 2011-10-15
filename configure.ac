# The autoconf initilization.
AC_INIT(pidgin-autohide, 0.3b, [andreas.bumen@dotnine.de], [pidgin-autohide])

# Detect the canonical host and target build environment
AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE([no-dependencies])
AC_CONFIG_HEADERS([config.h:config.h.in])

# Define some common variables.
AC_DEFINE_UNQUOTED(AUTHOR, "Andreas Bumen <$PACKAGE_BUGREPORT>", [Package author])

# Notices
AC_PREREQ(2.53)
AC_REVISION($Revision: 1.6 $)

# Checking for programs.
AC_DISABLE_STATIC
AC_PROG_LIBTOOL
AC_PROG_MAKE_SET
AC_PROG_CC

# Checking for pidgin.
PKG_CHECK_MODULES(PIDGIN, [pidgin >= 2.0.0], HAVE_PIDGIN="yes", HAVE_PIDGIN="no")

# Checking for pidgin flags.
AC_SUBST(PIDGIN_CFLAGS)
AC_SUBST(PIDGIN_LDFLAGS)
PIDGIN_LIBDIR=`pkg-config --variable=libdir pidgin`
AC_SUBST(PIDGIN_LIBDIR)

# Checking for glib.
PKG_CHECK_MODULES(GLIB, [glib-2.0], HAVE_GLIB="yes", HAVE_GLIB="no")

# Checking for glib flags.
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LDFLAGS)

AC_PROG_INTLTOOL
GETTEXT_PACKAGE=pidgin-autohide
AC_SUBST(GETTEXT_PACKAGE)
#ALL_LINGUAS="es fr it nl pl pt pt_BR sl hu zh_CN ro gl ru bg de sv"
AM_GLIB_GNU_GETTEXT 

# Checking for gtk.
PKG_CHECK_MODULES(GTK, [gtk+-2.0], HAVE_GTK="yes", HAVE_GTK="no")

# Checking for gtk flags.
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LDFLAGS)

# Creating files.
AC_OUTPUT([
Makefile
src/Makefile
po/Makefile.in
])
