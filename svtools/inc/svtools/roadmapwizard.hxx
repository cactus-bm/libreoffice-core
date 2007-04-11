/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: roadmapwizard.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: vg $ $Date: 2007-04-11 19:33:11 $
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

#ifndef SVTOOLS_INC_ROADMAPWIZARD_HXX
#define SVTOOLS_INC_ROADMAPWIZARD_HXX

#ifndef INCLUDED_SVTDLLAPI_H
#include "svtools/svtdllapi.h"
#endif

#ifndef _SVTOOLS_WIZARDMACHINE_HXX_
#include <svtools/wizardmachine.hxx>
#endif

//........................................................................
namespace svt
{
//........................................................................

    struct RoadmapWizardImpl;

    struct RoadmapWizardTypes
    {
    public:
        typedef sal_Int16  PathId;
    };
    //====================================================================
    //= RoadmapWizard
    //====================================================================
    /** is - no, not a wizard for a raodmap, but the base class for wizards
        <em>supporting</em> a roadmap.

        The basic new concept introduced is a <em>path</em>:<br/>
        A <em>path</em> is a sequence of states, which are to be executed in a linear order.
        Elements in the path can be skipped, depending on choices the user makes.

        In the most simple wizards, you will have only one path consisting of <code>n</code> elements,
        which are to be visited successively.

        In a slightly more complex wizard, you will have one linear path, were certain
        steps might be skipped due to user input. For instance, the user may decide to not specify
        certain aspects of the to-be-created object (e.g. by unchecking a check box),
        and the wizard then will simply disable the step which corresponds to this step.

        In a yet more advanced wizards, you will have several paths of length <code>n1</code> and
        <code>n2</code>, which share at least the first <code>k</code> states (where <code>k</code>
        is at least 1), and an arbitrary number of other states.
    */
    class SVT_DLLPUBLIC RoadmapWizard : public OWizardMachine, public RoadmapWizardTypes
    {
    private:
        RoadmapWizardImpl*  m_pImpl;

    public:
        RoadmapWizard( Window* _pParent, const ResId& _rRes, sal_uInt32 _nButtonFlags,
            const ResId& _rRoadmapTitleResource, sal_Bool _bCheckButtonStates = sal_False );
        ~RoadmapWizard( );

        void            SetRoadmapBitmap( const BitmapEx& _rBitmap );
        const BitmapEx& GetRoadmapBitmap( ) const;

        void            SetRoadmapInteractive( sal_Bool _bInteractive );
        sal_Bool        IsRoadmapInteractive();
        virtual void    Resize();
        virtual void    StateChanged( StateChangedType nStateChange );
        /** returns current state
         */
        bool    isStateEnabled( WizardState _nState );

    protected:
        /** declares a valid path in the wizard

            The very first path which is declared is automatically activated.

            Note that all paths which are declared must have the very first state in
            common. Also note that due to a restriction of the very base class (<type>WizardDialog</type>),
            this common first state must be 0.

            You cannot declare new paths once the wizard started, so it's recommended that
            you do all declarations within your derivee's constructor.

            @see activatePath

            @param _nId
                the unique id you wish to give this path. This id can later on be used
                to refer to the path which you just declared

            @param _nFirstState
                the first state in this path. Must not be WZS_INVALID_STATE.<br/>
                Declare an arbitrary number of states after this one, and terminate the sequence
                with a WZS_INVALID_STATE.
        */
        void    declarePath( PathId _nPathId, WizardState _nFirstState, ... );

        /** activates a path which has previously been declared with <member>declarePath</member>

            You can only activate paths which share the first <code>k</code> states with the path
            which is previously active (if any), where <code>k</code> is the index of the
            current state within the current page.

            <example>
            Say you have paths, <code>(0,1,2,5)</code> and <code>(0,1,4,5)</code>. This means that after
            step <code>1</code>, you either continue with state <code>2</code> or state <code>4</code>,
            and after this, you finish in state <code>5</code>.<br/>
            Now if the first path is active, and your current state is <code>1</code>, then you can
            easily switch to the second path, since both paths start with <code>(0,1)</code>.<br/>
            However, if your current state is <code>2</code>, then you can not switch to the second
            path anymore.
            </example>

            @param _nPathId
                the id of the path. The path must have been declared (under this id) with
                <member>declarePath</member> before it can be activated.

            @param _bDecideForIt
                If <TRUE/>, the path will be completely activated, even if it is a conflicting path
                (i.e. there is another path which shares the first <code>k</code> states with
                the to-be-activated path.)<br/>
                If <FALSE/>, then the new path is checked for conflicts with other paths. If such
                conflicts exists, the path is not completely activated, but only up to the point
                where it does <em>not</em> conflict.<br/>
                With the paths in the example above, if you activate the second path (when both are
                already declared), then only steps <code>0</code> and <code>1</code> are activated,
                since they are common to both paths.
        */
        void    activatePath( PathId _nPathId, bool _bDecideForIt = false );

        /** determine the next state to travel from the given one

            This method (which is declared in <type>OWizardMachine</type> and overwritten here)
            ensures that traveling happens along the active path.

            @see activatePath
        */
        virtual WizardState     determineNextState( WizardState _nCurrentState );

        /** en- or disables a state

            In the wizard's roadmap, states to travel to can be freely chosen. To prevent
            users from selecting a state which is currently not available, you can declare this
            state as being disabled.

            A situation where you need this may be when you have a checkbox which, when checked
            by the user, enables a page with additional settings. As long as this checkbox is
            not checked, the respective state would be disabled.

            Note that in theory, you can declare multiple paths, instead of disabling states.
            For instance, if you have a path where one state can be potentially disabled, then
            you could declare a second path, which does not contain this state. However, the
            disadvantage is that then, not the complete path would be visible in the roadmap,
            but only all steps up to the point where the both paths diverge.<br/>
            Another disadvantage is that the number of needed paths grows exponentially with
            the number of states which can be potentially disabled.

            @see declarePath
        */
        void    enableState( WizardState _nState, bool _bEnable = true );

        // OWizardMachine overriables
        virtual void            enterState( WizardState _nState );

        /** returns a human readable name for a given state
            @pure
        */
        virtual String  getStateDisplayName( WizardState _nState ) = 0;

        /** asks for a new label of the wizard page

         */
        void    updateRoadmapItemLabel( WizardState _nState );

    protected:
#if OSL_DEBUG_LEVEL > 0
        const sal_Char* checkInvariants() const;
        friend const char* CheckInvariants( const void* pVoid );
#endif

    private:
        SVT_DLLPRIVATE void ResizeFixedLine();

    private:
        DECL_DLLPRIVATE_LINK( OnRoadmapItemSelected, void* );

        /** inserts an entry for the given state into the roadmap, at the given index

            This method automatically cares for the enabled/disabled state of the item,
            and for its text.
        */
        SVT_DLLPRIVATE void implInsertState( WizardState _nState, sal_Int32 _nIndex );

        /** updates the roadmap control to show the given path, as far as possible
            (modulo conflicts with other paths)
        */
        SVT_DLLPRIVATE void implUpdateRoadmap( );
    };

//........................................................................
} // namespace svt
//........................................................................

#endif // OOO_SVTOOLS_INC_ROADMAPWIZARD_HXX
