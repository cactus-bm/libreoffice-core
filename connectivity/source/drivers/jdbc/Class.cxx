/*************************************************************************
 *
 *  $RCSfile: Class.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: oj $ $Date: 2001-05-04 09:58:43 $
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
#ifndef _CONNECTIVITY_JAVA_LANG_CLASS_HXX_
#include "java/lang/Class.hxx"
#endif
#ifndef _CONNECTIVITY_JAVA_TOOLS_HXX_
#include "java/tools.hxx"
#endif
#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

using namespace connectivity;
//**************************************************************
//************ Class: java.lang.Class
//**************************************************************

jclass java_lang_Class::theClass = 0;

java_lang_Class::~java_lang_Class()
{}

jclass java_lang_Class::getMyClass()
{
    // die Klasse muss nur einmal geholt werden, daher statisch
    if( !theClass ){
        SDBThreadAttach t;
        if( !t.pEnv ) return (jclass)NULL;
        jclass tempClass = t.pEnv->FindClass("java/lang/Class"); OSL_ENSURE(tempClass,"Java : FindClass nicht erfolgreich!");
        jclass globClass = (jclass)t.pEnv->NewGlobalRef( tempClass );
        t.pEnv->DeleteLocalRef( tempClass );
        saveClassRef( globClass );
    }
    return theClass;
}

void java_lang_Class::saveClassRef( jclass pClass )
{
    if( SDBThreadAttach::IsJavaErrorOccured() || pClass==NULL  )
        return;
    // der uebergebe Klassen-Handle ist schon global, daher einfach speichern
    theClass = pClass;
}

java_lang_Class * java_lang_Class::forName( const ::rtl::OUString& _par0 )
{
    jobject out(NULL);
    SDBThreadAttach t;
    if( t.pEnv ){
        jvalue args[1];
        // Parameter konvertieren
        args[0].l = convertwchar_tToJavaString(t.pEnv,_par0);
        // temporaere Variable initialisieren
        char * cSignature = "(Ljava/lang/String;)Ljava/lang/Class;";
        char * cMethodName = "forName";
        // Java-Call absetzen
        jmethodID mID = t.pEnv->GetStaticMethodID( getMyClass(), cMethodName, cSignature );
        if( mID ){
            out = t.pEnv->CallStaticObjectMethod( getMyClass(), mID, args[0].l );
            if(!out)
                out = t.pEnv->FindClass(::rtl::OUStringToOString(_par0, RTL_TEXTENCODING_ASCII_US));
            ThrowSQLException(t.pEnv,0);
            // und aufraeumen
        } //mID
        t.pEnv->DeleteLocalRef((jstring)args[0].l);
    } //t.pEnv
    // ACHTUNG: der Aufrufer wird Eigentuemer des zurueckgelieferten Zeigers !!!
    return out==0 ? NULL : new java_lang_Class( t.pEnv, out );
}

sal_Bool java_lang_Class::isAssignableFrom( java_lang_Class * _par0 )
{
    jboolean out;
    SDBThreadAttach t;
    if( t.pEnv ){
        jvalue args[1];
        // Parameter konvertieren
        args[0].l = _par0 != NULL ? ((java_lang_Object *)_par0)->getJavaObject() : NULL;
        // temporaere Variable initialisieren
        char * cSignature = "(Ljava/lang/Class;)Z";
        char * cMethodName = "isAssignableFrom";
        // Java-Call absetzen
        jmethodID mID = t.pEnv->GetMethodID( getMyClass(), cMethodName, cSignature );
        if( mID ){
            out = t.pEnv->CallBooleanMethod( object, mID, args[0].l );
            ThrowSQLException(t.pEnv,0);
            // und aufraeumen
        } //mID
    } //t.pEnv
    return out;
}

java_lang_Object * java_lang_Class::newInstance()
{
    jobject out(NULL);
    SDBThreadAttach t;
    if( t.pEnv ){
        // temporaere Variable initialisieren
        char * cSignature = "()Ljava/lang/Object;";
        char * cMethodName = "newInstance";
        // Java-Call absetzen
        jmethodID mID = t.pEnv->GetMethodID( getMyClass(), cMethodName, cSignature );
        if( mID ){
            out = t.pEnv->CallObjectMethod( object, mID);
            ThrowSQLException(t.pEnv,0);
        } //mID
    } //t.pEnv
    // ACHTUNG: der Aufrufer wird Eigentuemer des zurueckgelieferten Zeigers !!!
    return out==0 ? NULL : new java_lang_Object( t.pEnv, out );
}

jobject java_lang_Class::newInstanceObject()
{
    jobject out(NULL);
    SDBThreadAttach t;
    if( t.pEnv )
    {
        // temporaere Variable initialisieren
        char * cSignature = "()Ljava/lang/Object;";
        char * cMethodName = "newInstance";
        // Java-Call absetzen
        jmethodID mID = t.pEnv->GetMethodID( getMyClass(), cMethodName, cSignature );
        if( mID ){
            out = t.pEnv->CallObjectMethod( object, mID);
            ThrowSQLException(t.pEnv,0);
        } //mID
    } //t.pEnv
    // ACHTUNG: der Aufrufer wird Eigentuemer des zurueckgelieferten Zeigers !!!
    return out;
}

::rtl::OUString java_lang_Class::getName()
{
    jstring out = (jstring)NULL;
    SDBThreadAttach t;
    ::rtl::OUString aStr;
    if( t.pEnv ){
        // temporaere Variable initialisieren
        char * cSignature = "()Ljava/lang/String;";
        char * cMethodName = "getName";
        // Java-Call absetzen
        jmethodID mID = t.pEnv->GetMethodID( getMyClass(), cMethodName, cSignature );
        if( mID ){
            out = (jstring)t.pEnv->CallObjectMethod( object, mID);
            ThrowSQLException(t.pEnv,0);
            aStr = JavaString2String(t.pEnv, (jstring)out );
        } //mID
    } //t.pEnv
    // ACHTUNG: der Aufrufer wird Eigentuemer des zurueckgelieferten Zeigers !!!
    return aStr;
}

