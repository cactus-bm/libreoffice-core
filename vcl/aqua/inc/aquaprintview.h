/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: aquaprintview.h,v $
 * $Revision: 1.3.114.1 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#ifndef _VCL_AQUAPRINTVIEW_H
#define _VCL_AQUAPRINTVIEW_H

#include "premac.h"
#include <Cocoa/Cocoa.h>
#include "postmac.h"

#include "vcl/print.hxx"

class AquaSalInfoPrinter;

struct PrintAccessoryViewState
{
    bool        bNeedRestart;
    sal_Int32   nLastPage;

    PrintAccessoryViewState()
    : bNeedRestart( false ), nLastPage( 0 ) {}
};

@interface AquaPrintView : NSView
{
    vcl::PrinterController*     mpController;
    AquaSalInfoPrinter*         mpInfoPrinter;
}
-(id)initWithController: (vcl::PrinterController*)pController withInfoPrinter: (AquaSalInfoPrinter*)pInfoPrinter;
-(MacOSBOOL)knowsPageRange: (NSRangePointer)range;
-(NSRect)rectForPage: (int)page;
-(NSPoint)locationOfPrintRect: (NSRect)aRect;
-(void)drawRect: (NSRect)rect;
@end

@interface AquaPrintAccessoryView : NSObject
{
}
+(NSObject*)setupPrinterPanel: (NSPrintOperation*)pOp withController: (vcl::PrinterController*)pController withState: (PrintAccessoryViewState*)pState;
@end


#endif
