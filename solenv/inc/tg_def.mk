#*************************************************************************
#
#   $RCSfile: tg_def.mk,v $
#
#   $Revision: 1.26 $
#
#   last change: $Author: rt $ $Date: 2004-08-23 09:18:15 $
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

# -------
# - DEF -
# -------
.IF "$(MULTI_DEF_FLAG)" == ""
$(DEF1TARGETN) .NULL : DEF1

$(DEF2TARGETN) .NULL : DEF2

$(DEF3TARGETN) .NULL : DEF3

$(DEF4TARGETN) .NULL : DEF4

$(DEF5TARGETN) .NULL : DEF5

$(DEF6TARGETN) .NULL : DEF6

$(DEF7TARGETN) .NULL : DEF7

$(DEF8TARGETN) .NULL : DEF8

$(DEF9TARGETN) .NULL : DEF9
.ENDIF



.IF "$(MULTI_DEF_FLAG)"==""
DEF1 DEF2 DEF3 DEF4 DEF5 DEF6 DEF7 DEF8 DEF9:  \
                $($@:+"DEPN")
#				$(SLB)$/$($(@:s/DEF/DEFLIB/:+"NAME")).lib
.IF "$(GUI)"=="MAC"
    @dmake "$(DEF$(TNR)TARGETN)" MULTI_DEF_FLAG=true TNR:=$(TNR) $(MFLAGS) $(CALLMACROS)
.ELSE
    @dmake $(DEF$(TNR)TARGETN) MULTI_DEF_FLAG=true TNR:=$(TNR) $(MFLAGS) $(CALLMACROS)
.ENDIF
.ELSE			# "$(MULTI_DEF_FLAG)"==""


#######################################################
# Anweisungen fuer das Linken
# unroll begin

.IF "$(DEF$(TNR)TARGETN)"!=""

#to make intuitiv naming work
.IF "$(DEF$(TNR)LIBNAME)"!=""
DEFLIB$(TNR)NAME*=$(DEF$(TNR)LIBNAME)
.ENDIF			# "$(DEF$(TNR)LIBNAME)"!=""

.IF "$(DEFLIB$(TNR)NAME)"!=""
.IF "$(UPDATER)"!=""
DEF$(TNR)DEPN+=$(SLB)$/$(DEFLIB$(TNR)NAME).lib
.ENDIF			# "$(UPDATER)"!=""
.ENDIF

.IF "$(SHL$(TNR)VERSIONMAP)"!=""
.IF "$(DEF$(TNR)EXPORTFILE)"==""
.IF "$(GUI)"=="WNT"
DEF$(TNR)EXPORTFILE=$(MISC)$/$(SHL$(TNR)VERSIONMAP:b)_$(SHL$(TNR)TARGET).dxp
$(DEF$(TNR)EXPORTFILE) : $(SHL$(TNR)VERSIONMAP)
    +$(TYPE) $< | $(AWK) -f $(SOLARENV)$/bin$/getcsym.awk > $@

.ENDIF			# "$(GUI)"=="WNT"
.ENDIF			# "$(DEF$(TNR)EXPORTFILE)"==""
.ENDIF			# "$(SHL$(TNR)VERSIONMAP)"!=""

.IF "$(GUI)"=="WNT"

DEF$(TNR)FILTER=$(SOLARENV)$/inc$/dummy.flt

.IF "$(MWS_BUILD)"!=""
.IF "$(UPDATER)"!=""
.IF "$(DEFLIB$(TNR)NAME)"!=""

DEF$(TNR)UNIQE:=$(mktmp $(GUI))

# %_disk is a 4nt special; don't exppect it to work in any other shell
BUILD_DRIVE$(TNR):=$(shell +echo %_disk)
#BUILD_DRIVE$(TNR):=O

.IF "$(BUILD_DRIVE$(TNR))"=="O"
# in case of RE build, protect against failed lock
EXPORT$(TNR)_PROTECT=$(TMP)$/$(DEF$(TNR)UNIQE:b).bat && 
.ENDIF			# "$(BUILD_DRIVE$(TNR))"=="O"

.ENDIF			# "$(DEFLIB$(TNR)NAME)"!=""
.ENDIF			# "$(UPDATER)"!=""
.ENDIF			# "$(MWS_BUILD)"!=""

.IF "$(APP$(TNR)HEAP)"==""
.IF "$(UPDATER)"=="" || "$(link_always)"==""
$(DEF$(TNR)TARGETN) : \
        $(DEF$(TNR)DEPN) \
        $(DEF$(TNR)EXPORTFILE)
.ELSE			# "$(UPDATER)"=="" || "$(link_always)"==""
$(DEF$(TNR)TARGETN) .PHONY : \
        $(DEF$(TNR)DEPN) \
        $(DEF$(TNR)EXPORTFILE)
.ENDIF			# "$(UPDATER)"=="" || "$(link_always)"==""
.IF "$(MWS_BUILD)"!=""
.IF "$(UPDATER)"!=""
.IF "$(DEFLIB$(TNR)NAME)"!=""
.IF "$(BUILD_DRIVE$(TNR))"=="O"
#
# don't forget to have the right DEFSTAG set!
#
    +$(PERL) $(COMMON_ENV_TOOLS)$/lockcidef.pl -u$(DEF$(TNR)UNIQE:b) update $(DEFSTAG)
.ENDIF			# "$(BUILD_DRIVE$(TNR))"=="O"
.ENDIF				# "$(DEFLIB$(TNR)NAME)"!=""
.ENDIF			# "$(UPDATER)"!=""
.ENDIF			# "$(MWS_BUILD)"!=""
#	+-attrib -r defs$/$(OUTPATH)
    @+-$(RM) $@.tmpfile
    @echo ------------------------------
    @echo Making Module-Definitionfile : $@
    @echo LIBRARY	  $(SHL$(TNR)TARGET) 								 >$@.tmpfile
    @echo DESCRIPTION	'StarView 3.00 $(DEF$(TNR)DES) $(UPD) $(UPDMINOR)' >>$@.tmpfile
    @echo DATA		  READ WRITE SHARED  							>>$@.tmpfile
    @echo HEAPSIZE	  0 											>>$@.tmpfile
    @echo EXPORTS													>>$@.tmpfile
#	getversioninfo fuer alle!!
    @echo GetVersionInfo		>>$@.tmpfile
.IF "$(NO_SHL$(TNR)DESCRIPTION)"==""
    @echo component_getDescriptionFunc	>>$@.tmpfile
.ENDIF			# "$(NO_SHL$(TNR)DESCRIPTION)"==""
.IF "$(DEFLIB$(TNR)NAME)"!=""
.IF "$(SHL$(TNR)USE_EXPORTS)"!="ordinal"
    @-+$(EXPORT$(TNR)_PROTECT) $(RM) $(MISC)$/$(SHL$(TNR)TARGET).exp
    @+$(EXPORT$(TNR)_PROTECT) $(LIBMGR) -EXTRACT:/ /OUT:$(MISC)$/$(SHL$(TNR)TARGET).exp $(SLB)$/$(DEFLIB$(TNR)NAME).lib
.IF "$(DEF$(TNR)CEXP)"!=""
    @+$(EXPORT$(TNR)_PROTECT) $(LDUMP2) -A $(DEF$(TNR)CEXP) -E 20 -F $(MISC)$/$(SHL$(TNR)TARGET).flt $(MISC)$/$(SHL$(TNR)TARGET).exp			   >>$@.tmpfile
.ELSE
    @+$(EXPORT$(TNR)_PROTECT) $(LDUMP2) -E 20 -F $(MISC)$/$(SHL$(TNR)TARGET).flt $(MISC)$/$(SHL$(TNR)TARGET).exp			   >>$@.tmpfile
.ENDIF
    +$(EXPORT$(TNR)_PROTECT) $(RM) $(MISC)$/$(SHL$(TNR)TARGET).exp
.ELSE			# "$(SHL$(TNR)USE_EXPORTS)"!="ordinal"
    @+$(EXPORT$(TNR)_PROTECT) $(DUMPBIN) -DIRECTIVES $(SLB)$/$(DEFLIB$(TNR)NAME).lib | $(GREP) EXPORT: > $(MISC)$/$(SHL$(TNR)TARGET).direct
.IF "$(DEF$(TNR)CEXP)"!=""
    @+$(EXPORT$(TNR)_PROTECT) $(LDUMP2) -D -A $(DEF$(TNR)CEXP) -E 20 -F $(DEF$(TNR)FILTER) $(MISC)$/$(SHL$(TNR)TARGET).direct >>$@.tmpfile
.ELSE
    @+$(EXPORT$(TNR)_PROTECT) $(LDUMP2) -D -E 20 -F $(DEF$(TNR)FILTER) $(MISC)$/$(SHL$(TNR)TARGET).direct >>$@.tmpfile
.ENDIF
.ENDIF			# "$(SHL$(TNR)USE_EXPORTS)"!="ordinal"
# now *\defs\$(OUTPATH)	exists, commit it
.IF "$(MWS_BUILD)"!=""
.IF "$(UPDATER)"!=""
.IF "$(BUILD_DRIVE$(TNR))"=="O"
#
# don't forget to have the right DEFSTAG set!
#
    +$(PERL) $(COMMON_ENV_TOOLS)$/lockcidef.pl -u$(DEF$(TNR)UNIQE:b) commit
    +$(TMP)$/$(DEF$(TNR)UNIQE:b).bat && $(RM) $(TMP)$/$(DEF$(TNR)UNIQE:b).bat
.ENDIF			# "$(BUILD_DRIVE$(TNR))"=="O"
.ENDIF			# "$(UPDATER)"!=""
.ENDIF			# "$(MWS_BUILD)"!=""
.ENDIF				# "$(DEFLIB$(TNR)NAME)"!=""
.IF "$(DEF$(TNR)EXPORT1)"!=""
    @echo $(DEF$(TNR)EXPORT1)										>>$@.tmpfile
.ENDIF
.IF "$(DEF$(TNR)EXPORT2)"!=""
    @echo $(DEF$(TNR)EXPORT2)										>>$@.tmpfile
.ENDIF
.IF "$(DEF$(TNR)EXPORT3)"!=""
    @echo $(DEF$(TNR)EXPORT3)										>>$@.tmpfile
.ENDIF
.IF "$(DEF$(TNR)EXPORT4)"!=""
    @echo $(DEF$(TNR)EXPORT4)										>>$@.tmpfile
.ENDIF
.IF "$(DEF$(TNR)EXPORT5)"!=""
    @echo $(DEF$(TNR)EXPORT5)										>>$@.tmpfile
.ENDIF
.IF "$(DEF$(TNR)EXPORT6)"!=""
    @echo $(DEF$(TNR)EXPORT6)										>>$@.tmpfile
.ENDIF
.IF "$(DEF$(TNR)EXPORT7)"!=""
    @echo $(DEF$(TNR)EXPORT7)										>>$@.tmpfile
.ENDIF
.IF "$(DEF$(TNR)EXPORT8)"!=""
    @echo $(DEF$(TNR)EXPORT8)										>>$@.tmpfile
.ENDIF
.IF "$(DEF$(TNR)EXPORT9)"!=""
    @echo $(DEF$(TNR)EXPORT9)										>>$@.tmpfile
.ENDIF
.IF "$(DEF$(TNR)EXPORT10)"!=""
    @echo $(DEF$(TNR)EXPORT10)										>>$@.tmpfile
.ENDIF
.IF "$(DEF$(TNR)EXPORT11)"!=""
    @echo $(DEF$(TNR)EXPORT11)										>>$@.tmpfile
.ENDIF
.IF "$(DEF$(TNR)EXPORT12)"!=""
    @echo $(DEF$(TNR)EXPORT12)										>>$@.tmpfile
.ENDIF
.IF "$(DEF$(TNR)EXPORT13)"!=""
    @echo $(DEF$(TNR)EXPORT13)										>>$@.tmpfile
.ENDIF
.IF "$(DEF$(TNR)EXPORT14)"!=""
    @echo $(DEF$(TNR)EXPORT14)										>>$@.tmpfile
.ENDIF
.IF "$(DEF$(TNR)EXPORT15)"!=""
    @echo $(DEF$(TNR)EXPORT15)										>>$@.tmpfile
.ENDIF
.IF "$(DEF$(TNR)EXPORT16)"!=""
    @echo $(DEF$(TNR)EXPORT16)										>>$@.tmpfile
.ENDIF
.IF "$(DEF$(TNR)EXPORT17)"!=""
    @echo $(DEF$(TNR)EXPORT17)										>>$@.tmpfile
.ENDIF
.IF "$(DEF$(TNR)EXPORT18)"!=""
    @echo $(DEF$(TNR)EXPORT18)										>>$@.tmpfile
.ENDIF
.IF "$(DEF$(TNR)EXPORT19)"!=""
    @echo $(DEF$(TNR)EXPORT19)										>>$@.tmpfile
.ENDIF
.IF "$(DEF$(TNR)EXPORT20)"!=""
    @echo $(DEF$(TNR)EXPORT20)										>>$@.tmpfile
.ENDIF
.IF "$(DEF$(TNR)EXPORTFILE)"!=""
    +$(TYPE) $(DEF$(TNR)EXPORTFILE) >> $@.tmpfile
.ENDIF
    @+-$(RM) $@
    @+$(RENAME) $@.tmpfile $@
.ENDIF			# "$(APP$(TNR)HEAP)"==""
.ENDIF			# "$(GUI)"=="WNT"

.IF "$(GUI)"=="UNX"
$(DEF$(TNR)TARGETN): \
        $(DEF$(TNR)DEPN) \
        $(DEF$(TNR)EXPORTFILE)
    @+echo ------------------------------
    @+echo Making Module-Definitionfile : $@
    @+echo just a dummy for UNIX
    @+echo > $@
.ENDIF
.ENDIF			# "$(DEF$(TNR)TARGETN)"!=""


# Anweisungen fuer das Linken
# unroll end
#######################################################

.ENDIF			# "$(MULTI_DEF_FLAG)"==""

