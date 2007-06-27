/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: parser.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 22:21:10 $
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
#ifndef _PARSER_HXX
#define _PARSER_HXX

#if defined WNT

#include <tools/string.hxx>
#include <winshell.hxx>

void * NewBinaryFromString( const String & rBinStr );
String CreateStringFromData( const void *pData, ULONG nBytes );

String CreateStringFromItemIDList( const CItemIDList & rIDList );

String GetURLFromHostNotation( const String & rPath );
String GetHostNotationFromURL( const String & rURL );

CItemIDList MakeIDToken( const String &rToken );
CItemIDList ParseSpecialURL( const String & rURL );

#endif

#endif // _PARSER_HXX
