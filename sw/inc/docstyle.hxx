/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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
#ifndef _DOCSTYLE_HXX
#define _DOCSTYLE_HXX

#include <rtl/ref.hxx>

#ifndef _SVSTDARR_HXX
#define _SVSTDARR_STRINGSDTOR
#include <svl/svstdarr.hxx>
#endif
#include <svl/style.hxx>
#include <svl/itemset.hxx>
#include "swdllapi.h"

class SwDoc;
class SwDocStyleSheetPool;
class SwPageDesc;
class SwCharFmt;
class SwTxtFmtColl;
class SwFrmFmt;
class SwNumRule;

/*--------------------------------------------------------------------
    Local helper class.
 --------------------------------------------------------------------*/
class SwPoolFmtList : public SvStringsDtor
{
public:
    SwPoolFmtList() {}
    void Append( char cChar, const String& rStr );
    void Erase();
};


/*--------------------------------------------------------------------
    Temporary StyleSheet.
 --------------------------------------------------------------------*/
class SW_DLLPUBLIC SwDocStyleSheet : public SfxStyleSheetBase
{
    friend class SwDocStyleSheetPool;
    friend class SwStyleSheetIterator;

    SwCharFmt*          pCharFmt;
    SwTxtFmtColl*       pColl;
    SwFrmFmt*           pFrmFmt;
    const SwPageDesc*   pDesc;
    const SwNumRule*    pNumRule;

    SwDoc&              rDoc;
    SfxItemSet          aCoreSet;

    BOOL                bPhysical;


    // Make empty shell a real StyleSheet (Core).
    SW_DLLPRIVATE void              Create();

    // Fill StyleSheet with data.
    enum FillStyleType {
        FillOnlyName,
        FillAllInfo,
        FillPhysical
    };

    SW_DLLPRIVATE BOOL FillStyleSheet( FillStyleType eFType );

protected:
    virtual ~SwDocStyleSheet();

public:
    SwDocStyleSheet( SwDoc&                 rDoc,
                     const String&          rName,
                     SwDocStyleSheetPool&   rPool,
                     SfxStyleFamily         eFam,
                     USHORT                 nMask);

    SwDocStyleSheet( const SwDocStyleSheet& );

    void                    Reset();

    void                    SetMask(USHORT nMsk)            { nMask = nMsk;     }
    void                    SetFamily(SfxStyleFamily eFam)  { nFamily = eFam;   }

    BOOL                    IsPhysical() const              { return bPhysical; }
    void                    SetPhysical(BOOL bPhys);

    // add optional parameter <bResetIndentAttrsAtParagraphStyle>, default value FALSE,
    // which indicates that the indent attributes at a paragraph style should
    // be reset in case that a list style is applied to the paragraph style and
    // no indent attributes are applied.
    void                    SetItemSet( const SfxItemSet& rSet,
                                        const bool bResetIndentAttrsAtParagraphStyle = false );

    virtual SfxItemSet&     GetItemSet();
    // new method for paragraph styles to merge indent attributes of applied list
    // style into the given item set, if the list style indent attributes are applicable.
    void MergeIndentAttrsOfListStyle( SfxItemSet& rSet );
    virtual const String&   GetParent() const;
    virtual const String&   GetFollow() const;

    virtual ULONG GetHelpId( String& rFile );
    virtual void SetHelpId( const String& r, ULONG nId );

    // Preset the members without physical access.
    // Used by StyleSheetPool.
    void                    PresetName(const String& rName)  { aName   = rName; }
    void                    PresetNameAndFamily(const String& rName);
    void                    PresetParent(const String& rName){ aParent = rName; }
    void                    PresetFollow(const String& rName){ aFollow = rName; }

    virtual BOOL            SetName( const String& rStr);
    virtual BOOL            SetParent( const String& rStr);
    virtual BOOL            SetFollow( const String& rStr);

    virtual BOOL            HasFollowSupport() const;
    virtual BOOL            HasParentSupport() const;
    virtual BOOL            HasClearParentSupport() const;
    virtual String          GetDescription();
    virtual String          GetDescription(SfxMapUnit eUnit);

    SwCharFmt*              GetCharFmt();
    SwTxtFmtColl*           GetCollection();
    SwFrmFmt*               GetFrmFmt();
    const SwPageDesc*       GetPageDesc();
    const SwNumRule*        GetNumRule();
    void                    SetNumRule(const SwNumRule& rRule);

    virtual BOOL            IsUsed() const;
};

/*--------------------------------------------------------------------
   Iterator for Pool.
 --------------------------------------------------------------------*/

class SwStyleSheetIterator : public SfxStyleSheetIterator, public SfxListener
{
    rtl::Reference< SwDocStyleSheet > mxIterSheet;
    rtl::Reference< SwDocStyleSheet > mxStyleSheet;
    SwPoolFmtList       aLst;
    USHORT              nLastPos;
    BOOL                bFirstCalled;

    void                AppendStyleList(const SvStringsDtor& rLst,
                                        BOOL    bUsed,
                                        USHORT  nSection,
                                        char    cType);

public:
    SwStyleSheetIterator( SwDocStyleSheetPool* pBase,
                          SfxStyleFamily eFam, USHORT n=0xFFFF );
    virtual ~SwStyleSheetIterator();

    virtual USHORT Count();
    virtual SfxStyleSheetBase *operator[](USHORT nIdx);
    virtual SfxStyleSheetBase* First();
    virtual SfxStyleSheetBase* Next();
    virtual SfxStyleSheetBase* Find(const UniString& rStr);

    virtual void Notify( SfxBroadcaster&, const SfxHint& );
};


class SwDocStyleSheetPool : public SfxStyleSheetBasePool
{
    rtl::Reference< SwDocStyleSheet > mxStyleSheet;
    SwDoc&              rDoc;
    BOOL                bOrganizer : 1;     // TRUE: for Organizer.


    virtual SfxStyleSheetBase* Create( const String&, SfxStyleFamily, USHORT nMask);
    virtual SfxStyleSheetBase* Create( const SfxStyleSheetBase& );

    using SfxStyleSheetBasePool::Find;

public:
    SwDocStyleSheetPool( SwDoc&, BOOL bOrganizer = FALSE );

    virtual void Replace( SfxStyleSheetBase& rSource,
                          SfxStyleSheetBase& rTarget );
    virtual SfxStyleSheetBase& Make(const String&, SfxStyleFamily, USHORT nMask, USHORT nPos = 0xffff);

    virtual SfxStyleSheetBase* Find( const String&, SfxStyleFamily eFam,
                                    USHORT n=0xFFFF );

    virtual BOOL SetParent( SfxStyleFamily eFam, const String &rStyle,
                            const String &rParent );

    virtual void Remove( SfxStyleSheetBase* pStyle);

    void    SetOrganizerMode( BOOL bMode )  { bOrganizer = bMode; }
    BOOL    IsOrganizerMode() const         { return bOrganizer; }

    virtual SfxStyleSheetIterator* CreateIterator( SfxStyleFamily,
                                                    USHORT nMask );

    SwDoc& GetDoc() const { return rDoc; }

    void dispose();

    virtual void SAL_CALL acquire(  ) throw ();
    virtual void SAL_CALL release(  ) throw ();

protected:
    virtual ~SwDocStyleSheetPool();

    //For not-so-clever compilers.
private:
    SwDocStyleSheetPool( const SwDocStyleSheetPool& );
};


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
