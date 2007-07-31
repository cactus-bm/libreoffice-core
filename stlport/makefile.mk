#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.40 $
#
#   last change: $Author: hr $ $Date: 2007-07-31 13:10:28 $
#
#   The Contents of this file are made available subject to
#   the terms of GNU Lesser General Public License Version 2.1.
#
#
#     GNU Lesser General Public License Version 2.1
#     =============================================
#     Copyright 2005 by Sun Microsystems, Inc.
#     901 San Antonio Road, Palo Alto, CA 94303, USA
#
#     This library is free software; you can redistribute it and/or
#     modify it under the terms of the GNU Lesser General Public
#     License version 2.1, as published by the Free Software Foundation.
#
#     This library is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#     Lesser General Public License for more details.
#
#     You should have received a copy of the GNU Lesser General Public
#     License along with this library; if not, write to the Free Software
#     Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#     MA  02111-1307  USA
#
#*************************************************************************

PRJ=.

PRJNAME=so_stlport
TARGET=so_stlport

# --- Settings -----------------------------------------------------

.INCLUDE :	settings.mk

.IF "$(USE_SYSTEM_STL)"=="YES"
$(INCCOM)$/stlport$/hash_map : systemstl/hash_map
        @echo "You choose to build without stlport, so some headers will be used to bring the sgi extensions into the std namespace"
        -$(MKDIR) $(INCCOM)$/stlport
        $(COPY) systemstl/functional $(INCCOM)$/stlport
        $(COPY) systemstl/hash_map $(INCCOM)$/stlport
        $(COPY) systemstl/hash_set $(INCCOM)$/stlport
        $(COPY) systemstl/numeric $(INCCOM)$/stlport
        $(COPY) systemstl/slist $(INCCOM)$/stlport
        $(COPY) systemstl/rope $(INCCOM)$/stlport
        $(COPY) systemstl/vector $(INCCOM)$/stlport
.ENDIF


# --- Files --------------------------------------------------------
.EXPORT : CC CXX
.IF "$(COMID)"=="gcc3"
    TARFILE_NAME=STLport-4.5
       .IF "$(OS)$(BUILD_OS_MAJOR)$(BUILD_OS_MINOR)"=="MACOSX103" || "$(OS)$(BUILD_OS_MAJOR)$(BUILD_OS_MINOR)"=="MACOSX104"
               PATCH_FILE_NAME=STLport-4.5-macxp-panther.patch
       .ELSE
               PATCH_FILE_NAME=STLport-4.5.patch
       .ENDIF
.ELSE			# "$(COMID)"=="gcc3"
    .IF "$(OS)"=="MACOSX"
        TARFILE_NAME=STLport-4.0
        PATCH_FILE_NAME=STLport-4.0.macosx.patch
    .ELSE
        .IF "$(GUI)"=="WNT"
            .IF "$(CCNUMVER)"<="001300000000"
                TARFILE_NAME=STLport-4.0
                PATCH_FILE_NAME=STLport-4.0.patch
            .ELSE			# "$(CCNUMVER)"<="001300000000"
                TARFILE_NAME=STLport-4.5-0119
                PATCH_FILE_NAME=STLport-4.5-0119.patch
            .ENDIF			# "$(CCNUMVER)"<="001300000000"
        .ELSE
                TARFILE_NAME=STLport-4.0
        # To disable warnings from within STLport headers on unxsoli4 and
        # unxsols4, STLport-4.0.patch had to be extended mechanically by
        #
        #   cd unxsol.../misc/build/STLport-4.0/stlport && \
        #   find . -type f -exec sed -i \
        #     -e 's/^\([ \t]*__STL_BEGIN_NAMESPACE[ \t]*\)$/#if defined \
        #       __SUNPRO_CC\n#pragma disable_warn\n#endif\n&/' \
        #     -e 's/^\([ \t]*__STL_END_NAMESPACE[ \t]*\)$/&\n#if defined \
        #       __SUNPRO_CC\n#pragma enable_warn\n#endif/' {} \;
        #
        # (causing lots of files to become modified) and by additionally
        # changing unxsol.../misc/build/STLport-4.0/stlport/math.h,
        # unxsol.../misc/build/STLport-4.0/stlport/stl/_config.h,
        # unxsol.../misc/build/STLport-4.0/stlport/stl/_list.h, and
        # unxsol.../misc/build/STLport-4.0/stlport/stl/type_traits.h manually.
        # (Obviously due to the way the C++ compiler generates code for
        # instantiations of inline function templates from STLport headers, it
        # does not work to simply add "#pragma disable_warn" to stl/_prolog.h
        # and "#pragma enable_warn" to stl/_epilog.h, as seemingly some internal
        # STLport headers are read in by the compiler only at the end of a
        # compilation unit, outside the scope of stl/_prolog.h and
        # stl/_epilog.h.)
                PATCH_FILE_NAME=STLport-4.0.patch
        .ENDIF
    .ENDIF			# "$(OS)"=="MACOSX"
.ENDIF			# "$(COMID)"=="gcc3"

.IF "$(USE_SHELL)"=="4nt"
TAR_EXCLUDES=*/SC5/*
.ENDIF          # "$(USE_SHELL)"=="4nt"

ADDITIONAL_FILES=src$/gcc-3.0.mak src$/gcc-3.0-macosx.mak src$/gcc-3.0-freebsd.mak src$/sunpro8.mak src$/sunpro11.mak src$/gcc-3.0-mingw.mak


CONFIGURE_ACTION=none
CONFIGURE_FLAGS=

BUILD_DIR=src

.IF "$(COM)"=="MSC"
BUILD_ACTION=nmake
.IF "$(CCNUMVER)"<="001400000000"
BUILD_FLAGS=-f vc7.mak EXFLAGS="/EHsc"
.ELSE			# "$(CCNUMVER)"<="001400000000"
BUILD_FLAGS=-f vc7.mak EXFLAGS="/EHa /Zc:wchar_t-" CCNUMVER=$(CCNUMVER)
.ENDIF			# "$(CCNUMVER)"<="001400000000"
.ENDIF

.IF "$(COM)"=="GCC"
    .IF "$(COMID)"=="gcc3"
        # FreeBSD needs a special makefile
        .IF "$(OS)"=="FREEBSD"
            BUILD_FLAGS=-f gcc-3.0-freebsd.mak
        .ELIF "$(OS)"=="MACOSX"
            BUILD_FLAGS=-f gcc-3.0-macosx.mak
        .ELIF "$(GUI)"=="WNT"
            BUILD_FLAGS=-f gcc-3.0-mingw.mak
        .ELSE
            BUILD_FLAGS=-f gcc-3.0.mak
        .ENDIF
    .ELSE # "$(COMID)"=="gcc3"
        # MacOS X/Darwin need a special makefile
        .IF "$(OS)"=="MACOSX"
            BUILD_FLAGS=-f gcc-apple-macosx.mak
        .ELIF "$(OS)"=="FREEBSD"
            BUILD_FLAGS=-f gcc-freebsd.mak
        .ELSE # "$(OS)"=="MACOSX"
            BUILD_FLAGS=-f gcc.mak
        .ENDIF # "$(OS)"=="MACOSX"
    .ENDIF # "$(COMID)"=="gcc3"
    BUILD_ACTION=$(GNUMAKE)
    # build in parallel
    BUILD_FLAGS+= -j$(MAXPROCESS)
.ENDIF
.IF "$(HAVE_LD_HASH_STYLE)"  == "TRUE"
CXX+= -Wl,--hash-style=both
.ENDIF

.IF "$(COM)"=="C52"
BUILD_ACTION=make
.IF "$(CCNUMVER)">="00050008"
BUILD_FLAGS=-f sunpro11.mak
.ELIF "$(CCNUMVER)">="00050005"
BUILD_FLAGS=-f sunpro8.mak
.ELSE
BUILD_FLAGS=-f sunpro6.mak
.ENDIF # "$(CCNUMVER)">="00050008"

OUT2INC= \
    stlport$/SC5$/*.SUNWCCh
.ENDIF

.IF "$(OS)"=="IRIX"
TARFILE_NAME=STLport-4.5
PATCH_FILE_NAME=STLport-4.5.patch
BUILD_ACTION=gmake
BUILD_FLAGS=-f gcc-3.0.mak
BUILD_FLAGS+= -j$(MAXPROCESS)
.ENDIF

OUTDIR2INC= \
    stlport

.IF "$(GUI)"=="WNT"
.IF "$(COM)"=="GCC"

OUT2LIB= \
    lib$/lib*_static.a

OUT2BIN= \
    lib$/*.dll

.ELSE

OUT2LIB= \
    lib$/*.lib

OUT2BIN= \
    lib$/*.dll \
    lib$/*.pdb

.ENDIF # "$(COM)"=="GCC"

.ELSE          # "$(GUI)"=="WNT"

OUT2LIB= \
    lib$/lib*

.ENDIF          # "$(GUI)"=="WNT"

# --- Targets ------------------------------------------------------

.IF "$(STLPORT4)"!="NO_STLPORT4"
all :
       @echo "         An already available installation of STLport has been chosen in the configure process."
       @echo "         Therefore the version provided here does not need to be built in addition."
.ENDIF

.INCLUDE : 	set_ext.mk
.INCLUDE :	target.mk
.INCLUDE :	tg_ext.mk

.IF "$(GUI)"=="WNT"
.IF "$(COM)"!="GCC"
.IF "$(CCNUMVER)"<="001300000000"

$(MISC)$/$(TARFILE_ROOTDIR) : avoid_win32_patches
avoid_win32_patches :
    @$(ECHONL)
    @echo ERROR! this module can't use automated patch creation
    @echo on windows.
    @$(ECHONL)
    force_dmake_to_error

$(PACKAGE_DIR)$/so_custom_patch :  $(PACKAGE_DIR)$/$(PATCH_FLAG_FILE)
.IF "$(USE_SHELL)"=="4nt"
    win32_custom.bat $(PACKAGE_DIR) $(BACK_PATH) && $(TOUCH) $@
.ELSE			# "$(USE_SHELL)"=="4nt"
    win32_custom.sh $(PACKAGE_DIR) $(BACK_PATH) && $(TOUCH) $@
.ENDIF			# "$(USE_SHELL)"=="4nt"

$(PACKAGE_DIR)$/$(CONFIGURE_FLAG_FILE) : $(PACKAGE_DIR)$/so_custom_patch

.IF "$(USE_NEW_SDK)"!=""
$(PACKAGE_DIR)$/win32_sdk_patch :  $(PACKAGE_DIR)$/$(PATCH_FLAG_FILE)
.IF "$(USE_SHELL)"=="4nt"
    win32_sdk.bat $(PACKAGE_DIR) $(BACK_PATH) && $(TOUCH) $@
.ELSE			# "$(USE_SHELL)"=="4nt"
    win32_sdk.sh $(PACKAGE_DIR) $(BACK_PATH) && $(TOUCH) $@
.ENDIF			# "$(USE_SHELL)"=="4nt"

$(PACKAGE_DIR)$/$(CONFIGURE_FLAG_FILE) : $(PACKAGE_DIR)$/win32_sdk_patch
.ENDIF			# "$(USE_NEW_SDK)"!=""
.ENDIF			# COMVER<=001300000000
.ENDIF "$(COM)"=="GCC"
.ENDIF          # "$(GUI)"=="WNT"
