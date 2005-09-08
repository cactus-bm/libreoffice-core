/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: scriptedtext.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 10:13:39 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

#ifndef _SVTOOLS_SCRIPTEDTEXT_HXX
#define _SVTOOLS_SCRIPTEDTEXT_HXX

#ifndef INCLUDED_SVTDLLAPI_H
#include "svtools/svtdllapi.h"
#endif

#ifndef _SV_GEN_HXX
#include <tools/gen.hxx>
#endif
#ifndef _COM_SUN_STAR_I18N_XBREAKITERATOR_HPP_
#include <com/sun/star/i18n/XBreakIterator.hpp>
#endif


namespace rtl { class OUString; }
class OutputDevice;
class Font;
class SvtScriptedTextHelper_Impl;


//_____________________________________________________________________________

/**
This class provides drawing text with different script types on any output devices.
*/
class SVT_DLLPUBLIC SvtScriptedTextHelper
{
private:
    SvtScriptedTextHelper_Impl* mpImpl;             /// Implementation of class functionality.

                                /** Assignment operator not implemented to prevent usage. */
    SvtScriptedTextHelper&      operator=( const SvtScriptedTextHelper& );

public:
                                /** Constructor sets an output device and no fonts.
                                    @param  _rOutDevice
                                    A reference to an output device. */
                                SvtScriptedTextHelper( OutputDevice& _rOutDevice );

                                /** Constructor sets an output device and fonts for all script types.
                                    @param  _rOutDevice
                                    A reference to an output device.
                                    @param  _pLatinFont
                                    The font for latin characters.
                                    @param  _pAsianFont
                                    The font for asian characters.
                                    @param  _pCmplxFont
                                    The font for complex text layout. */
                                SvtScriptedTextHelper(
                                    OutputDevice& _rOutDevice,
                                    Font* _pLatinFont,
                                    Font* _pAsianFont,
                                    Font* _pCmplxFont );

                                /** Copy constructor. */
                                SvtScriptedTextHelper(
                                    const SvtScriptedTextHelper& _rCopy );

                                /** Destructor. */
    virtual                     ~SvtScriptedTextHelper();

                                /** Sets new fonts and recalculates the text width.
                                    @param  _pLatinFont
                                    The font for latin characters.
                                    @param  _pAsianFont
                                    The font for asian characters.
                                    @param  _pCmplxFont
                                    The font for complex text layout. */
    void                        SetFonts( Font* _pLatinFont, Font* _pAsianFont, Font* _pCmplxFont );

                                /** Sets the default font of the current output device to all script types. */
    void                        SetDefaultFont();

                                /** Sets a new text and calculates all script breaks and the text width.
                                    @param  _rText
                                    The new text.
                                    @param  _xBreakIter
                                    The break iterator for iterating through the script portions. */
    void                        SetText(
                                    const ::rtl::OUString& _rText,
                                    const ::com::sun::star::uno::Reference< ::com::sun::star::i18n::XBreakIterator >& _xBreakIter );

                                /** Returns the previously set text.
                                    @return  The current text. */
    const ::rtl::OUString&      GetText() const;

                                /** Returns the calculated width the text will take in the current output device.
                                    @return  The calculated text width. */
    sal_Int32                   GetTextWidth() const;

                                /** Returns the maximum height the text will take in the current output device.
                                    @return  The maximum text height. */
    sal_Int32                   GetTextHeight() const;

                                /** Returns a size struct containing the width and height of the text in the current output device.
                                    @return  A size struct with the text dimensions. */
    const Size&                 GetTextSize() const;

                                /** Draws the text in the current output device.
                                    @param _rPos
                                    The position of the top left edge of the text. */
    void                        DrawText( const Point& _rPos );
};

//_____________________________________________________________________________

#endif

