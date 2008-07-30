/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: dialogbuttonhbox.hxx,v $
 *
 * $Revision: 1.3 $
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

#ifndef CORE_DIALOGBUTTONHBOX_HXX
#define CORE_DIALOGBUTTONHBOX_HXX

#include "box.hxx"
#include "flow.hxx"

namespace layoutimpl
{

class DialogButtonHBox : public HBox
{
public:
    DialogButtonHBox();

    void setOrdering( rtl::OUString const& ordering );
    void SAL_CALL addChild( css::uno::Reference< css::awt::XLayoutConstrains > const& xChild ) throw ( css::uno::RuntimeException, css::awt::MaxChildrenException );
    void SAL_CALL removeChild( css::uno::Reference< css::awt::XLayoutConstrains > const& xChild ) throw ( css::uno::RuntimeException );

private:
    enum Ordering { PLATFORM, GNOME, KDE, MACOS, WINDOWS };

    void orderChildren();
    void gnomeOrdering();
    void kdeOrdering();
    void macosOrdering();
    void windowsOrdering();

    static Ordering const DEFAULT_ORDERING;
    Ordering mnOrdering;
    Flow mFlow;

    ChildData *mpAction; /* [..]?, [Retry?] */
    ChildData *mpAffirmative; /* OK, Yes, Save */
    ChildData *mpAlternate; /* NO, [Ignore?], Don't save, Quit without saving */
    ChildData *mpApply; /* Deprecated? */
    ChildData *mpCancel; /* Cancel, Close */
    ChildData *mpFlow;
    ChildData *mpHelp;
    ChildData *mpReset;

    std::list< Box_Base::ChildData *> maOther;
};

} // namespace layoutimpl

#endif /* CORE_DIALOGBUTTONHBOX_HXX */
