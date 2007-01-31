#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: tg_shl.mk,v $
#
#   $Revision: 1.105 $
#
#   last change: $Author: rt $ $Date: 2007-01-31 08:59:13 $
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

#######################################################
# targets for linking
# unroll begin

.IF "$(SHL$(TNR)TARGETN)"!=""

.IF "$(OS)"=="AIX"
SHL$(TNR)STDLIBS=
.ENDIF

.IF "$(SHLLINKARCONLY)" != ""
SHL$(TNR)STDLIBS=
STDSHL=
.ELSE
SHL$(TNR)ARCHIVES=
.ENDIF

# decide how to link
.IF "$(SHL$(TNR)CODETYPE)"=="C"
SHL$(TNR)LINKER=$(LINKC)
SHL$(TNR)STDSHL=$(subst,CPPRUNTIME, $(STDSHL))
SHL$(TNR)LINKFLAGS+=$(LINKCFLAGS)
.ELSE			# "$(SHL$(TNR)CODETYPE)"=="C"
SHL$(TNR)LINKER=$(LINK)
SHL$(TNR)STDSHL=$(subst,CPPRUNTIME,$(STDLIBCPP) $(STDSHL))
SHL$(TNR)LINKFLAGS+=$(LINKFLAGS)
.ENDIF			# "$(SHL$(TNR)CODETYPE)"=="C"

.IF "$(SHL$(TNR)USE_EXPORTS)"==""
SHL$(TNR)DEF*=$(MISC)$/$(SHL$(TNR)TARGET).def
.ENDIF			# "$(SHL$(TNR)USE_EXPORTS)"==""

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#+++++++++++	description fallbak	++++++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.IF "$(SHL$(TNR)TARGET)"!=""
.IF "$(COMP$(TNR)TYPELIST)"==""

#fallback
LOCAL$(TNR)DESC:=$(subst,/,$/ $(shell $(FIND) . -name "{$(subst,$($(WINVERSIONNAMES)_MAJOR),* $(subst,$(UPD)$(DLLPOSTFIX), $(SHL$(TNR)TARGET)))}.xml"))
.IF "$(LOCAL$(TNR)DESC)"==""
$(MISC)$/%{$(subst,$(UPD)$(DLLPOSTFIX),_dflt $(SHL$(TNR)TARGET))}.xml : $(SOLARENV)$/src$/default_description.xml
    $(COPY) $< $@
.ELSE           # "$(LOCALDESC$(TNR))"==""
SHL$(TNR)DESCRIPTIONOBJ*=$(SLO)$/$(LOCAL$(TNR)DESC:b)$($(WINVERSIONNAMES)_MAJOR)_description.obj
.ENDIF          # "$(LOCALDESC$(TNR))"==""

.ENDIF          # "$(COMP$(TNR)TYPELIST)"==""
.ENDIF			# "$(SHL$(TNR)TARGET)"!="

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#+++++++++++    version object      ++++++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.IF "$(VERSIONOBJ)"!=""
SHL$(TNR)VERSIONOBJ:=$(VERSIONOBJ:d){$(subst,$(UPD)$(DLLPOSTFIX),_dflt $(SHL$(TNR)TARGET))}$(VERSIONOBJ:f)
USE_VERSIONH:=$(INCCOM)$/$(SHL$(TNR)VERSIONOBJ:b).h
.IF "$(GUI)" == "UNX"
SHL$(TNR)VERSIONOBJDEP:=$(VERSIONOBJ:d){$(subst,$(UPD)$(DLLPOSTFIX),_dflt $(SHL$(TNR)TARGET))}$(VERSIONOBJ:f:s/.o/.obj/)
.ELSE           # "$(GUI)" == "UNX"
SHL$(TNR)VERSIONOBJDEP:=$(VERSIONOBJ:d){$(subst,$(UPD)$(DLLPOSTFIX),_dflt $(SHL$(TNR)TARGET))}$(VERSIONOBJ:f)
.ENDIF          # "$(GUI)" == "UNX"
$(MISC)$/$(SHL$(TNR)VERSIONOBJ:b).c : $(SOLARENV)$/src$/version.c $(INCCOM)$/$(SHL$(TNR)VERSIONOBJ:b).h
#    $(COPY) $(SOLARENV)$/src$/version.c $@
    $(TYPE) $(SOLARENV)$/src$/version.c | $(SED) s/_version.h/$(SHL$(TNR)VERSIONOBJ:b).h/ > $@

.INIT : $(SHL$(TNR)VERSIONOBJDEP)
.ENDIF			# "$(VERSIONOBJ)"!=""

.IF "$(GUI)" != "UNX"
.IF "$(GUI)" == "WNT"
.IF "$(COM)" == "MSC"
.IF "$(SHL$(TNR)IMPLIB)" == ""
SHL$(TNR)IMPLIB=i$(TARGET)_t$(TNR)
.ENDIF			# "$(SHL$(TNR)IMPLIB)" == ""
USE_$(TNR)IMPLIB=-implib:$(LB)$/$(SHL$(TNR)IMPLIB).lib
SHL$(TNR)IMPLIBN=$(LB)$/$(SHL$(TNR)IMPLIB).lib
ALLTAR : $(SHL$(TNR)IMPLIBN)
.ENDIF			# "$(COM)" == "MSC"

.IF "$(USE_DEFFILE)"==""
USE_$(TNR)IMPLIB_DEPS=$(LB)$/$(SHL$(TNR)IMPLIB).lib
.ENDIF			# "$(USE_DEFFILE)"==""
.ENDIF			# "$(GUI)" == "WNT"
USE_SHL$(TNR)DEF=$(SHL$(TNR)DEF)
.ELSE			# "$(GUI)" != "UNX"
USE_SHL$(TNR)DEF=
SHL$(TNR)DEPN+:=$(SHL$(TNR)DEPNU)

.IF "$(SHL$(TNR)VERSIONMAP)"==""
# to activate vmaps remove "#"
USE_SHL$(TNR)VERSIONMAP=$(MISC)$/$(SHL$(TNR)TARGET).vmap
.ENDIF			# "$(SHL$(TNR)VERSIONMAP)"==""

.IF "$(USE_SHL$(TNR)VERSIONMAP)"!=""

.IF "$(SHL$(TNR)FILTERFILE)"!=""
.IF "$(SHL$(TNR)VERSIONMAP)"!=""
#eine von beiden ist zuviel
USE_SHL$(TNR)VERSIONMAP=$(MISC)$/$(SHL$(TNR)TARGET).vmap
$(USE_SHL$(TNR)VERSIONMAP) .PHONY: 
    @echo -----------------------------
    @echo you should only use versionmap OR exportfile
    @echo -----------------------------
#	force_dmake_to_error

.ENDIF			# "$(SHL$(TNR)VERSIONMAP)"!=""

.IF "$(OS)"!="IRIX"
SHL$(TNR)VERSIONMAPPARA=$(LINKVERSIONMAPFLAG) $(USE_SHL$(TNR)VERSIONMAP)
.ENDIF

$(USE_SHL$(TNR)VERSIONMAP): \
                    $(SHL$(TNR)OBJS)\
                    $(SHL$(TNR)LIBS)\
                    $(SHL$(TNR)FILTERFILE)
    @$(RM) $@.dump
.IF "$(SHL$(TNR)OBJS)"!=""
# dump remaining objects on the fly
.IF "$(OS)"=="MACOSX"
    @-nm $(SHL$(TNR)OBJS:s/.obj/.o/) > $@.dump
.ELSE
    @nm $(SHL$(TNR)OBJS:s/.obj/.o/) > $@.dump
.ENDIF
.ENDIF			# "$(SHL$(TNR)OBJS)!"=""
    @$(TYPE) /dev/null $(SHL$(TNR)LIBS:s/.lib/.dump/) >> $@.dump
    $(PERL) $(SOLARENV)$/bin$/mapgen.pl -d $@.dump -s $(SHL$(TNR)INTERFACE) -f $(SHL$(TNR)FILTERFILE) -m $@

.ELSE			# "$(SHL$(TNR)FILTERFILE)"!=""
USE_SHL$(TNR)VERSIONMAP=$(MISC)$/$(SHL$(TNR)TARGET).vmap
$(USE_SHL$(TNR)VERSIONMAP) : 
    @echo -----------------------------
    @echo SHL$(TNR)FILTERFILE not set!
    @echo -----------------------------
    @$(TOUCH) $@
    @echo dummy file to keep the dependencies for later use.
#	force_dmake_to_error
.ENDIF			# "$(SHL$(TNR)FILTERFILE)"!=""
.ELSE			# "$(USE_SHL$(TNR)VERSIONMAP)"!=""

#and now for the plain non-generic way...
.IF "$(SHL$(TNR)VERSIONMAP)"!=""
USE_SHL$(TNR)VERSIONMAP=$(MISC)$/$(SHL$(TNR)VERSIONMAP:b)_$(SHL$(TNR)TARGET)$(SHL$(TNR)VERSIONMAP:e)
.IF "$(OS)"!="IRIX"
SHL$(TNR)VERSIONMAPPARA=$(LINKVERSIONMAPFLAG) $(USE_SHL$(TNR)VERSIONMAP)
.ENDIF

$(USE_SHL$(TNR)VERSIONMAP): $(SHL$(TNR)VERSIONMAP)
    @-$(RM) -f $@ >& $(NULLDEV)

# The following files will only be generated and needed on Mac OS X as temporary files
# in order to generate exported symbols list out of Linux/Solaris map files
.IF "$(OS)"=="MACOSX"
    @+-$(RM) -f $@.symregexp >& $(NULLDEV)
    @+-$(RM) -f $@.expsymlist >& $(NULLDEV)
.ENDIF

# Its questionable if the following condition '.IF "$(COMID)"=="gcc3"' makes sense and what 
# happens if somebody will change it in the future
.IF "$(COMID)"=="gcc3"
    tr -d "\015" < $(SHL$(TNR)VERSIONMAP) | $(AWK) -f $(SOLARENV)$/bin$/addsym.awk > $@
.ELSE           # "$(COMID)"=="gcc3"
    tr -d "\015" < $(SHL$(TNR)VERSIONMAP) > $@
.ENDIF          # "$(COMID)"=="gcc3"
    @chmod a+w $@

# Mac OS X post-processing generate an exported symbols list from the generated map file
# for details on exported symbols list see man ld on Mac OS X
.IF "$(OS)"=="MACOSX"
    +-cat $@ | $(AWK) -f $(SOLARENV)$/bin$/unxmap-to-macosx-explist.awk | grep -v "\*\|?" > $@.exported-symbols
    +-cat $@ | $(AWK) -f $(SOLARENV)$/bin$/unxmap-to-macosx-explist.awk | grep "\*\|?" > $@.symbols-regexp

# Shared libraries will be build out of the *.obj files specified in SHL?OBJS and SHL?LIBS
# Extract RTTI symbols from all the objects that will be used to build a shared library
.IF "$(SHL$(TNR)OBJS)"!=""
    +-echo $(foreach,i,$(SHL$(TNR)OBJS:s/.obj/.o/) $i) | xargs -n1 nm -gx | $(SOLARENV)$/bin$/addsym-macosx.sh $@.symbols-regexp $(MISC)$/symbols-regexp.tmp >> $@.exported-symbols
.ENDIF
.IF "$(SHL$(TNR)LIBS)"!=""	
    +-$(TYPE) $(foreach,j,$(SHL$(TNR)LIBS) $j) | $(SED) s\#$(ROUT)\#$(PRJ)$/$(ROUT)\#g | xargs -n1 nm -gx | $(SOLARENV)$/bin$/addsym-macosx.sh $@.symbols-regexp $(MISC)$/symbols-regexp.tmp >> $@.exported-symbols
.ENDIF
# overwrite the map file generate into the local output tree with the generated 
# exported symbols list
    +cp $@.exported-symbols $@ 
.ENDIF # .IF "$(OS)"=="MACOSX"

.ENDIF			# "$(SHL$(TNR)VERSIONMAP)"!=""
.ENDIF			# "$(USE_SHL$(TNR)VERSIONMAP)"!=""
.ENDIF			# "$(GUI)" != "UNX"

.IF "$(UNIXVERSIONNAMES)"!=""
.IF "$(OS)"!="MACOSX"
.IF "$(GUI)"=="UNX"
.IF "$(OS)"=="IRIX"
SHL$(TNR)SONAME=$(SONAME_SWITCH)$(SHL$(TNR)TARGETN:f)
.ELSE
SHL$(TNR)SONAME=\"$(SONAME_SWITCH)$(SHL$(TNR)TARGETN:f)\"
.ENDIF
.ENDIF			# "$(GUI)"!="UNX"
.ENDIF			# "$(OS)"!="MACOSX"
.ENDIF			# "$(UNIXVERSIONNAMES)"!=""

.IF "$(SHL$(TNR)RES)"!=""
SHL$(TNR)ALLRES+=$(SHL$(TNR)RES)
SHL$(TNR)LINKRES*=$(MISC)$/$(SHL$(TNR)TARGET).res
.ENDIF			# "$(SHL$(TNR)RES)"!=""

.IF "$(SHL$(TNR)DEFAULTRES)$(use_shl_versions)"!=""
SHL$(TNR)DEFAULTRES*=$(MISC)$/$(SHL$(TNR)TARGET)_def.res
SHL$(TNR)ALLRES+=$(SHL$(TNR)DEFAULTRES)
SHL$(TNR)LINKRES*=$(MISC)$/$(SHL$(TNR)TARGET).res
.ENDIF			# "$(SHL$(TNR)DEFAULTRES)$(use_shl_versions)"!=""

.IF "$(NO_SHL$(TNR)DESCRIPTION)"==""
#SHL$(TNR)DESCRIPTIONOBJ*=$(SLO)$/default_description.obj
SHL$(TNR)DESCRIPTIONOBJ*=$(SLO)$/{$(subst,$(UPD)$(DLLPOSTFIX),_dflt $(SHL$(TNR)TARGET))}_description.obj
.ENDIF			# "$(NO_SHL$(TNR)DESCRIPTION)"==""

#.IF "$(SHL$(TNR)TARGETN)"!=""

.IF "$(linkinc)"!=""
.IF "$(GUI)"=="WNT"
$(MISC)$/$(SHL$(TNR)TARGET)_linkinc.ls .PHONY:
    @-$(RM) $@ >& $(NULLDEV)
    $(SED) -f $(COMMON_ENV_TOOLS)\chrel.sed $(foreach,i,$(SHL$(TNR)LIBS) $(i:s/.lib/.lin/)) >> $@
.ENDIF

LINKINCTARGETS+=$(MISC)$/$(SHL$(TNR)TARGETN:b)_linkinc.ls
$(SHL$(TNR)TARGETN) : $(LINKINCTARGETS)

.ELSE
.IF "$(SHL$(TNR)USE_EXPORTS)"=="name"
.IF "$(GUI)"=="WNT"
SHL$(TNR)LINKLIST=$(MISC)$/$(SHL$(TNR)TARGET)_link.lst
$(MISC)$/$(SHL$(TNR)TARGET)_link.lst : $(SHL$(TNR)LIBS)
    @-$(RM) $@ >& $(NULLDEV)
    $(SED) -f $(COMMON_ENV_TOOLS)\chrel.sed $(foreach,i,$(SHL$(TNR)LIBS) $(i:s/.lib/.lin/)) >> $@
.ENDIF
.ENDIF			# "$(SHL$(TNR)USE_EXPORTS)"=="name"

$(MISC)$/%linkinc.ls:
    echo . > $@
.ENDIF          # "$(linkinc)"!=""

$(SHL$(TNR)TARGETN) : \
                    $(SHL$(TNR)OBJS)\
                    $(SHL$(TNR)DESCRIPTIONOBJ)\
                    $(SHL$(TNR)LIBS)\
                    $(USE_$(TNR)IMPLIB_DEPS)\
                    $(USE_SHL$(TNR)DEF)\
                    $(USE_SHL$(TNR)VERSIONMAP)\
                    $(SHL$(TNR)RES)\
                    $(SHL$(TNR)DEPN) \
                    $(SHL$(TNR)LINKLIST)
    @echo ------------------------------
    @echo Making: $(SHL$(TNR)TARGETN)
.IF "$(GUI)" == "WNT"
.IF "$(SHL$(TNR)DEFAULTRES)"!=""
    @-$(RM) $(MISC)$/$(SHL$(TNR)DEFAULTRES:b).rc >& $(NULLDEV)
.IF "$(SHL$(TNR)ICON)" != ""
    @echo 1 ICON $(SHL$(TNR)ICON) >> $(MISC)$/$(SHL$(TNR)DEFAULTRES:b).rc
.ENDIF
.IF "$(use_shl_versions)" != ""
.IF "$(SHL$(TNR)ADD_VERINFO)"!=""
    @echo $(EMQ)#include $(EMQ)"$(SHL$(TNR)ADD_VERINFO)$(EMQ)" >> $(MISC)$/$(SHL$(TNR)DEFAULTRES:b).rc
.ELSE			# "$(SHL$(TNR)ADD_VERINFO)"!=""
    @echo $(EMQ)#define ADDITIONAL_VERINFO1 >> $(MISC)$/$(SHL$(TNR)DEFAULTRES:b).rc
    @echo $(EMQ)#define ADDITIONAL_VERINFO2 >> $(MISC)$/$(SHL$(TNR)DEFAULTRES:b).rc
    @echo $(EMQ)#define ADDITIONAL_VERINFO3 >> $(MISC)$/$(SHL$(TNR)DEFAULTRES:b).rc	
.ENDIF			# "$(SHL$(TNR)ADD_VERINFO)"!=""
    @echo $(EMQ)#define VERVARIANT	$(BUILD) >> $(MISC)$/$(SHL$(TNR)DEFAULTRES:b).rc
    @echo $(EMQ)#define ORG_NAME	$(SHL$(TNR)TARGET)$(DLLPOST) >> $(MISC)$/$(SHL$(TNR)DEFAULTRES:b).rc
    @echo $(EMQ)#define INTERNAL_NAME $(SHL$(TNR)TARGET:b) >> $(MISC)$/$(SHL$(TNR)DEFAULTRES:b).rc
    @echo $(EMQ)#include $(EMQ)"shlinfo.rc$(EMQ)" >> $(MISC)$/$(SHL$(TNR)DEFAULTRES:b).rc
.ENDIF			# "$(use_shl_versions)" != ""
    $(RC) -DWIN32 -I$(SOLARTESDIR) $(INCLUDE) $(RCLINKFLAGS) $(MISC)$/$(SHL$(TNR)DEFAULTRES:b).rc
.ENDIF			# "$(SHL$(TNR)DEFAULTRES)"!=""
.IF "$(SHL$(TNR)ALLRES)"!=""
.IF "$(USE_SHELL)"=="4nt"
    $(COPY) /b $(SHL$(TNR)ALLRES:s/res /res+/) $(SHL$(TNR)LINKRES)
.ELSE			# "$(USE_SHELL)"=="4nt"
    $(TYPE) $(SHL$(TNR)ALLRES) > $(SHL$(TNR)LINKRES)
.ENDIF			# "$(USE_SHELL)"=="4nt"
.ENDIF			# "$(SHL$(TNR)ALLRES)"!=""
.IF "$(linkinc)"==""
.IF "$(SHL$(TNR)USE_EXPORTS)"!="name"
.IF "$(USE_DEFFILE)"!=""
.IF "$(COM)"=="GCC"
    @echo $(SHL$(TNR)LINKER) $(SHL$(TNR)LINKFLAGS) $(LINKFLAGSSHL) -o$@ \
        $(STDOBJ) $(SHL$(TNR)VERSIONOBJ) $(SHL$(TNR)DESCRIPTIONOBJ) | tr -d "\r\n" > $(MISC)$/$(TARGET).$(@:b)_$(TNR).cmd
    @$(TYPE) $(SHL$(TNR)LIBS) | $(SED) s\#$(ROUT)\#$(PRJ)$/$/$(ROUT)\#g | tr -d "\r\n" >> $(MISC)$/$(TARGET).$(@:b)_$(TNR).cmd
    @echo  $(SHL$(TNR)STDLIBS) $(SHL$(TNR)STDSHL) $(STDSHL$(TNR)) $(SHL$(TNR)RES) >> $(MISC)$/$(TARGET).$(@:b)_$(TNR).cmd
    $(MISC)$/$(TARGET).$(@:b)_$(TNR).cmd
.ELSE
    $(SHL$(TNR)LINKER) @$(mktmp \
        $(SHL$(TNR)LINKFLAGS) \
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
        $(SHL$(TNR)STDSHL) $(STDSHL$(TNR)) \
        $(SHL$(TNR)LINKRES) \
    ) $(LINKOUTPUTFILTER)
    @$(LS) $@ >& $(NULLDEV)
    @echo linking $@.manifest ...
    $(IFEXIST) $@.manifest $(THEN) mt.exe -manifest $@.manifest -outputresource:$@$(EMQ);2 $(FI)
    $(IFEXIST) $@.manifest $(THEN) $(RM:s/+//) $@.manifest $(FI)
.ENDIF			# "$(COM)"=="GCC"
.ELSE			# "$(USE_DEFFILE)"!=""
    $(SHL$(TNR)LINKER) @$(mktmp	$(SHL$(TNR)LINKFLAGS)			\
        $(LINKFLAGSSHL) $(SHL$(TNR)BASEX)		\
        $(SHL$(TNR)STACK) -out:$(SHL$(TNR)TARGETN)	\
        -map:$(MISC)$/$(@:B).map				\
        $(LB)$/$(SHL$(TNR)IMPLIB).exp				\
        $(STDOBJ)							\
        $(SHL$(TNR)OBJS) $(SHL$(TNR)VERSIONOBJ) $(SHL$(TNR)DESCRIPTIONOBJ)   \
        $(SHL$(TNR)LIBS)                         \
        $(SHL$(TNR)STDLIBS)                      \
        $(SHL$(TNR)STDSHL) $(STDSHL$(TNR))                           \
        $(SHL$(TNR)LINKRES) \
    ) $(LINKOUTPUTFILTER)
    @$(LS) $@ >& $(NULLDEV)
    @echo linking $@.manifest ...
    $(IFEXIST) $@.manifest $(THEN) mt.exe -manifest $@.manifest -outputresource:$@$(EMQ);2 $(FI)
    $(IFEXIST) $@.manifest $(THEN) $(RM:s/+//) $@.manifest $(FI)
.ENDIF			# "$(USE_DEFFILE)"!=""
.ELSE			# "$(SHL$(TNR)USE_EXPORTS)"!="name"
    $(SHL$(TNR)LINKER) @$(mktmp	$(SHL$(TNR)LINKFLAGS)			\
        $(LINKFLAGSSHL) $(SHL$(TNR)BASEX)		\
        $(SHL$(TNR)STACK) -out:$(SHL$(TNR)TARGETN)	\
        -map:$(MISC)$/$(@:B).map				\
        $(USE_$(TNR)IMPLIB) \
        $(STDOBJ)							\
        $(SHL$(TNR)OBJS) $(SHL$(TNR)VERSIONOBJ) $(SHL$(TNR)DESCRIPTIONOBJ))   \
        @$(MISC)$/$(SHL$(TNR)TARGET)_link.lst \
        @$(mktmp $(SHL$(TNR)STDLIBS)                      \
        $(SHL$(TNR)STDSHL) $(STDSHL$(TNR))                           \
        $(SHL$(TNR)LINKRES) \
    )
    @echo linking $@.manifest ...
    $(IFEXIST) $@.manifest $(THEN) mt.exe -manifest $@.manifest -outputresource:$@$(EMQ);2 $(FI)
    $(IFEXIST) $@.manifest $(THEN) $(RM:s/+//) $@.manifest $(FI)
.ENDIF			# "$(SHL$(TNR)USE_EXPORTS)"!="name"
.ELSE			# "$(linkinc)"==""
        -$(RM) del $(MISC)$/$(SHL$(TNR)TARGET).lnk
        -$(RM) $(MISC)$/$(SHL$(TNR)TARGET).lst
        $(TYPE) $(mktmp \
        $(SHL$(TNR)LINKFLAGS) \
        $(LINKFLAGSSHL) $(SHL$(TNR)BASEX) \
        $(SHL$(TNR)STACK) $(MAPFILE) \
        -out:$@ \
        $(LB)$/$(SHL$(TNR)IMPLIB).exp \
        $(STDOBJ) \
        $(SHL$(TNR)OBJS) \
        $(SHL$(TNR)STDLIBS) \
        $(SHL$(TNR)STDSHL) $(STDSHL$(TNR)) \
        $(SHL$(TNR)LINKRES) \
        ) >> $(MISC)$/$(SHL$(TNR)TARGET).lnk
        $(TYPE) $(MISC)$/$(SHL$(TNR)TARGETN:b)_linkinc.ls  >> $(MISC)$/$(SHL$(TNR)TARGET).lnk
        $(SHL$(TNR)LINKER) @$(MISC)$/$(SHL$(TNR)TARGET).lnk
        @echo linking $@.manifest ...
        $(IFEXIST) $@.manifest $(THEN) mt.exe -manifest $@.manifest -outputresource:$@$(EMQ);2 $(FI)
        $(IFEXIST) $@.manifest $(THEN) $(RM:s/+//) $@.manifest $(FI)
.ENDIF			# "$(linkinc)"==""
.ENDIF			# "$(GUI)" == "WNT"
.IF "$(GUI)"=="UNX"
.IF "$(OS)"=="MACOSX"
    @-$(RM) $(MISC)$/$(@:b).list
    @-$(RM) $(MISC)$/$(TARGET).$(@:b)_$(TNR).cmd
    @echo $(STDSLO) $(SHL$(TNR)OBJS:s/.obj/.o/) \
    $(SHL$(TNR)VERSIONOBJ) $(SHL$(TNR)DESCRIPTIONOBJ:s/.obj/.o/) \
    `cat /dev/null $(SHL$(TNR)LIBS) | sed s\#$(ROUT)\#$(PRJ)$/$(ROUT)\#g` | tr -s " " "\n" > $(MISC)$/$(@:b).list
    @echo $(SHL$(TNR)LINKER) $(SHL$(TNR)LINKFLAGS) $(SHL$(TNR)VERSIONMAPPARA) $(LINKFLAGSSHL) -L$(PRJ)$/$(ROUT)$/lib $(SOLARLIB) -o $@ \
    `macosx-dylib-link-list $(PRJNAME) $(SOLARVERSION)$/$(INPATH)$/lib $(PRJ)$/$(INPATH)$/lib $(SHL$(TNR)STDLIBS)` \
    $(SHL$(TNR)STDLIBS) $(SHL$(TNR)ARCHIVES) $(SHL$(TNR)STDSHL) $(STDSHL$(TNR)) -filelist $(MISC)$/$(@:b).list $(LINKOUTPUT_FILTER) > $(MISC)$/$(TARGET).$(@:b)_$(TNR).cmd
    @cat $(MISC)$/$(TARGET).$(@:b)_$(TNR).cmd
    @+source $(MISC)$/$(TARGET).$(@:b)_$(TNR).cmd
    @echo "Making: $@.jnilib"
    @macosx-create-bundle $@
.IF "$(UPDATER)"=="YES"
.IF "$(SHL$(TNR)NOCHECK)"==""
    $(SOLARENV)$/bin$/checkdll.sh -L$(LB) $(SOLARLIB) $(SHL$(TNR)TARGETN)
.ENDIF				# "$(SHL$(TNR)NOCHECK)"!=""
.ENDIF
.ELSE			# "$(OS)"=="MACOSX"
    @-$(RM) $(MISC)$/$(TARGET).$(@:b)_$(TNR).cmd
    @echo $(SHL$(TNR)LINKER) $(SHL$(TNR)LINKFLAGS) $(SHL$(TNR)SONAME) $(LINKFLAGSSHL) $(SHL$(TNR)VERSIONMAPPARA) -L$(PRJ)$/$(ROUT)$/lib $(SOLARLIB) $(STDSLO) $(SHL$(TNR)OBJS:s/.obj/.o/) \
    $(SHL$(TNR)VERSIONOBJ) $(SHL$(TNR)DESCRIPTIONOBJ:s/.obj/.o/) -o $@ \
    `cat /dev/null $(SHL$(TNR)LIBS) | tr -s " " "\n" | $(SED) s\#$(ROUT)\#$(PRJ)$/$(ROUT)\#g` \
    $(SHL$(TNR)STDLIBS) $(SHL$(TNR)ARCHIVES) $(SHL$(TNR)STDSHL) $(STDSHL$(TNR)) $(LINKOUTPUT_FILTER) > $(MISC)$/$(TARGET).$(@:b)_$(TNR).cmd
    @cat $(MISC)$/$(TARGET).$(@:b)_$(TNR).cmd
    @+source $(MISC)$/$(TARGET).$(@:b)_$(TNR).cmd
.IF "$(UPDATER)"=="YES"
.IF "$(SHL$(TNR)NOCHECK)"==""
    -$(RM) $(SHL$(TNR)TARGETN:d)check_$(SHL$(TNR)TARGETN:f)
    $(RENAME) $(SHL$(TNR)TARGETN) $(SHL$(TNR)TARGETN:d)check_$(SHL$(TNR)TARGETN:f)
    $(SOLARENV)$/bin$/checkdll.sh -L$(LB) $(SOLARLIB:s/2.6//) $(SHL$(TNR)TARGETN:d)check_$(SHL$(TNR)TARGETN:f)
.ENDIF				# "$(SHL$(TNR)NOCHECK)"!=""
.ENDIF			# "$(UPDATER)"=="YES"
.ENDIF			# "$(OS)"=="MACOSX"
.IF "$(UNIXVERSIONNAMES)"!=""
    $(RM) $(LB)$/$(SHL$(TNR)TARGETN:b)
    cd $(LB) && ln -s $(SHL$(TNR)TARGETN:f) $(SHL$(TNR)TARGETN:b)
.ENDIF			# "$(UNIXVERSIONNAMES)"!=""
    @ls -l $@
.ENDIF			# "$(GUI)" == "UNX"
.IF "$(TARGETTHREAD)"!="MT"
    @echo ----------------------------------------------------------
    @echo -
    @echo - THREAD WARNING! - this library was linked single threaded 
    @echo - and must not be used in any office installation!
    @echo -
    @echo ----------------------------------------------------------
.ENDIF			# "$(TARGETTHREAD)"!="MT"

.IF "$(TESTDIR)"!=""
.IF "$(NO_TESTS)"==""

ALLTAR : runtest_$(SHL$(TNR)TARGET)

runtest_$(SHL$(TNR)TARGET) : $(SHL$(TNR)TARGETN)
    testshl $(SHL$(TNR)TARGETN) sce$/$(SHL$(TNR)TARGET).sce -msg -skip
.ENDIF			# "$(NO_TESTS)"==""
.ENDIF			# "$(TESTDIR)"!=""
.ENDIF			# "$(SHL$(TNR)TARGETN)"!=""

# unroll end
#######################################################

#-------------------------------------------------------------------------

#######################################################
# Anweisungen fuer das Linken
# unroll begin

.IF "$(SHL$(TNR)IMPLIBN)" != ""

.IF "$(UPDATER)"=="YES"
USELIB$(TNR)DEPN+=$(SHL$(TNR)LIBS)
.ELSE
.ENDIF

.IF "$(USE_DEFFILE)"!=""
USE_SHL$(TNR)TARGET=$(SHL$(TNR)TARGETN)
.ENDIF

.IF "$(GUI)" != "UNX"
$(SHL$(TNR)IMPLIBN):	\
                    $(SHL$(TNR)DEF) \
                    $(USE_SHL$(TNR)TARGET) \
                    $(USELIB$(TNR)DEPN) \
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
    @echo build of $(SHL$(TNR)TARGETN) creates $@
.ENDIF			# "$(GUI)" == "WNT"
.ELSE
    @echo no ImportLibs on Mac and *ix
    @-$(RM) $@
    @$(TOUCH) $@
.ENDIF
.ENDIF

# unroll end
#######################################################
