/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: propmultiplex.hxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 02:37:07 $
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

#ifndef _COMPHELPER_PROPERTY_MULTIPLEX_HXX_
#define _COMPHELPER_PROPERTY_MULTIPLEX_HXX_

#ifndef _COMPHELPER_PROPERTY_STATE_HXX_
#include <comphelper/propstate.hxx>
#endif
#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>
#endif
#ifndef INCLUDED_COMPHELPERDLLAPI_H
#include "comphelper/comphelperdllapi.h"
#endif

//=========================================================================
//= property helper classes
//=========================================================================

//.........................................................................
namespace comphelper
{
//.........................................................................

    class OPropertyChangeMultiplexer;

    //==================================================================
    //= OPropertyChangeListener
    //==================================================================
    /// simple listener adapter for property sets
    class COMPHELPER_DLLPUBLIC OPropertyChangeListener
    {
        friend class OPropertyChangeMultiplexer;

        OPropertyChangeMultiplexer* m_pAdapter;
        ::osl::Mutex&               m_rMutex;

    public:
        OPropertyChangeListener(::osl::Mutex& _rMutex)
            : m_pAdapter(NULL), m_rMutex(_rMutex) { }
        virtual ~OPropertyChangeListener();

        virtual void _propertyChanged(const ::com::sun::star::beans::PropertyChangeEvent& _rEvent) throw( ::com::sun::star::uno::RuntimeException) = 0;
        virtual void _disposing(const ::com::sun::star::lang::EventObject& _rSource) throw( ::com::sun::star::uno::RuntimeException);

    protected:
        /** If the derivee also owns the mutex which we know as reference, then call this within your
            derivee's dtor.
        */
        void    disposeAdapter();

        // pseudo-private. Making it private now could break compatibility
        void    setAdapter( OPropertyChangeMultiplexer* _pAdapter );
    };

    //==================================================================
    //= OPropertyChangeMultiplexer
    //==================================================================
    /// multiplexer for property changes
    class COMPHELPER_DLLPUBLIC OPropertyChangeMultiplexer   :public cppu::WeakImplHelper1< ::com::sun::star::beans::XPropertyChangeListener>
    {
        friend class OPropertyChangeListener;
         ::com::sun::star::uno::Sequence< ::rtl::OUString >     m_aProperties;
         ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>   m_xSet;
        OPropertyChangeListener*                    m_pListener;
        sal_Int32                                   m_nLockCount;
        sal_Bool                                    m_bListening        : 1;
        sal_Bool                                    m_bAutoSetRelease   : 1;


        virtual ~OPropertyChangeMultiplexer();
    public:
        OPropertyChangeMultiplexer(OPropertyChangeListener* _pListener, const  ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>& _rxSet, sal_Bool _bAutoReleaseSet = sal_True);

    // XEventListener
        virtual void SAL_CALL disposing( const  ::com::sun::star::lang::EventObject& Source ) throw( ::com::sun::star::uno::RuntimeException);

    // XPropertyChangeListener
        virtual void SAL_CALL propertyChange( const  ::com::sun::star::beans::PropertyChangeEvent& evt ) throw( ::com::sun::star::uno::RuntimeException);

        /// incremental lock
        void        lock();
        /// incremental unlock
        void        unlock();
        /// get the lock count
        sal_Int32   locked() const { return m_nLockCount; }

        void addProperty(const ::rtl::OUString& aPropertyName);
        void dispose();
    };

//.........................................................................
}   // namespace comphelper
//.........................................................................

#endif // _COMPHELPER_PROPERTY_MULTIPLEX_HXX_

