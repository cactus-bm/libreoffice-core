/*************************************************************************
 *
 *  $RCSfile: xerecord.hxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: rt $ $Date: 2005-03-29 13:45:10 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef SC_XERECORD_HXX
#define SC_XERECORD_HXX

#ifndef SC_XLCONST_HXX
#include "xlconst.hxx"
#endif
#ifndef SC_XESTREAM_HXX
#include "xestream.hxx"
#endif

// Base classes to export Excel records =======================================

/** Base class for all Excel records.

    Derive from this class to implement any functionality performed during
    saving the records - except really writing a record (i.e. write a list of
    records contained in the class). Derive from XclExpRecord (instead from
    this class) to write common records.
 */
class XclExpRecordBase
{
public:
    virtual             ~XclExpRecordBase();

    /** Overwrite this method to do any operation while saving the record. */
    virtual void        Save( XclExpStream& rStrm );

    /** Calls Save(XclExpStream&) nCount times. */
    void                SaveRepeated( XclExpStream& rStrm, sal_uInt32 nCount );
};

// ----------------------------------------------------------------------------

/** Base class for single records with any content.

    This class handles writing the record header. Derived classes only have to
    write the record body. Calculating the record size before saving optimizes
    the write process (the stream does not have to seek back and update the
    written record size). But it is not required to calculate a valid size
    (maybe it would be too complex or just impossible until the record is
    really written).
 */
class XclExpRecord : public XclExpRecordBase
{
public:
    /** @param nRecId  The record ID of this record. May be set later with SetRecId().
        @param nRecSize  The predicted record size. May be set later with SetRecSize(). */
    explicit            XclExpRecord(
                            sal_uInt16 nRecId = EXC_ID_UNKNOWN,
                            sal_uInt32 nRecSize = 0 );

    virtual             ~XclExpRecord();

    /** Returns the current record ID. */
    inline sal_uInt16   GetRecId() const { return mnRecId; }
    /** Returns the current record size prediction. */
    inline sal_uInt32   GetRecSize() const { return mnRecSize; }

    /** Sets a new record ID. */
    inline void         SetRecId( sal_uInt16 nRecId ) { mnRecId = nRecId; }
    /** Sets a new record size prediction. */
    inline void         SetRecSize( sal_uInt32 nRecSize ) { mnRecSize = nRecSize; }
    /** Adds a size value to the record size prediction. */
    inline void         AddRecSize( sal_uInt32 nRecSize ) { mnRecSize += nRecSize; }
    /** Sets record ID and size with one call. */
    void                SetRecHeader( sal_uInt16 nRecId, sal_uInt32 nRecSize );

    /** Writes the record header and calls WriteBody(). */
    virtual void        Save( XclExpStream& rStrm );

private:
    /** Writes the body of the record (without record header).
        @descr  Usually this method will be overwritten by derived classes. */
    virtual void        WriteBody( XclExpStream& rStrm );

private:
    sal_uInt32          mnRecSize;      /// The predicted record size.
    sal_uInt16          mnRecId;        /// The record ID.
};

// ----------------------------------------------------------------------------

/** A record without body. Only the record ID and the size 0 will be written. */
class XclExpEmptyRecord : public XclExpRecord
{
public:
    /** @param nRecId  The record ID of this record. */
    inline explicit     XclExpEmptyRecord( sal_uInt16 nRecId );
};

inline XclExpEmptyRecord::XclExpEmptyRecord( sal_uInt16 nRecId ) :
    XclExpRecord( nRecId, 0 )
{
}

// ============================================================================

/** A record with a single value of type Type.
    @descr  Requires operator<<( XclExpStream&, const Type& ). */
template< typename Type >
class XclExpValueRecord : public XclExpRecord
{
public:
    /** @param nRecId  The record ID of this record.
        @param rValue  The value for the record body.
        @param nSize  Record size. Uses sizeof( Type ), if this parameter is omitted. */
    inline explicit     XclExpValueRecord(
                            sal_uInt16 nRecId,
                            const Type& rValue,
                            sal_uInt32 nSize = sizeof( Type ) );

    /** Returns the value of the record. */
    inline const Type&  GetValue() const { return maValue; }
    /** Sets a new record value. */
    inline void         SetValue( const Type& rValue ) { maValue = rValue; }

private:
    /** Writes the body of the record. */
    virtual void        WriteBody( XclExpStream& rStrm );

private:
    Type                maValue;        /// The record data.
};

template< typename Type >
inline XclExpValueRecord< Type >::XclExpValueRecord( sal_uInt16 nRecId, const Type& rValue, sal_uInt32 nSize ) :
    XclExpRecord( nRecId, nSize ),
    maValue( rValue )
{
}

template< typename Type >
void XclExpValueRecord< Type >::WriteBody( XclExpStream& rStrm )
{
    rStrm << maValue;
}

// ----------------------------------------------------------------------------

/** A record containing an unsigned 16-bit value. */
typedef XclExpValueRecord< sal_uInt16 >     XclExpUInt16Record;

/** A record containing an unsigned 32-bit value. */
typedef XclExpValueRecord< sal_uInt32 >     XclExpUInt32Record;

/** A record containing a double value. */
typedef XclExpValueRecord< double >         XclExpDoubleRecord;

// ----------------------------------------------------------------------------

/** Record which contains a Boolean value.
    @descr  The value is stored as 16-bit value: 0x0000 = FALSE, 0x0001 = TRUE. */
class XclExpBoolRecord : public XclExpRecord
{
public:
    /** @param nRecId  The record ID of this record.
        @param nValue  The value for the record body. */
    inline explicit     XclExpBoolRecord( sal_uInt16 nRecId, bool bValue ) :
                            XclExpRecord( nRecId, 2 ), mbValue( bValue ) {}

    /** Returns the Boolean value of the record. */
    inline bool         GetBool() const { return mbValue; }
    /** Sets a new Boolean record value. */
    inline void         SetBool( bool bValue ) { mbValue = bValue; }

private:
    /** Writes the body of the record. */
    virtual void        WriteBody( XclExpStream& rStrm );

private:
    bool                mbValue;        /// The record data.
};

// ----------------------------------------------------------------------------

/** Record which exports a memory data array. */
class XclExpDummyRecord : public XclExpRecord
{
public:
    /** @param nRecId  The record ID of this record.
        @param pRecData  Pointer to the data array representing the record body.
        @param nRecSize  Size of the data array. */
    explicit            XclExpDummyRecord(
                            sal_uInt16 nRecId, const void* pRecData, sal_uInt32 nRecSize );

    /** Sets a data array. */
    void                SetData( const void* pRecData, sal_uInt32 nRecSize );

private:
    /** Writes the body of the record. */
    virtual void        WriteBody( XclExpStream& rStrm );

private:
    const void*         mpData;         /// The record data.
};

// List of records ============================================================

/** A list of Excel record objects.

    Provides saving the compete list. This class is derived from
    XclExpRecordBase, so it can be used as record in another record list.
    Requires RecType::Save( XclExpStream& ).
 */
template< typename RecType = XclExpRecordBase >
class XclExpRecordList : public XclExpRecordBase
{
public:
    typedef ScfRef< RecType > RecordRefType;

    /** Returns pointer to an existing record or 0 on error. */
    inline bool         Empty() const { return maRecs.empty(); }
    /** Returns pointer to an existing record or 0 on error. */
    inline size_t       Size() const { return maRecs.size(); }

    /** Returns true, if the passed index points to an exiting record. */
    inline bool         HasRecord( size_t nPos ) const
                            { return (0 <= nPos) && (nPos < maRecs.size()); }
    /** Returns reference to an existing record or empty reference on error. */
    inline RecordRefType GetRecord( size_t nPos ) const
                            { return (nPos < maRecs.size()) ? maRecs[ nPos ] : RecordRefType(); }
    /** Returns reference to the first existing record or empty reference, if list is empty. */
    inline RecordRefType GetFirstRecord() const
                            { return maRecs.empty() ? RecordRefType() : maRecs.front(); }
    /** Returns reference to the last existing record or empty reference, if list is empty. */
    inline RecordRefType GetLastRecord() const
                            { return maRecs.empty() ? RecordRefType() : maRecs.back(); }

    /** Inserts a record at the specified position into the list. */
    inline void         InsertRecord( RecordRefType xRec, size_t nPos )
                            { if( xRec.get() ) maRecs.insert( maRecs.begin() + ::std::min( nPos, maRecs.size() ), xRec ); }
    /** Appends a record to the list. */
    inline void         AppendRecord( RecordRefType xRec )
                            { if( xRec.get() ) maRecs.push_back( xRec ); }
    /** Replaces the record at the specified position from the list with the passed record. */
    inline void         ReplaceRecord( RecordRefType xRec, size_t nPos )
                            { RemoveRecord( nPos ); InsertRecord( xRec, nPos ); }

    /** Inserts a newly created record at the specified position into the list. */
    inline void         InsertNewRecord( RecType* pRec, size_t nPos )
                            { if( pRec ) InsertRecord( RecordRefType( pRec ), nPos ); }
    /** Appends a newly created record to the list. */
    inline void         AppendNewRecord( RecType* pRec )
                            { if( pRec ) AppendRecord( RecordRefType( pRec ) ); }
    /** Replaces the record at the specified position from the list with the passed newly created record. */
    inline void         ReplaceNewRecord( RecType* pRec, size_t nPos )
                            { RemoveRecord( nPos ); InsertNewRecord( pRec, nPos ); }

    /** Removes the record at the specified position from the list. */
    inline void         RemoveRecord( size_t nPos )
                            { if( nPos < maRecs.size() ) maRecs.erase( maRecs.begin() + nPos ); }
    /** Removes all records from the list. */
    inline void         RemoveAllRecords() { maRecs.clear(); }

    /** Writes the complete record list. */
    virtual void        Save( XclExpStream& rStrm );

private:
    typedef ::std::vector< RecordRefType > RecordVec;
    RecordVec           maRecs;
};

template< typename RecType >
void XclExpRecordList< RecType >::Save( XclExpStream& rStrm )
{
    for( typename RecordVec::iterator aIt = maRecs.begin(), aEnd = maRecs.end(); aIt != aEnd; ++aIt )
        (*aIt)->Save( rStrm );
}

// ============================================================================

#endif

