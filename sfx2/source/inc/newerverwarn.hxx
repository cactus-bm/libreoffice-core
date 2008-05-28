/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: newerverwarn.hxx,v $
 * $Revision: 1.4 $
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
#ifndef _SFX2_NEWERVERSIONWARNING_HXX
#define _SFX2_NEWERVERSIONWARNING_HXX

#include <vcl/button.hxx>
#include <vcl/fixed.hxx>
#include <sfx2/basedlgs.hxx>

namespace sfx2
{
    class NewerVersionWarningDialog : public SfxModalDialog
    {
    private:
        FixedImage      m_aImage;
        FixedText       m_aInfoText;
        FixedLine       m_aButtonLine;
        PushButton      m_aUpdateBtn;
        CancelButton    m_aLaterBtn;

        ::rtl::OUString m_sVersion;

        DECL_LINK(      UpdateHdl, PushButton* );
        DECL_LINK(      LaterHdl, CancelButton* );

        void            InitButtonWidth();

    public:
        NewerVersionWarningDialog( Window* pParent, const ::rtl::OUString& rVersion );
        ~NewerVersionWarningDialog();
    };
} // namespace sfx2

#endif // #ifndef _SFX2_NEWERVERSIONWARNING_HXX

