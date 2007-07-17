/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: subsettableshapemanager.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: obo $ $Date: 2007-07-17 15:17:13 $
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

#ifndef INCLUDED_SLIDESHOW_SUBSETTABLESHAPEMANAGER_HXX
#define INCLUDED_SLIDESHOW_SUBSETTABLESHAPEMANAGER_HXX

#include "shapemanager.hxx"
#include "intrinsicanimationeventhandler.hxx"
#include <boost/shared_ptr.hpp>

/* Definition of SubsettableShapeManager interface */

namespace slideshow
{
    namespace internal
    {
        class DocTreeNode;
        class AttributableShape;

        /** SubsettableShapeManager interface

            Implementers of this interface manage creation and
            revocation of shape subsets. Shape subsets are shapes that
            represent (and animate) only parts of an original's shape
            content.
         */
        class SubsettableShapeManager : public ShapeManager
        {
        public:
            /** Query a subset of the given original shape

                This method queries a new (but not necessarily unique)
                shape, which displays only the given subset of the
                original one. Calling this method multiple times with
                the same original shape and DocTreeNode content always
                returns the same shape.

                Requesting a subset from an original shape leads to
                the original shape ceasing to display the subsetted
                content. In other words, shape content is always
                displayed in exactly one shape.

                @param rOrigShape
                The shape the subset is to be created for

                @param rSubsetShape
                The subset to display in the generated shape.
             */
            virtual boost::shared_ptr<AttributableShape> getSubsetShape(
                const boost::shared_ptr<AttributableShape>& rOrigShape,
                const DocTreeNode&                          rTreeNode ) = 0;

            /** Revoke a previously queried subset shape.

                With this method, a previously requested subset shape
                is revoked again. If the last client revokes a given
                subset, it will cease to be displayed, and the
                original shape will again show the subset data.

                @param rOrigShape
                The shape the subset was created from

                @param rSubsetShape
                The subset created from rOrigShape
             */
            virtual void revokeSubset(
                const boost::shared_ptr<AttributableShape>& rOrigShape,
                const boost::shared_ptr<AttributableShape>& rSubsetShape ) = 0;

            // Evil hackish way of getting intrinsic animation slide-wise

            /** Register an event handler that will be called when
                user paint parameters change.

                @param rHandler
                Handler to call when a shape listener changes
            */
            virtual void addIntrinsicAnimationHandler( const IntrinsicAnimationEventHandlerSharedPtr& rHandler ) = 0;
            virtual void removeIntrinsicAnimationHandler( const IntrinsicAnimationEventHandlerSharedPtr& rHandler ) = 0;

            /** Notify that shape-intrinsic animations are now enabled.

                @return true, if this event was processed by
                anybody. If false is returned, no handler processed
                this event (and probably, nothing will happen at all)
            */
            virtual bool notifyIntrinsicAnimationsEnabled() = 0;

            /** Notify that shape-intrinsic animations are now disabled.

                @return true, if this event was processed by
                anybody. If false is returned, no handler processed
                this event (and probably, nothing will happen at all)
            */
            virtual bool notifyIntrinsicAnimationsDisabled() = 0;
        };

        typedef ::boost::shared_ptr< SubsettableShapeManager > SubsettableShapeManagerSharedPtr;
    }
}

#endif /* INCLUDED_SLIDESHOW_SUBSETTABLESHAPEMANAGER_HXX */
