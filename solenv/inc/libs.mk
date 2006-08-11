#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: libs.mk,v $
#
#   $Revision: 1.102 $
#
#   last change: $Author: hr $ $Date: 2006-08-11 17:39:52 $
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
LIBSMKREV!:="$$Revision: 1.102 $$"

.IF "$(COM)"=="WTC"
LIBPRE=libr
.ENDIF

.IF "$(GUI)"=="UNX" || "$(GUI)"=="MAC"

#
#externe libs in plattform.mk
#
AWTLIB*=-ljawt
AVMEDIALIB=-lavmedia$(OFFICEUPD)$(DLLPOSTFIX)
ICUINLIB=-licui18n
ICULELIB=-licule
ICUUCLIB=-licuuc
I18NUTILLIB=-li18nutil$(COMID)
.INCLUDE .IGNORE : i18npool$/version.mk
I18NISOLANGLIB=-li18nisolang$(ISOLANG_MAJOR)$(COMID)
.IF "$(WITH_GPC)"!="NO"
GPC3RDLIB=-lgpc
.ENDIF
SALHELPERLIB=-luno_salhelper$(COMID)
XMLSCRIPTLIB =-lxcr$(OFFICEUPD)$(DLLPOSTFIX)
.INCLUDE .IGNORE : comphelper$/version.mk
COMPHELPERLIB=-lcomphelp$(COMPHLP_MAJOR)$(COMID)
CONNECTIVITYLIB=-lconnectivity
LDAPBERLIB=-lldapber
TOOLSLIBST=-latools
DVOSUTILLIBST=-ldvosutil
DUNOUTILLIBST=-ldunoutil
DVCLUTILLIBST=-ldvclutil
BPICONVLIB=-lbpiconv
TOOLSLIB=-ltl$(OFFICEUPD)$(DLLPOSTFIX)
# make sure some ancient libs are empty
OSLLIB=
RTLLIB=
TECLIB=
CPPULIB=-luno_cppu
CPPUHELPERLIB=-luno_cppuhelper$(COMID)
.INCLUDE .IGNORE : ucbhelper$/version.mk
UCBHELPERLIB=-lucbhelper$(UCBHELPER_MAJOR)$(COMID)
REGLIB=-lreg
.INCLUDE .IGNORE : vos$/version.mk
VOSLIB=-lvos$(VOS_MAJOR)$(COMID)
XMLOFFLIB=-lxo$(OFFICEUPD)$(DLLPOSTFIX)
XMLOFFLLIB=-lxol
STORELIB=-lstore
SALLIB=-luno_sal
.INCLUDE .IGNORE : connectivity$/version.mk
ODBCLIB=-lodbc$(ODBC_MAJOR)
ODBCBASELIB=-lodbcbase$(ODBC_MAJOR)
DBFILELIB=-lfile$(OFFICEUPD)$(DLLPOSTFIX)
RMCXTLIB=-lrmcxt
BTSTRPLIB=-lbtstrp
BTSTRPDTLIB=-lbootstrpdt
TRANSEXLIB=-ltransex
IOTXLIB=
OTXLIB=-lotx_ind
OSXLIB=-losx
UNOLIB=-luno$(OFFICEUPD)$(DLLPOSTFIX)
UNOTOOLSLIB=-lutl$(OFFICEUPD)$(DLLPOSTFIX)
UCRLIB=-lucr$(OFFICEUPD)$(DLLPOSTFIX)
UASLIB=-luas$(OFFICEUPD)$(DLLPOSTFIX)
USRLIB=-lusr$(OFFICEUPD)$(DLLPOSTFIX)
SOTLIB=-lsot$(OFFICEUPD)$(DLLPOSTFIX)
ONELIB=-lone$(OFFICEUPD)$(DLLPOSTFIX)
MOZBASELIBST=$(STATIC) -lnspr4 -lxpcombase_s $(DYNAMIC)
MOZBASELIB=-lnspr4 -lxpcom
.IF "$(WITH_OPENLDAP)" == "YES"
LDAPSDKLIB=-lldap
.ELSE
LDAPSDKLIB=-lldap50
.ENDIF
ONELIB=
UNOLIB=
SVLIB=-lvcl$(OFFICEUPD)$(DLLPOSTFIX)
ICOLIB=-lico$(OFFICEUPD)$(DLLPOSTFIX)
VCLLIBST=-lvcl
VCLLIB=$(SVLIB)
BASEGFXLIB=-lbasegfx$(OFFICEUPD)$(DLLPOSTFIX)
BASEBMPLIB=-lbasebmp$(OFFICEUPD)$(DLLPOSTFIX)
CANVASTOOLSLIB=-lcanvastools$(OFFICEUPD)$(DLLPOSTFIX)
CPPCANVASLIB=-lcppcanvas$(OFFICEUPD)$(DLLPOSTFIX)
.IF "$(SYSTEM_AGG)" == "YES"
AGGLIB=-lagg
.ELSE
AGGLIB=-lagg$(OFFICEUPD)$(DLLPOSTFIX)
.ENDIF
.IF "$(SYSTEM_FREETYPE)"=="YES"
FREETYPELIB=$(FREETYPE_LIBS)
FREETYPELIBST=$(FREETYPE_LIBS)
.ELSE
FREETYPELIB=-lfreetype
FREETYPELIBST=$(STATIC) -lfreetype $(DYNAMIC)
.ENDIF
XPLIB=-l_xp
TKLIB=-ltk$(OFFICEUPD)$(DLLPOSTFIX)
SVTOOLLIB=-lsvt$(OFFICEUPD)$(DLLPOSTFIX)
XMLSECLIB=-lxmlsec1
XMLSECLIB-NSS=-lxmlsec1-nss
LIBXML2LIB=-lxml2
NSS3LIB=-lnss3
NSPR4LIB=-lnspr4
PLC4LIB=-lplc4
NSSCRYPTOLIBS=$(LIBXML2LIB) $(XMLSECLIB) $(XMLSECLIB-NSS) $(NSS3LIB) $(NSPR4LIB) $(PLC4LIB)
BROOKERLIB=-lbrooker$(OFFICEUPD)$(DLLPOSTFIX)
SIMPLECMLIB=-lsimplecm$(OFFICEUPD)$(DLLPOSTFIX)
COMMUNILIB=-lcommuni$(OFFICEUPD)$(DLLPOSTFIX)
BTCOMMUNILIB=-lbtcommuni$(OFFICEUPD)$(DLLPOSTFIX)
AUTOMATIONLIB=-lsts$(OFFICEUPD)$(DLLPOSTFIX)
SVLLIB=-lsvl$(OFFICEUPD)$(DLLPOSTFIX)
SVMEMLIB=
SVUNZIPLIB=-lsvunzip
SVUNZIPDLL=-lzip$(OFFICEUPD)$(DLLPOSTFIX)
PLUGCTORLIB=-lplugctor
# PLUGCTORLIB=
SO2LIB=-lso$(OFFICEUPD)$(DLLPOSTFIX)
TKTLIB=-ltkt$(OFFICEUPD)$(DLLPOSTFIX)
SJLIB=-lj$(OFFICEUPD)$(DLLPOSTFIX)_g
GOODIESLIB=-lgo$(OFFICEUPD)$(DLLPOSTFIX)
MAILLIB=-lmail
DOCMGRLIB=-ldmg$(OFFICEUPD)$(DLLPOSTFIX)
BASICLIB=-lsb$(OFFICEUPD)$(DLLPOSTFIX)
BASICLIGHTLIB=-lsbl$(OFFICEUPD)$(DLLPOSTFIX)
SDBLIB=-lsdb$(OFFICEUPD)$(DLLPOSTFIX)
DBTOOLSLIB=-ldbtools$(OFFICEUPD)$(DLLPOSTFIX)
HM2LIBSH=-lhmwrpdll
HM2LIBST=-lhmwrap
LINGULIB=$(HM2LIBST)
LNGLIB=-llng$(OFFICEUPD)$(DLLPOSTFIX)
.IF "$(SYSTEM_EXPAT)"=="YES"
EXPAT3RDLIB=-lexpat
EXPATASCII3RDLIB=-lexpat
.ELSE
EXPAT3RDLIB=-lexpat_xmlparse -lexpat_xmltok
EXPATASCII3RDLIB=-lascii_expat_xmlparse -lexpat_xmltok
.ENDIF
.IF "$(SYSTEM_ZLIB)"=="YES"
ZLIB3RDLIB=-lz
.ELSE
ZLIB3RDLIB=-lzlib
.ENDIF
.IF "$(SYSTEM_JPEG)"=="YES"
.IF "$(SOLAR_JAVA)" != "" && "$(JDK)" != "gcj" && "$(OS)" != "MACOSX"
#i34482# Blackdown/Sun jdk is in the libsearch patch and has a libjpeg :-(
.IF "$(OS)" == "FREEBSD"
JPEG3RDLIB=/usr/local/lib/libjpeg.so
.ELIF "$(CPUNAME)" == "X86_64"
JPEG3RDLIB=/usr/lib64/libjpeg.so
.ELSE
JPEG3RDLIB=/usr/lib/libjpeg.so
.ENDIF
.ELSE
JPEG3RDLIB=-ljpeg
.ENDIF
.ELSE
JPEG3RDLIB=-ljpeglib
.ENDIF
NEON3RDLIB=-lneon
.IF "$(SYSTEM_DB)" == "YES"
BERKELEYLIB=-ldb
BERKELEYCPPLIB=-ldb_cxx
.ELSE
BERKELEYLIB=-ldb-4.2
BERKELEYCPPLIB=-ldb_cxx-4.2
.ENDIF
CURLLIB=-lcurl
SFX2LIB=-lsfx$(OFFICEUPD)$(DLLPOSTFIX)
SFXLIB=-lsfx$(OFFICEUPD)$(DLLPOSTFIX)
SFXDEBUGLIB=
FWELIB=-lfwe$(OFFICEUPD)$(DLLPOSTFIX)
FWILIB=-lfwi$(OFFICEUPD)$(DLLPOSTFIX)
SVXLIB=-lsvx$(OFFICEUPD)$(DLLPOSTFIX)
BASCTLLIB=-lbasctl$(OFFICEUPD)$(DLLPOSTFIX)
BASICIDELIB=-lybctl
SVXLLIB=-lsvxl
CHAOSLIB=-lcnt$(OFFICEUPD)$(DLLPOSTFIX)
UUILIB=-luui$(OFFICEUPD)$(DLLPOSTFIX)
DGLIB=
SIMLIB=-lysim
SCHLIB=-lysch
SMLIB=-lysm
SBALIB=-lsbawrap
#SBALIB=-lsba$(OFFICEUPD)$(DLLPOSTFIX)
OFALIB=-lofa$(OFFICEUPD)$(DLLPOSTFIX)
SETUPLIB=-lset$(OFFICEUPD)$(DLLPOSTFIX)
SCHEDLIB=-lss$(OFFICEUPD)$(DLLPOSTFIX)
SSLLIB=-lssl$(OFFICEUPD)$(DLLPOSTFIX)
SSCLIB=-lssc$(OFFICEUPD)$(DLLPOSTFIX)
SSWLIB=-lssw$(OFFICEUPD)$(DLLPOSTFIX)
PRXLIB=-llprx2$(OFFICEUPD)$(DLLPOSTFIX)
TRACERLIB=-ltrc$(OFFICEUPD)$(DLLPOSTFIX)
PAPILIB=-lpap$(OFFICEUPD)$(DLLPOSTFIX)
TRCLIB=$(TRACERLIB)
SCLIB=-lsclib
SDLIB=-lsdlib
SDLLIB=-lsdl
SWLIB=-lswlib
ISWLIB=-lsw$(OFFICEUPD)$(DLLPOSTFIX)
ISCLIB=-lsc$(OFFICEUPD)$(DLLPOSTFIX)
ISDLIB=-lsd$(OFFICEUPD)$(DLLPOSTFIX)
PKGCHKLIB=-lpkgchk$(OFFICEUPD)$(DLLPOSTFIX)
SYSSHELLLIB=-lsysshell
SSOOPTLIB=-lssoopt$(OFFICEUPD)$(DLLPOSTFIX)
JVMACCESSLIB = -ljvmaccess$(COMID)
CPPUNITLIB = -lcppunit$(DLLPOSTFIX)
.IF "$(SYSTEM_LIBXML)"=="YES"
XML2LIB=$(LIBXML_LIBS)
.ELSE
XML2LIB=-lxml2
.ENDIF
JVMFWKLIB = -ljvmfwk

# #110743#
# For BinFilters, some libs were added.
#

BFSFXLIB=-lbf_sfx
BFSVXLIB=-lbf_svx$(OFFICEUPD)$(DLLPOSTFIX)
BFSCHLIB=-lbf_ysch
BFSMLIB=-lbf_ysm
BFSCLIB=-lbf_sclib
BFSDLIB=-lbf_sdlib
BFSWLIB=-lbf_swlib
BFOFALIB=-lbf_ofa$(OFFICEUPD)$(DLLPOSTFIX)
LEGACYSMGRLIB=-llegacy_binfilters$(OFFICEUPD)$(DLLPOSTFIX)
BFXMLOFFLIB=-lbf_xo$(OFFICEUPD)$(DLLPOSTFIX)


#
# USED_%NAME%_LIBS
# Variablen, in denen in gueltiger Reihenfolge ALLE Libraries,
# die unterhalb von %NAME% liegen, zusammengefasst werden
#

# Libraries
USED_OSL_LIBS =
USED_VOS_LIBS =		$(OSLLIB)
USED_UNO_LIBS =		$(VOSLIB) $(OSLLIB)
USED_TOOLS_LIBS =
USED_SOT_LIBS = 	$(TOOLSLIB)
USED_VCL_LIBS =		$(SOTLIB) $(TOOLSLIB) $(USED_UNO_LIBS)

# Applikationen
USED_BOOTSTRP_LIBS= $(TOOLSLIB)
USED_RCLIENT_LIBS =	$(VCLLIB) $(SOTLIB) $(TOOLSLIB) $(UNOLIB) \
                    $(USED_UNO_LIBS)

.ELSE				# "$(GUI)"=="UNX" || "$(GUI)"=="MAC"
AWTLIB*=jawt.lib
AVMEDIALIB=$(LIBPRE) iavmedia.lib
ICUINLIB=icuin.lib
ICULELIB=icule.lib
ICUUCLIB=icuuc.lib
I18NUTILLIB=$(LIBPRE) ii18nutil.lib
I18NISOLANGLIB=$(LIBPRE) ii18nisolang.lib
.IF "$(GPC)"!="NO"
GPC3RDLIB=gpc.lib
.ENDIF
SALHELPERLIB=$(LIBPRE) isalhelper.lib
XMLSCRIPTLIB=ixcr.lib
COMPHELPERLIB=icomphelp.lib
CONNECTIVITYLIB=connectivity.lib
LDAPBERLIB=ldapber.lib
CPPULIB=$(LIBPRE) icppu.lib
CPPUHELPERLIB=$(LIBPRE) icppuhelper.lib
UCBHELPERLIB=$(LIBPRE) iucbhelper.lib
ODBCLIB=$(LIBPRE) iodbc.lib
ODBCBASELIB=$(LIBPRE) iodbcbase.lib
DBFILELIB=$(LIBPRE) ifile$(OFFICEUPD).lib
TOOLSLIB=$(LIBPRE)itools.lib
TOOLSLIBST=$(LIBPRE) atools.lib
DVOSUTILLIBST=$(LIBPRE) dvosutil.lib
DUNOUTILLIBST=$(LIBPRE) dunoutil.lib
DVCLUTILLIBST=$(LIBPRE) dvclutil.lib
BPICONVLIB=$(LIBPRE) bpiconv.lib
SALLIB=$(LIBPRE) isal.lib
# make sure some ancient libs are empty
OSLLIB=
RTLLIB=
TECLIB=
VOSLIB=$(LIBPRE) ivos.lib
UNOLIB=$(LIBPRE) iuno.lib
UNOTOOLSLIB=$(LIBPRE) iutl.lib
RMCXTLIB=$(LIBPRE) irmcxt.lib
XMLOFFLIB=$(LIBPRE) ixo.lib
XMLOFFLLIB=$(LIBPRE) xol.lib
STORELIB=$(LIBPRE) istore.lib
IOTXLIB=$(LIBPRE) iotx.lib
OTXLIB=$(LIBPRE) otx_ind.lib
OSXLIB=$(LIBPRE) osx.lib
REGLIB=$(LIBPRE) ireg.lib
UCRLIB=$(LIBPRE) ucr.lib
UASLIB=$(LIBPRE) uas.lib
EXTLIB=$(LIBPRE) iext.lib
USRLIB=$(LIBPRE) usr.lib
SOTLIB=$(LIBPRE) sot.lib
ONELIB=$(LIBPRE) one.lib
MOZBASELIBST=$(LIBPRE) nspr4_s.lib $(LIBPRE) xpcombase_s.lib
MOZBASELIB=$(LIBPRE) nspr4.lib $(LIBPRE) xpcom.lib
LDAPSDKLIB=$(LIBPRE) nsldap32v50.lib
ONELIB=
UNOLIB=
PAPILIB=$(LIBPRE) ipap.lib
TRACERLIB=$(LIBPRE) itrc.lib
TRCLIB=$(TRACERLIB)
SETUPLIB=$(LIBPRE) iset.lib
SCHEDLIB=$(LIBPRE) ss.lib
SSCLIB=$(LIBPRE) ssc.lib
SSLLIB=$(LIBPRE) ssl.lib
SSWLIB=$(LIBPRE) ssw.lib
SFX2LIB=$(LIBPRE) sfx.lib
SFXLIB=$(SFX2LIB)
FWELIB=$(LIBPRE) ifwe.lib
FWILIB=$(LIBPRE) ifwi.lib
BTSTRPLIB=$(LIBPRE) btstrp.lib
BTSTRPDTLIB=$(LIBPRE) bootstrpdt.lib
TRANSEXLIB=$(LIBPRE) transex.lib
ICOLIB= $(LIBPRE) icom.lib
SVTOOLLIB=$(LIBPRE) svtool.lib
XMLSECLIB=$(LIBPRE) libxmlsec.lib
XMLSECLIB-MS=$(LIBPRE) libxmlsec-mscrypto.lib
XMLSECLIB-NSS=$(LIBPRE) libxmlsec-nss.lib
LIBXML2LIB=$(LIBPRE) libxml2.lib
NSS3LIB=$(LIBPRE) nss3.lib
NSPR4LIB=$(LIBPRE) nspr4.lib
PLC4LIB=$(LIBPRE) plc4.lib
NSSCRYPTOLIBS=$(LIBXML2LIB) $(XMLSECLIB) $(XMLSECLIB-NSS) $(NSS3LIB) $(NSPR4LIB) $(PLC4LIB)
MSCRYPTOLIBS=$(LIBXML2LIB) $(XMLSECLIB) $(XMLSECLIB-MS) $(LIBPRE) crypt32.lib $(LIBPRE) advapi32.lib
BROOKERLIB=$(LIBPRE) ibrooker.lib
SIMPLECMLIB=$(LIBPRE) isimplecm.lib
COMMUNILIB=$(LIBPRE) icommuni.lib
BTCOMMUNILIB=$(LIBPRE) ibtcommuni.lib
AUTOMATIONLIB=$(LIBPRE) ists.lib
SVLLIB=$(LIBPRE) isvl.lib
PLUGCTORLIB=$(LIBPRE) plugctor.lib
PLUGAPPLIB=$(LIBPRE) plugapp.lib
SVMEMLIB=$(LIBPRE) svmem.lib
SVUNZIPLIB=$(LIBPRE) svunzip.lib
SVUNZIPDLL=$(LIBPRE) unzipdll.lib
GOODIESLIB=$(LIBPRE) igo.lib
MAILLIB=$(LIBPRE) mail.lib
DOCMGRLIB=$(LIBPRE) docmgr.lib
BASICLIB=$(LIBPRE) basic.lib
BASICLIGHTLIB=$(LIBPRE) basicl.lib
SO2LIB=$(LIBPRE) so2.lib
TKTLIB=$(LIBPRE) tkt.lib
SJLIB=$(LIBPRE) sj.lib
SVXLIB=$(LIBPRE) isvx.lib
BASCTLLIB=$(LIBPRE) basctl.lib
BASICIDELIB=ybctl.lib
SVXLLIB=$(LIBPRE) svxl.lib
SDBLIB=$(LIBPRE) sdb.lib
DBTOOLSLIB=$(LIBPRE) idbtools.lib
HM2LIBSH=$(LIBPRE) hmwrpdll.lib
HM2LIBST=$(LIBPRE) hmwrap.lib
LINGULIB=$(HM2LIBST)
LNGLIB=$(LIBPRE) ilng.lib
EXPAT3RDLIB=$(LIBPRE) expat_xmltok.lib $(LIBPRE) expat_xmlparse.lib
EXPATASCII3RDLIB=$(LIBPRE) expat_xmltok.lib $(LIBPRE) ascii_expat_xmlparse.lib
ZLIB3RDLIB=$(LIBPRE) zlib.lib
JPEG3RDLIB=$(LIBPRE) jpeglib.lib
NEON3RDLIB=$(LIBPRE) neon.lib
BERKELEYLIB=$(LIBPRE) libdb42.lib
BERKELEYCPPLIB=
CURLLIB=$(LIBPRE) libcurl.lib
CHAOSLIB=$(LIBPRE) ichaos.lib
UUILIB=$(LIBPRE) iuui.lib
DGLIB=
SIMLIB=$(LIBPRE) ysim.lib
SCHLIB=$(LIBPRE) ysch.lib
SMLIB=$(LIBPRE) ysm.lib
SBALIB=$(LIBPRE) sbawrap.lib
OFALIB=$(LIBPRE) aofa.lib
SCLIB=$(LIBPRE) sclib.lib
SDLIB=$(LIBPRE) sdlib.lib
SDLLIB=$(LIBPRE) sdl.lib
SWLIB=$(LIBPRE) swlib.lib
PRXLIB=$(LIBPRE) ilprx2.lib
ISWLIB=$(LIBPRE) _sw.lib
ISCLIB=$(LIBPRE) sci.lib
ISDLIB=$(LIBPRE) sdi.lib
SVLIB=$(LIBPRE) ivcl.lib
VCLLIB=$(SVLIB)
BASEGFXLIB=$(LIBPRE) ibasegfx.lib
BASEBMPLIB=$(LIBPRE) ibasebmp.lib
CANVASTOOLSLIB=$(LIBPRE) icanvastools.lib
CPPCANVASLIB=$(LIBPRE) icppcanvas.lib
AGGLIB=$(LIBPRE) iagg.lib
TKLIB=$(LIBPRE) itk.lib
SVXLLIB=$(LIBPRE) svxl.lib
FREETYPELIB=freetype.lib
FREETYPELIBST=freetype.lib
PKGCHKLIB=$(LIBPRE) ipkgchk.lib
SYSSHELLLIB=sysshell.lib
SSOOPTLIB=issoopt$(OFFICEUPD)$(DLLPOSTFIX).lib
JVMACCESSLIB = $(LIBPRE) ijvmaccess.lib
CPPUNITLIB = cppunit.lib
XML2LIB = $(LIBPRE) libxml2.lib
JVMFWKLIB = $(LIBPRE) ijvmfwk.lib

# #110743#
# For BinFilters, some libs were added.
#

BFSVXLIB=$(LIBPRE) bf_svx.lib
BFSFXLIB=$(LIBPRE) bf_sfx.lib
BFSCHLIB=$(LIBPRE) bf_ysch.lib
BFSMLIB=$(LIBPRE) bf_ysm.lib
BFSCLIB=$(LIBPRE) bf_sclib.lib
BFSDLIB=$(LIBPRE) bf_sdlib.lib
BFSWLIB=$(LIBPRE) bf_swlib.lib
BFOFALIB=$(LIBPRE) bf_ofa.lib
BFXMLOFFLIB=$(LIBPRE) ibf_xo.lib
LEGACYSMGRLIB=$(LIBPRE) ilegacy_binfilters.lib

.ENDIF              # "$(GUI)"=="UNX" || "$(GUI)"=="MAC"

