/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: salatsuifontutils.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: ihi $ $Date: 2007-09-13 16:31:17 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_vcl.hxx"

#include <boost/assert.hpp>
#include <vector>
#include <string>
#include <numeric>

#ifndef _SV_SALATSUIFONTUTILS_HXX
#include <salatsuifontutils.hxx>
#endif

#include <salgdi.h>

// we have to get the font attributes from the name table
// since neither head's macStyle nor OS/2's panose are easily available
// during font enumeration. macStyle bits would be not sufficient anyway
// and SFNT fonts on Mac usually do not contain an OS/2 table.
static void UpdateAttributesFromPSName( const String& aPSName, ImplDevFontAttributes& rDFA )
{
    if( (aPSName.SearchAscii("Regular") != STRING_NOTFOUND)
    ||  (aPSName.SearchAscii("Normal") != STRING_NOTFOUND)
    ||  (aPSName.SearchAscii("Plain") != STRING_NOTFOUND) )
    {
       rDFA.meWidthType = WIDTH_NORMAL;
       rDFA.meWeight    = WEIGHT_NORMAL;
       rDFA.meItalic    = ITALIC_NONE;
    }

    // heuristics for font weight
    if (aPSName.SearchAscii("Medium") != STRING_NOTFOUND)
        rDFA.meWeight = WEIGHT_NORMAL;
    else if (aPSName.SearchAscii("ExtraBlack") != STRING_NOTFOUND)
        rDFA.meWeight = WEIGHT_BLACK;
    else if (aPSName.SearchAscii("Black") != STRING_NOTFOUND)
        rDFA.meWeight = WEIGHT_BLACK;
    //else if (aPSName.SearchAscii("Book") != STRING_NOTFOUND)
    //    rDFA.meWeight = WEIGHT_SEMIBOLD;
    else if (aPSName.SearchAscii("SemiBold") != STRING_NOTFOUND)
        rDFA.meWeight = WEIGHT_SEMIBOLD;
    else if (aPSName.SearchAscii("UltraBold") != STRING_NOTFOUND)
        rDFA.meWeight = WEIGHT_ULTRABOLD;
    else if (aPSName.SearchAscii("ExtraBold") != STRING_NOTFOUND)
        rDFA.meWeight = WEIGHT_BLACK;
    else if (aPSName.SearchAscii("Bold") != STRING_NOTFOUND)
        rDFA.meWeight = WEIGHT_BOLD;
    else if (aPSName.SearchAscii("UltraLight") != STRING_NOTFOUND)
        rDFA.meWeight = WEIGHT_ULTRALIGHT;
    else if (aPSName.SearchAscii("Light") != STRING_NOTFOUND)
        rDFA.meWeight = WEIGHT_LIGHT;
    else if (aPSName.SearchAscii("Thin") != STRING_NOTFOUND)
        rDFA.meWeight = WEIGHT_THIN;

    // heuristics for font slant
    if (aPSName.SearchAscii("Italic") != STRING_NOTFOUND)
        rDFA.meItalic = ITALIC_NORMAL;
    if (aPSName.SearchAscii("Cursive") != STRING_NOTFOUND)
        rDFA.meItalic = ITALIC_NORMAL;
    if (aPSName.SearchAscii("Oblique") != STRING_NOTFOUND)
        rDFA.meItalic = ITALIC_OBLIQUE;
    else if (aPSName.SearchAscii("Inclined") != STRING_NOTFOUND)
        rDFA.meItalic = ITALIC_OBLIQUE;

    // heuristics for font width
    if (aPSName.SearchAscii("Condensed") != STRING_NOTFOUND)
        rDFA.meWidthType = WIDTH_CONDENSED;
    else if (aPSName.SearchAscii("Narrow") != STRING_NOTFOUND)
        rDFA.meWidthType = WIDTH_SEMI_CONDENSED;
    else if (aPSName.SearchAscii("Expanded") != STRING_NOTFOUND)
        rDFA.meWidthType = WIDTH_EXPANDED;
    else if (aPSName.SearchAscii("Wide") != STRING_NOTFOUND)
        rDFA.meWidthType = WIDTH_EXPANDED;

    // heuristics for font pitch
    if( (aPSName.SearchAscii("Mono") != STRING_NOTFOUND)
    ||  (aPSName.SearchAscii("Courier") != STRING_NOTFOUND)
    ||  (aPSName.SearchAscii("Monaco") != STRING_NOTFOUND) )
        rDFA.mePitch = PITCH_FIXED;

    // heuristics for font semantic
    if( (aPSName.SearchAscii("Symbol") != STRING_NOTFOUND)
    ||  (aPSName.SearchAscii("DingBats") != STRING_NOTFOUND) )
        rDFA.mbSymbolFlag  = true;
}

// -----------------------------------------------------------------------

static bool GetDevFontAttributes( ATSUFontID nFontID, ImplDevFontAttributes& rDFA )
{
    // all ATSU fonts are device fonts that can be directly rotated
    rDFA.mbOrientation = true;
    rDFA.mbDevice      = true;
    rDFA.mnQuality     = 0;

    // reset the attributes
    rDFA.meFamily     = FAMILY_DONTKNOW;
    rDFA.mePitch      = PITCH_VARIABLE;
    rDFA.meWidthType  = WIDTH_NORMAL;
    rDFA.meWeight     = WEIGHT_NORMAL;
    rDFA.meItalic     = ITALIC_NONE;
    rDFA.mbSymbolFlag = false;

    // TODO: how to find out WITHOUT opening the font???
    rDFA.mbSubsettable  = true;
    rDFA.mbEmbeddable   = false;
    // TODO: these members are needed only for our X11 platform targets
    rDFA.meAntiAlias    = ANTIALIAS_DONTKNOW;
    rDFA.meEmbeddedBitmap = EMBEDDEDBITMAP_DONTKNOW;

    // iterate over all available name strings of the font
    ItemCount nFontNameCount = 0;
    OSStatus rc = ATSUCountFontNames( nFontID, &nFontNameCount );
    if( rc != noErr )
        return false;

    int nBestNameValue = 0;
    for( ItemCount nNameIndex = 0; nNameIndex < nFontNameCount; ++nNameIndex )
    {
        ByteCount nNameLength = 0;

        FontNameCode     eFontNameCode;
        FontPlatformCode eFontNamePlatform;
        FontScriptCode   eFontNameScript;
        FontLanguageCode eFontNameLanguage;
        rc = ATSUGetIndFontName( nFontID, nNameIndex, 0, NULL,
            &nNameLength, &eFontNameCode, &eFontNamePlatform, &eFontNameScript, &eFontNameLanguage );
        if( rc != noErr )
            continue;

        // ignore non-interesting name entries
        if( (eFontNameCode != kFontFamilyName)
        &&  (eFontNameCode != kFontStyleName)
        &&  (eFontNameCode != kFontPostscriptName) )
            continue;

        // heuristic to find the most common font name
        int nNameValue = (eFontNameLanguage==0x409) ? 0 : -1; // prefer English
        const int nPlatformEncoding = ((int)eFontNamePlatform << 8) + (int)eFontNameScript;
        switch( nPlatformEncoding )
        {
            case 0x000: nNameValue += 20; break;    // Unicode 1.0
            case 0x001: nNameValue += 21; break;    // Unicode 1.1
            case 0x002: nNameValue += 22; break;    // iso10646_1993
            case 0x003: nNameValue += 23; break;    // UCS-2
            case 0x004: nNameValue +=  0; break;    // TODO?: UCS-4
            case 0x100: nNameValue +=  2; break;    // TODO?: Mac Roman
            case 0x301: nNameValue += 28; break;    // Win UCS-2
            case 0x30A: nNameValue +=  0; break;    // TODO: Win-UCS-4
            case 0x300: nNameValue +=  5;           // Win Symbol encoded name!
                        rDFA.mbSymbolFlag = true;   // (often seen for symbol fonts)
                        break;
            default:    break;
        }

        // get the encoded name
        typedef std::vector<char> NameBuffer;
        NameBuffer aNameBuffer( nNameLength+1 ); // extra byte helps for debugging
        rc = ATSUGetIndFontName( nFontID, nNameIndex, nNameLength, &aNameBuffer[0],
           &nNameLength, &eFontNameCode, &eFontNamePlatform, &eFontNameScript, &eFontNameLanguage );
        if( rc != noErr )
            continue;

        // convert to unicode name
        UniString aUtf16Name;
        if( nNameValue >= 5 )      // we are just interested in UTF16 encoded names
            aUtf16Name = UniString( (const sal_Unicode*)&aNameBuffer[0], nNameLength/2 );
        else if( nNameValue >= 1 ) // the important PSNAME often has apple-roman encoding though
            aUtf16Name = UniString( &aNameBuffer[0], nNameLength, RTL_TEXTENCODING_APPLE_ROMAN );
        else                       // ignore other encodings
            continue;

        // ignore empty strings
        if( aUtf16Name.Len() <= 0 )
            continue;

        // handle the name depending on its namecode
        switch( eFontNameCode ) {
            case kFontFamilyName:
                // ignore font names starting with '.'
                if( aUtf16Name.GetChar(0) == '.' )
                    nNameValue = 0;
                else if( rDFA.maName.Len() )
                {
                    // even if a family name is not the one we are looking for
                    // it is still useful as a font name alternative
                    if( rDFA.maMapNames.Len() )
                        rDFA.maMapNames += ';';
                    rDFA.maMapNames += (nBestNameValue < nNameValue) ? rDFA.maName : aUtf16Name;
                }
                if( nBestNameValue < nNameValue )
                {
                    // get the best family name
                    nBestNameValue = nNameValue;
                    rDFA.maName = aUtf16Name;
                }
                break;
            case kFontStyleName:
                // get a style name matching to the family name
                if( nBestNameValue <= nNameValue )
                    rDFA.maStyleName = aUtf16Name;
                break;
            case kFontPostscriptName:
                // use the postscript name to get some useful info
                UpdateAttributesFromPSName( aUtf16Name, rDFA );
                break;
            default:
                // TODO: use other name entries too?
                break;
        }
    }

#if 0 // multiple-master fonts are mostly obsolete nowadays
      // if we still want to support them this should probably be done one frame higher
    ItemCount nMaxInstances = 0;
    rc = ATSUCountFontInstances ( nFontID, &nMaxInstances );
    for( ItemCount nInstanceIndex = 0; nInstanceIndex < nMaxInstances; ++nInstanceIndex )
    {
        ItemCount nMaxVariations = 0;
        rc = ATSUGetFontInstance( nFontID, nInstanceIndex, 0, NULL, NULL, &nMaxVariations );
        if( (rc == noErr) && (nMaxVariations > 0) )
        {
            AquaLog("\tnMaxVariations=%d\n",(int)nMaxVariations);
            typedef ::std::vector<ATSUFontVariationAxis> VariationAxisVector;
            typedef ::std::vector<ATSUFontVariationValue> VariationValueVector;
            VariationAxisVector aVariationAxes( nMaxVariations );
            VariationValueVector aVariationValues( nMaxVariations );
            ItemCount nVariationCount = 0;
            rc = ATSUGetFontInstance ( nFontID, nInstanceIndex, nMaxVariations,
                &aVariationAxes[0], &aVariationValues[0], &nVariationCount );
            AquaLog("\tnVariationCount=%d\n",(int)nVariationCount);
            for( ItemCount nVariationIndex = 0; nVariationIndex < nMaxVariations; ++nVariationIndex )
            {
                const char* pTag = (const char*)&aVariationAxes[nVariationIndex];
                AquaLog("\tvariation[%d] \'%c%c%c%c\' is %d\n", (int)nVariationIndex,
                    pTag[3],pTag[2],pTag[1],pTag[0], (int)aVariationValues[nVariationIndex]);
            }
       }
    }
#endif

#if 0 // selecting non-defaulted font features is not enabled yet
    ItemCount nMaxFeatures = 0;
    rc = ATSUCountFontFeatureTypes( nFontID, &nMaxFeatures );
    if( (rc == noErr) && (nMaxFeatures > 0) )
    {
        typedef std::vector<ATSUFontFeatureType> FeatureVector;
        FeatureVector aFeatureVector( nMaxFeatures );
        ItemCount nFeatureCount = 0;
        rc = ATSUGetFontFeatureTypes( nFontID, nMaxFeatures, &aFeatureVector[0], &nFeatureCount );
        AquaLog("nFeatureCount=%d\n",(int)nFeatureCount);
        for( ItemCount nFeatureIndex = 0; nFeatureIndex < nFeatureCount; ++nFeatureIndex )
        {
            ItemCount nMaxSelectors = 0;
            rc = ATSUCountFontFeatureSelectors( nFontID, aFeatureVector[nFeatureIndex], &nMaxSelectors );
            AquaLog("\tFeature[%d] = %d has %d selectors\n",
               (int)nFeatureIndex, (int)aFeatureVector[nFeatureIndex], (int)nMaxSelectors );
            typedef std::vector<ATSUFontFeatureSelector> SelectorVector;
            SelectorVector aSelectorVector( nMaxSelectors );
            typedef std::vector<MacOSBoolean> BooleanVector;
            BooleanVector aEnabledVector( nMaxSelectors );
            BooleanVector aExclusiveVector( nMaxSelectors );
            ItemCount nSelectorCount = 0;
            rc = ATSUGetFontFeatureSelectors ( nFontID, aFeatureVector[nFeatureIndex], nMaxSelectors,
                &aSelectorVector[0], &aEnabledVector[0], &nSelectorCount, &aExclusiveVector[0]);
            for( ItemCount nSelectorIndex = 0; nSelectorIndex < nSelectorCount; ++nSelectorIndex )
            {
                FontNameCode eFontNameCode;
                rc = ATSUGetFontFeatureNameCode( nFontID, aFeatureVector[nFeatureIndex],
                    aSelectorVector[nSelectorIndex], &eFontNameCode );
                AquaLog("\t\tselector[%d] n=%d e=%d, x=%d\n",
                    (int)nSelectorIndex, (int)eFontNameCode,
                    aEnabledVector[nSelectorIndex], aExclusiveVector[nSelectorIndex] );
            }
        }
    }
#endif

    bool bRet = (rDFA.maName.Len() > 0);
    return bRet;
}

// -----------------------------------------------------------------------

SystemFontList::SystemFontList()
{
   ItemCount nATSUICompatibleFontsAvailable = 0;
   if (ATSUFontCount(&nATSUICompatibleFontsAvailable) != noErr)
      return;

   if (nATSUICompatibleFontsAvailable <= 0)
      return;

   typedef std::vector<ATSUFontID> ATSUIFontIDContainer_t;
   ATSUIFontIDContainer_t fontIDList(nATSUICompatibleFontsAvailable);
   ItemCount nFontItemsCount = 0;
   if (ATSUGetFontIDs(&fontIDList[0], fontIDList.capacity(), &nFontItemsCount) != noErr)
       return;

   BOOST_ASSERT(nATSUICompatibleFontsAvailable == nFontItemsCount && "Strange I would expect them to be equal");

   mFontContainer.reserve( nFontItemsCount );
   ATSUIFontIDContainer_t::const_iterator it = fontIDList.begin();
   for(; it != fontIDList.end(); ++it )
   {
        ImplDevFontAttributes aDevFontAttr;
        if( !GetDevFontAttributes( *it, aDevFontAttr ) )
            continue;
        ImplMacFontData* pFontData = new ImplMacFontData(  aDevFontAttr, *it );
        mFontContainer.push_back(pFontData);
    }
}

// -----------------------------------------------------------------------

SystemFontList::~SystemFontList()
{
    ImplFontDataContainer_t::const_iterator it = mFontContainer.begin();
    for(; it != mFontContainer.end(); ++it )
        delete *it;
}

// -----------------------------------------------------------------------

void SystemFontList::AnnounceFonts( ImplDevFontList& rFontList ) const
{
    ImplFontDataContainer_t::const_iterator it = mFontContainer.begin();
    for(; it != mFontContainer.end(); ++it )
        rFontList.Add( (*it)->Clone() );
}

// -----------------------------------------------------------------------

