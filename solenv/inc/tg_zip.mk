#*************************************************************************
#
#   $RCSfile: tg_zip.mk,v $
#
#   $Revision: 1.22 $
#
#   last change: $Author: rt $ $Date: 2004-12-16 12:58:28 $
#
#   The Contents of this file are made available subject to the terms of
#   either of the following licenses
#
#          - GNU Lesser General Public License Version 2.1
#          - Sun Industry Standards Source License Version 1.1
#
#   Sun Microsystems Inc., October, 2000
#
#   GNU Lesser General Public License Version 2.1
#   =============================================
#   Copyright 2000 by Sun Microsystems, Inc.
#   901 San Antonio Road, Palo Alto, CA 94303, USA
#
#   This library is free software; you can redistribute it and/or
#   modify it under the terms of the GNU Lesser General Public
#   License version 2.1, as published by the Free Software Foundation.
#
#   This library is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public
#   License along with this library; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#   MA  02111-1307  USA
#
#
#   Sun Industry Standards Source License Version 1.1
#   =================================================
#   The contents of this file are subject to the Sun Industry Standards
#   Source License Version 1.1 (the "License"); You may not use this file
#   except in compliance with the License. You may obtain a copy of the
#   License at http://www.openoffice.org/license.html.
#
#   Software provided under this License is provided on an "AS IS" basis,
#   WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
#   WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
#   MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
#   See the License for the specific provisions governing your rights and
#   obligations concerning the Software.
#
#   The Initial Developer of the Original Code is: Sun Microsystems, Inc.
#
#   Copyright: 2000 by Sun Microsystems, Inc.
#
#   All Rights Reserved.
#
#   Contributor(s): _______________________________________
#
#
#
#*************************************************************************


.IF "$(MULTI_ZIP_FLAG)" == ""
$(ZIP1TARGETN) .NULL : ZIP1
ZIP1 ?= TNR!:=1

$(ZIP2TARGETN) .NULL : ZIP2
ZIP2 ?= TNR!:=2

$(ZIP3TARGETN) .NULL : ZIP3
ZIP3 ?= TNR!:=3

$(ZIP4TARGETN) .NULL : ZIP4
ZIP4 ?= TNR!:=4

$(ZIP5TARGETN) .NULL : ZIP5
ZIP5 ?= TNR!:=5

$(ZIP6TARGETN) .NULL : ZIP6
ZIP6 ?= TNR!:=6

$(ZIP7TARGETN) .NULL : ZIP7
ZIP7 ?= TNR!:=7

$(ZIP8TARGETN) .NULL : ZIP8
ZIP8 ?= TNR!:=8

$(ZIP9TARGETN) .NULL : ZIP9
ZIP9 ?= TNR!:=9


$(ZIP1DEPFILE) .NULL : ZIPDEP1
ZIPDEP1 ?= TNR!:=1

$(ZIP2DEPFILE) .NULL : ZIPDEP2
ZIPDEP2 ?= TNR!:=2

$(ZIP3DEPFILE) .NULL : ZIPDEP3
ZIPDEP3 ?= TNR!:=3

$(ZIP4DEPFILE) .NULL : ZIPDEP4
ZIPDEP4 ?= TNR!:=4

$(ZIP5DEPFILE) .NULL : ZIPDEP5
ZIPDEP5 ?= TNR!:=5

$(ZIP6DEPFILE) .NULL : ZIPDEP6
ZIPDEP6 ?= TNR!:=6

$(ZIP7DEPFILE) .NULL : ZIPDEP7
ZIPDEP7 ?= TNR!:=7

$(ZIP8DEPFILE) .NULL : ZIPDEP8
ZIPDEP8 ?= TNR!:=8

$(ZIP9DEPFILE) .NULL : ZIPDEP9
ZIPDEP9 ?= TNR!:=9
.ENDIF

.IF "$(MULTI_ZIP_FLAG)"==""
ZIP1 ZIP2 ZIP3 ZIP4 ZIP5 ZIP6 ZIP7 ZIP8 ZIP9:
    +@dmake $(ZIP$(TNR)TARGETN) MULTI_ZIP_FLAG=true TNR:=$(TNR) $(MFLAGS) $(CALLMACROS)

ZIPDEP1 ZIPDEP2 ZIPDEP3 ZIPDEP4 ZIPDEP5 ZIPDEP6 ZIPDEP7 ZIPDEP8 ZIPDEP9:
    +@dmake $(ZIPDEP$(TNR)FILE) MULTI_ZIP_FLAG=true TNR:=$(TNR) $(MFLAGS) $(CALLMACROS)
.ELSE


#######################################################
# Anweisungen fuer das Linken
# unroll begin

.IF "$(GUI)"=="WNT"
command_seperator=&&
#command_seperator=^
avoid_cvs_dir=-x "*CVS*"
.ELSE
command_seperator=;
avoid_cvs_dir=-x "*CVS*"
.ENDIF

.IF "$(ZIP$(TNR)TARGET)"!=""
ZIP$(TNR)DIR*=$(ZIPDIR)
ZIP$(TNR)FLAGS*=$(ZIPFLAGS)
.IF "$(zip$(TNR)generatedlangs)"!=""
zip$(TNR)langdirs*=$(alllangiso)
.ELSE           # "$(zip$(TNR)generatedlangs)"!=""
.IF "$(GUI)"=="UNX" || "$(USE_SHELL)"!="4nt"
zip$(TNR)langdirs:=$(shell +find {$(subst,$/$(LANGDIR), $(null,$(ZIP$(TNR)DIR) . $(ZIP$(TNR)DIR)))}/ -type d ! -name CVS ! -name "." | sed "s/\.\/\///" | sed "s/\.\///" )
.ELSE			# "$(GUI)"=="UNX"
zip$(TNR)langdirs:=$(subst,CVS, $(shell +-dir {$(subst,$/$(LANGDIR), $(ZIP$(TNR)DIR))} /ba:d ))
.ENDIF			# "$(GUI)"=="UNX"
.ENDIF          # "$(zip$(TNR)generatedlangs)"!=""
.IF "$(ZIP$(TNR)FORCEALLLANG)"!=""
zip$(TNR)alllangiso:=$(foreach,i,$(completelangiso) $(foreach,j,$(zip$(TNR)langdirs) $(eq,$i,$j  $i $(NULL))))
.ELSE          # "$(ZIP$(TNR)ALLLANG)" != ""
zip$(TNR)alllangiso:=$(foreach,i,$(alllangiso) $(foreach,j,$(zip$(TNR)langdirs) $(eq,$i,$j  $i $(NULL))))
.ENDIF          # "$(ZIP$(TNR)ALLLANG)" != ""
.ENDIF			# "$(ZIP$(TNR)TARGET)"!=""

.IF "$(ZIP$(TNR)TARGETN)"!=""

ZIP$(TNR)TMP:=$(mktmp iii)

$(MISC)$/$(TARGET).$(PWD:f).$(ZIP$(TNR)TARGET).dpzz : $(ZIP$(TNR)TARGETN)

.IF "$(common_build_zip)"!=""
ZIP$(TNR)HELPVAR={$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(BIN))$/$(ZIP$(TNR)TARGET)}
.ELSE			# "$(common_build_zip)"!=""
ZIP$(TNR)HELPVAR=$(BIN)$/$(ZIP$(TNR)TARGET)
.ENDIF			# "$(common_build_zip)"!=""

$(ZIP$(TNR)DEPFILE) :
    +echo # > $(MISC)$/$(@:f)
.IF "$(common_build_zip)"!=""
.IF "$(ZIP$(TNR)DIR)" != ""
    @+echo type 1
    +-$(CDD) $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP$(TNR)DIR))) $(command_seperator) $(ZIPDEP) $(ZIP$(TNR)FLAGS) -prefix $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP$(TNR)DIR)))$/ $(subst,$(OUTPATH),$(COMMON_OUTDIR) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/))) $(foreach,j,$(ZIP$(TNR)LIST) "{$(subst,LANGDIR,{$(subst,$(ZIP$(TNR)TARGET)_, $(@:f:b))} $j )}") $(avoid_cvs_dir) >> $(PWD)$/$(PRJ)$/$(ROUT)$/misc$/$(@:f)
.ELSE			# "$(ZIP$(TNR)DIR)" != ""
    @+echo type 2
    +-$(ZIPDEP) $(ZIP$(TNR)FLAGS) $(subst,$(OUTPATH),$(COMMON_OUTDIR) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/))) $(foreach,j,$(ZIP$(TNR)LIST) "{$(subst,LANGDIR,{$(subst,$(ZIP$(TNR)TARGET)_, $(@:f:b))} $j )}") $(avoid_cvs_dir) >> $(MISC)$/$(@:f)
.ENDIF			# "$(ZIP$(TNR)DIR)" != ""
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP$(TNR)DIR)" != ""
    @+echo type 3
    +-$(CDD) $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP$(TNR)DIR))) $(command_seperator) $(ZIPDEP) $(ZIP$(TNR)FLAGS) -prefix $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP$(TNR)DIR)))$/ $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/)) $(foreach,j,$(ZIP$(TNR)LIST) "{$(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP$(TNR)TARGET)_, $(subst,$/misc$/,$/bin$/ $(@:db)))} $j )}") $(avoid_cvs_dir) >> $(PWD)$/$(PRJ)$/$(ROUT)$/misc$/$(@:f)
.ELSE			# "$(ZIP$(TNR)DIR)" != ""
    @+echo type 4
    +-$(ZIPDEP) $(ZIP$(TNR)FLAGS) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/)) $(foreach,j,$(ZIP$(TNR)LIST) "{$(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP$(TNR)TARGET)_, $(subst,$/misc$/,$/bin$/ $(@:db)))} $j )}") $(avoid_cvs_dir) >> $(MISC)$/$(@:f)
.ENDIF			# "$(ZIP$(TNR)DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
    @+echo $@ : makefile.mk >> $(MISC)$/$(@:f)

   
$(ZIP$(TNR)TARGETN) : delzip
    @+echo ------------------------------
    @+echo Making: $@
    @$(!eq,$?,$(?:s/delzip/zzz/) +-$(RM) +echo) $(uniq $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)) >&$(NULLDEV)
    @+echo rebuilding zipfiles $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
    @+echo ------------------------------ $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
.IF "$(common_build_zip)"!=""
.IF "$(ZIP$(TNR)DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP$(TNR)TMP).{$(subst,$(ZIP$(TNR)HELPVAR),_ $(@:db))}.zip >& $(NULLDEV)
    +-$(CDD) $(subst,LANGDIR,{$(subst,$(ZIP$(TNR)HELPVAR)_, $(@:db))} $(ZIP$(TNR)DIR)) $(command_seperator) zip $(ZIP$(TNR)FLAGS) $(ZIP$(TNR)TMP).{$(subst,$(ZIP$(TNR)HELPVAR),_ $(@:db))}.zip $(subst,LANGDIR_away$/, $(ZIP$(TNR)LIST:s/LANGDIR/LANGDIR_away/)) -x delzip $(avoid_cvs_dir) 
    +$(COPY) $(ZIP$(TNR)TMP).{$(subst,$(ZIP$(TNR)HELPVAR),_ $(@:db))}.zip $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)    
    +$(RM) $(ZIP$(TNR)TMP).{$(subst,$(ZIP$(TNR)HELPVAR),_ $(@:db))}.zip
.ELSE			# "$(ZIP$(TNR)DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) >& $(NULLDEV)
    +-zip $(ZIP$(TNR)FLAGS) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $(foreach,j,$(ZIP$(TNR)LIST) $(subst,LANGDIR,{$(subst,$(ZIP$(TNR)HELPVAR)_, $(@:db))} $j )) -x delzip $(avoid_cvs_dir)
.ENDIF			# "$(ZIP$(TNR)DIR)" != ""
    @+-$(GNUCOPY) -p $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $@.$(INPATH) >& $(NULLDEV)
    @+-$(RM) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)
    @+-$(RM) $@ >& $(NULLDEV)
.IF "$(GUI)"=="UNX"
    @+if ( -r $@.$(INPATH) ) $(RENAME) $@.$(INPATH) $@
#	@+if ( -r $@ ) $(TOUCH) $@
.ELSE			# "$(GUI)"=="UNX"
    @+if exist $@.$(INPATH) $(RENAME) $@.$(INPATH) $@
#	@+if exist $@ $(TOUCH) $@
.ENDIF			# "$(GUI)"=="UNX"
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP$(TNR)DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP$(TNR)TMP).{$(subst,$(ZIP$(TNR)HELPVAR),_ $(@:db))}.zip >& $(NULLDEV)
    +-$(CDD) $(subst,LANGDIR,{$(subst,$(ZIP$(TNR)HELPVAR)_, $(@:db))} $(ZIP$(TNR)DIR)) $(command_seperator) zip $(ZIP$(TNR)FLAGS) $(ZIP$(TNR)TMP).{$(subst,$(ZIP$(TNR)HELPVAR),_ $(@:db))}.zip $(subst,LANGDIR_away$/, $(ZIP$(TNR)LIST:s/LANGDIR/LANGDIR_away/)) -x delzip  $(avoid_cvs_dir)
    +$(COPY) $(ZIP$(TNR)TMP).{$(subst,$(ZIP$(TNR)HELPVAR),_ $(@:db))}.zip  $@ 
    +$(RM) $(ZIP$(TNR)TMP).{$(subst,$(ZIP$(TNR)HELPVAR),_ $(@:db))}.zip
.ELSE			# "$(ZIP$(TNR)DIR)" != ""
    +-zip $(ZIP$(TNR)FLAGS) $@ $(foreach,j,$(ZIP$(TNR)LIST) $(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP$(TNR)TARGET)_, $(@:db))} $j )) -x delzip $(avoid_cvs_dir)
.ENDIF			# "$(ZIP$(TNR)DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
.ENDIF

# unroll end
#######################################################

.ENDIF		# "$(MULTI_ZIP_FLAG)"==""
