/*************************************************************************
 *
 *  $RCSfile: rsclex.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: hr $ $Date: 2004-09-08 15:34:46 $
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
#include <tools/stack.hxx>

#include <hash_set>
#include <rtl/strbuf.hxx>
#include <rtl/string.hxx>

// a buffer for unique strings
class StringContainer
{
    std::hash_set< rtl::OString, rtl::OStringHash >     m_aStrings;
public:
    StringContainer() {}
    ~StringContainer() {}

    const char* putString( const char* pString );
};


enum MODE_ENUM { MODE_MODELESS, MODE_APPLICATIONMODAL, MODE_SYSTEMMODAL };

enum JUSTIFY_ENUM { JUST_CENTER, JUST_RIGHT, JUST_LEFT };

enum SHOW_ENUM { SHOW_NORMAL, SHOW_MINIMIZED, SHOW_MAXIMIZED };

enum ENUMHEADER { HEADER_NAME, HEADER_NUMBER };

enum REF_ENUM { TYPE_NOTHING, TYPE_REF, TYPE_COPY };

struct RSCHEADER {
    RscTop *    pClass;
    RscExpType  nName1;
    REF_ENUM    nTyp;
    RscTop *    pRefClass;
    RscExpType  nName2;
};

/************** O b j e c t s t a c k ************************************/
struct Node {
    Node*   pPrev;
    RSCINST aInst;
    USHORT  nTupelRec;  // Rekursionstiefe fuer Tupel
    Node() { pPrev = NULL; nTupelRec = 0; };
};

class ObjectStack {
    private :
        Node* pRoot;
    public :

        ObjectStack ()   { pRoot = NULL; }

        const RSCINST & Top  ()     { return pRoot->aInst; }
        BOOL        IsEmpty()   { return( pRoot == NULL ); }
        void        IncTupelRec() { pRoot->nTupelRec++; }
        void        DecTupelRec() { pRoot->nTupelRec--; }
        USHORT      TupelRecCount() const { return pRoot->nTupelRec; }
        void        Push( RSCINST aInst )
                    {
                        Node* pTmp;

                        pTmp         = pRoot;
                        pRoot        = new Node;
                        pRoot->aInst = aInst;
                        pRoot->pPrev = pTmp;
                    }
        void        Pop()
                    {
                       Node* pTmp;

                       pTmp  = pRoot;
                       pRoot = pTmp->pPrev;
                       delete pTmp;
                    }
};

/****************** F o r w a r d s **************************************/
#if defined( RS6000 )
extern "C" int yyparse();   // forward Deklaration fuer erzeugte Funktion
extern "C" void yyerror( char * );
extern "C" int  yylex( void );
#elif defined( HP9000 ) || defined( SCO ) || defined ( IRIX ) || defined ( SOLARIS )
extern "C" int yyparse();   // forward Deklaration fuer erzeugte Funktion
extern "C" void yyerror( const char * );
extern "C" int  yylex( void );
#else
#if defined ( WTC ) || defined ( GCC )
int yyparse();              // forward Deklaration fuer erzeugte Funktion
#else
yyparse();              // forward Deklaration fuer erzeugte Funktion
#endif
void yyerror( char * );
int  yylex( void );
#endif

class RscTypCont;
class RscFileInst;

extern RscTypCont*              pTC;
extern RscFileInst *            pFI;
extern RscExpression *          pExp;
extern ObjectStack              S;
extern StringContainer*         pStringContainer;
