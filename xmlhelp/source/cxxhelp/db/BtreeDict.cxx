/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: BtreeDict.cxx,v $
 * $Revision: 1.16 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_xmlhelp.hxx"
#include <stdlib.h>
#ifndef INCLUDED_STL_VECTOR
#include <vector>
#define INCLUDED_STL_VECTOR
#endif
#include <db/BtreeDict.hxx>
#include <db/Block.hxx>
#include <db/BlockFactory.hxx>
#include <db/BlockManager.hxx>
#include <db/DBEnv.hxx>
#include <util/RandomAccessStream.hxx>
#include <util/DBEnvImpl.hxx>
#ifndef _XMLSEARCH_UTIL_RANDOMACCESSSTREAM_HXX_
#include <util/RandomAccessStream.hxx>
#endif



const sal_Int32 xmlsearch::db::BtreeDict::ENTHEADERLEN = 6;
const sal_Int32 xmlsearch::db::BtreeDict::BLOCKSIZE = 2048;
const sal_Int32 xmlsearch::db::BtreeDict::HEADERLEN = 8;
const sal_Int32 xmlsearch::db::BtreeDict::DATALEN =
xmlsearch::db::BtreeDict::BLOCKSIZE - xmlsearch::db::BtreeDict::HEADERLEN;

const sal_Int32 xmlsearch::db::BtreeDict::nBlocksLimit = 64;
const sal_Int32 xmlsearch::db::BtreeDict::MaxKeyLength = 255;
const sal_Int32 xmlsearch::db::BtreeDict::lastPtrIndex = 508;   //!!! Careful with that number, Eugene


namespace xmlsearch {

    namespace db {

        class DictBlock
            : public Block
        {
            friend class BtreeDict;


        public:

            DictBlock( const DBEnv* dbenv )
                : Block( dbenv )
            {
            }

            sal_Int32 free() const
            {
                return getFree() + firstEntry();
            }

            sal_Int32 numberOfEntries() const
            {
                return getInteger( 0 );
            }

            sal_Int32 nthPointer( sal_Int32 n ) const
            {
                return getInteger( 4*(n+1) );
            }

            sal_Int32 getChildIdx( sal_Int32 index ) const
            {
                return nthPointer( BtreeDict::lastPtrIndex - index );
            }

            sal_Int32 entryKeyLength( sal_Int32 i ) const
            {

                return getData()[i] & 0xFF;
            }

            sal_Int32 entryID( sal_Int32  i ) const
            {
                return getInteger(i + 2);
            }

            sal_Int32 entryCompression( sal_Int32 i ) const
            {
                return getData()[i+1] & 0xFF;
            }

            sal_Int32 entryLength( sal_Int32 entry ) const
            {
                return BtreeDict::ENTHEADERLEN + entryKeyLength( entry );
            }

            sal_Int32 entryKey( sal_Int32 entry ) const
            {
                return entry + BtreeDict::ENTHEADERLEN;
            }

            sal_Int32 firstEntry() const
            {
                return 4;
            }

            sal_Int32 nextEntry( sal_Int32 entry ) const
            {
                return entry + entryLength( entry );
            }

            void restoreKeyInBuffer( sal_Int32 entry, sal_Int8* buffer ) const
            {
                sal_Int32 howMany = entryKeyLength( entry );
                sal_Int32 where = entryCompression( entry );
                sal_Int32 from = entryKey( entry );
                while( howMany-- > 0 )
                    buffer[ where++ ] = getData()[ from++ ];
            }

            rtl::OUString restoreKey( sal_Int32 entry, sal_Int8* buffer ) const
            {
                sal_Int32 howMany = entryKeyLength( entry );
                sal_Int32 where = entryCompression( entry );
                sal_Int32 from = entryKey( entry );
                while( howMany-- > 0 )
                    buffer[ where++ ] = getData()[ from++ ];

                return rtl::OUString( (sal_Char*)(buffer),where,RTL_TEXTENCODING_UTF8 );
            }

            rtl::OUString findID( sal_Int32 id ) const throw( xmlsearch::excep::XmlSearchException  )
            {
                sal_Int8 buffer[ BtreeDict::MaxKeyLength ];
                sal_Int32 freeSpace = free();
                for( sal_Int32 ent = firstEntry(); ent < freeSpace; ent = nextEntry( ent ) )
                    if( entryID( ent ) == id )  // found
                        return restoreKey( ent,buffer );
                    else
                        restoreKeyInBuffer( ent,buffer );

                throw xmlsearch::excep::XmlSearchException(
                    rtl::OUString::createFromAscii( "DictBlock::findID -> ID not found in block" ) );
            }


            void withPrefix( const BtreeDict* owner,
                             const rtl::OUString& prefix,
                             sal_Int32 prefLen,
                             std::vector<sal_Int32>& result) const
                throw( xmlsearch::excep::XmlSearchException );


//              void withPrefix( const BtreeDict* owner,
//                               const rtl::OUString& prefix,
//                               sal_Int32 prefLen,
//                               std::vector<sal_Int32>& result) const
//                  throw( xmlsearch::excep::XmlSearchException )
//              {
//                  sal_Int8 buffer[ BtreeDict::MaxKeyLength ];
//                  const int freeSpace = free();
//                  int entryPtr = firstEntry();
//                  if( isLeaf() )
//                      while (entryPtr < freeSpace)
//                      {
//                          if( restoreKey( entryPtr,buffer).compareTo( prefix,prefix.getLength() ) == 0 )
//                              result.push_back( entryID( entryPtr ) );
//                          entryPtr = nextEntry(entryPtr);
//                      }
//                  else
//                  {
//                      owner->lock( getNum() );
//                      sal_Int32 entryIndex  = 0;
//                      while( entryPtr < freeSpace )
//                      {
//                          rtl::OUString key = restoreKey( entryPtr,buffer );

//                          if( key.getLength() > prefLen )
//                              key = key.copy( 0,prefLen );
//                          sal_Int32 cmp = key.compareTo(prefix);

//                          if( cmp < 0 )
//                          {
//                              entryPtr = nextEntry( entryPtr );
//                              ++entryIndex;
//                          }
//                          else if( cmp == 0 )
//                          {
//                              result.push_back( entryID( entryPtr ) );
//                              owner->accessBlock( getChildIdx( entryIndex ) )->withPrefix( owner,prefix,prefLen,result );
//                              entryPtr = nextEntry( entryPtr );
//                              ++entryIndex;
//                          }
//                          else
//                          {
//                              owner->unlock( getNum() );
//                              owner->accessBlock( getChildIdx( entryIndex ) )->withPrefix( owner,prefix,prefLen,result );
//                              return;
//                          }
//                      }

//                      owner->unlock( getNum() );
//                      owner->accessBlock( getChildIdx( numberOfEntries() ) )->withPrefix( owner,prefix,prefLen,result );
//                  }
//              }


            void setBlockNumbers( sal_Int32* blocks )
            {
                for( sal_Int32 e = firstEntry(); e < getFree() ; e = nextEntry(e) )
                    blocks[ entryID(e) ] = getNum();
            }


        protected:

            void doMap( BtreeDict* owner,EntryProcessor* processor ) const
                throw( xmlsearch::excep::XmlSearchException )
            {
                sal_Int8 buffer[ BtreeDict::MaxKeyLength ];
                sal_Int32 freeSpace = free();
                sal_Int32 entryPtr = firstEntry();

                if( isLeaf() )
                    while( entryPtr < freeSpace )
                    {
                        processor->processEntry( restoreKey( entryPtr,buffer ),
                                                 entryID( entryPtr ) );
                        entryPtr = nextEntry( entryPtr );
                    }
                else
                {
                    owner->lock( getNum() );
                    sal_Int8 entryIdx  = 0;
                    while( entryPtr < freeSpace )
                    {
                        owner->accessBlock( getChildIdx( entryIdx ) )->doMap( owner,processor );
                        processor->processEntry( restoreKey( entryPtr,buffer ),
                                                 entryID( entryPtr ) );
                        entryPtr = nextEntry( entryPtr );
                        ++entryIdx;
                    }
                    owner->accessBlock( getChildIdx( entryIdx ) )->doMap( owner,processor );
                    owner->unlock( getNum() );
                }
            }


        };  // end class DictBlock


    }   // end namespace db

}       // end namespace xmlsearch




using namespace xmlsearch;
using namespace xmlsearch::excep;
using namespace xmlsearch::db;
using namespace xmlsearch::util;




void DictBlock::withPrefix( const BtreeDict* owner,
                            const rtl::OUString& prefix,
                            sal_Int32 prefLen,
                            std::vector<sal_Int32>& result) const
    throw( xmlsearch::excep::XmlSearchException )
{
    sal_Int8 buffer[ BtreeDict::MaxKeyLength ];
    const int freeSpace = free();
    int entryPtr = firstEntry();
    if( isLeaf() )
        while( entryPtr < freeSpace )
        {
            rtl::OUString key = restoreKey( entryPtr,buffer);
            if( key.indexOf( prefix ) != -1 )
                result.push_back( entryID( entryPtr ) );
            entryPtr = nextEntry(entryPtr);
        }
    else
    {
        owner->lock( getNum() );
        sal_Int32 entryIndex  = 0;
        while( entryPtr < freeSpace )
        {
            rtl::OUString key = restoreKey( entryPtr,buffer );

            if( key.getLength() > prefLen )
                key = key.copy( 0,prefLen );

            sal_Int32 cmp = key.indexOf(prefix);

            if( cmp != -1 )
                result.push_back( entryID( entryPtr ) );
            owner->accessBlock( getChildIdx( entryIndex ) )->withPrefix( owner,prefix,prefLen,result );
            entryPtr = nextEntry( entryPtr );
            ++entryIndex;
        }

        owner->unlock( getNum() );
        owner->accessBlock( getChildIdx( numberOfEntries() ) )->withPrefix( owner,prefix,prefLen,result );
    }
}




class BlockProcessorImpl
    : public BlockProcessor
{
public:

    BlockProcessorImpl( BtreeDict* bla )
        : bla_( bla )
    {
    }


    ~BlockProcessorImpl()
    {
    }


    void process( Block* block ) const;

private:

    BtreeDict* bla_;
};



void BlockProcessorImpl::process( Block* block ) const
{
    DictBlock* bla = dynamic_cast<DictBlock*>( block );
    bla->setBlockNumbers( bla_->get_blocks() );
}





BtreeDict::BtreeDict( const util::IndexAccessor& indexAccessor ) throw( IOException )
    : blockManager_( new DBEnvImpl( indexAccessor ) ),   // may throw IOExcption
      blocksL_( 0 ),
      blocks_( 0 )
{

    RandomAccessStream* SCHEMA = indexAccessor.getStream( rtl::OUString::createFromAscii( "SCHEMA" ),
                                                          rtl::OUString::createFromAscii( "r" ) );

    if( SCHEMA )
    {
        sal_Int32 len = SCHEMA->length();
        char* bff = new char[ 1 + len ];
        bff[ len ] = 0;
        SCHEMA->readBytes( reinterpret_cast<sal_Int8*>( bff ),len );
        delete SCHEMA;

        rtl::OString aStr( bff );

        sal_Int32 idx = 3 + aStr.lastIndexOf( "rt=" );
        root_ = atoi( bff + idx );

        idx = 4 + aStr.lastIndexOf( "id1=" );
        sal_Int32 count = atoi( bff + idx );
        blocks_ = new sal_Int32[ blocksL_ = count ];

        delete[] bff;
        BlockProcessorImpl blProc( this );
        blockManager_.mapBlocks( blProc );
    }
    else
        throw IOException( rtl::OUString::createFromAscii( "BtreeDict::BtreeDict -> no SCHEMA/schema" ) );
}


BtreeDict::~BtreeDict()
{
    delete[] blocks_;
}


const DictBlock* BtreeDict::accessBlock( sal_Int32 id ) const
{
    const Block* bl = blockManager_.accessBlock( id );
    return dynamic_cast< const DictBlock* >( bl );
}


void BtreeDict::lock( sal_Int32 blNum ) const throw( excep::IllegalIndexException )
{
  blockManager_.lock( blNum );
}


void BtreeDict::unlock( sal_Int32 blNum ) const throw( excep::IllegalIndexException )
{
  blockManager_.unlock( blNum );
}

sal_Int32 BtreeDict::fetch( const rtl::OUString& key ) const throw( excep::XmlSearchException )
{
    /**
     *  Here is the correct code. To function it requires a bug fix in the old Javacode of
     *  Jaczeks index engine.
     */

    rtl::OString searchString( key.getStr(),key.getLength(),RTL_TEXTENCODING_UTF8 );

    return find( accessBlock( root_ ),
                 reinterpret_cast< const sal_Int8* >( searchString.getStr() ),
                 searchString.getLength() );


    /**
     *   The key has to be replaced by UTF8-encoded byterepresentations of unicode string,
     *   as a workaround to Jazceks bug.
     */

//      rtl::OString Key( key.getStr(),key.getLength(),RTL_TEXTENCODING_ISO_8859_1 );

//      sal_Int32 len = key.getLength();
//      sal_Int8 *searchStr = new sal_Int8[ 1+len ];
//      searchStr[len] = 0;

//      rtl_copyMemory( searchStr,(const sal_Int8*)(Key.getStr()),len );
//      sal_Int32 ret = find( accessBlock( root_ ),
//                            searchStr,
//                            len );
//      delete[] searchStr;
//      return ret;
}


rtl::OUString BtreeDict::fetch( sal_Int32 conceptID ) const throw( excep::XmlSearchException )
{
    if( blocksL_ <= conceptID )
        throw excep::XmlSearchException( rtl::OUString() );

    return findID( blocks_[conceptID], conceptID );
}


std::vector< sal_Int32 > BtreeDict::withPrefix( const rtl::OUString& prefix ) const
    throw( excep::XmlSearchException )
{
    std::vector< sal_Int32 > result;
    const DictBlock *bl = accessBlock( root_ );
    bl->withPrefix( this,prefix,prefix.getLength(),result );
    return result;
}



sal_Int32 BtreeDict::find( const DictBlock* bl,
                           const sal_Int8* key,
                           sal_Int32 inputKeyLen ) const throw( excep::XmlSearchException )
{
    sal_Int32 entryPtr    = bl->firstEntry();
    sal_Int32 freeSpace   = bl->free();
    sal_Int32 nCharsEqual = 0;
    sal_Int32 compression = 0;

    for( sal_Int32 entryIdx = 0; ; )
    {
        if( entryPtr == freeSpace )
            return find( bl,
                         key,
                         inputKeyLen,
                         bl->numberOfEntries() );

        else if( compression == nCharsEqual )
        {
            sal_Int32 keyLen = bl->entryKeyLength( entryPtr );
            sal_Int32 keyPtr = bl->entryKey( entryPtr ), i;
            for( i = 0; i < keyLen && key[ nCharsEqual ] == bl->getData()[keyPtr + i]; i++ )
                ++nCharsEqual;
            if( i == keyLen )
            {
                if( nCharsEqual == inputKeyLen )
                    return bl->entryID( entryPtr );
            }
            else if( ( key[ nCharsEqual ] & 0xFF ) < ( bl->getData()[ keyPtr + i ] & 0xFF) )
                return find( bl,key,inputKeyLen,entryIdx );
        }

        else if( compression < nCharsEqual ) // compression dropped
            return find( bl,key,inputKeyLen,entryPtr == freeSpace ? bl->numberOfEntries() : entryIdx );


        do
        {
            entryPtr = bl->nextEntry( entryPtr );
            ++entryIdx;
        } while( bl->entryCompression( entryPtr ) > nCharsEqual );

        compression = bl->entryCompression( entryPtr );
    }
}




sal_Int32 BtreeDict::find( const DictBlock* bl,
               const sal_Int8* key,
               sal_Int32 inputKeyLen,
               sal_Int32 index ) const throw( xmlsearch::excep::XmlSearchException )
{
  return bl->isLeaf() ? 0 : find( child( bl,index ),key,inputKeyLen );
}



const DictBlock* BtreeDict::child( const DictBlock* bl,sal_Int32 index) const throw( excep::XmlSearchException )
{
  return accessBlock( bl->getChildIdx(index) );
}


rtl::OUString BtreeDict::findID( sal_Int32 blNum,sal_Int32 id ) const throw( xmlsearch::excep::XmlSearchException )
{
  return accessBlock( blNum )->findID( id );
}




// Definitions for DBEnvImpl



DBEnvImpl::DBEnvImpl( const util::IndexAccessor& indexAccessor )
{
    file_ = indexAccessor.getStream( rtl::OUString::createFromAscii( "DICTIONARY" ),
                                     rtl::OUString::createFromAscii( "r" ) );
}


DBEnvImpl::~DBEnvImpl()
{
    delete file_;
}


sal_Int32 DBEnvImpl::getEntryHeaderLen() const
{
    return BtreeDict::ENTHEADERLEN;
}


sal_Int32 DBEnvImpl::getBlockCount() const
{
    if( file_ )
        return file_->length() / BtreeDict::BLOCKSIZE;
    else
        return 0;
}

sal_Int32 DBEnvImpl::getMaximumBlockCount() const
{
    return BtreeDict::nBlocksLimit;
}


sal_Int32 util::DBEnvImpl::getDataLen() const
{
    return BtreeDict::DATALEN;
}


sal_Int32 DBEnvImpl::getBlockLen() const
{
    return BtreeDict::BLOCKSIZE;
}


void DBEnvImpl::read( sal_Int32 blNum,xmlsearch::db::Block*& block ) const
{
    if( ! block )
        block = new xmlsearch::db::DictBlock( this );

    if( file_ )
    {
        file_->seek( blNum * getBlockLen() );
        block->read( file_ );
    }
}


void DBEnvImpl::write( sal_Int32 blNum,xmlsearch::db::Block* block )
{
    (void)blNum;
    if( ! block )
        return;
}
