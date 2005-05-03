/*************************************************************************
 *
 *  $RCSfile: animationnodefactory.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: obo $ $Date: 2005-05-03 14:06:40 $
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

// must be first
#include <canvas/debug.hxx>
#include <canvas/verbosetrace.hxx>

#include <animationnodefactory.hxx>
#include <paralleltimecontainer.hxx>
#include <sequentialtimecontainer.hxx>
#include <propertyanimationnode.hxx>
#include <animationsetnode.hxx>
#include <animationpathmotionnode.hxx>
#include <animationcolornode.hxx>
#include <animationtransformnode.hxx>
#include <animationtransitionfilternode.hxx>
#include <animationaudionode.hxx>
#include <animationcommandnode.hxx>
#include <nodetools.hxx>
#include <tools.hxx>

#include <animations/animationnodehelper.hxx>

#ifndef _COM_SUN_STAR_DRAWING_XSHAPE_HPP_
#include <com/sun/star/drawing/XShape.hpp>
#endif
#ifndef _COM_SUN_STAR_ANIMATIONS_XANIMATE_HPP_
#include <com/sun/star/animations/XAnimate.hpp>
#endif
#ifndef _COM_SUN_STAR_ANIMATIONS_ANIMATIONNODETYPE_HPP_
#include <com/sun/star/animations/AnimationNodeType.hpp>
#endif
#ifndef _COM_SUN_STAR_PRESENTATION_EFFECTNODETYPE_HPP_
#include <com/sun/star/presentation/EffectNodeType.hpp>
#endif
#ifndef _COM_SUN_STAR_PRESENTATION_TEXTANIMATIONTYPE_HPP_
#include <com/sun/star/presentation/TextAnimationType.hpp>
#endif
#ifndef _COM_SUN_STAR_ANIMATIONS_XANIMATESET_HPP_
#include <com/sun/star/animations/XAnimateSet.hpp>
#endif
#ifndef _COM_SUN_STAR_ANIMATIONS_XITERATECONTAINER_HPP_
#include <com/sun/star/animations/XIterateContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_PRESENTATION_SHAPEANIMATIONSUBTYPE_HPP_
#include <com/sun/star/presentation/ShapeAnimationSubType.hpp>
#endif
#ifndef _COM_SUN_STAR_ANIMATIONS_XANIMATEMOTION_HPP_
#include <com/sun/star/animations/XAnimateMotion.hpp>
#endif
#ifndef _COM_SUN_STAR_ANIMATIONS_XANIMATECOLOR_HPP_
#include <com/sun/star/animations/XAnimateColor.hpp>
#endif
#ifndef _COM_SUN_STAR_ANIMATIONS_XANIMATETRANSFORM_HPP_
#include <com/sun/star/animations/XAnimateTransform.hpp>
#endif
#ifndef _COM_SUN_STAR_ANIMATIONS_ANIMATIONTRANSFORMTYPE_HPP_
#include <com/sun/star/animations/AnimationTransformType.hpp>
#endif
#ifndef _COM_SUN_STAR_ANIMATIONS_XTRANSITIONFILTER_HPP_
#include <com/sun/star/animations/XTransitionFilter.hpp>
#endif
#ifndef _COM_SUN_STAR_ANIMATIONS_XAUDIO_HPP_
#include <com/sun/star/animations/XAudio.hpp>
#endif
#ifndef _COM_SUN_STAR_PRESENTATION_PARAGRAPHTARGET_HPP_
#include <com/sun/star/presentation/ParagraphTarget.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif

#ifndef _BGFX_NUMERIC_FTOOLS_HXX
#include <basegfx/numeric/ftools.hxx>
#endif

#ifndef BOOST_BIND_HPP_INCLUDED
#include <boost/bind.hpp>
#endif
#ifndef BOOST_MEM_FN_HPP_INCLUDED
#include <boost/mem_fn.hpp>
#endif

#include <vector>
#include <algorithm>
#include <iterator>

using namespace ::com::sun::star;

namespace presentation
{
    namespace internal
    {
        namespace
        {
            // forward declaration needed by NodeCreator
            BaseNodeSharedPtr implCreateAnimationNode( const uno::Reference< animations::XAnimationNode >&  xNode,
                                                       const BaseContainerNodeSharedPtr&                    rParent,
                                                       const NodeContext&                                   rContext );

            class NodeCreator
            {
            public:
                NodeCreator( BaseContainerNodeSharedPtr&    rParent,
                             const NodeContext&             rContext ) :
                    mrParent( rParent ),
                    mrContext( rContext )
                {
                }

                void operator()( const uno::Reference< animations::XAnimationNode >& xChildNode ) const
                {
                    createChild( xChildNode, mrContext );
                }

            protected:
                void createChild( const uno::Reference< animations::XAnimationNode >&   xChildNode,
                                  const NodeContext&                                    rContext ) const
                {
                    BaseNodeSharedPtr pChild( implCreateAnimationNode( xChildNode,
                                                                       mrParent,
                                                                       rContext ) );

                    OSL_ENSURE( pChild.get(), "NodeCreator::operator(): child creation failed" );

                    // TODO(Q1): This yields circular references, which, it seems, is
                    // unavoidable here
                    if( pChild.get() )
                        mrParent->appendChildNode( pChild );
                }

                BaseContainerNodeSharedPtr&     mrParent;
                const NodeContext&              mrContext;
            };

            /** Same as NodeCreator, only that NodeContext's
                SubsetShape is cloned for every child node.

                This is used for iterated animation node generation
             */
            class CloningNodeCreator : private NodeCreator
            {
            public:
                CloningNodeCreator( BaseContainerNodeSharedPtr& rParent,
                                    const NodeContext&          rContext ) :
                    NodeCreator( rParent, rContext )
                {
                }

                void operator()( const uno::Reference< animations::XAnimationNode >& xChildNode ) const
                {
                    NodeContext aContext( mrContext );

                    // TODO(Q1): There's a catch here. If you clone a
                    // subset whose actual subsetting has already been
                    // realized (i.e. if enableSubsetShape() has been
                    // called already), and the original of your clone
                    // goes out of scope, then your subset will be
                    // gone (LayerManager::revokeSubset() be
                    // called). As of now, this behaviour is not
                    // triggered here (we either clone, XOR we enable
                    // subset initially), but one might consider
                    // reworking DrawShape/ShapeSubset to avoid this.

                    // clone ShapeSubset, since each node needs their
                    // own version of the ShapeSubset (otherwise,
                    // e.g. activity counting does not work - subset
                    // would be removed after first animation node
                    // disables it).
                    //
                    // NOTE: this is only a problem for animation
                    // nodes that explicitely call
                    // disableSubsetShape(). Independent shape subsets
                    // (like those created for ParagraphTargets)
                    // solely rely on the ShapeSubset destructor to
                    // normalize things, which does the right thing
                    // here: the subset is only removed after _the
                    // last_ animation node releases the shared ptr.
                    aContext.mpMasterShapeSubset.reset(
                        new ShapeSubset( *aContext.mpMasterShapeSubset ) );

                    createChild( xChildNode, aContext );
                }
            };

            /** Create animation nodes for text iterations

                This method clones the animation nodes below xIterNode
                for every iterated shape entity.
             */
            bool implCreateIteratedNodes( const uno::Reference< animations::XIterateContainer >&    xIterNode,
                                          BaseContainerNodeSharedPtr&                               rParent,
                                          const NodeContext&                                        rContext )
            {
                ENSURE_AND_THROW( xIterNode.is(),
                                  "implCreateIteratedNodes(): Invalid node" );

                const double nIntervalTimeout( xIterNode->getIterateInterval() );


                // valid iterate interval? We're ruling out monstrous
                // values here, to avoid pseudo 'hangs' in the
                // presentation
                if( nIntervalTimeout < 0.0 ||
                    nIntervalTimeout > 1000.0 )
                {
                    return false; // not an active iteration
                }

                if( ::basegfx::fTools::equalZero( nIntervalTimeout ) )
                    OSL_TRACE( "implCreateIteratedNodes(): iterate interval close to zero, there's "
                               "no point in defining such an effect (visually equivalent to whole-shape effect)" );

                // Determine target shape (or subset)
                // ==================================

                // TODO(E1): I'm not too sure what to expect here...
                ENSURE_AND_RETURN( xIterNode->getTarget().hasValue(),
                                   "implCreateIteratedNodes(): no target on ITERATE node" );

                uno::Reference< drawing::XShape > xTargetShape( xIterNode->getTarget(),
                                                                uno::UNO_QUERY );

                ::com::sun::star::presentation::ParagraphTarget aTarget;
                sal_Int16                                       nSubItem( xIterNode->getSubItem() );
                bool                                            bParagraphTarget( false );

                if( !xTargetShape.is() )
                {
                    // no shape provided. Maybe a ParagraphTarget?
                    if( !(xIterNode->getTarget() >>= aTarget) )
                        ENSURE_AND_RETURN( false,
                                           "implCreateIteratedNodes(): could not extract any "
                                           "target information" );

                    xTargetShape = aTarget.Shape;

                    ENSURE_AND_RETURN( xTargetShape.is(),
                                       "implCreateIteratedNodes(): invalid shape in ParagraphTarget" );

                    // we've a paragraph target to iterate over, thus,
                    // the whole animation container refers only to
                    // the text
                    nSubItem = ::com::sun::star::presentation::ShapeAnimationSubType::ONLY_TEXT;

                    bParagraphTarget = true;
                }


                // Lookup shape, and fill NodeContext
                // ==================================

                AttributableShapeSharedPtr  pTargetShape(
                    lookupAttributableShape( rContext.maContext.mpLayerManager,
                                             xTargetShape ) );
                const DocTreeNodeSupplier&  rTreeNodeSupplier( pTargetShape->getTreeNodeSupplier() );
                ShapeSubsetSharedPtr        pTargetSubset;

                NodeContext aContext( rContext );

                // paragraph targets already need a subset as the
                // master shape (they're representing only a single
                // paragraph)
                if( bParagraphTarget )
                {
                    ENSURE_AND_RETURN(
                        aTarget.Paragraph >= 0 &&
                        rTreeNodeSupplier.getNumberOfTreeNodes(
                            DocTreeNode::NODETYPE_LOGICAL_PARAGRAPH ) > aTarget.Paragraph,
                        "implCreateIteratedNodes(): paragraph index out of range" );

                    pTargetSubset.reset(
                        new ShapeSubset(
                            pTargetShape,
                            // retrieve index aTarget.Paragraph of
                            // type PARAGRAPH from this shape
                            rTreeNodeSupplier.getTreeNode(
                                aTarget.Paragraph,
                                DocTreeNode::NODETYPE_LOGICAL_PARAGRAPH ),
                            rContext.maContext.mpLayerManager ) );

                    // iterate target is not the whole shape, but only
                    // the selected paragraph - subset _must_ be
                    // independent, to be able to affect visibility
                    // independent of master shape
                    aContext.mbIsIndependentSubset = true;

                    // already enable parent subset right here, to
                    // make potentially generated subsets subtract
                    // their content from the parent subset (and not
                    // the original shape). Otherwise, already
                    // subsetted parents (e.g. paragraphs) would not
                    // have their characters removed, when the child
                    // iterations start.
                    // Furthermore, the setup of initial shape
                    // attributes of course needs the subset shape
                    // generated, to apply e.g. visibility changes.
                    pTargetSubset->enableSubsetShape();
                }
                else
                {
                    pTargetSubset.reset(
                        new ShapeSubset( pTargetShape,
                                         rContext.maContext.mpLayerManager ) );
                }

                aContext.mpMasterShapeSubset = pTargetSubset;
                uno::Reference< animations::XAnimationNode > xNode( xIterNode,
                                                                    uno::UNO_QUERY_THROW );


                // Generate subsets
                // ================

                if( bParagraphTarget ||
                    nSubItem != ::com::sun::star::presentation::ShapeAnimationSubType::ONLY_TEXT )
                {
                    // prepend with animations for
                    // full Shape (will be subtracted
                    // from the subset parts within
                    // the Shape::createSubset()
                    // method). For ONLY_TEXT effects,
                    // we skip this part, to animate
                    // only the text.
                    //
                    // OR
                    //
                    // prepend with subset animation for full
                    // _paragraph_, from which the individual
                    // paragraph subsets are subtracted. Note that the
                    // subitem is superfluous here, we always assume
                    // ONLY_TEXT, if a paragraph is referenced as the
                    // master of an iteration effect.
                    NodeCreator aCreator( rParent, aContext );
                    if( !::anim::for_each_childNode( xNode,
                                                     aCreator ) )
                    {
                        ENSURE_AND_RETURN( false,
                                           "implCreateIteratedNodes(): iterated child node creation failed" );
                    }
                }

                // TODO(F2): This does not do the correct
                // thing. Having nSubItem be set to ONLY_BACKGROUND
                // should result in the text staying unanimated in the
                // foreground, while the shape moves in the background
                // (this behaviour is perfectly possible with the
                // slideshow engine, only that the text won't be
                // currently visible, because animations are always in
                // the foreground)
                if( nSubItem != ::com::sun::star::presentation::ShapeAnimationSubType::ONLY_BACKGROUND )
                {
                    // determine type of subitem iteration (logical
                    // text unit to animate)
                    DocTreeNode::NodeType eIterateNodeType(
                        DocTreeNode::NODETYPE_LOGICAL_CHARACTER_CELL );

                    switch( xIterNode->getIterateType() )
                    {
                        case ::com::sun::star::presentation::TextAnimationType::BY_PARAGRAPH:
                            eIterateNodeType = DocTreeNode::NODETYPE_LOGICAL_PARAGRAPH;
                            break;

                        case ::com::sun::star::presentation::TextAnimationType::BY_WORD:
                            eIterateNodeType = DocTreeNode::NODETYPE_LOGICAL_WORD;
                            break;

                        case ::com::sun::star::presentation::TextAnimationType::BY_LETTER:
                            eIterateNodeType = DocTreeNode::NODETYPE_LOGICAL_CHARACTER_CELL;
                            break;

                        default:
                            ENSURE_AND_THROW(false,
                                             "implCreateIteratedNodes(): Unexpected IterateType on XIterateContainer");
                            break;
                    }

                    if( bParagraphTarget &&
                        eIterateNodeType != DocTreeNode::NODETYPE_LOGICAL_WORD &&
                        eIterateNodeType != DocTreeNode::NODETYPE_LOGICAL_CHARACTER_CELL )
                    {
                        // will not animate the whole paragraph, when
                        // only the paragraph is animated at all.
                        OSL_ENSURE( false,
                                    "implCreateIteratedNodes(): Ignoring paragraph iteration for paragraph master" );
                    }
                    else
                    {
                        // setup iteration parameters
                        // --------------------------

                        // iterate target is the whole shape (or the
                        // whole parent subshape), thus, can save
                        // loads of subset shapes by generating them
                        // only when the effects become active -
                        // before and after the effect active
                        // duration, all attributes are shared by
                        // master shape and subset (since the iterated
                        // effects are all the same).
                        aContext.mbIsIndependentSubset = false;

                        // determine number of nodes for given subitem
                        // type
                        sal_Int32 nTreeNodes( 0 );
                        if( bParagraphTarget )
                        {
                            // create the iterated subset _relative_ to
                            // the given paragraph index (i.e. animate the
                            // given subset type, but only when it's part
                            // of the given paragraph)
                            nTreeNodes = rTreeNodeSupplier.getNumberOfSubsetTreeNodes(
                                pTargetSubset->getSubset(),
                                eIterateNodeType );
                        }
                        else
                        {
                            // generate normal subset
                            nTreeNodes = rTreeNodeSupplier.getNumberOfTreeNodes(
                                eIterateNodeType );
                        }


                        // iterate node, generate copies of the children for each subset
                        // -------------------------------------------------------------

                        // NodeContext::mnStartDelay contains additional node delay. This will
                        // make the duplicated nodes for each iteration start increasingly later.
                        aContext.mnStartDelay = nIntervalTimeout;

                        for( sal_Int32 i=0; i<nTreeNodes; ++i )
                        {
                            // create subset with the corresponding tree nodes
                            if( bParagraphTarget )
                            {
                                // create subsets relative to paragraph subset
                                aContext.mpMasterShapeSubset.reset(
                                    new ShapeSubset(
                                        pTargetSubset,
                                        rTreeNodeSupplier.getSubsetTreeNode(
                                            pTargetSubset->getSubset(),
                                            i,
                                            eIterateNodeType ) ) );
                            }
                            else
                            {
                                // create subsets from main shape
                                aContext.mpMasterShapeSubset.reset(
                                    new ShapeSubset( pTargetSubset,
                                                     rTreeNodeSupplier.getTreeNode(
                                                         i,
                                                         eIterateNodeType ) ) );
                            }

                            CloningNodeCreator aCreator( rParent, aContext );
                            if( !::anim::for_each_childNode( xNode,
                                                             aCreator ) )
                            {
                                ENSURE_AND_RETURN( false,
                                                   "implCreateIteratedNodes(): iterated child node creation failed" );
                            }

                            aContext.mnStartDelay += nIntervalTimeout;
                        }
                    }
                }

                // done with iterate child generation
                return true;
            }

            BaseNodeSharedPtr implCreateAnimationNode( const uno::Reference< animations::XAnimationNode >&  xNode,
                                                       const BaseContainerNodeSharedPtr&                    rParent,
                                                       const NodeContext&                                   rContext )
            {
                ENSURE_AND_THROW( xNode.is(), "implCreateAnimationNode(): invalid XAnimationNode" );

                BaseNodeSharedPtr           pCreatedNode;
                BaseContainerNodeSharedPtr  pCreatedContainer;

                // create the internal node, corresponding to xNode
                switch( xNode->getType() )
                {
                    case animations::AnimationNodeType::CUSTOM:
                        OSL_ENSURE( false, "implCreateAnimationNode(): CUSTOM not yet implemented" );
                        return pCreatedNode;

                    case animations::AnimationNodeType::PAR:
                        pCreatedNode = pCreatedContainer = BaseContainerNodeSharedPtr( new ParallelTimeContainer( xNode, rParent, rContext ) );
                        break;

                    case animations::AnimationNodeType::ITERATE:
                        // map iterate container to ParallelTimeContainer.
                        // the iterating functionality is to be found
                        // below, (see method implCreateIteratedNodes)
                        pCreatedNode = pCreatedContainer = BaseContainerNodeSharedPtr( new ParallelTimeContainer( xNode, rParent, rContext ) );
                        break;

                    case animations::AnimationNodeType::SEQ:
                        pCreatedNode = pCreatedContainer = BaseContainerNodeSharedPtr( new SequentialTimeContainer( xNode, rParent, rContext ) );
                        break;

                    case animations::AnimationNodeType::ANIMATE:
                        pCreatedNode.reset( new PropertyAnimationNode( xNode, rParent, rContext ) );
                        break;

                    case animations::AnimationNodeType::SET:
                        pCreatedNode.reset( new AnimationSetNode( xNode, rParent, rContext ) );
                        break;

                    case animations::AnimationNodeType::ANIMATEMOTION:
                        pCreatedNode.reset( new AnimationPathMotionNode( xNode, rParent, rContext ) );
                        break;

                    case animations::AnimationNodeType::ANIMATECOLOR:
                        pCreatedNode.reset( new AnimationColorNode( xNode, rParent, rContext ) );
                        break;

                    case animations::AnimationNodeType::ANIMATETRANSFORM:
                        pCreatedNode.reset( new AnimationTransformNode( xNode, rParent, rContext ) );
                        break;

                    case animations::AnimationNodeType::TRANSITIONFILTER:
                        pCreatedNode.reset( new AnimationTransitionFilterNode( xNode, rParent, rContext ) );
                        break;

                    case animations::AnimationNodeType::AUDIO:
                        pCreatedNode.reset( new AnimationAudioNode( xNode, rParent, rContext ) );
                        break;

                    case animations::AnimationNodeType::COMMAND:
                        pCreatedNode.reset( new AnimationCommandNode( xNode, rParent, rContext ) );
                        break;

                    default:
                        OSL_ENSURE( false, "implCreateAnimationNode(): invalid AnimationNodeType" );
                        return pCreatedNode;
                }

                // TODO(Q1): This yields circular references, which, it seems, is
                // unavoidable here

                // HACK: node objects need shared_ptr to themselves,
                // which we pass them here.
                pCreatedNode->setSelf( pCreatedNode );

                // if we've got a container node object, recursively add
                // its children
                if( pCreatedContainer.get() )
                {
                    uno::Reference< animations::XIterateContainer > xIterNode( xNode,
                                                                               uno::UNO_QUERY );

                    // when this node is an XIterateContainer with
                    // active iterations, this method will generate
                    // the appropriate children
                    if( xIterNode.is() )
                    {
                        // note that implCreateIteratedNodes() might
                        // choose not to generate any child nodes
                        // (e.g. when the iterate timeout is outside
                        // sensible limits). Then, no child nodes are
                        // generated at all, since typically, child
                        // node attribute are incomplete for iteration
                        // children.
                        implCreateIteratedNodes( xIterNode,
                                                 pCreatedContainer,
                                                 rContext );
                    }
                    else
                    {
                        // no iterate subset node, just plain child generation now
                        NodeCreator aCreator( pCreatedContainer, rContext );
                        if( !::anim::for_each_childNode( xNode,
                                                         aCreator ) )
                        {
                            OSL_ENSURE( false,
                                        "implCreateAnimationNode(): child node creation failed" );
                            return BaseNodeSharedPtr();
                        }
                    }
                }

                return pCreatedNode;
            }
        }

        AnimationNodeSharedPtr AnimationNodeFactory::createAnimationNode( const uno::Reference< animations::XAnimationNode >&   xNode,
                                                                          const SlideShowContext&                               rContext )
        {
            ENSURE_AND_THROW( xNode.is(), "AnimationNodeFactory::createAnimationNode(): invalid XAnimationNode" );

            return BaseNodeSharedPtr( implCreateAnimationNode( xNode,
                                                               BaseContainerNodeSharedPtr(), // no parent
                                                               NodeContext( rContext ) ) );
        }

#if defined(VERBOSE) && defined(DBG_UTIL)
        void AnimationNodeFactory::showTree( AnimationNodeSharedPtr& pRootNode )
        {
            if( pRootNode.get() )
                DEBUG_NODES_SHOWTREE( dynamic_cast<BaseContainerNode*>( pRootNode.get() ) );
        }
#endif
    }
}
