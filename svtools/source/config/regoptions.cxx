/*************************************************************************
 *
 *  $RCSfile: regoptions.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: fs $ $Date: 2001-12-20 13:11:37 $
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
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc..
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef SVTOOLS_REGOPTIONS_HXX
#include "regoptions.hxx"
#endif

#ifndef _UNOTOOLS_CONFIGNODE_HXX_
#include <unotools/confignode.hxx>
#endif
#ifndef _DATE_HXX
#include <tools/date.hxx>
#endif
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif

//........................................................................
namespace svt
{
//........................................................................

    using namespace ::utl;
    using namespace ::com::sun::star::uno;

    //====================================================================
    //= local helpers
    //====================================================================
    //--------------------------------------------------------------------
    /** converts a string representation of a date into a integer representation
        <p>No semantic check is made, i.e. if the string describes the 40.12, this is not recognized. In opposite,
        the string must have a valid structure (DD.MM.YYYY).</p>
        @return
            the integer representation of the date which can be used with the Date class,
            or 0 if the given string was no valid date representation
    */
    static sal_Int32 lcl_convertString2Date( const ::rtl::OUString& _rStringRep )
    {
        sal_Int32 nDateIntRep = 0;
        if ( _rStringRep.getLength() == 2 + 1 + 2 + 1 + 4 ) // DD.MM.YYYY
        {
            // validate the string
            sal_Bool bValid = sal_True;

            const sal_Unicode* pStringRep = _rStringRep.getStr();
            sal_Int32 nLen = _rStringRep.getLength();
            for (   sal_Int32 nPos = 0;
                    bValid && ( nPos < nLen );
                    ++nPos, ++pStringRep
                )
            {
                if ( ( 2 == nPos ) || ( 5 == nPos ) )
                    bValid = '.' == *pStringRep;        // the number separators
                else
                    bValid = ( *pStringRep >= '0' ) && ( *pStringRep <= '9' );
            }

            // passed the test?
            if ( bValid )
            {
                Date aDate;
                aDate.SetDay    ( (USHORT)_rStringRep.copy( 0, 2 ).toInt32( ) );
                aDate.SetMonth  ( (USHORT)_rStringRep.copy( 3, 2 ).toInt32( ) );
                aDate.SetYear   ( (USHORT)_rStringRep.copy( 6, 4 ).toInt32( ) );
                nDateIntRep = aDate.GetDate();
            }
        }

        return nDateIntRep;
    }

    //--------------------------------------------------------------------
    static const ::rtl::OUString& lcl_fillToken( const sal_Int32 /* [in] */ _nToken, const sal_Int16 /* [in] */ _nDigits, ::rtl::OUString& /* [out] */ _rToken )
    {
        // convert into string
        ::rtl::OUString sLeanToken = ::rtl::OUString::valueOf( _nToken );
        // check length
        if ( sLeanToken.getLength() < _nDigits )
        {   // fill
            OSL_ENSURE( _nDigits <= 4, "lcl_fillToken: invalid digit number!" );
            _rToken = ::rtl::OUString( "0000", _nDigits - sLeanToken.getLength(), RTL_TEXTENCODING_ASCII_US );
            _rToken += sLeanToken;
        }
        else
            _rToken = sLeanToken;

        return _rToken;
    }

    //--------------------------------------------------------------------
    /** converts a integer representation of a date into a string representation
    */
    static ::rtl::OUString lcl_ConvertDate2String( const Date& _rDate )
    {
        OSL_ENSURE( _rDate.IsValid(), "lcl_ConvertDate2String: invalid integer representation!" );

        sal_Unicode cSeparator( '.' );
        ::rtl::OUString sSeparator( &cSeparator, 1 );

        ::rtl::OUString sStringRep;
        ::rtl::OUString sToken;
        sStringRep += lcl_fillToken( (sal_Int32)_rDate.GetDay(), 2, sToken );
        sStringRep += sSeparator;
        sStringRep += lcl_fillToken( (sal_Int32)_rDate.GetMonth(), 2, sToken );
        sStringRep += sSeparator;
        sStringRep += lcl_fillToken( (sal_Int32)_rDate.GetYear(), 4, sToken );

        return sStringRep;
    }

    //--------------------------------------------------------------------
    /// checks whether a given trigger date is reached (i.e. is _before_ the current date)
    static sal_Bool lcl_reachedTriggerDate( const Date& _rTriggerDate )
    {
        return _rTriggerDate <= Date();
    }

    //--------------------------------------------------------------------
    #define DECLARE_STATIC_LAZY_USTRING( name ) \
    static const ::rtl::OUString lcl_get##name##Name()  \
    {   \
        static const ::rtl::OUString sName = ::rtl::OUString::createFromAscii( #name ); \
        return sName;   \
    }

    DECLARE_STATIC_LAZY_USTRING( ReminderDate );
    DECLARE_STATIC_LAZY_USTRING( RequestDialog );
    DECLARE_STATIC_LAZY_USTRING( ShowMenuItem );

    //====================================================================
    //= RegOptionsImpl
    //====================================================================
    class RegOptionsImpl
    {
    private:
        OConfigurationTreeRoot  m_aRegistrationNode;    // the configuration node we need to access our persistent data

        String                  m_sRegistrationURL;         // the URL to use when doing an online registration
        Date                    m_aReminderDate;            // the reminder date as found in the configuration
        sal_Int32               m_nDialogCounter;           // the dialog counter - see getDialogPermission
        sal_Bool                m_bShowMenuItem;            // the flag indicating if the registration menu item is allowed

        static  RegOptionsImpl* s_pSingleInstance;          // the one and only instance of this class
        static  sal_Int32       s_nInstanceCount;           // reference counter for the instances
        static  sal_Bool        s_bThisSessionDone;         // the flag indicating if for this session, everything beeing relevant has already been done

    private:
        RegOptionsImpl( );

        static  ::osl::Mutex&   getStaticMutex();           // get the mutex used to protect the static members of this class

        void                    commit( );

    private:
        RegOptions::DialogPermission    implGetDialogPermission( ) const;

    public:
        static RegOptionsImpl*  registerClient( );
        static void             revokeClient( );

        inline sal_Bool         hasURL( ) const             { return ( 0 != m_sRegistrationURL.Len() ); }
        inline sal_Bool         allowMenu( ) const          { return hasURL() && m_bShowMenuItem; }
        inline String           getRegistrationURL( ) const { return m_sRegistrationURL; }

        RegOptions::DialogPermission    getDialogPermission( ) const;
        void                            markSessionDone( );
        void                            activateReminder( sal_Int32 _nDaysFromNow );
    };

    //--------------------------------------------------------------------
    RegOptionsImpl* RegOptionsImpl::s_pSingleInstance = NULL;
    sal_Bool        RegOptionsImpl::s_bThisSessionDone = sal_False;
    sal_Int32       RegOptionsImpl::s_nInstanceCount = 0;

    //--------------------------------------------------------------------
    ::osl::Mutex& RegOptionsImpl::getStaticMutex()
    {
        static ::osl::Mutex* s_pStaticMutex = NULL;
        if ( !s_pStaticMutex )
        {
            ::osl::MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
            if ( !s_pStaticMutex )
            {
                static ::osl::Mutex s_aStaticMutex;
                s_pStaticMutex = &s_aStaticMutex;
            }
        }
        return *s_pStaticMutex;
    }

    //--------------------------------------------------------------------
    void RegOptionsImpl::commit( )
    {
        m_aRegistrationNode.commit( );
    }

    //--------------------------------------------------------------------
    RegOptionsImpl* RegOptionsImpl::registerClient( )
    {
        ::osl::MutexGuard aGuard( getStaticMutex() );

        if ( !s_pSingleInstance )
            s_pSingleInstance = new RegOptionsImpl;

        ++s_nInstanceCount;
        return s_pSingleInstance;
    }

    //--------------------------------------------------------------------
    void RegOptionsImpl::revokeClient( )
    {
        ::osl::MutexGuard aGuard( getStaticMutex() );
        OSL_ENSURE( s_nInstanceCount, "RegOptionsImpl::revokeClient: there are no clients alive!" );
        OSL_ENSURE( s_pSingleInstance || !s_nInstanceCount, "RegOptionsImpl::revokeClient: invalid instance pointer!" );

        if ( s_nInstanceCount )
        {
            if ( s_pSingleInstance )
                // commit the changes done by this client
                s_pSingleInstance->commit();

            if ( 0 == --s_nInstanceCount )
            {
                delete s_pSingleInstance;
                s_pSingleInstance = NULL;
            }
        }
    }

    //--------------------------------------------------------------------
    RegOptionsImpl::RegOptionsImpl( )
        :m_nDialogCounter   ( 0 )
        ,m_bShowMenuItem    ( sal_False )
    {
        // create the config node for all our registration information
        m_aRegistrationNode = OConfigurationTreeRoot::createWithServiceFactory(
            ::comphelper::getProcessServiceFactory(),
            ::rtl::OUString::createFromAscii( "/org.openoffice.Office.Common/Help/Registration" )
        );

        // cache some data
        //the URL to use for online registration
        ::rtl::OUString sStringValue;
        m_aRegistrationNode.getNodeValue( ::rtl::OUString::createFromAscii( "URL" ) ) >>= sStringValue;
        m_sRegistrationURL = sStringValue;

        // the state of the dialog
        m_aRegistrationNode.getNodeValue( lcl_getRequestDialogName() ) >>= m_nDialogCounter;

        // the flag for showing the menu item
        sal_Bool bBoolValue = sal_False;
        m_aRegistrationNode.getNodeValue( lcl_getShowMenuItemName() ) >>= bBoolValue;
        m_bShowMenuItem = bBoolValue;

        // the reminder date (if any)
        sal_Int32 nIntDate = 0;
        sStringValue = ::rtl::OUString();
        m_aRegistrationNode.getNodeValue( lcl_getReminderDateName() ) >>= sStringValue;
        if ( sStringValue.getLength() )
        {
            nIntDate = lcl_convertString2Date( sStringValue );
            OSL_ENSURE( nIntDate, "RegOptionsImpl::RegOptionsImpl: incorrect value found for the reminder date!" );
        }
        m_aReminderDate.SetDate( nIntDate );
        OSL_ENSURE( !sStringValue.getLength() || m_aReminderDate.IsValid(),
            "RegOptionsImpl::RegOptionsImpl: inavlid reminder date value!" );
    }

    //--------------------------------------------------------------------
    RegOptions::DialogPermission RegOptionsImpl::implGetDialogPermission( ) const
    {
        RegOptions::DialogPermission eResult = RegOptions::dpDisabled;
        // no URL or a counter already decreased to zero means the dialog is disabled
        if ( hasURL() && ( m_nDialogCounter > 0 ) )
        {
            // during every session, the counter will be decreased
            // If it reaches zero, the dialog shall be executed
            if ( 1 == m_nDialogCounter )
            {
                if ( m_aReminderDate.IsValid( ) )
                {   // valid reminder date
                    // assume remind later
                    eResult = RegOptions::dpRemindLater;
                    // and check if we reached the reminder date
                    if ( lcl_reachedTriggerDate( m_aReminderDate ) )
                        eResult = RegOptions::dpThisSession;
                }
                else
                    eResult = RegOptions::dpThisSession;    // execute in this session
            }
            else
                eResult = RegOptions::dpNotThisSession;     // first trigger session not reached

            // a last check ....
            if  (   ( s_bThisSessionDone )                  // this session is already marked as "done"
                &&  ( RegOptions::dpThisSession == eResult )// but without this, the state would be "now"
                )
                eResult = RegOptions::dpDisabled;           // -> change state to "disabled"
        }

        return eResult;
    }

    //--------------------------------------------------------------------
    RegOptions::DialogPermission RegOptionsImpl::getDialogPermission( ) const
    {
        OSL_ENSURE( !s_bThisSessionDone, "RegOptionsImpl::getDialogPermission: should never be asked in this session, again!" );
            // Somebody already marked this session as "everything relevant happened". So why sombody (else?) asks
            // me again?

        return implGetDialogPermission( );
    }

    //--------------------------------------------------------------------
    void RegOptionsImpl::activateReminder( sal_Int32 _nDaysFromNow )
    {
        OSL_ENSURE( s_bThisSessionDone ||  ( implGetDialogPermission( ) != RegOptions::dpDisabled ), "RegOptionsImpl::activateReminder: invalid call!" );
        OSL_ENSURE( _nDaysFromNow > 0, "RegOptionsImpl::activateReminder: invalid argument!" );

        // calc the reminder
        m_aReminderDate = Date() + _nDaysFromNow;   // today (default ctor) + days

        // remember the date
        m_aRegistrationNode.setNodeValue(
            lcl_getReminderDateName(),
            makeAny( lcl_ConvertDate2String( m_aReminderDate ) )
        );
        // to be on the save side, write the counter
        m_aRegistrationNode.setNodeValue(
            lcl_getRequestDialogName(),
            makeAny( (sal_Int32)1 )
        );

        // mark this session as done
        if ( !s_bThisSessionDone )
            markSessionDone( );
    }

    //--------------------------------------------------------------------
    void RegOptionsImpl::markSessionDone( )
    {
        OSL_ENSURE( !s_bThisSessionDone, "RegOptionsImpl::markSessionDone: already marked!" );
        OSL_ENSURE( implGetDialogPermission( ) != RegOptions::dpDisabled, "RegOptionsImpl::markSessionDone: invalid call!" );
        if ( !s_bThisSessionDone )
        {
            RegOptions::DialogPermission eOldPermission = implGetDialogPermission( );

            s_bThisSessionDone = sal_True;

            if ( RegOptions::dpRemindLater == eOldPermission )
            {   // no action required. If we shall remind later, the counter is already at 1, we should not change this,
                // as the next smaller number (which is 0 :) means that the dialog would be disabled
                OSL_ENSURE( 1 == m_nDialogCounter, "RegOptionsImpl::markSessionDone: invalid session counter (1)!" );
            }
            else
            {
                OSL_ENSURE( m_nDialogCounter > 0, "RegOptionsImpl::markSessionDone: invalid session counter (2)!" );
                --m_nDialogCounter;

                // decrease the session counter
                m_aRegistrationNode.setNodeValue(
                    lcl_getRequestDialogName(),
                    makeAny( (sal_Int32)m_nDialogCounter )
                );

                // and clear the reminder date
                m_aRegistrationNode.setNodeValue(
                    lcl_getReminderDateName(),
                    Any()
                );
            }
        }
    }

    //====================================================================
    //= RegOptions
    //====================================================================
    //--------------------------------------------------------------------
    RegOptions::RegOptions()
        :m_pImpl( NULL )
    {
    }

    //--------------------------------------------------------------------
    void RegOptions::ensureImpl( )
    {
        if ( !m_pImpl )
            m_pImpl = RegOptionsImpl::registerClient();
    }

    //--------------------------------------------------------------------
    RegOptions::~RegOptions()
    {
        if ( m_pImpl )
        {
            RegOptionsImpl::revokeClient();
            m_pImpl = NULL;
        }
    }

    //--------------------------------------------------------------------
    String RegOptions::getRegistrationURL( ) const
    {
        const_cast< RegOptions* >( this )->ensureImpl( );
        return m_pImpl->getRegistrationURL();
    }

    //--------------------------------------------------------------------
    RegOptions::DialogPermission RegOptions::getDialogPermission( ) const
    {
        const_cast< RegOptions* >( this )->ensureImpl( );
        return m_pImpl->getDialogPermission();
    }

    //--------------------------------------------------------------------
    void RegOptions::markSessionDone( )
    {
        const_cast< RegOptions* >( this )->ensureImpl( );
        m_pImpl->markSessionDone();
    }

    //--------------------------------------------------------------------
    void RegOptions::activateReminder( sal_Int32 _nDaysFromNow )
    {
        const_cast< RegOptions* >( this )->ensureImpl( );
        m_pImpl->activateReminder( _nDaysFromNow );
    }

    //--------------------------------------------------------------------
    sal_Bool RegOptions::allowMenu( ) const
    {
        /// we cache this setting, 'cause it is needed very often
        static sal_Bool bKnowMenuPermission = sal_False;
        static sal_Bool nAllowMenu          = sal_False;

        if ( !bKnowMenuPermission )
        {
            const_cast< RegOptions* >( this )->ensureImpl( );
            nAllowMenu = m_pImpl->allowMenu();
            bKnowMenuPermission = sal_True;
        }
        return nAllowMenu;
    }

//........................................................................
}   // namespace svt
//........................................................................

/*************************************************************************
 * history:
 *  $Log: not supported by cvs2svn $
 *
 *  Revision 1.0 19.12.01 13:48:37  fs
 ************************************************************************/

