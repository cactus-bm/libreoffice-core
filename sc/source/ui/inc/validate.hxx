/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: validate.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 22:04:39 $
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

#ifndef SC_VALIDATE_HXX
#define SC_VALIDATE_HXX

#ifndef _SFXTABDLG_HXX
#include <sfx2/tabdlg.hxx>
#endif
#ifndef _SV_EDIT_HXX
#include <vcl/edit.hxx>
#endif
#ifndef _SV_FIXED_HXX
#include <vcl/fixed.hxx>
#endif
#ifndef _SV_LSTBOX_HXX
#include <vcl/lstbox.hxx>
#endif
#ifndef _SVEDIT_HXX
#include <svtools/svmedit.hxx>
#endif


// ============================================================================

/** The "Validity" tab dialog. */
class ScValidationDlg : public SfxTabDialog
{
public:
    explicit                    ScValidationDlg( Window* pParent, const SfxItemSet* pArgSet );
    virtual                     ~ScValidationDlg();
};


// ============================================================================

/** The tab page "Criteria" from the Validation dialog. */
class ScTPValidationValue : public SfxTabPage
{
public:
    explicit                    ScTPValidationValue( Window* pParent, const SfxItemSet& rArgSet );
    virtual                     ~ScTPValidationValue();

    static SfxTabPage*          Create( Window* pParent, const SfxItemSet& rArgSet );
    static USHORT*              GetRanges();

    virtual BOOL                FillItemSet( SfxItemSet& rArgSet );
    virtual void                Reset( const SfxItemSet& rArgSet );

private:
    void                        Init();

    String                      GetFirstFormula() const;
    String                      GetSecondFormula() const;

    void                        SetFirstFormula( const String& rFmlaStr );
    void                        SetSecondFormula( const String& rFmlaStr );

                                DECL_LINK( SelectHdl, ListBox* );
                                DECL_LINK( CheckHdl, CheckBox* );

    FixedText                   maFtAllow;
    ListBox                     maLbAllow;
    CheckBox                    maCbAllow;      /// Allow blank cells.
    CheckBox                    maCbShow;       /// Show selection list in cell.
    CheckBox                    maCbSort;       /// Sort selection list in cell.
    FixedText                   maFtValue;
    ListBox                     maLbValue;
    FixedText                   maFtMin;
    Edit                        maEdMin;
    MultiLineEdit               maEdList;       /// Entries for explicit list
    FixedText                   maFtMax;
    Edit                        maEdMax;
    FixedText                   maFtHint;       /// Hint text for cell range validity.

    String                      maStrMin;
    String                      maStrMax;
    String                      maStrValue;
    String                      maStrRange;
    String                      maStrList;
    sal_Unicode                 mcFmlaSep;      /// List separator in formulas.
};


//==================================================================

class ScTPValidationHelp : public SfxTabPage
{
private:
    TriStateBox     aTsbHelp;
    FixedLine       aFlContent;
    FixedText       aFtTitle;
    Edit            aEdtTitle;
    FixedText       aFtInputHelp;
    MultiLineEdit   aEdInputHelp;

    const SfxItemSet& mrArgSet;

    void    Init();

    // Handler ------------------------
    // DECL_LINK( SelectHdl, ListBox * );

public:
            ScTPValidationHelp( Window* pParent, const SfxItemSet& rArgSet );
            ~ScTPValidationHelp();

    static  SfxTabPage* Create      ( Window* pParent, const SfxItemSet& rArgSet );
    static  USHORT*     GetRanges   ();
    virtual BOOL        FillItemSet ( SfxItemSet& rArgSet );
    virtual void        Reset       ( const SfxItemSet& rArgSet );
};

//==================================================================

class ScTPValidationError : public SfxTabPage
{
private:
    TriStateBox     aTsbShow;
    FixedLine       aFlContent;
    FixedText       aFtAction;
    ListBox         aLbAction;
    PushButton      aBtnSearch;
    FixedText       aFtTitle;
    Edit            aEdtTitle;
    FixedText       aFtError;
    MultiLineEdit   aEdError;

    const SfxItemSet& mrArgSet;

    void    Init();

    // Handler ------------------------
    DECL_LINK( SelectActionHdl, ListBox * );
    DECL_LINK( ClickSearchHdl, PushButton * );

public:
            ScTPValidationError( Window* pParent, const SfxItemSet& rArgSet );
            ~ScTPValidationError();

    static  SfxTabPage* Create      ( Window* pParent, const SfxItemSet& rArgSet );
    static  USHORT*     GetRanges   ();
    virtual BOOL        FillItemSet ( SfxItemSet& rArgSet );
    virtual void        Reset       ( const SfxItemSet& rArgSet );
};


#endif // SC_VALIDATE_HXX

