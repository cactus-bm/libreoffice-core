/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: Query.cxx,v $
 * $Revision: 1.12 $
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
#include <qe/Query.hxx>
#include <qe/XmlIndex.hxx>
#include <qe/ConceptData.hxx>
#include <qe/QueryProcessor.hxx>
#include <qe/ContextTables.hxx>


using namespace xmlsearch::qe;


sal_Int32* QueryHit::getMatches( sal_Int32& matchesL )
{
    matchesL = matchesL_;
    return matches_;
}


/******************************************************************************/
/*                                                                            */
/*                     HitStore                                               */
/*                                                                            */
/******************************************************************************/


HitStore::HitStore( double initialStandard,sal_Int32 limit,sal_Int32 nColumns )
    : limit_( limit ),
      nColumns_( nColumns ),
      index_( 0 ),
      free_( 0 ),
      standard_( initialStandard ),
      heap_( limit )
{
    for( sal_uInt32 i = 0; i < heap_.size(); ++i )
        heap_[i] = 0;
}



HitStore::~HitStore()
{
    for( sal_uInt32 i = 0; i < heap_.size(); ++i )
        delete heap_[i];
}



bool HitStore::goodEnough( double penalty, sal_Int32 begin, sal_Int32 end )
{
    return free_ == limit_ ? heap_[0]->worseThan( penalty,begin,end ) : true;
}


QueryHit* HitStore::createQueryHit( double penalty,sal_Int32 doc,sal_Int32 begin,sal_Int32 end )
{
    QueryHit* hit = new QueryHit( nColumns_,penalty,doc,begin,end );
    if( free_ == limit_ )
    { // goodEnough'ness checked already
        delete heap_[0];
        heap_[0] = hit;
        heapify( 0 );
        standard_ = heap_[0]->getPenalty();
    }
    else if( free_ < limit_ )
    {
        heap_[ free_++ ] = hit;
        if( free_ == limit_ )
        { // we have the needed number
            for( sal_Int32 i = free_/2; i >= 0; --i ) // build heap
                heapify( i );
            standard_ = heap_[0]->getPenalty();
        }
    }
    return hit;
}


struct CompareQueryHit
{
    bool operator()( const QueryHit* l,const QueryHit* r )
    {
        return l->compareTo( r );
    }
};


#include <algorithm>


QueryHit* HitStore::firstBestQueryHit()
{
    if( free_ > 0)
    {
        CompareQueryHit bla;
        heap_.resize( free_ );
          std::stable_sort( heap_.begin(),heap_.end(),bla );
        index_ = 0;
        return nextBestQueryHit();
    }
    else
        return 0;
}


QueryHit* HitStore::nextBestQueryHit()
{
    return index_ < free_ ? heap_[ index_++ ] : 0;
}


void HitStore::heapify( sal_Int32 i )
{
    for( sal_Int32 r,l,worst; ; )
    {
        r = (i + 1) << 1; l = r - 1;
        worst = l < free_ && heap_[i]->betterThan( heap_[l] ) ? l : i;
        if( r < free_ && heap_[ worst ]->betterThan( heap_[r] ) )
            worst = r;
        if (worst != i)
        {
            QueryHit* temp = heap_[ worst ];
            heap_[ worst ] = heap_[ i ];
            heap_[i] = temp;
            i = worst;      // continue
        }
        else
            break;
    }
}


//  sal_Int32 HitStore::partition( sal_Int32 p,sal_Int32 r )
//  {
//      QueryHit* x = heap_[ ((p + r) >> 1) & 0x7FFFFFFF ];
//      sal_Int32 i = p - 1, j = r + 1;
//      while( true )
//      {
//          while( x->compareTo( heap_[--j] ) )
//              ;
//          while( heap_[++i]->compareTo( x ) )
//              ;
//          if( i < j )
//          {
//              QueryHit* t = heap_[i];
//              heap_[i] = heap_[j];
//              heap_[j] = t;
//          }
//          else
//              return j;
//      }
//  }


//  void HitStore::quicksort( sal_Int32 p,sal_Int32 r )
//  {
//      while( p < r )
//      {
//          sal_Int32 q = partition( p,r );
//          quicksort(p, q);
//          p = q + 1;
//      }
//  }



/******************************************************************************/
/*                                                                            */
/*                                 Query                                      */
/*                                                                            */
/******************************************************************************/


#define MissingTermPenalty 10.0


Query::Query( XmlIndex* env,
              sal_Int32 nColumns,
              sal_Int32 nHits,
              sal_Int32 missingPenaltiesL,
              double* missingPenalties )
    : env_( env ),
      ctx_( env ? env->getContextInfo() : 0 ),
      store_( nColumns * MissingTermPenalty - 0.0001,nHits,nColumns ),
      nHitsRequested_( nHits ),
      nColumns_( nColumns ),
      currentStandard_( nColumns * MissingTermPenalty - 0.0001 ),
      missingPenaltyL_( nColumns ),
      upperboundTemplateL_( nColumns ),
      penaltiesL_( missingPenaltiesL ),
      missingPenalty_( new double[ nColumns ] ),
      upperboundTemplate_( new double[ nColumns ] ),
      penalties_( missingPenalties ),
      ignoredElementsL_( 0 ),
      ignoredElements_( 0 ),
      missingTermsPenalty_( 0.0 )
{
    // for the EmptyQuery case (awaits arch improvement pass)

    if( missingPenalties )
        for( sal_Int32 i = 0;i < nColumns_; ++i )
            missingPenalty_[i] = missingPenalties[i];
    else
        for( sal_Int32 i = 0;i < nColumns_; ++i )
            missingPenalty_[i] = MissingTermPenalty;

    makePenaltiesTable();
    //  _roleFillerList = RoleFiller.STOP;
}


Query::~Query()
{
    delete[] missingPenalty_;
    delete[] upperboundTemplate_;
    delete[] penalties_;
    delete[] ignoredElements_;
}


void Query::setIgnoredElements( const sal_Int32 ignoredElementsL,const rtl::OUString* ignoredElements )
{
    if( ctx_ )
        ignoredElements_ = ctx_->getIgnoredElementsSet( ignoredElementsL_,
                                                        ignoredElementsL,ignoredElements );

    if( ! ctx_ )
    {
        ignoredElementsL_ = 0;
        ignoredElements_   = 0;
    }
}



void Query::missingTerms( sal_Int32 nMissingTerms )
{
    missingTermsPenalty_ = MissingTermPenalty * nMissingTerms;
}



ConceptData* Query::makeConceptData( sal_Int32 col,sal_Int32 concept,double penalty,sal_Int32 queryNo )
{
    return new ConceptData( concept,col,penalty,queryNo,nColumns_,env_->getContextInfo() );;
}


void Query::getHits( std::vector< QueryHitData* >& data,sal_Int32 n )
{
    if( n <= 0 )
        return;

    QueryHit* qh = store_.firstBestQueryHit();

    while( qh )
    {
        data.push_back( env_->hitToData( qh ) );
        qh = data.size() < sal_uInt32( n ) ? store_.nextBestQueryHit() : 0;
    }
}


QueryHit* Query::maybeCreateQueryHit( double penalty,
                                      sal_Int32 doc, sal_Int32 begin, sal_Int32 end, sal_Int32 parentContext )
{
    // hits are located using only terms actually present in text
    // if B is not present, the query A B C reduces to A C and penalties
    // are computed as if B did not occur in query
    // to meaningfully merge results from different servers, some of which
    // may have B, penalty has to be normalized to the common computing scheme

    QueryHit* res =
        ( store_.goodEnough( penalty += missingTermsPenalty_,begin,end )
          && ( ! ignoredElements_ || ctx_->notIgnored( parentContext,ignoredElementsL_,ignoredElements_ ) ) )
        ?
        store_.createQueryHit( penalty,doc,begin,end )
        :
        0;
    return res;
}


void Query::makePenaltiesTable()
{
    sal_Int32 nPatterns = 1 << nColumns_;
    delete[] penalties_;
    penalties_ = new double[ penaltiesL_ = nPatterns ];
    for (sal_Int32 i = 0; i < nPatterns; ++i )
        penalties_[i] = computePenalty(i);
}


double Query::computePenalty( sal_Int32 n )
{
    double penalty = 0.0;
    for( sal_Int32 i = 0; i < nColumns_; ++i )
        if( ( n & 1 << i ) == 0 )
            penalty += missingPenalty_[i];
    return penalty;
}


void Query::resetForNextDocument()
{
    currentStandard_ = store_.getCurrentStandard();
    // "everything's missing"
    for( sal_Int32 i = 0; i < nColumns_; i++ )
        upperboundTemplate_[i] = missingPenalty_[i];
    vote_ = false;
}


bool Query::vote()
{
    double sum = 0.0;
    for( sal_Int32 i = 0; i < nColumns_; i++ )
        sum += upperboundTemplate_[i];
    return vote_ = (sum <= currentStandard_ );
}


void Query::updateEstimate( sal_Int32 role,double penalty )
{
    if( penalty < upperboundTemplate_[ role ] )
        upperboundTemplate_[ role ] = penalty;
}


/******************************************************************************/
/*                                                                            */
/*                         QueryHitIterator                                   */
/*                                                                            */
/******************************************************************************/



QueryHitIterator::QueryHitIterator( const QueryResults* result )
    : index_( -1 ),
      result_( result )
{
}


QueryHitIterator::~QueryHitIterator()
{
    delete result_;
}


bool QueryHitIterator::next()
{
    return accessible_ = ( ++index_ < sal_Int32( result_->queryHits_.size() ) );
}


QueryHitData* QueryHitIterator::getHit( const PrefixTranslator* ) const
{
    if( accessible_ )
        return result_->queryHits_[index_];
    else
        return 0;
}


double QueryHitIterator::getPenalty()
{
    if( accessible_ )
        return result_->queryHits_[index_]->getPenalty();
    else
        return 1.0E30;
}

