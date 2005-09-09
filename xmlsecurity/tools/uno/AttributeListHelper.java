/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: AttributeListHelper.java,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 17:42:47 $
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

package com.sun.star.xml.security.uno;

import java.util.Vector;
import com.sun.star.xml.sax.XAttributeList;

/**
 * Class to construct an attribute list, and provide a XAttributeList
 * interface.
 *
 * @author      Michael Mi
 * @version     %I%, %G%
 */
public class AttributeListHelper implements com.sun.star.xml.sax.XAttributeList
{
    private Vector m_AttributeList;

    public AttributeListHelper()
    {
        m_AttributeList = new Vector();
    }

    public void clear()
    {
        m_AttributeList.removeAllElements();
    }

    public void setAttribute(String name, String type, String value)
    {
        int nLength = m_AttributeList.size();
        boolean bFound = false;

        for (int i=0; i<nLength; ++i)
        {
            if (getNameByIndex((short)i).equals(name))
            {
                Vector attribute = (Vector)m_AttributeList.get(i);
                attribute.setElementAt(type,1);
                attribute.setElementAt(value,2);
                bFound = true;
                break;
            }
        }

        if (!bFound)
        {
            Vector attribute = new Vector();
            attribute.addElement(name);
            attribute.addElement(type);
            attribute.addElement(value);
            m_AttributeList.addElement(attribute);
        }
    }

    public String getAttributeItem(short index, int itemIndex)
    {
        String item = null;

        if (index>=0 && index<getLength())
        {
            Vector attribute = (Vector)m_AttributeList.get(index);
            item = (String)(attribute.get(itemIndex));
        }

        return item;
    }

    /* XAttributeList */
    public short getLength()
    {
        return (short)m_AttributeList.size();
    }

    public String getNameByIndex(short i)
    {
        return getAttributeItem(i, 0);
    }

    public String getTypeByIndex(short i)
    {
        return getAttributeItem(i, 1);
    }

    public String getValueByIndex(short i)
    {
        return getAttributeItem(i, 2);
    }

    public String getTypeByName(String aName)
    {
        int nLength = m_AttributeList.size();
        String type = null;

        for (int i=0; i<nLength; ++i)
        {
            if (getNameByIndex((short)i).equals(aName))
            {
                type = getTypeByIndex((short)i);
                break;
            }
        }

        return type;
    }

    public String getValueByName(String aName)
    {
        int nLength = m_AttributeList.size();
        String value = null;

        for (int i=0; i<nLength; ++i)
        {
            if (getNameByIndex((short)i).equals(aName))
            {
                value = getValueByIndex((short)i);
                break;
            }
        }
        return value;
    }
}

