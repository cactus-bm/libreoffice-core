#*************************************************************************
#
#   $RCSfile: tg_shl.mk,v $
#
#   $Revision: 1.44 $
#
#   last change: $Author: hjs $ $Date: 2001-09-28 17:11:33 $
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

MKFILENAME:=tg_shl.mk

#######################################################
# Anweisungen fuer Rekursion

.IF "$(MULTI_SHL_FLAG)" == ""
$(SHL1TARGETN) .NULL : SHL1

$(SHL2TARGETN) .NULL : SHL2

$(SHL3TARGETN) .NULL : SHL3

$(SHL4TARGETN) .NULL : SHL4

$(SHL5TARGETN) .NULL : SHL5

$(SHL6TARGETN) .NULL : SHL6

$(SHL7TARGETN) .NULL : SHL7

$(SHL8TARGETN) .NULL : SHL8

$(SHL9TARGETN) .NULL : SHL9
.ENDIF



.IF "$(MULTI_SHL_FLAG)"==""
SHL1 SHL2 SHL3 SHL4 SHL5 SHL6 SHL7 SHL8 SHL9:
.IF "$(GUI)"=="UNX"
    @dmake $(SHL$(TNR)TARGETN) MULTI_SHL_FLAG=true TNR:=$(TNR) $(MFLAGS) $(CALLMACROS)
.ELSE
    @dmake $(SHL$(TNR)TARGETN) MULTI_SHL_FLAG=true TNR:=$(TNR) $(MFLAGS) $(CALLMACROS)
.ENDIF
.ELSE

#######################################################
# Anweisungen fuer das Linken
# unroll begin

.IF "$(OS)"=="AIX"
SHL$(TNR)STDLIBS=
.ENDIF

# Link in static data members for template classes
.IF "$(OS)"=="MACOSX"
SHL$(TNR)STDLIBS+=$(STATICLIB)
.ENDIF

.IF "$(SHLLINKARCONLY)" != ""
SHL$(TNR)STDLIBS=
STDSHL=
.ELSE
SHL$(TNR)ARCHIVES=
.ENDIF

SHL$(TNR)DEF*=$(MISC)$/$(SHL$(TNR)TARGET).def

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#+++++++++++	description fallbak	++++++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.IF "$(SHL$(TNR)TARGET)"!=""
.IF "$(COMP$(TNR)TYPELIST)"==""

#fallback
LOCAL$(TNR)DESC:=$(subst,/,$/ $(shell find . -name {$(subst,$($(WINVERSIONNAMES)_MAJOR),* $(subst,$(UPD)$(DLLPOSTFIX), $(SHL$(TNR)TARGET)))}.xml))
.IF "$(LOCAL$(TNR)DESC)"==""
$(MISC)$/%{$(subst,$(UPD)$(DLLPOSTFIX),_dflt $(SHL$(TNR)TARGET))}.xml : $(SOLARENV)$/src$/default_description.xml
    +$(COPY) $< $@
.ELSE           # "$(LOCALDESC$(TNR))"==""
SHL$(TNR)DESCRIPTIONOBJ*=$(SLO)$/$(LOCAL$(TNR)DESC:b)$($(WINVERSIONNAMES)_MAJOR)_description.obj
.ENDIF          # "$(LOCALDESC$(TNR))"==""

.ENDIF          # "$(COMP$(TNR)TYPELIST)"==""
.ENDIF			# "$(SHL$(TNR)TARGET)"!="

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#+++++++++++    version object      ++++++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.IF "$(VERSIONOBJ)"!=""
.IF "$(UPDATER)"=="YES"
SHL$(TNR)VERSIONOBJ:=$(VERSIONOBJ:d){$(subst,$(UPD)$(DLLPOSTFIX),_dflt $(SHL$(TNR)TARGET))}$(VERSIONOBJ:f)
SHL$(TNR)VERSIONH:=$(INCCOM)$/_version.h
.ELSE			# "$(UPDATER)"=="YES"
SHL$(TNR)VERSIONOBJ:=$(VERSIONOBJ)
.ENDIF			# "$(UPDATER)"=="YES"
.ENDIF			# "$(VERSIONOBJ)"!=""

.IF "$(GUI)" != "UNX"
.IF "$(GUI)" == "WNT"
.IF "$(COM)" == "MSC"
.IF "$(SHL$(TNR)IMPLIB)" != ""
USE_$(TNR)IMPLIB=-implib:$(LB)$/$(SHL$(TNR)IMPLIB).lib
.ELSE			# "$(SHL$(TNR)IMPLIB)" != ""
USE_$(TNR)IMPLIB=-implib:$(LB)$/i$(TARGET)$(TNR).lib
.ENDIF			# "$(SHL$(TNR)IMPLIB)" != ""
.ENDIF			# "$(COM)" == "MSC"

.IF "$(USE_DEFFILE)"==""
USE_$(TNR)IMPLIB_DEPS=$(LB)$/$(SHL$(TNR)IMPLIB).lib
.ENDIF			# "$(USE_DEFFILE)"==""
.ENDIF			# "$(GUI)" == "WNT"
USE_SHL$(TNR)DEF=$(SHL$(TNR)DEF)
.ELSE			# "$(GUI)" != "UNX"
USE_SHL$(TNR)DEF=
SHL$(TNR)DEPN+:=$(SHL$(TNR)DEPNU)

# to activate vmaps remove "#"
#USE_SHL$(TNR)VERSIONMAP=$(MISC)$/$(SHL$(TNR)TARGET).vmap

.IF "$(USE_SHL$(TNR)VERSIONMAP)"!=""

.IF "$(DEF$(TNR)EXPORTFILE)"!=""
.IF "$(SHL$(TNR)VERSIONMAP)"!=""
#eine von beiden ist zuviel
USE_SHL$(TNR)VERSIONMAP=$(MISC)$/$(SHL$(TNR)TARGET).vmap
$(USE_SHL$(TNR)VERSIONMAP) .PHONY: 
    @+echo -----------------------------
    @+echo you should only use versionmap OR exportfile
    @+echo -----------------------------
#	force_dmake_to_error

.ENDIF			# "$(SHL$(TNR)VERSIONMAP)"!=""

.IF "$(OS)"!="MACOSX"
.IF "$(OS)"!="IRIX"
SHL$(TNR)VERSIONMAPPARA=$(LINKVERSIONMAPFLAG) $(USE_SHL$(TNR)VERSIONMAP)
.ENDIF
.ENDIF

$(USE_SHL$(TNR)VERSIONMAP): \
                    $(SHL$(TNR)OBJS)\
                    $(SHL$(TNR)LIBS)\
                    $(DEF$(TNR)EXPORTFILE)
    @+$(RM) $@.dump
.IF "$(SHL$(TNR)OBJS)"!=""
# dump remaining objects on the fly
.IF "$(OS)"=="MACOSX"
    @-+nm $(SHL$(TNR)OBJS:s/.obj/.o/) > $@.dump
.ELSE
    @+nm $(SHL$(TNR)OBJS:s/.obj/.o/) > $@.dump
.ENDIF
.ENDIF			# "$(SHL$(TNR)OBJS)!"=""
    @+$(TYPE) /dev/null $(SHL$(TNR)LIBS:s/.lib/.dump/) >> $@.dump
    @+$(SOLARENV)$/bin$/genmap -d $@.dump -e $(DEF$(TNR)EXPORTFILE) -o $@

.ELSE			# "$(DEF$(TNR)EXPORTFILE)"!=""
USE_SHL$(TNR)VERSIONMAP=$(MISC)$/$(SHL$(TNR)TARGET).vmap
$(USE_SHL$(TNR)VERSIONMAP) .PHONY: 
    @+echo -----------------------------
    @+echo DEF$(TNR)EXPORTFILE not set!
    @+echo -----------------------------
#	force_dmake_to_error
.ENDIF			# "$(DEF$(TNR)EXPORTFILE)"!=""
.ELSE			# "$(USE_SHL$(TNR)VERSIONMAP)"!=""

#and now for the plain non-generic way...
.IF "$(SHL$(TNR)VERSIONMAP)"!=""
USE_SHL$(TNR)VERSIONMAP=$(MISC)$/$(SHL$(TNR)VERSIONMAP:f)
.IF "$(OS)"!="MACOSX"
.IF "$(OS)"!="IRIX"
SHL$(TNR)VERSIONMAPPARA=$(LINKVERSIONMAPFLAG) $(USE_SHL$(TNR)VERSIONMAP)
.ENDIF
.ENDIF

$(USE_SHL$(TNR)VERSIONMAP): $(SHL$(TNR)VERSIONMAP)
    @+-$(RM) -f $@ >& $(NULLDEV)
    @+tr -d "\015" < $(SHL$(TNR)VERSIONMAP) > $@
    @+chmod a+w $@
    
.ENDIF			# "$(SHL$(TNR)VERSIONMAP)"!=""
.ENDIF			# "$(USE_SHL$(TNR)VERSIONMAP)"!=""
.ENDIF			# "$(GUI)" != "UNX"

.IF "$(UNIXVERSIONNAMES)"!=""
.IF "$(OS)"!="MACOSX"
.IF "$(GUI)"=="UNX"
SHL$(TNR)SONAME=\"$(SONAME_SWITCH)$(SHL$(TNR)TARGETN:b:b)\"
.ENDIF			# "$(GUI)"!="UNX"
.ENDIF			# "$(OS)"!="MACOSX"
.ENDIF			# "$(UNIXVERSIONNAMES)"!=""

.IF "$(NO_REC_RES)"!=""
.IF "$(SHL$(TNR)RES)"!=""
SHL$(TNR)RES!:=$(subst,$(RES)$/,$(RES)$/$(defaultlangext)$/ $(SHL$(TNR)RES))
SHL$(TNR)ALLRES+=$(SHL$(TNR)RES)
SHL$(TNR)LINKRES*=$(MISC)$/$(SHL$(TNR)TARGET).res
.ENDIF			# "$(SHL$(TNR)RES)"!=""
.ENDIF

.IF "$(SHL$(TNR)DEFAULTRES)$(use_shl_versions)"!=""
SHL$(TNR)DEFAULTRES*=$(MISC)$/$(SHL$(TNR)TARGET)_def.res
SHL$(TNR)ALLRES+=$(SHL$(TNR)DEFAULTRES)
SHL$(TNR)LINKRES*=$(MISC)$/$(SHL$(TNR)TARGET).res
.ENDIF			# "$(SHL$(TNR)DEFAULTRES)$(use_shl_versions)"!=""

.IF "$(NO_SHL$(TNR)DESCRIPTION)"==""
#SHL$(TNR)DESCRIPTIONOBJ*=$(SLO)$/default_description.obj
SHL$(TNR)DESCRIPTIONOBJ*=$(SLO)$/{$(subst,$(UPD)$(DLLPOSTFIX),_dflt $(SHL$(TNR)TARGET))}_description.obj
.ENDIF			# "$(NO_SHL$(TNR)DESCRIPTION)"==""

.IF "$(SHL$(TNR)TARGETN)"!=""

.IF "$(linkinc)"!=""
LINKINCTARGETS+=$(MISC)$/$(SHL$(TNR)TARGETN:b)_linkinc.ls
$(SHL$(TNR)TARGETN) : $(LINKINCTARGETS)
.ENDIF          # "$(linkinc)"!=""

$(SHL$(TNR)TARGETN) : \
                    $(SHL$(TNR)OBJS)\
                    $(SHL$(TNR)DESCRIPTIONOBJ)\
                    $(SHL$(TNR)LIBS)\
                    $(USE_$(TNR)IMPLIB_DEPS)\
                    $(USE_SHL$(TNR)DEF)\
                    $(USE_SHL$(TNR)VERSIONMAP)\
                    $(SHL$(TNR)RES)\
                    $(SHL$(TNR)VERSIONH)\
                    $(SHL$(TNR)DEPN)
    @echo ------------------------------
    @echo Making: $(SHL$(TNR)TARGETN)
.IF "$(UPDATER)"=="YES"
        @-+$(RM) $(SLO)$/{$(subst,$(UPD)$(DLLPOSTFIX),_dflt $(SHL$(TNR)TARGET))}_version.obj 
.ENDIF
.IF "$(GUI)"=="OS2"
.IF "$(UPDATER)"=="YES"
.IF "$(COM)"=="ICC"
        $(CC) -c -Fo$(SLO)$/{$(subst,$(UPD)$(DLLPOSTFIX),_dflt $(SHL$(TNR)TARGET))}_version.obj /Ge+ /Gs+ /Gt+ /Gd+ -DOS2 $(ENVCDEFS) -I$(INCCOM) $(SOLARENV)$/src$/version.cxx
.ELSE			# "$(COM)"=="ICC" 
        $(CC) -c -o$(SLO)$/{$(subst,$(UPD)$(DLLPOSTFIX),_dflt $(SHL$(TNR)TARGET))}_version.obj -Zomf -Zso -Zsys -DOS2 $(ENVCDEFS) -I$(INCCOM) $(SOLARENV)$/src$/version.cxx
.ENDIF			# "$(COM)"=="ICC" 
.ENDIF			# "$(UPDATER)"=="YES"
#
#	todo: try with $(LINKEXTENDLINE)!
#
    +-$(RM) $@
.IF "$(COM)"=="ICC"
    $(LINK) $(LINKFLAGS) $(LINKFLAGSSHL) @$(mktmp \
        $(STDSLO:+"+\n") $(SHL$(TNR)VERSIONOBJ:+"+\n") $(SHL$(TNR)OBJS:+"+\n")), \
        $(@), \
        $(MISC)$/$(@:b).map, \
        @$(mktmp $(SHL$(TNR)LIBS:+"+\n") $(SHL$(TNR)STDLIBS:+"+\n") $(STDSHL:+"+\n")), \
        $(SHL$(TNR)DEF:+"\n")
.ELSE
    $(LINK) -o $@ -Zdll -Zmap=$(MISC)$/$(@:b).map -L$(LB)  $(SHL$(TNR)LIBS:^"-l") -Ln:\toolkit4\lib -Ln:\emx09d\lib\mt  -Ln:\emx09d\lib -L$(SOLARLIBDIR) $(STDSLO) $(STDSHL:^"-l") $(SHL$(TNR)STDLIBS:^"-l") $(SHL$(TNR)OBJS) $(SHL$(TNR)VERSIONOBJ) $(SHL$(TNR)DESCRIPTIONOBJ) $(SHL$(TNR)DEF)
.ENDIF
.IF "$(SHL$(TNR)RES)" != ""
    $(RCLINK) $(RCLINKFLAGS) $(SHL$(TNR)RES) $@
.ENDIF			# "$(COMEX)"=="3"
.ENDIF			# "$(GUI)"=="OS2"
.IF "$(GUI)" == "WIN"
.IF "$(COM)"=="BLC"
    +$(LINK) @$(mktmp$ $(LINKFLAGS) $(LINKFLAGSSHL) $(STDSLO) $(SHL$(TNR)OBJS), $@, $(MISC)\$(@:b).map, $(SHL$(TNR)LIBS) $(SHL$(TNR)STDLIBS) $(STDSHL), $(SHL$(TNR)DEF)) >& $(TMP)$/$(PRJNAME)$(USER).tmp
    @+$(TYPE) $(TMP)$/$(PRJNAME)$(USER).tmp
    @+$(RM) $(TMP)$/$(PRJNAME)$(USER).tmp
.ENDIF			# "$(COM)"=="BLC"
.ENDIF			# "$(GUI)"=="WIN"
.IF "$(GUI)" == "WNT"
.IF "$(UPDATER)"=="YES"
.IF "$(COM)"=="GCC"
            gcc -c -o$(SLO)$/{$(subst,$(UPD)$(DLLPOSTFIX),_dflt $(SHL$(TNR)TARGET))}_version.obj -DWNT $(ENVCDEFS) -I$(INCCOM) $(SOLARENV)$/src$/version.cxx
.ELSE
            cl -c -Fo$(SLO)$/{$(subst,$(UPD)$(DLLPOSTFIX),_dflt $(SHL$(TNR)TARGET))}_version.obj -DWNT $(ENVCDEFS) -I$(INCCOM) $(SOLARENV)$/src$/version.cxx
.ENDIF			# "$(COM)"=="GCC"
.ENDIF			# "$(UPDATER)"=="YES"
.IF "$(SHL$(TNR)DEFAULTRES)"!=""
    @+-$(RM) $(MISC)$/$(SHL$(TNR)DEFAULTRES:b).rc >& $(NULLDEV)
.IF "$(SHL$(TNR)ICON)" != ""
    @-+echo 1 ICON $(SHL$(TNR)ICON) >> $(MISC)$/$(SHL$(TNR)DEFAULTRES:b).rc
.ENDIF
.IF "$(use_shl_versions)" != ""
    @-+echo #define VERVARIANT	$(BUILD) >> $(MISC)$/$(SHL$(TNR)DEFAULTRES:b).rc
    @-+echo #define ORG_NAME	$(SHL$(TNR)TARGET)$(DLLPOST) >> $(MISC)$/$(SHL$(TNR)DEFAULTRES:b).rc
    @-+echo #define INTERNAL_NAME $(SHL$(TNR)TARGET:b) >> $(MISC)$/$(SHL$(TNR)DEFAULTRES:b).rc
     @-+echo #include "shlinfo.rc" >> $(MISC)$/$(SHL$(TNR)DEFAULTRES:b).rc
.ENDIF			# "$(use_shl_versions)" != ""
    $(RC) -DWIN32 -I$(SOLARTESDIR) $(INCLUDE) $(RCLINKFLAGS) $(MISC)$/$(SHL$(TNR)DEFAULTRES:b).rc
.ENDIF			# "$(SHL$(TNR)DEFAULTRES)"!=""
.IF "$(SHL$(TNR)ALLRES)"!=""
    +$(COPY) /b $(SHL$(TNR)ALLRES:s/res /res+/) $(SHL$(TNR)LINKRES)
.ENDIF			# "$(SHL$(TNR)ALLRES)"!=""
.IF "$(linkinc)"==""
.IF "$(USE_DEFFILE)"!=""
.IF "$(BOTH)"!=""
.IF "$(PRODUCT)"=="full"
.IF "$(UPDATER)"=="YES"
#
#	product mit debug gelinkt
#
    $(LINK) @$(mktmp \
        $(LINKFLAGS) \
        $(LINKFLAGSSHL) \
        $(SHL$(TNR)STACK) $(SHL$(TNR)BASEX)	\
        /DEBUG /PDB:NONE \
        -out:$(BIN)$/_$(SHL$(TNR)TARGET).dll \
        -map:$(MISC)$/_$(@:b).map \
        -def:$(SHL$(TNR)DEF) \
        $(USE_$(TNR)IMPLIB) \
        $(STDOBJ) \
        $(SHL$(TNR)VERSIONOBJ) $(SHL$(TNR)DESCRIPTIONOBJ) $(SHL$(TNR)OBJS) \
        $(SHL$(TNR)LIBS) \
        $(SHL$(TNR)STDLIBS) \
        $(STDSHL) \
        $(SHL$(TNR)LINKRES) \
    )
.ENDIF			# "$(UPDATER)"=="YES"
.ENDIF			# "$(PRODUCT)"=="full"
.ENDIF			# "$(BOTH)"!=""
.IF "$(COM)"=="GCC"
    @+echo $(LINK) $(LINKFLAGS) $(LINKFLAGSSHL) -o$@ \
        $(STDOBJ) $(SHL$(TNR)VERSIONOBJ) $(SHL$(TNR)DESCRIPTIONOBJ) | tr -d �\r\n� > $(MISC)$/$(@:b).cmd
    @+$(TYPE) $(SHL$(TNR)LIBS) | sed s\#$(ROUT)\#$(PRJ)$/$/$(ROUT)\#g | tr -d �\r\n� >> $(MISC)$/$(@:b).cmd
    @+echo  $(SHL$(TNR)STDLIBS) $(STDSHL) $(SHL$(TNR)RES) >> $(MISC)$/$(@:b).cmd
    $(MISC)$/$(@:b).cmd
.ELSE
    $(LINK) @$(mktmp \
        $(LINKFLAGS) \
        $(LINKFLAGSSHL) \
        $(SHL$(TNR)STACK) $(SHL$(TNR)BASEX)	\
        -out:$@ \
        -map:$(MISC)$/$(@:b).map \
        -def:$(SHL$(TNR)DEF) \
        $(USE_$(TNR)IMPLIB) \
        $(STDOBJ) \
        $(SHL$(TNR)VERSIONOBJ) $(SHL$(TNR)DESCRIPTIONOBJ) $(SHL$(TNR)OBJS) \
        $(SHL$(TNR)LIBS) \
        $(SHL$(TNR)STDLIBS) \
        $(STDSHL) \
        $(SHL$(TNR)LINKRES) \
    )
.ENDIF			# "$(COM)"=="GCC"
.ELSE			# "$(USE_DEFFILE)"!=""
    $(LINK) @$(mktmp	$(LINKFLAGS)			\
        $(LINKFLAGSSHL) $(SHL$(TNR)BASEX)		\
        $(SHL$(TNR)STACK) -out:$(SHL$(TNR)TARGETN)	\
        -map:$(MISC)$/$(@:B).map				\
        $(LB)$/$(SHL$(TNR)IMPLIB).exp				\
        $(STDOBJ)							\
        $(SHL$(TNR)OBJS) $(SHL$(TNR)VERSIONOBJ) $(SHL$(TNR)DESCRIPTIONOBJ)   \
        $(SHL$(TNR)LIBS)                         \
        $(SHL$(TNR)STDLIBS)                      \
        $(STDSHL)                           \
        $(SHL$(TNR)LINKRES) \
    )
.IF "$(BOTH)"!=""
.IF "$(PRODUCT)"=="full"
.IF "$(UPDATER)"=="YES"
#
#	product mit debug gelinkt
#
    $(LINK) @$(mktmp	$(LINKFLAGS)			\
        $(LINKFLAGSSHL) $(SHL$(TNR)BASEX)		\
        /DEBUG /PDB:NONE \
        $(SHL$(TNR)STACK) -out:$(BIN)$/_$(SHL$(TNR)TARGET).dll	\
        -map:$(MISC)$/_$(@:B).map				\
        $(LB)$/$(SHL$(TNR)IMPLIB).exp				\
        $(STDOBJ)							\
        $(SHL$(TNR)OBJS) $(SHL$(TNR)VERSIONOBJ) $(SHL$(TNR)DESCRIPTIONOBJ)    \
        $(SHL$(TNR)LIBS)                         \
        $(SHL$(TNR)STDLIBS)                      \
        $(STDSHL)                           \
        $(SHL$(TNR)LINKRES) \
    )
.ENDIF			# "$(UPDATER)"=="YES"
.ENDIF			# "$(PRODUCT)"=="full"
.ENDIF			# "$(BOTH)"!=""
.ENDIF			# "$(USE_DEFFILE)"!=""
.ELSE			# "$(linkinc)"==""
        +if exist $(MISC)$/$(SHL$(TNR)TARGET).lnk del $(MISC)$/$(SHL$(TNR)TARGET).lnk
        +if exist $(MISC)$/$(SHL$(TNR)TARGET).lst del $(MISC)$/$(SHL$(TNR)TARGET).lst
        +type $(mktmp \
        $(LINKFLAGS) \
        $(LINKFLAGSSHL) $(SHL$(TNR)BASEX) \
        $(SHL$(TNR)STACK) $(MAPFILE) \
        -out:$@ \
        $(LB)$/$(SHL$(TNR)IMPLIB).exp \
        $(STDOBJ) \
        $(SHL$(TNR)OBJS) \
        $(SHL$(TNR)STDLIBS) \
        $(STDSHL) \
        $(SHL$(TNR)LINKRES) \
        ) >> $(MISC)$/$(SHL$(TNR)TARGET).lnk
        +type $(MISC)$/$(SHL$(TNR)TARGETN:b)_linkinc.ls  >> $(MISC)$/$(SHL$(TNR)TARGET).lnk
        $(LINK) @$(MISC)$/$(SHL$(TNR)TARGET).lnk
.ENDIF			# "$(linkinc)"==""
.ENDIF			# "$(GUI)" == "WNT"
.IF "$(GUI)"=="UNX"
.IF "$(UPDATER)"=="YES"
.IF "$(OS)"=="SOLARIS"
.IF "$(COM)"=="GCC"
        $(CC) -c -fPIC -o $(SLO)$/{$(subst,$(UPD)$(DLLPOSTFIX),_dflt $(SHL$(TNR)TARGET))}_version.o -DUNX $(ENVCDEFS) -I$(INCCOM) $(SOLARENV)$/src$/version.cxx
.ELSE		
        $(CC) -c -KPIC -o $(SLO)$/{$(subst,$(UPD)$(DLLPOSTFIX),_dflt $(SHL$(TNR)TARGET))}_version.o -DUNX $(ENVCDEFS) -I$(INCCOM) $(SOLARENV)$/src$/version.cxx
.ENDIF		
.ENDIF
.IF "$(OS)"=="MACOSX"
        $(CC) -c -dynamic -o $(SLO)$/{$(subst,$(UPD)$(DLLPOSTFIX),_dflt $(SHL$(TNR)TARGET))}_version.o -DUNX $(ENVCDEFS) -I$(INCCOM) $(SOLARENV)$/src$/version.cxx
        @echo "------------------------------"
        @echo "Updating static data member initializations"
        @+dmake -f $(SOLARENV)$/$(OUTPATH)$/inc/makefile.mk $(MFLAGS) $(CALLMACROS) "PRJ=$(PRJ)" "PRJNAME=$(PRJNAME)" "TARGET=$(TARGET)"
.ENDIF
.IF "$(OS)"=="LINUX" || "$(OS)"=="NETBSD" || "$(OS)"=="FREEBSD"
        $(CC) -c -fPIC -o $(SLO)$/{$(subst,$(UPD)$(DLLPOSTFIX),_dflt $(SHL$(TNR)TARGET))}_version.o -DUNX $(ENVCDEFS) -I$(INCCOM) $(SOLARENV)$/src$/version.cxx
.ENDIF
.IF "$(OS)"=="IRIX"
        $(CC) -o $(SLO)$/{$(subst,$(UPD)$(DLLPOSTFIX),_dflt $(SHL$(TNR)TARGET))}_version.o -DUNX $(ENVCDEFS) -I$(INCCOM) $(SOLARENV)$/src$/version.cxx
        @+if ( ! -e $(SOLARLIBDIR) ) mkdir $(SOLARLIBDIR)
        @+if ( ! -e $(SOLARLIBDIR)/so_locations ) touch $(SOLARLIBDIR)/so_locations
.ENDIF			# "$(OS)"=="IRIX"
.ENDIF
.IF "$(OS)"=="MACOSX"
    @+-$(RM) $(MISC)$/$(@:b).list
    @+-$(RM) $(MISC)$/$(@:b).cmd
    @+echo $(STDSLO) $(SHL$(TNR)OBJS:s/.obj/.o/) \
    $(SHL$(TNR)VERSIONOBJ) $(SHL$(TNR)DESCRIPTIONOBJ:s/.obj/.o/) \
    `cat /dev/null $(SHL$(TNR)LIBS) | sed s\#$(ROUT)\#$(PRJ)$/$(ROUT)\#g` | tr -s " " "\n" > $(MISC)$/$(@:b).list
    @+echo $(LINK) $(LINKFLAGS) $(LINKFLAGSSHL) -L$(PRJ)$/$(ROUT)$/lib $(SOLARLIB) -o $@ \
    $(SHL$(TNR)STDLIBS) $(SHL$(TNR)ARCHIVES) $(STDSHL) -filelist $(MISC)$/$(@:b).list $(LINKOUTPUT_FILTER) > $(MISC)$/$(@:b).cmd
    @cat $(MISC)$/$(@:b).cmd
    @+source $(MISC)$/$(@:b).cmd
# This is a hack as libstatic and libcppuhelper have a circular dependency
.IF "$(PRJNAME)"=="cppuhelper"
    @echo "------------------------------"
    @echo "Rerunning static data member initializations"
    @+dmake -u -f $(SOLARENV)$/$(OUTPATH)$/inc/makefile.mk $(MFLAGS) $(CALLMACROS) "PRJ=$(PRJ)" "PRJNAME=$(PRJNAME)" "TARGET=$(TARGET)"
.ENDIF
.IF "$(SHL$(TNR)VERSIONMAP)"!=""
    @strip -i -r -u -s $(SHL$(TNR)VERSIONMAP) $@
.ENDIF
    @echo "Making: $@.framework"
    @create-bundle $@
.IF "$(UPDATER)"=="YES"
.IF "$(SHL$(TNR)NOCHECK)"==""
    +$(SOLARENV)$/bin$/checkdll.sh -L$(LB) $(SOLARLIB) $(SHL$(TNR)TARGETN).framework
.ENDIF				# "$(SHL$(TNR)NOCHECK)"!=""
.ENDIF
.ELSE			# "$(OS)"=="MACOSX"
    @+-$(RM) $(MISC)$/$(@:b).cmd
    @+echo $(LINK) $(LINKFLAGS) $(SHL$(TNR)SONAME) $(LINKFLAGSSHL) $(SHL$(TNR)VERSIONMAPPARA) -L$(PRJ)$/$(ROUT)$/lib $(SOLARLIB) $(STDSLO) $(SHL$(TNR)OBJS:s/.obj/.o/) \
    $(SHL$(TNR)VERSIONOBJ) $(SHL$(TNR)DESCRIPTIONOBJ:s/.obj/.o/) -o $@ \
    `cat /dev/null $(SHL$(TNR)LIBS) | tr -s " " "\n" | sed s\#$(ROUT)\#$(PRJ)$/$(ROUT)\#g` \
    $(SHL$(TNR)STDLIBS) $(SHL$(TNR)ARCHIVES) $(STDSHL) $(LINKOUTPUT_FILTER) > $(MISC)$/$(@:b).cmd
    @cat $(MISC)$/$(@:b).cmd
    @+source $(MISC)$/$(@:b).cmd
.IF "$(OS)"=="S390"
    +mv -f ($@:s/$(DLLPOST)/.x/) $(LB)
.ENDIF
.IF "$(UPDATER)"=="YES"
.IF "$(SHL$(TNR)NOCHECK)"==""
    +$(SOLARENV)$/bin$/checkdll.sh -L$(LB) $(SOLARLIB:s/2.6//) $(SHL$(TNR)TARGETN)
.ENDIF				# "$(SHL$(TNR)NOCHECK)"!=""
.ENDIF			# "$(UPDATER)"=="YES"
.IF "$(UNIXVERSIONNAMES)"!=""
    +$(RM) $(LB)$/$(SHL$(TNR)TARGETN:b:b:b)
    +$(RM) $(LB)$/$(SHL$(TNR)TARGETN:b:b)
    +cd $(LB) && ln -s $(SHL$(TNR)TARGETN:f) $(SHL$(TNR)TARGETN:b:b)
    +cd $(LB) && ln -s $(SHL$(TNR)TARGETN:f:b:b) $(SHL$(TNR)TARGETN:b:b:b)
.ENDIF			# "$(UNIXVERSIONNAMES)"!=""
.ENDIF			# "$(OS)"=="MACOSX"
    @ls -l $@
.ENDIF			# "$(GUI)" == "UNX"
.IF "$(GUI)"=="MAC"
    @+-$(RM) $@ $@.xSYM
    $(LINK) $(LINKFLAGS) $(LINKFLAGSSHL) $(foreach,i,$(shell $(UNIX2MACPATH) $(PRJ)$/$(ROUT)$/lib $(SOLARLIB:s/-L//)) -L"$i") $(shell $(UNIX2MACPATH) $(STDSLO) $(SHL$(TNR)OBJS) `cat /dev/null $(SHL$(TNR)LIBS) | sed s\#$(ROUT)\#$(PRJ)$/$(ROUT)\#g` $(SHL$(TNR)VERSIONOBJ) $(SHL$(TNR)DESCRIPTIONOBJ)) $(SHL$(TNR)STDLIBS) $(SHL$(TNR)ARCHIVES) $(STDSHL) $(LINKOUTPUT_FILTER) -o $(shell $(UNIX2MACPATH) $@)
.ENDIF			# "$(GUI)"=="MAC"
.ENDIF			# "$(SHL$(TNR)TARGETN)"!=""

# Anweisungen fuer das Linken
# unroll end
#######################################################

.ENDIF	# MULTI_SHL_FLAG

#-------------------------------------------------------------------------


.IF "$(MULTI_SHLIMP_FLAG)"==""
# MULTITARGET: SHLNIMP --- hier einfuegen

$(SHL1IMPLIBN) .NULL : SHL1IMP

$(SHL2IMPLIBN) .NULL : SHL2IMP

$(SHL3IMPLIBN) .NULL : SHL3IMP

$(SHL4IMPLIBN) .NULL : SHL4IMP

$(SHL5IMPLIBN) .NULL : SHL5IMP

$(SHL6IMPLIBN) .NULL : SHL6IMP

$(SHL7IMPLIBN) .NULL : SHL7IMP

$(SHL8IMPLIBN) .NULL : SHL8IMP

$(SHL9IMPLIBN) .NULL : SHL9IMP
.ENDIF



.IF "$(MULTI_SHLIMP_FLAG)"==""

# MULTITARGET: SHLNIMP --- hier einfuegen

SHL1IMP SHL2IMP SHL3IMP SHL4IMP SHL5IMP SHL6IMP SHL7IMP SHL8IMP SHL9IMP:
.IF "$(GUI)" != "MAC"
    @dmake $(SHL$(TNR)IMPLIBN) MULTI_SHLIMP_FLAG=true TNR:=$(TNR) $(MFLAGS) $(CALLMACROS)
.ELSE
    @dmake "$(SHL$(TNR)IMPLIBN)" MULTI_SHLIMP_FLAG=true TNR:=$(TNR) $(MFLAGS) $(CALLMACROS)
.ENDIF
.ELSE


#######################################################
# Anweisungen fuer das Linken
# unroll begin

.IF "$(SHL$(TNR)IMPLIBN)" != ""

.IF "$(UPDATER)"=="YES"
USELIBDEPN=$(SHL$(TNR)LIBS)
.ELSE
.ENDIF

.IF "$(USE_DEFFILE)"!=""
USE_SHLTARGET=$(SHL$(TNR)TARGETN)
.ENDIF

.IF "$(GUI)" != "UNX"
$(SHL$(TNR)IMPLIBN):	\
                    $(SHL$(TNR)DEF) \
                    $(USE_SHLTARGET) \
                    $(USELIBDEPN)
.ELSE
$(SHL$(TNR)IMPLIBN):	\
                    $(SHL$(TNR)LIBS)
.ENDIF
    @echo ------------------------------
    @echo Making: $(SHL$(TNR)IMPLIBN)
.IF "$(GUI)" == "WNT"
# bei use_deffile implib von linker erstellt
.IF "$(USE_DEFFILE)"==""
    $(IMPLIB) $(IMPLIBFLAGS) @$(mktmp -out:$(SHL$(TNR)IMPLIBN) \
    -def:$(SHL$(TNR)DEF) )
.ELSE			# "$(GUI)" == "WNT"
    @+if exist $@ $(TOUCH) $@
    @+if not exist $@ echo rebuild $(SHL$(TNR)TARGETN) to get $@
.ENDIF			# "$(GUI)" == "WNT"
.ELSE
.IF "$(GUI)" == "WIN" || "$(GUI)" == "OS2"
.IF "$(USE_DEFFILE)"==""
    $(IMPLIB) $(IMPLIBFLAGS) $@ $(SHL$(TNR)DEF)
.ELSE
    $(IMPLIB) $(IMPLIBFLAGS) $@ $(SHL$(TNR)TARGETN)
.ENDIF
.ELSE
    @echo keine ImportLibs auf Mac und *ix
    @+-$(RM) $@
    @$(TOUCH) $@
.ENDIF
.ENDIF
.ENDIF

# Anweisungen fuer das Linken
# unroll end
#######################################################

.ENDIF	# MULTI_SHLIMP_FLAG

#------------------------------------------------------------------------


