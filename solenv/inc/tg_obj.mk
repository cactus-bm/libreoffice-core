#*************************************************************************
#
#   $RCSfile: tg_obj.mk,v $
#
#   $Revision: 1.10 $
#
#   last change: $Author: hr $ $Date: 2003-04-28 16:45:13 $
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

MKFILENAME:=TG_OBJ.MK

.IF "$(OBJTARGET)"!=""
$(OBJTARGET): $(OBJFILES) $(IDLOBJFILES)
    @echo ------------------------------
    @echo Making: $@
.IF "$(GUI)"=="WNT"
.IF "$(COM)"=="GCC"
    +$(ECHONL) $(foreach,i,$(OBJFILES:f) $(ROBJ)$/$(i)) >> $@
    @+$(TYPE) $@ | tr -d �\n\r� > $@
    @+$(ECHONL) >> $@
.ELSE			# "$(COM)"=="GCC"
.IF "$(LIBTARGET)"!="NO"
    @-$(TYPE) $(mktmp $(&:+"\n")) > $(@:s/.lib/.lin/)
.ENDIF          # "$(LIBTARGET)"!="NO"
    $(LIBMGR) $(LIBFLAGS) /OUT:$@ @$(mktmp $(&:+"\n"))
.ENDIF			# "$(COM)"=="GCC"
.ENDIF			# "$(GUI)"=="WNT"
.IF "$(GUI)"=="UNX"
    +echo $(foreach,i,$(OBJFILES:f) $(ROBJ)$/$(i:s/.obj/.o/)) | xargs -n1 > $@
.IF "$(OS)"=="MACOSX"
    @-+nm `cat $(OBJTARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(@:d)$(@:b).dump
.ELSE
    @+nm `cat $(OBJTARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(@:d)$(@:b).dump
.ENDIF
.ENDIF			# "$(GUI)"=="UNX"
.ENDIF			# "$(OBJTARGET)"!=""


.IF "$(SVXLIGHT)"!=""
.IF "$(SVXLIGHTOBJTARGET)"!=""
$(SVXLIGHTOBJTARGET): $(REAL_SVXLIGHTOBJFILES)
    @echo ------------------------------
    @echo Making: $@
.IF "$(GUI)"=="WNT"
.IF "$(COM)"=="GCC"
#     $(LIBMGR) $(LIBFLAGS) $@ $(OBJFILES)
    +$(ECHONL) $(foreach,i,$(REAL_SVXLIGHTOBJFILES:f) $(ROBJ)$/$(i)) >> $@
.ELSE
.IF "$(LIBTARGET)"!="NO"
    @-$(TYPE) $(mktmp $(&:+"\n")) > $(@:s/.lib/.lin/)
.ENDIF          # "$(LIBTARGET)"!="NO"
    $(LIBMGR) $(LIBFLAGS) /OUT:$@ @$(mktmp $(&:+"\n"))
.ENDIF
.ENDIF			# "$(GUI)"=="WNT"
.IF "$(GUI)"=="UNX"
    +echo $(foreach,i,$(REAL_SVXLIGHTOBJFILES:f) $(ROBJ)$/$(i:s/.obj/.o/)) | xargs -n1 > $@
.ENDIF			# "$(GUI)"=="UNX"
.ENDIF			# "$(SVXLIGHTOBJTARGET)"!=""
.ENDIF			# "$(SVXLIGHT)"!=""

.IF "$(SECOND_BUILD)"!=""
.IF "$($(SECOND_BUILD)OBJTARGET)"!=""
$($(SECOND_BUILD)OBJTARGET): $(REAL_$(SECOND_BUILD)_OBJFILES)
    @echo ------------------------------
    @echo Making: $@
.IF "$(GUI)"=="WNT"
.IF "$(COM)"=="GCC"
#     $(LIBMGR) $(LIBFLAGS) $@ $(OBJFILES)
    +$(ECHONL) $(foreach,i,$(REAL_$(SECOND_BUILD)_OBJFILES:f) $(ROBJ)$/$(i)) > $@
.ELSE
.IF "$(LIBTARGET)"!="NO"
    @-$(TYPE) $(mktmp $(&:+"\n")) > $(@:s/.lib/.lin/)
.ENDIF          # "$(LIBTARGET)"!="NO"
    $(LIBMGR) $(LIBFLAGS) /OUT:$@ @$(mktmp $(&:+"\n"))
.ENDIF
.ENDIF			# "$(GUI)"=="WNT"
.IF "$(GUI)"=="UNX"
    +echo $(foreach,i,$(REAL_$(SECOND_BUILD)_OBJFILES:f) $(ROBJ)$/$(i:s/.obj/.o/)) | xargs -n1 >> $@
.ENDIF			# "$(GUI)"=="UNX"
.ENDIF			# "$($(SECOND_BUILD)OBJTARGET)"!=""
.ENDIF			# "$(SECOND_BUILD)"!=""

