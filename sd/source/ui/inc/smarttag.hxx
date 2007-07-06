/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: smarttag.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2007-07-06 13:14:03 $
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

#ifndef _SD_SMARTTAG_HXX_
#define _SD_SMARTTAG_HXX_

#include "helper/simplereferencecomponent.hxx"
#include <rtl/ref.hxx>
#include <set>
#include <svx/svdhdl.hxx>
#include <svx/svdview.hxx>

class KeyEvent;
class MouseEvent;
class SdrHdlList;

namespace sd {

class View;
class SmartHdl;

/** a smart tag represents a visual user interface element on the documents edit view
    that is not part of the document. It uses derivations from SmartHdl for its visuals.
    A SmartTag adds himself to the given view if created. It removes himself if it
    is disposed before the view is disposed.

    Derive from this class to implement your own smart tag.
*/
class SmartTag : public SimpleReferenceComponent
{
    friend class SmartTagSet;

public:
    explicit SmartTag( ::sd::View& rView );
    virtual ~SmartTag();

    /** returns true if the SmartTag consumes this event. */
    virtual bool MouseButtonDown( const MouseEvent&, SmartHdl& );

    /** returns true if the SmartTag consumes this event. */
    virtual bool KeyInput( const KeyEvent& rKEvt );

    /** returns true if this smart tag is currently selected */
    bool isSelected() const;

    ::sd::View& getView() const { return mrView; }

protected:
    virtual ULONG GetMarkablePointCount() const;
    virtual ULONG GetMarkedPointCount() const;
    virtual BOOL MarkPoint(SdrHdl& rHdl, BOOL bUnmark=FALSE);
    virtual void CheckPossibilities();
    virtual BOOL MarkPoints(const Rectangle* pRect, BOOL bUnmark);

    virtual void addCustomHandles( SdrHdlList& rHandlerList );
    virtual void select();
    virtual void deselect();
    virtual bool getContext( SdrViewContext& rContext );

    virtual void disposing();

    ::sd::View& mrView;
    bool mbSelected;

private:
    SmartTag( const SmartTag& );                // not implemented
    SmartTag& operator=( const SmartTag& ); // not implemented
};

typedef rtl::Reference< SmartTag > SmartTagReference;

/** class to administrate the available smart tags for a single view. */
class SmartTagSet
{
    friend class SmartTag;
public:
    explicit SmartTagSet( ::sd::View& rView );
    ~SmartTagSet();

    /** selects the given smart tag and updates all handles */
    void select( const SmartTagReference& xTag );

    /** deselects the current selected smart tag and updates all handles */
    void deselect();

    /** returns the currently selected tag or an empty reference. */
    const SmartTagReference& getSelected() const { return mxSelectedTag; }

    /** returns true if a SmartTag consumes this event. */
    bool MouseButtonDown( const MouseEvent& );

    /** returns true if a SmartTag consumes this event. */
    bool KeyInput( const KeyEvent& rKEvt );

    /** disposes all smart tags and clears the set */
    void Dispose();

    /** addes the handles from all smart tags to the given list */
    void addCustomHandles( SdrHdlList& rHandlerList );

    /** returns true if the currently selected smart tag has
        a special context, returned in rContext. */
    bool getContext( SdrViewContext& rContext ) const;

    // support point editing
    BOOL HasMarkablePoints() const;
    ULONG GetMarkablePointCount() const;
    BOOL HasMarkedPoints() const;
    ULONG GetMarkedPointCount() const;
    BOOL IsPointMarkable(const SdrHdl& rHdl) const;
    BOOL MarkPoint(SdrHdl& rHdl, BOOL bUnmark=FALSE);
    BOOL MarkPoints(const Rectangle* pRect, BOOL bUnmark);

    void CheckPossibilities();

private:
    SmartTagSet( const SmartTagSet& );              // not implemented
    SmartTagSet& operator=( const SmartTagSet& );   // not implemented

    /** adds a new smart tag to this set */
    void add( const SmartTagReference& xTag );

    /** removes the given smart tag from this set */
    void remove( const SmartTagReference& xTag );

     std::set< SmartTagReference > maSet;

    ::sd::View& mrView;
    SmartTagReference mxSelectedTag;
};

/** a derivation from this handle is the visual representation for a smart tag.
    One smart tag can have more than one handle.
*/
class SmartHdl : public SdrHdl
{
public:
    SmartHdl( const SmartTagReference& xTag, SdrObject* pObject, const Point& rPnt, SdrHdlKind eNewKind=HDL_SMARTTAG );
    SmartHdl( const SmartTagReference& xTag, const Point& rPnt, SdrHdlKind eNewKind=HDL_SMARTTAG );

    const SmartTagReference& getTag() const { return mxTag; }

    virtual bool isMarkable() const;
protected:
    SmartTagReference mxTag;
};

} // end of namespace sd

#endif      // _SD_SMARTTAG_HXX_

