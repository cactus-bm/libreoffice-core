/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: Timestamp.cxx,v $
 *
 *  $Revision: 1.14 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-20 01:37:27 $
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

#ifndef _CONNECTIVITY_JAVA_SQL_TIMESTAMP_HXX_
#include "java/sql/Timestamp.hxx"
#endif
#ifndef _CONNECTIVITY_JAVA_TOOLS_HXX_
#include "java/tools.hxx"
#endif
#ifndef _COMPHELPER_TYPES_HXX_
#include <comphelper/types.hxx>
#endif
#ifndef _DBHELPER_DBCONVERSION_HXX_
#include "connectivity/dbconversion.hxx"
#endif

using namespace ::comphelper;
using namespace connectivity;
//**************************************************************
//************ Class: java.sql.Date
//**************************************************************
const double fMilliSecondsPerDay = 86400000.0;
jclass java_sql_Date::theClass = 0;
java_sql_Date::java_sql_Date( const ::com::sun::star::util::Date& _rOut ) : java_util_Date( NULL, (jobject)NULL )
{
    SDBThreadAttach t;
    if( !t.pEnv )
        return;
    jvalue args[1];
    // Parameter konvertieren
    ::rtl::OUString sDateStr;
    sDateStr = ::dbtools::DBTypeConversion::toDateString(_rOut);
    args[0].l = convertwchar_tToJavaString(t.pEnv,sDateStr);

    // Java-Call fuer den Konstruktor absetzen
    // temporaere Variable initialisieren
    static const char * cSignature = "(Ljava/lang/String;)Ljava/sql/Date;";
    jobject tempObj;
    static jmethodID mID = NULL;
    if ( !mID  )
        mID  = t.pEnv->GetStaticMethodID( getMyClass(), "valueOf", cSignature );OSL_ENSURE(mID,"Unknown method id!");
    tempObj = t.pEnv->CallStaticObjectMethod( getMyClass(), mID, args[0].l );
    saveRef( t.pEnv, tempObj );
    t.pEnv->DeleteLocalRef( tempObj );
    // und aufraeumen
}

java_sql_Date::~java_sql_Date()
{}

jclass java_sql_Date::getMyClass()
{
    // die Klasse muss nur einmal geholt werden, daher statisch
    if( !theClass ){
        SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment geloescht worden!");
        if( !t.pEnv ) return (jclass)0;
        jclass tempClass = t.pEnv->FindClass("java/sql/Date"); OSL_ENSURE(tempClass,"Java : FindClass nicht erfolgreich!");
        jclass globClass = (jclass)t.pEnv->NewGlobalRef( tempClass );
        t.pEnv->DeleteLocalRef( tempClass );
        saveClassRef( globClass );
    }
    return theClass;
}
// -----------------------------------------------------------------------------

void java_sql_Date::saveClassRef( jclass pClass )
{
    if( pClass==0  )
        return;
    // der uebergebe Klassen-Handle ist schon global, daher einfach speichern
    theClass = pClass;
}
// -----------------------------------------------------------------------------
java_sql_Date::operator ::com::sun::star::util::Date()
{
    return ::dbtools::DBTypeConversion::toDate(toString());
}

//**************************************************************
//************ Class: java.sql.Time
//**************************************************************

jclass java_sql_Time::theClass = 0;

java_sql_Time::~java_sql_Time()
{}

jclass java_sql_Time::getMyClass()
{
    // die Klasse muss nur einmal geholt werden, daher statisch
    if( !theClass ){
        SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment geloescht worden!");
        if( !t.pEnv ) return (jclass)0;
        jclass tempClass = t.pEnv->FindClass("java/sql/Time"); OSL_ENSURE(tempClass,"Java : FindClass nicht erfolgreich!");
        jclass globClass = (jclass)t.pEnv->NewGlobalRef( tempClass );
        t.pEnv->DeleteLocalRef( tempClass );
        saveClassRef( globClass );
    }
    return theClass;
}

void java_sql_Time::saveClassRef( jclass pClass )
{
    if( pClass==0  )
        return;
    // der uebergebe Klassen-Handle ist schon global, daher einfach speichern
    theClass = pClass;
}

java_sql_Time::java_sql_Time( const ::com::sun::star::util::Time& _rOut ): java_util_Date( NULL, (jobject)NULL )
{
    SDBThreadAttach t;
    if( !t.pEnv )
        return;
    jvalue args[1];
    // Parameter konvertieren
    ::rtl::OUString sDateStr;
    sDateStr = ::dbtools::DBTypeConversion::toTimeString(_rOut);
    args[0].l = convertwchar_tToJavaString(t.pEnv,sDateStr);

    // Java-Call fuer den Konstruktor absetzen
    // temporaere Variable initialisieren
    static const char * cSignature = "(Ljava/lang/String;)Ljava/sql/Time;";
    jobject tempObj;
    static jmethodID mID = NULL;
    if ( !mID  )
        mID  = t.pEnv->GetStaticMethodID( getMyClass(), "valueOf", cSignature );OSL_ENSURE(mID,"Unknown method id!");
    tempObj = t.pEnv->CallStaticObjectMethod( getMyClass(), mID, args[0].l );
    t.pEnv->DeleteLocalRef((jstring)args[0].l);
    saveRef( t.pEnv, tempObj );
    t.pEnv->DeleteLocalRef( tempObj );
    // und aufraeumen
}
// -----------------------------------------------------------------------------
java_sql_Time::operator ::com::sun::star::util::Time()
{
    return ::dbtools::DBTypeConversion::toTime(toString());
}
//**************************************************************
//************ Class: java.sql.Timestamp
//**************************************************************

jclass java_sql_Timestamp::theClass = 0;

java_sql_Timestamp::~java_sql_Timestamp()
{}

jclass java_sql_Timestamp::getMyClass()
{
    // die Klasse muss nur einmal geholt werden, daher statisch
    if( !theClass ){
        SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment geloescht worden!");
        if( !t.pEnv ) return (jclass)0;
        jclass tempClass = t.pEnv->FindClass("java/sql/Timestamp"); OSL_ENSURE(tempClass,"Java : FindClass nicht erfolgreich!");
        jclass globClass = (jclass)t.pEnv->NewGlobalRef( tempClass );
        t.pEnv->DeleteLocalRef( tempClass );
        saveClassRef( globClass );
    }
    return theClass;
}

void java_sql_Timestamp::saveClassRef( jclass pClass )
{
    if( pClass==0  )
        return;
    // der uebergebe Klassen-Handle ist schon global, daher einfach speichern
    theClass = pClass;
}

java_sql_Timestamp::java_sql_Timestamp(const ::com::sun::star::util::DateTime& _rOut)
                   :java_util_Date( NULL, (jobject)NULL )
{
        SDBThreadAttach t;
    if( !t.pEnv )
        return;
    jvalue args[1];
    // Parameter konvertieren
    ::rtl::OUString sDateStr;
    sDateStr = ::dbtools::DBTypeConversion::toDateTimeString(_rOut);
    if ( _rOut.HundredthSeconds )
    {
        sDateStr += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("."));
        sDateStr += ::rtl::OUString::valueOf(static_cast<sal_Int32>(_rOut.HundredthSeconds));
    }

    args[0].l = convertwchar_tToJavaString(t.pEnv,sDateStr);

    // Java-Call fuer den Konstruktor absetzen
    // temporaere Variable initialisieren
    static const char * cSignature = "(Ljava/lang/String;)Ljava/sql/Timestamp;";
    jobject tempObj;
    static jmethodID mID = NULL;
    if ( !mID  )
        mID  = t.pEnv->GetStaticMethodID( getMyClass(), "valueOf", cSignature );OSL_ENSURE(mID,"Unknown method id!");
    tempObj = t.pEnv->CallStaticObjectMethod( getMyClass(), mID, args[0].l );

    saveRef( t.pEnv, tempObj );
    t.pEnv->DeleteLocalRef( tempObj );
    // und aufraeumen
}

sal_Int32 java_sql_Timestamp::getNanos()
{
    jint out(0);
    SDBThreadAttach t;
    if( t.pEnv ){

        // temporaere Variable initialisieren
        static const char * cSignature = "()I";
        static const char * cMethodName = "getNanos";
        // Java-Call absetzen
        static jmethodID mID = NULL;
        if ( !mID  )
            mID  = t.pEnv->GetMethodID( getMyClass(), cMethodName, cSignature );OSL_ENSURE(mID,"Unknown method id!");
        if( mID ){
            out = t.pEnv->CallIntMethod( object, mID);
        } //mID
    } //t.pEnv
    return (sal_Int32)out;
}

void java_sql_Timestamp::setNanos( sal_Int32 _par0 )
{
    SDBThreadAttach t;
    if( t.pEnv ){
        // temporaere Variable initialisieren
        static const char * cSignature = "(I)V";
        static const char * cMethodName = "setNanos";
        // Java-Call absetzen
        static jmethodID mID = NULL;
        if ( !mID  )
            mID  = t.pEnv->GetMethodID( getMyClass(), cMethodName, cSignature );OSL_ENSURE(mID,"Unknown method id!");
        if( mID ){
            t.pEnv->CallVoidMethod( object, mID, _par0 );
            // und aufraeumen
        } //mID
    } //t.pEnv
}
// -----------------------------------------------------------------------------
java_sql_Timestamp::operator ::com::sun::star::util::DateTime()
{
    return ::dbtools::DBTypeConversion::toDateTime(toString());
}
// -----------------------------------------------------------------------------


