/*************************************************************************
 *
 *  $RCSfile: ResultSetMetaData.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: hr $ $Date: 2004-11-09 12:14:06 $
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

#ifndef _CONNECTIVITY_JAVA_SQL_RESULTSETMETADATA_HXX_
#include "java/sql/ResultSetMetaData.hxx"
#endif
#ifndef _CONNECTIVITY_JAVA_TOOLS_HXX_
#include "java/tools.hxx"
#endif
using namespace connectivity;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
//  using namespace ::com::sun::star::sdbcx;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::lang;
//**************************************************************
//************ Class: java.sql.ResultSetMetaData
//**************************************************************

jclass java_sql_ResultSetMetaData::theClass = 0;
java_sql_ResultSetMetaData::java_sql_ResultSetMetaData( JNIEnv * pEnv, jobject myObj )
: java_lang_Object( pEnv, myObj )
{
    SDBThreadAttach::addRef();
}
java_sql_ResultSetMetaData::~java_sql_ResultSetMetaData()
{
    SDBThreadAttach::releaseRef();
}

jclass java_sql_ResultSetMetaData::getMyClass()
{
    // die Klasse muss nur einmal geholt werden, daher statisch
    if( !theClass ){
        SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment gel�scht worden!");
        if( !t.pEnv ) return (jclass)0;
        jclass tempClass = t.pEnv->FindClass("java/sql/ResultSetMetaData"); OSL_ENSURE(tempClass,"Java : FindClass nicht erfolgreich!");
        jclass globClass = (jclass)t.pEnv->NewGlobalRef( tempClass );
        t.pEnv->DeleteLocalRef( tempClass );
        saveClassRef( globClass );
    }
    return theClass;
}
// -------------------------------------------------------------------------

void java_sql_ResultSetMetaData::saveClassRef( jclass pClass )
{
    if( pClass==0  )
        return;
    // der uebergebe Klassen-Handle ist schon global, daher einfach speichern
    theClass = pClass;
}
// -------------------------------------------------------------------------

sal_Int32 SAL_CALL java_sql_ResultSetMetaData::getColumnDisplaySize( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    jint out(0);
    SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment gel�scht worden!");
    if( t.pEnv ){

        // temporaere Variable initialisieren
        static char * cSignature = "(I)I";
        static char * cMethodName = "getColumnDisplaySize";
        // Java-Call absetzen
        static jmethodID mID = NULL;
        if ( !mID  )
            mID  = t.pEnv->GetMethodID( getMyClass(), cMethodName, cSignature );OSL_ENSURE(mID,"Unknown method id!");
        if( mID ){
            out = t.pEnv->CallIntMethod( object, mID,column);
            ThrowSQLException(t.pEnv,*this);
        } //mID
    } //t.pEnv
    return (sal_Int32)out;
}
// -------------------------------------------------------------------------

sal_Int32 SAL_CALL java_sql_ResultSetMetaData::getColumnType( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    jint out(0);
    SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment gel�scht worden!");
    if( t.pEnv ){

        // temporaere Variable initialisieren
        static char * cSignature = "(I)I";
        static char * cMethodName = "getColumnType";
        // Java-Call absetzen
        static jmethodID mID = NULL;
        if ( !mID  )
            mID  = t.pEnv->GetMethodID( getMyClass(), cMethodName, cSignature );OSL_ENSURE(mID,"Unknown method id!");
        if( mID ){
            out = t.pEnv->CallIntMethod( object, mID,column);
            ThrowSQLException(t.pEnv,*this);
        } //mID
    } //t.pEnv
    return (sal_Int32)out;
}
// -------------------------------------------------------------------------

sal_Int32 SAL_CALL java_sql_ResultSetMetaData::getColumnCount(  ) throw(SQLException, RuntimeException)
{
    jint out(0);
    SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment gel�scht worden!");
    if( t.pEnv ){

        // temporaere Variable initialisieren
        static char * cSignature = "()I";
        static char * cMethodName = "getColumnCount";
        // Java-Call absetzen
        static jmethodID mID = NULL;
        if ( !mID  )
            mID  = t.pEnv->GetMethodID( getMyClass(), cMethodName, cSignature );OSL_ENSURE(mID,"Unknown method id!");
        if( mID ){
            out = t.pEnv->CallIntMethod( object, mID);
            ThrowSQLException(t.pEnv,*this);
        } //mID
    } //t.pEnv
    return (sal_Int32)out;
}
// -------------------------------------------------------------------------

sal_Bool SAL_CALL java_sql_ResultSetMetaData::isCaseSensitive( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    jboolean out(sal_False);
    SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment gel�scht worden!");
    if( t.pEnv ){
        // temporaere Variable initialisieren
        static char * cSignature = "(I)Z";
        static char * cMethodName = "isCaseSensitive";
        // Java-Call absetzen
        static jmethodID mID = NULL;
        if ( !mID  )
            mID  = t.pEnv->GetMethodID( getMyClass(), cMethodName, cSignature );OSL_ENSURE(mID,"Unknown method id!");
        if( mID )
            out = t.pEnv->CallBooleanMethod( object, mID, column );
            ThrowSQLException(t.pEnv,*this);
            // und aufraeumen
    } //t.pEnv
    return out;
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL java_sql_ResultSetMetaData::getSchemaName( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment gel�scht worden!");
    ::rtl::OUString aStr;
    if( t.pEnv ){
        // temporaere Variable initialisieren
        static char * cSignature = "(I)Ljava/lang/String;";
        static char * cMethodName = "getSchemaName";
        // Java-Call absetzen
        static jmethodID mID = NULL;
        if ( !mID  )
            mID  = t.pEnv->GetMethodID( getMyClass(), cMethodName, cSignature );OSL_ENSURE(mID,"Unknown method id!");
        if( mID )
        {
            jstring out = (jstring)t.pEnv->CallObjectMethod( object, mID, column );
            ThrowSQLException(t.pEnv,*this);
            aStr = JavaString2String(t.pEnv,out);
        }

            // und aufraeumen
    } //t.pEnv
    return aStr;
}
// -------------------------------------------------------------------------

::rtl::OUString SAL_CALL java_sql_ResultSetMetaData::getColumnName( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment gel�scht worden!");
    ::rtl::OUString aStr;
    if( t.pEnv ){
        // temporaere Variable initialisieren
        static char * cSignature = "(I)Ljava/lang/String;";
        static char * cMethodName = "getColumnName";
        // Java-Call absetzen
        static jmethodID mID = NULL;
        if ( !mID  )
            mID  = t.pEnv->GetMethodID( getMyClass(), cMethodName, cSignature );OSL_ENSURE(mID,"Unknown method id!");
        if( mID )
        {
            jstring out = (jstring)t.pEnv->CallObjectMethod( object, mID, column );
            ThrowSQLException(t.pEnv,*this);
            aStr = JavaString2String(t.pEnv,out);
        }

            // und aufraeumen
    } //t.pEnv
    return aStr;
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL java_sql_ResultSetMetaData::getTableName( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment gel�scht worden!");
    ::rtl::OUString aStr;
    if( t.pEnv ){
        // temporaere Variable initialisieren
        static char * cSignature = "(I)Ljava/lang/String;";
        static char * cMethodName = "getTableName";
        // Java-Call absetzen
        static jmethodID mID = NULL;
        if ( !mID  )
            mID  = t.pEnv->GetMethodID( getMyClass(), cMethodName, cSignature );OSL_ENSURE(mID,"Unknown method id!");
        if( mID )
        {
            jstring out = (jstring)t.pEnv->CallObjectMethod( object, mID, column );
            ThrowSQLException(t.pEnv,*this);
            aStr = JavaString2String(t.pEnv,out);
        }

            // und aufraeumen
    } //t.pEnv
    return aStr;
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL java_sql_ResultSetMetaData::getCatalogName( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment gel�scht worden!");
    ::rtl::OUString aStr;
    if( t.pEnv ){
        // temporaere Variable initialisieren
        static char * cSignature = "(I)Ljava/lang/String;";
        static char * cMethodName = "getCatalogName";
        // Java-Call absetzen
        static jmethodID mID = NULL;
        if ( !mID  )
            mID  = t.pEnv->GetMethodID( getMyClass(), cMethodName, cSignature );OSL_ENSURE(mID,"Unknown method id!");
        if( mID )
        {
            jstring out = (jstring)t.pEnv->CallObjectMethod( object, mID, column );
            ThrowSQLException(t.pEnv,*this);
            aStr = JavaString2String(t.pEnv,out);
        }

            // und aufraeumen
    } //t.pEnv
    return aStr;
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL java_sql_ResultSetMetaData::getColumnTypeName( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment gel�scht worden!");
    ::rtl::OUString aStr;
    if( t.pEnv ){
        // temporaere Variable initialisieren
        static char * cSignature = "(I)Ljava/lang/String;";
        static char * cMethodName = "getColumnTypeName";
        // Java-Call absetzen
        static jmethodID mID = NULL;
        if ( !mID  )
            mID  = t.pEnv->GetMethodID( getMyClass(), cMethodName, cSignature );OSL_ENSURE(mID,"Unknown method id!");
        if( mID )
        {
            jstring out = (jstring)t.pEnv->CallObjectMethod( object, mID, column );
            ThrowSQLException(t.pEnv,*this);
            aStr = JavaString2String(t.pEnv,out);
        }

            // und aufraeumen
    } //t.pEnv
    return aStr;
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL java_sql_ResultSetMetaData::getColumnLabel( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment gel�scht worden!");
    ::rtl::OUString aStr;
    if( t.pEnv ){
        // temporaere Variable initialisieren
        static char * cSignature = "(I)Ljava/lang/String;";
        static char * cMethodName = "getColumnLabel";
        // Java-Call absetzen
        static jmethodID mID = NULL;
        if ( !mID  )
            mID  = t.pEnv->GetMethodID( getMyClass(), cMethodName, cSignature );OSL_ENSURE(mID,"Unknown method id!");
        if( mID )
        {
            jstring out = (jstring)t.pEnv->CallObjectMethod( object, mID, column );
            ThrowSQLException(t.pEnv,*this);
            aStr = JavaString2String(t.pEnv,out);
        }

            // und aufraeumen
    } //t.pEnv
    return aStr;
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL java_sql_ResultSetMetaData::getColumnServiceName( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment gel�scht worden!");
    ::rtl::OUString aStr;
    if( t.pEnv ){
        // temporaere Variable initialisieren
        static char * cSignature = "(I)Ljava/lang/String;";
        static char * cMethodName = "getColumnClassName";
        // Java-Call absetzen
        static jmethodID mID = NULL;
        if ( !mID  )
            mID  = t.pEnv->GetMethodID( getMyClass(), cMethodName, cSignature );OSL_ENSURE(mID,"Unknown method id!");
        if( mID )
        {
            jstring out = (jstring)t.pEnv->CallObjectMethod( object, mID, column );
            ThrowSQLException(t.pEnv,*this);
            aStr = JavaString2String(t.pEnv,out);
        }

            // und aufraeumen
    } //t.pEnv
    return aStr;
}
// -------------------------------------------------------------------------

sal_Bool SAL_CALL java_sql_ResultSetMetaData::isCurrency( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    jboolean out(sal_False);
    SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment gel�scht worden!");
    if( t.pEnv ){
        // temporaere Variable initialisieren
        static char * cSignature = "(I)Z";
        static char * cMethodName = "isCurrency";
        // Java-Call absetzen
        static jmethodID mID = NULL;
        if ( !mID  )
            mID  = t.pEnv->GetMethodID( getMyClass(), cMethodName, cSignature );OSL_ENSURE(mID,"Unknown method id!");
        if( mID )
            out = t.pEnv->CallBooleanMethod( object, mID, column );
            ThrowSQLException(t.pEnv,*this);
            // und aufraeumen
    } //t.pEnv
    return out;
}
// -------------------------------------------------------------------------

sal_Bool SAL_CALL java_sql_ResultSetMetaData::isAutoIncrement( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    jboolean out(sal_False);
    SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment gel�scht worden!");
    if( t.pEnv ){
        // temporaere Variable initialisieren
        static char * cSignature = "(I)Z";
        static char * cMethodName = "isAutoIncrement";
        // Java-Call absetzen
        static jmethodID mID = NULL;
        if ( !mID  )
            mID  = t.pEnv->GetMethodID( getMyClass(), cMethodName, cSignature );OSL_ENSURE(mID,"Unknown method id!");
        if( mID )
            out = t.pEnv->CallBooleanMethod( object, mID, column);
            ThrowSQLException(t.pEnv,*this);
            // und aufraeumen
    } //t.pEnv
    return out;
}
// -------------------------------------------------------------------------


sal_Bool SAL_CALL java_sql_ResultSetMetaData::isSigned( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    jboolean out(sal_False);
    SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment gel�scht worden!");
    if( t.pEnv ){
        // temporaere Variable initialisieren
        static char * cSignature = "(I)Z";
        static char * cMethodName = "isSigned";
        // Java-Call absetzen
        static jmethodID mID = NULL;
        if ( !mID  )
            mID  = t.pEnv->GetMethodID( getMyClass(), cMethodName, cSignature );OSL_ENSURE(mID,"Unknown method id!");
        if( mID )
            out = t.pEnv->CallBooleanMethod( object, mID, column);
            ThrowSQLException(t.pEnv,*this);
            // und aufraeumen
    } //t.pEnv
    return out;
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL java_sql_ResultSetMetaData::getPrecision( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    jint out(0);
    SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment gel�scht worden!");
    if( t.pEnv ){
        // temporaere Variable initialisieren
        static char * cSignature = "(I)I";
        static char * cMethodName = "getPrecision";
        // Java-Call absetzen
        static jmethodID mID = NULL;
        if ( !mID  )
            mID  = t.pEnv->GetMethodID( getMyClass(), cMethodName, cSignature );OSL_ENSURE(mID,"Unknown method id!");
        if( mID ){
            out = t.pEnv->CallIntMethod( object, mID, column );
            ThrowSQLException(t.pEnv,*this);
            // und aufraeumen
        } //mID
    } //t.pEnv
    return (sal_Int32)out;
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL java_sql_ResultSetMetaData::getScale( sal_Int32 column ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException)
{
    jint out(0);
    SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment gel�scht worden!");
    if( t.pEnv ){
        // temporaere Variable initialisieren
        static char * cSignature = "(I)I";
        static char * cMethodName = "getScale";
        // Java-Call absetzen
        static jmethodID mID = NULL;
        if ( !mID  )
            mID  = t.pEnv->GetMethodID( getMyClass(), cMethodName, cSignature );OSL_ENSURE(mID,"Unknown method id!");
        if( mID ){
            out = t.pEnv->CallIntMethod( object, mID, column );
            ThrowSQLException(t.pEnv,*this);
            // und aufraeumen
        } //mID
    } //t.pEnv
    return (sal_Int32)out;
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL java_sql_ResultSetMetaData::isNullable( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    jint out(0);
    SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment gel�scht worden!");
    if( t.pEnv ){
        // temporaere Variable initialisieren
        static char * cSignature = "(I)I";
        static char * cMethodName = "isNullable";
        // Java-Call absetzen
        static jmethodID mID = NULL;
        if ( !mID  )
            mID  = t.pEnv->GetMethodID( getMyClass(), cMethodName, cSignature );OSL_ENSURE(mID,"Unknown method id!");
        if( mID ){
            out = t.pEnv->CallIntMethod( object, mID, column );
            ThrowSQLException(t.pEnv,*this);
            // und aufraeumen
        } //mID
    } //t.pEnv
    return (sal_Int32)out;
}
// -------------------------------------------------------------------------

sal_Bool SAL_CALL java_sql_ResultSetMetaData::isSearchable( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    jboolean out(sal_False);
    SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment gel�scht worden!");
    if( t.pEnv ){
        // temporaere Variable initialisieren
        static char * cSignature = "(I)Z";
        static char * cMethodName = "isSearchable";
        // Java-Call absetzen
        static jmethodID mID = NULL;
        if ( !mID  )
            mID  = t.pEnv->GetMethodID( getMyClass(), cMethodName, cSignature );OSL_ENSURE(mID,"Unknown method id!");
        if( mID ){
            out = t.pEnv->CallBooleanMethod( object, mID, column );
            ThrowSQLException(t.pEnv,*this);
            // und aufraeumen
        } //mID
    } //t.pEnv
    return out;
}
// -------------------------------------------------------------------------

sal_Bool SAL_CALL java_sql_ResultSetMetaData::isReadOnly( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    jboolean out(sal_False);
    SDBThreadAttach t;
    if( t.pEnv ){
        // temporaere Variable initialisieren
        static char * cSignature = "(I)Z";
        static char * cMethodName = "isReadOnly";
        // Java-Call absetzen
        static jmethodID mID = NULL;
        if ( !mID  )
            mID  = t.pEnv->GetMethodID( getMyClass(), cMethodName, cSignature );OSL_ENSURE(mID,"Unknown method id!");
        if( mID ){
            out = t.pEnv->CallBooleanMethod( object, mID, column );
            ThrowSQLException(t.pEnv,*this);
            // und aufraeumen
        } //mID
    } //t.pEnv
    return out;
}
// -------------------------------------------------------------------------

sal_Bool SAL_CALL java_sql_ResultSetMetaData::isDefinitelyWritable( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    jboolean out(sal_False);
    SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment gel�scht worden!");
    if( t.pEnv ){
        // temporaere Variable initialisieren
        static char * cSignature = "(I)Z";
        static char * cMethodName = "isDefinitelyWritable";
        // Java-Call absetzen
        static jmethodID mID = NULL;
        if ( !mID  )
            mID  = t.pEnv->GetMethodID( getMyClass(), cMethodName, cSignature );OSL_ENSURE(mID,"Unknown method id!");
        if( mID ){
            out = t.pEnv->CallBooleanMethod( object, mID, column );
            ThrowSQLException(t.pEnv,*this);
            // und aufraeumen
        } //mID
    } //t.pEnv
    return out;
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_ResultSetMetaData::isWritable( sal_Int32 column ) throw(SQLException, RuntimeException)
{
    jboolean out(sal_False);
    SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment gel�scht worden!");
    if( t.pEnv ){
        // temporaere Variable initialisieren
        static char * cSignature = "(I)Z";
        static char * cMethodName = "isWritable";
        // Java-Call absetzen
        static jmethodID mID = NULL;
        if ( !mID  )
            mID  = t.pEnv->GetMethodID( getMyClass(), cMethodName, cSignature );OSL_ENSURE(mID,"Unknown method id!");
        if( mID ){
            out = t.pEnv->CallBooleanMethod( object, mID, column );
            ThrowSQLException(t.pEnv,*this);
            // und aufraeumen
        } //mID
    } //t.pEnv
    return out;
}
// -------------------------------------------------------------------------

