/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: xlat.c,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 16:42:41 $
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

/* $Id: xlat.c,v 1.3 2005-09-08 16:42:41 rt Exp $
 *
 * Data translation from Unicode to MS encodings
 * If the host system provides this functionality
 * this file should be rewritten to use it and
 * the large translation arrays should be removed
 *
 * Author: Alexander Gelfenbain
 *
  */

#include "xlat.h"

#include "u2big5.inc"
#include "u2johab.inc"
#include "u2prc.inc"
#include "u2shiftjis.inc"
#include "u2wansung.inc"

#define MISSING_CODE 0

static sal_uInt16 xlat(sal_uInt16pair p[], sal_uInt32 n, sal_uInt16 src)
{
    int l = 0, r = n - 1, i;
    sal_uInt16 t, res = MISSING_CODE;

    do {
        i = (l + r) >> 1;
        t = p[i].s;
        if (src >= t) l = i + 1;
        if (src <= t) r = i - 1;
    } while (l <= r);

    if (l - r == 2) {
        res =  p[l-1].d;
    }

    return res;
}

sal_uInt16 TranslateChar12(sal_uInt16 src)
{
    return xlat(xlat_1_2, sizeof(xlat_1_2) / sizeof(xlat_1_2[0]), src);
}

sal_uInt16 TranslateChar13(sal_uInt16 src)
{
    return xlat(xlat_1_3, sizeof(xlat_1_3) / sizeof(xlat_1_3[0]), src);
}

sal_uInt16 TranslateChar14(sal_uInt16 src)
{
    return xlat(xlat_1_4, sizeof(xlat_1_4) / sizeof(xlat_1_4[0]), src);
}

sal_uInt16 TranslateChar15(sal_uInt16 src)
{
    return xlat(xlat_1_5, sizeof(xlat_1_5) / sizeof(xlat_1_5[0]), src);
}

sal_uInt16 TranslateChar16(sal_uInt16 src)
{
    return xlat(xlat_1_6, sizeof(xlat_1_6) / sizeof(xlat_1_5[0]), src);
}

void TranslateString12(sal_uInt16 *src, sal_uInt16 *dst, sal_uInt32 n)
{
    sal_uInt32 i;
    sal_uInt16 lastS, lastD;

    if (n == 0) return;

    lastD = dst[0] = xlat(xlat_1_2, sizeof(xlat_1_2) / sizeof(xlat_1_2[0]), lastS = src[0]);

    for (i=1; i < n; i++) {
        if (src[i] == lastS) {
            dst[i] = lastD;
        } else {
            lastD = dst[i] = xlat(xlat_1_2, sizeof(xlat_1_2) / sizeof(xlat_1_2[0]), lastS = src[i]);
        }
    }
}

void TranslateString13(sal_uInt16 *src, sal_uInt16 *dst, sal_uInt32 n)
{
    sal_uInt32 i;
    sal_uInt16 lastS, lastD;

    if (n == 0) return;

    lastD = dst[0] = xlat(xlat_1_3, sizeof(xlat_1_3) / sizeof(xlat_1_3[0]), lastS = src[0]);

    for (i=1; i < n; i++) {
        if (src[i] == lastS) {
            dst[i] = lastD;
        } else {
            lastD = dst[i] = xlat(xlat_1_3, sizeof(xlat_1_3) / sizeof(xlat_1_3[0]), lastS = src[i]);
        }
    }
}

void TranslateString14(sal_uInt16 *src, sal_uInt16 *dst, sal_uInt32 n)
{
    sal_uInt32 i;
    sal_uInt16 lastS, lastD;

    if (n == 0) return;

    lastD = dst[0] = xlat(xlat_1_4, sizeof(xlat_1_4) / sizeof(xlat_1_4[0]), lastS = src[0]);

    for (i=1; i < n; i++) {
        if (src[i] == lastS) {
            dst[i] = lastD;
        } else {
            lastD = dst[i] = xlat(xlat_1_4, sizeof(xlat_1_4) / sizeof(xlat_1_4[0]), lastS = src[i]);
        }
    }
}

void TranslateString15(sal_uInt16 *src, sal_uInt16 *dst, sal_uInt32 n)
{
    sal_uInt32 i;
    sal_uInt16 lastS, lastD;

    if (n == 0) return;

    lastD = dst[0] = xlat(xlat_1_5, sizeof(xlat_1_5) / sizeof(xlat_1_5[0]), lastS = src[0]);

    for (i=1; i < n; i++) {
        if (src[i] == lastS) {
            dst[i] = lastD;
        } else {
            lastD = dst[i] = xlat(xlat_1_5, sizeof(xlat_1_5) / sizeof(xlat_1_5[0]), lastS = src[i]);
        }
    }
}

void TranslateString16(sal_uInt16 *src, sal_uInt16 *dst, sal_uInt32 n)
{
    sal_uInt32 i;
    sal_uInt16 lastS, lastD;

    if (n == 0) return;

    lastD = dst[0] = xlat(xlat_1_6, sizeof(xlat_1_6) / sizeof(xlat_1_6[0]), lastS = src[0]);

    for (i=1; i < n; i++) {
        if (src[i] == lastS) {
            dst[i] = lastD;
        } else {
            lastD = dst[i] = xlat(xlat_1_6, sizeof(xlat_1_6) / sizeof(xlat_1_6[0]), lastS = src[i]);
        }
    }
}



