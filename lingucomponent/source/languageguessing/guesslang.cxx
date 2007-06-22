/*************************************************************************
 *
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: guesslang.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2007-06-22 08:32:42 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_lingucomponent.hxx"

#include <iostream>

#include <tools/debug.hxx>

#include <sal/config.h>
#include <cppuhelper/factory.hxx>
#include <cppuhelper/implementationentry.hxx>
#include <cppuhelper/implbase2.hxx>
#include <tools/string.hxx>

#include <simpleguesser.hxx>
#include <guess.hxx>

//#include <cppuhelper/queryinterface.hxx> // helper for queryInterface() impl

//#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/registry/XRegistryKey.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/linguistic2/XLanguageGuessing.hpp>

#ifndef INCLUDED_SVTOOLS_PATHOPTIONS_HXX
#include <svtools/pathoptions.hxx>
#endif
#ifndef _UNOTOOLS_LOCALFILEHELPER_HXX
#include <unotools/localfilehelper.hxx>
#endif
#ifndef _OSL_THREAD_H_
#include <osl/thread.h>
#endif

using namespace ::rtl;
using namespace ::osl;
using namespace ::cppu;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::linguistic2;

namespace css = ::com::sun::star;

//==================================================================================================

#define SERVICENAME     "com.sun.star.linguistic2.LanguageGuessing"

#define IMPLNAME        "com.sun.star.lingu2.LanguageGuessing"

static Sequence< OUString > getSupportedServiceNames_LangGuess_Impl()
{
    Sequence<OUString> names(1);
    names[0] = OUString(RTL_CONSTASCII_USTRINGPARAM(SERVICENAME));
    return names;
}

static OUString getImplementationName_LangGuess_Impl()
{
    return OUString( RTL_CONSTASCII_USTRINGPARAM(IMPLNAME) );
}

static osl::Mutex &  GetLangGuessMutex()
{
    static osl::Mutex aMutex;
    return aMutex;
}


class LangGuess_Impl :
    public ::cppu::WeakImplHelper2<
        XLanguageGuessing,
        XServiceInfo >
{
    SimpleGuesser guesser;
    css::uno::Reference< css::uno::XComponentContext >  m_xContext;

    LangGuess_Impl(const LangGuess_Impl &); // not defined
    void operator =(const LangGuess_Impl &); // not defined
    virtual ~LangGuess_Impl() {}

public:
    explicit LangGuess_Impl(css::uno::Reference< css::uno::XComponentContext > const & context);

    // XServiceInfo implementation
    virtual OUString SAL_CALL getImplementationName(  ) throw(RuntimeException);
    virtual sal_Bool SAL_CALL supportsService( const OUString& ServiceName ) throw(RuntimeException);
    virtual Sequence< OUString > SAL_CALL getSupportedServiceNames(  ) throw(RuntimeException);
    static Sequence< OUString > SAL_CALL getSupportedServiceNames_Static(  );

    // XLanguageGuessing implementation
    virtual ::com::sun::star::lang::Locale SAL_CALL guessPrimaryLanguage( const ::rtl::OUString& aText, ::sal_Int32 nStartPos, ::sal_Int32 nLen ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL disableLanguages( const ::com::sun::star::uno::Sequence< ::com::sun::star::lang::Locale >& aLanguages ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL enableLanguages( const ::com::sun::star::uno::Sequence< ::com::sun::star::lang::Locale >& aLanguages ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::lang::Locale > SAL_CALL getAvailableLanguages(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::lang::Locale > SAL_CALL getEnabledLanguages(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::lang::Locale > SAL_CALL getDisabledLanguages(  ) throw (::com::sun::star::uno::RuntimeException);

    // implementation specific
    void SetFingerPrintsDB( const rtl::OUString &fileName ) throw (RuntimeException);

    static const OUString & SAL_CALL getImplementationName_Static() throw();

};

//*************************************************************************

LangGuess_Impl::LangGuess_Impl(css::uno::Reference< css::uno::XComponentContext > const & context) :
    m_xContext(context)
{
    // set default fingerprint path to where those get installed
    String aPhysPath;
    String aURL( SvtPathOptions().GetFingerprintPath() );
    utl::LocalFileHelper::ConvertURLToPhysicalName( aURL, aPhysPath );
#ifdef WNT
        aPhysPath += '\\';
#else
        aPhysPath += '/';
#endif

    SetFingerPrintsDB( aPhysPath );

    //
    // disable currently not functional languages...
    //
    struct LangCountry
    {
        const char *pLang;
        const char *pCountry;
    };
    LangCountry aDisable[] =
    {
        {"gv", ""}, {"sco", ""}, {"tl", ""},            // not lang-id available yet...
        {"hy", ""}, {"drt", ""},                        // 0 bytes fingerprints...
        {"zh", "CN"}, {"zh", "TW"}, {"ja", ""}, {"ko", ""},   // not yet correct functional...
        {"ka", ""}, {"hi", ""}, {"mr", ""}, {"ne", ""},
        {"sa", ""}, {"ta", ""}, {"tl", ""}, {"th", ""},
        {"qu", ""}, {"yi", ""}
    };
    sal_Int32 nNum = sizeof(aDisable) / sizeof(aDisable[0]);
    Sequence< Locale > aDisableSeq( nNum );
    Locale *pDisableSeq = aDisableSeq.getArray();
    for (sal_Int32 i = 0;  i < nNum;  ++i)
    {
        Locale aLocale;
        aLocale.Language = OUString::createFromAscii( aDisable[i].pLang );
        aLocale.Country  = OUString::createFromAscii( aDisable[i].pCountry );
        pDisableSeq[i] = aLocale;
    }
    disableLanguages( aDisableSeq );
    DBG_ASSERT( nNum == getDisabledLanguages().getLength(), "size mismatch" );
}

//*************************************************************************

/* TL: currently not part of the API
Sequence< com::sun::star::lang::Locale > SAL_CALL LangGuess_Impl::guessLanguages(
        const rtl::OUString   &rText,
        sal_Int32       nStartPos,
        sal_Int32       nLen )
    throw (RuntimeException)
{
    Sequence< com::sun::star::lang::Locale > aRes;

    OString o = OUStringToOString( rText, RTL_TEXTENCODING_UTF8 );
    vector<Guess> gs = guesser.GuessLanguage(o.pData->buffer);

    aRes.realloc(gs.size());

    com::sun::star::lang::Locale *pRes = aRes.getArray();

#ifdef DEBUG
    std::cout << " We have " << gs.size() << " candidates" << std::endl;
#endif

    for(int i = 0; i < gs.size() ; i++ ){
        com::sun::star::lang::Locale current_aRes;

        current_aRes.Language   = OUString(RTL_CONSTASCII_USTRINGPARAM(gs[i].getLanguage().c_str()));
        current_aRes.Country    = OUString(RTL_CONSTASCII_USTRINGPARAM(gs[i].getCountry().c_str()));

        pRes[i] = current_aRes;
    }

    return aRes;
}
*/
//*************************************************************************
Locale SAL_CALL LangGuess_Impl::guessPrimaryLanguage(
        const ::rtl::OUString& rText,
        ::sal_Int32 nStartPos,
        ::sal_Int32 nLen )
    throw (lang::IllegalArgumentException, uno::RuntimeException)
{
    osl::MutexGuard aGuard( GetLangGuessMutex() );

    lang::Locale aRes;
    if (nStartPos >=0 && nLen >= 0 && nStartPos + nLen <= rText.getLength())
    {
        OString o( OUStringToOString( rText.copy(nStartPos, nLen), RTL_TEXTENCODING_UTF8 ) );
        Guess g = guesser.GuessPrimaryLanguage((char*)o.getStr());
        aRes.Language   = OUString::createFromAscii(g.GetLanguage().c_str());
        aRes.Country    = OUString::createFromAscii(g.GetCountry().c_str());
    }
    else
        throw lang::IllegalArgumentException();

    return aRes;
}

//*************************************************************************
#define DEFAULT_CONF_FILE_NAME "fpdb.conf"

void LangGuess_Impl::SetFingerPrintsDB(
        const rtl::OUString &filePath )
    throw (RuntimeException)
{
    //! text encoding for file name / path needs to be in the same encoding the OS uses
    OString path = OUStringToOString( filePath, osl_getThreadTextEncoding() );
    OString conf_file_name( DEFAULT_CONF_FILE_NAME );
    OString conf_file_path(path);
    conf_file_path += conf_file_name;

    //cout << "Conf file : " << conf_file_path.getStr() << " directory : " << path.getStr() << endl;

    guesser.SetDBPath((const char*)conf_file_path.getStr(), (const char*)path.getStr());
}

//*************************************************************************
uno::Sequence< Locale > SAL_CALL LangGuess_Impl::getAvailableLanguages(  )
        throw (uno::RuntimeException)
{
    osl::MutexGuard aGuard( GetLangGuessMutex() );

    Sequence< com::sun::star::lang::Locale > aRes;
    vector<Guess> gs = guesser.GetAllManagedLanguages();
    aRes.realloc(gs.size());

    com::sun::star::lang::Locale *pRes = aRes.getArray();

    for(size_t i = 0; i < gs.size() ; i++ ){
        com::sun::star::lang::Locale current_aRes;
        current_aRes.Language   = OUString(RTL_CONSTASCII_USTRINGPARAM(gs[i].GetLanguage().c_str()));
        current_aRes.Country    = OUString(RTL_CONSTASCII_USTRINGPARAM(gs[i].GetCountry().c_str()));
        pRes[i] = current_aRes;
    }

    return aRes;
}

//*************************************************************************
uno::Sequence< Locale > SAL_CALL LangGuess_Impl::getEnabledLanguages(  )
        throw (uno::RuntimeException)
{
    osl::MutexGuard aGuard( GetLangGuessMutex() );

    Sequence< com::sun::star::lang::Locale > aRes;
    vector<Guess> gs = guesser.GetAvailableLanguages();
    aRes.realloc(gs.size());

    com::sun::star::lang::Locale *pRes = aRes.getArray();

    for(size_t i = 0; i < gs.size() ; i++ ){
        com::sun::star::lang::Locale current_aRes;
        current_aRes.Language   = OUString(RTL_CONSTASCII_USTRINGPARAM(gs[i].GetLanguage().c_str()));
        current_aRes.Country    = OUString(RTL_CONSTASCII_USTRINGPARAM(gs[i].GetCountry().c_str()));
        pRes[i] = current_aRes;
    }

    return aRes;
}

//*************************************************************************
uno::Sequence< Locale > SAL_CALL LangGuess_Impl::getDisabledLanguages(  )
        throw (uno::RuntimeException)
{
    osl::MutexGuard aGuard( GetLangGuessMutex() );

    Sequence< com::sun::star::lang::Locale > aRes;
    vector<Guess> gs = guesser.GetUnavailableLanguages();
    aRes.realloc(gs.size());

    com::sun::star::lang::Locale *pRes = aRes.getArray();

    for(size_t i = 0; i < gs.size() ; i++ ){
        com::sun::star::lang::Locale current_aRes;
        current_aRes.Language   = OUString(RTL_CONSTASCII_USTRINGPARAM(gs[i].GetLanguage().c_str()));
        current_aRes.Country    = OUString(RTL_CONSTASCII_USTRINGPARAM(gs[i].GetCountry().c_str()));
        pRes[i] = current_aRes;
    }

    return aRes;
}

//*************************************************************************
void SAL_CALL LangGuess_Impl::disableLanguages(
        const uno::Sequence< Locale >& rLanguages )
    throw (lang::IllegalArgumentException, uno::RuntimeException)
{
    osl::MutexGuard aGuard( GetLangGuessMutex() );

    sal_Int32 nLanguages = rLanguages.getLength();
    const Locale *pLanguages = rLanguages.getConstArray();

    for (sal_Int32 i = 0;  i < nLanguages;  ++i)
    {
        string language;

        OString l = OUStringToOString( pLanguages[i].Language, RTL_TEXTENCODING_ASCII_US );
        OString c = OUStringToOString( pLanguages[i].Country, RTL_TEXTENCODING_ASCII_US );

        language += l.getStr();
        language += "-";
        language += c.getStr();
        guesser.DisableLanguage(language);
    }
}

//*************************************************************************
void SAL_CALL LangGuess_Impl::enableLanguages(
        const uno::Sequence< Locale >& rLanguages )
    throw (lang::IllegalArgumentException, uno::RuntimeException)
{
    osl::MutexGuard aGuard( GetLangGuessMutex() );

    sal_Int32 nLanguages = rLanguages.getLength();
    const Locale *pLanguages = rLanguages.getConstArray();

    for (sal_Int32 i = 0;  i < nLanguages;  ++i)
    {
        string language;

        OString l = OUStringToOString( pLanguages[i].Language, RTL_TEXTENCODING_ASCII_US );
        OString c = OUStringToOString( pLanguages[i].Country, RTL_TEXTENCODING_ASCII_US );

        language += l.getStr();
        language += "-";
        language += c.getStr();
        guesser.EnableLanguage(language);
    }
}

//*************************************************************************
OUString SAL_CALL LangGuess_Impl::getImplementationName(  )
    throw(RuntimeException)
{
    osl::MutexGuard aGuard( GetLangGuessMutex() );
    return OUString( RTL_CONSTASCII_USTRINGPARAM(IMPLNAME) );
}

//*************************************************************************
sal_Bool SAL_CALL LangGuess_Impl::supportsService( const OUString& ServiceName )
    throw(RuntimeException)
{
    osl::MutexGuard aGuard( GetLangGuessMutex() );
    Sequence< OUString > aSNL = getSupportedServiceNames();
    const OUString * pArray = aSNL.getArray();
    for( sal_Int32 i = 0; i < aSNL.getLength(); i++ )
        if( pArray[i] == ServiceName )
            return sal_True;
    return sal_False;
}

//*************************************************************************
Sequence<OUString> SAL_CALL LangGuess_Impl::getSupportedServiceNames(  )
    throw(RuntimeException)
{
    osl::MutexGuard aGuard( GetLangGuessMutex() );
    return getSupportedServiceNames_Static();
}

//*************************************************************************
Sequence<OUString> SAL_CALL LangGuess_Impl::getSupportedServiceNames_Static(  )
{
    OUString aName( RTL_CONSTASCII_USTRINGPARAM(SERVICENAME) );
    return Sequence< OUString >( &aName, 1 );
}

//*************************************************************************


/**
 * Function to create a new component instance; is needed by factory helper implementation.
 * @param xMgr service manager to if the components needs other component instances
 */
Reference< XInterface > SAL_CALL LangGuess_Impl_create(
    Reference< XComponentContext > const & xContext )
    SAL_THROW( () )
{
    return static_cast< ::cppu::OWeakObject * >( new LangGuess_Impl(xContext) );
}

//##################################################################################################
//#### EXPORTED ### functions to allow for registration and creation of the UNO component
//##################################################################################################

static struct ::cppu::ImplementationEntry s_component_entries [] =
{
    {
        LangGuess_Impl_create, getImplementationName_LangGuess_Impl,
        getSupportedServiceNames_LangGuess_Impl,
        ::cppu::createSingleComponentFactory,
        0, 0
    },
    { 0, 0, 0, 0, 0, 0 }
};

extern "C"
{

void SAL_CALL component_getImplementationEnvironment(
    sal_Char const ** ppEnvTypeName, uno_Environment ** /*ppEnv*/ )
{
    *ppEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
}

sal_Bool SAL_CALL component_writeInfo(
    XMultiServiceFactory * xMgr, registry::XRegistryKey * xRegistry )
{
    return ::cppu::component_writeInfoHelper(
        xMgr, xRegistry, s_component_entries );
}

void * SAL_CALL component_getFactory(
    sal_Char const * implName, lang::XMultiServiceFactory * xMgr,
    registry::XRegistryKey * xRegistry )
{
    return ::cppu::component_getFactoryHelper(
        implName, xMgr, xRegistry, s_component_entries );
}

}

