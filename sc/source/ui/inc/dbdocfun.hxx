/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dbdocfun.hxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: rt $ $Date: 2007-04-25 15:58:43 $
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

#ifndef SC_DBDOCFUN_HXX
#define SC_DBDOCFUN_HXX

#ifndef SC_ADDRESS_HXX
#include "address.hxx"
#endif

#ifndef _SOLAR_H
#include <tools/solar.h>
#endif

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif

class String;

struct ScImportParam;
struct ScQueryParam;
struct ScSortParam;
struct ScSubTotalParam;

class SfxViewFrame;
class SbaSelectionList;
class ScDBData;
class ScDocShell;
class ScAddress;
class ScRange;
class ScDPObject;

namespace com { namespace sun { namespace star {
    namespace beans {
        struct PropertyValue;
    }
    namespace sdbc {
        class XResultSet;
    }
} } }

// ---------------------------------------------------------------------------
// -----------------------------------------------------------------
class SbaSelectionList: public List , public SvRefBase
{
public:
    SbaSelectionList():
        List(CONTAINER_MAXBLOCKSIZE,100,100){}
};

SV_DECL_IMPL_REF(SbaSelectionList)


class ScDBDocFunc
{
friend class ScDBFunc;

private:
    ScDocShell&     rDocShell;

public:
                    ScDBDocFunc( ScDocShell& rDocSh ): rDocShell(rDocSh) {}
                    ~ScDBDocFunc() {}

    void            UpdateImport( const String& rTarget, const String& rDBName,
                        const String& rTableName, const String& rStatement,
                        BOOL bNative, BYTE nType,
                        const ::com::sun::star::uno::Reference<
                        ::com::sun::star::sdbc::XResultSet >& xResultSet,
                        const SbaSelectionList* pSelection );

    BOOL            DoImport( SCTAB nTab, const ScImportParam& rParam,
                        const ::com::sun::star::uno::Reference<
                        ::com::sun::star::sdbc::XResultSet >& xResultSet,
                        const SbaSelectionList* pSelection, BOOL bRecord,
                        BOOL bAddrInsert = FALSE );

    BOOL            DoImportUno( const ScAddress& rPos,
                                const com::sun::star::uno::Sequence<
                                    com::sun::star::beans::PropertyValue>& aArgs );

    static void     ShowInBeamer( const ScImportParam& rParam, SfxViewFrame* pFrame );

    BOOL            Sort( SCTAB nTab, const ScSortParam& rSortParam,
                            BOOL bRecord, BOOL bPaint, BOOL bApi );

    SC_DLLPUBLIC BOOL           Query( SCTAB nTab, const ScQueryParam& rQueryParam,
                            const ScRange* pAdvSource, BOOL bRecord, BOOL bApi );

    BOOL            DoSubTotals( SCTAB nTab, const ScSubTotalParam& rParam,
                                    const ScSortParam* pForceNewSort,
                                    BOOL bRecord, BOOL bApi );

    BOOL            AddDBRange( const String& rName, const ScRange& rRange, BOOL bApi );
    BOOL            DeleteDBRange( const String& rName, BOOL bApi );
    BOOL            RenameDBRange( const String& rOld, const String& rNew, BOOL bApi );
    BOOL            ModifyDBData( const ScDBData& rNewData, BOOL bApi );    // Name unveraendert

    BOOL            RepeatDB( const String& rDBName, BOOL bRecord, BOOL bApi );

    BOOL            DataPilotUpdate( ScDPObject* pOldObj, const ScDPObject* pNewObj,
                                        BOOL bRecord, BOOL bApi, BOOL bAllowMove = FALSE );
};



#endif
