/*************************************************************************
 *
 *  $RCSfile: ttcr.h,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: pl $ $Date: 2002-08-02 12:11:25 $
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
 *  Contributor(s): Alexander Gelfenbain
 *
 *
 ************************************************************************/

/* $Id: ttcr.h,v 1.2 2002-08-02 12:11:25 pl Exp $ */

/**
 *
 * @file ttcr.h
 * @brief TrueType font creator
 * @author Alexander Gelfenbain
 */

#ifndef __TTCR_H
#define __TTCR_H

#include "sft.h"
#include "list.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct _TrueTypeCreator TrueTypeCreator;

/* TrueType data types */
    typedef struct {
        sal_uInt16 aw;
        sal_Int16  lsb;
    } longHorMetrics;

/* A generic base class for all TrueType tables */
    typedef struct {
        sal_uInt32  tag;                         /* table tag                                                */
        sal_uInt8   *rawdata;                    /* raw data allocated by GetRawData_*()                     */
        void        *data;                       /* table specific data                                      */
    } TrueTypeTable;

/** Error codes for most functions */
    enum TTCRErrCodes {
        TTCR_OK = 0,                        /**< no error                                               */
        TTCR_ZEROGLYPHS = 1,                /**< At least one glyph should be defined                   */
        TTCR_UNKNOWN = 2,                   /**< Unknown TrueType table                                 */
        TTCR_GLYPHSEQ = 3,                  /**< Glyph IDs are not sequential in the glyf table         */
        TTCR_NONAMES = 4,                   /**< 'name' table does not contain any names                */
        TTCR_NAMETOOLONG = 5,               /**< 'name' table is too long (string data > 64K)           */
        TTCR_POSTFORMAT = 6                 /**< unsupported format of a 'post' table                   */
    };

/* ============================================================================
 *
 * TrueTypeCreator methods
 *
 * ============================================================================ */

/**
 * TrueTypeCreator constructor.
 * Allocates all internal structures.
 */
    void TrueTypeCreatorNewEmpty(sal_uInt32 tag, TrueTypeCreator **_this);

/**
 * TrueTypeCreator destructor. It calls destructors for all TrueTypeTables added to it.
 */
    void TrueTypeCreatorDispose(TrueTypeCreator *_this);

/**
 * Adds a TrueType table to the TrueType creator.
 * SF_TABLEFORMAT value.
 * @return value of SFErrCodes type
 */
    int AddTable(TrueTypeCreator *_this, TrueTypeTable *table);

/**
 * Removes a TrueType table from the TrueType creator if it is stored there.
 * It also calls a TrueTypeTable destructor.
 * Note: all generic tables (with tag 0) will be removed if this function is
 * called with the second argument of 0.
 * @return value of SFErrCodes type
 */
    void RemoveTable(TrueTypeCreator *_this, sal_uInt32 tag);



/**
 * Writes a TrueType font generated by the TrueTypeCreator to a segment of
 * memory that this method allocates. When it is not needed anymore the caller
 * is supposed to call free() on it.
 * @return value of SFErrCodes type
 */
    int StreamToMemory(TrueTypeCreator *_this, sal_uInt8 **ptr, sal_uInt32 *length);

/**
 * Writes a TrueType font  generated by the TrueTypeCreator to a file
 * @return value of SFErrCodes type
 */
    int StreamToFile(TrueTypeCreator *_this, const char* fname);


/* ============================================================================
 *
 * TrueTypeTable methods
 *
 * ============================================================================ */

/**
 * Destructor for the TrueTypeTable object.
 */
    void TrueTypeTableDispose(TrueTypeTable *);

/**
 * This function converts the data of a TrueType table to a raw array of bytes.
 * It may allocates the memory for it and returns the size of the raw data in bytes.
 * If memory is allocated it does not need to be freed by the caller of this function,
 * since the pointer to it is stored in the TrueTypeTable and it is freed by the destructor
 * @return TTCRErrCode
 *
 */

    int GetRawData(TrueTypeTable *, sal_uInt8 **ptr, sal_uInt32 *len, sal_uInt32 *tag);

/**
 *
 * Creates a new raw TrueType table. The difference between this constructor and
 * TrueTypeTableNew_tag constructors is that the latter create structured tables
 * while this constructor just copies memory pointed to by ptr to its buffer
 * and stores its length. This constructor is suitable for data that is not
 * supposed to be processed in any way, just written to the resulting TTF file.
 */
    TrueTypeTable *TrueTypeTableNew(sal_uInt32 tag,
                                    sal_uInt32 nbytes,
                                    sal_uInt8 *ptr);

/**
 * Creates a new 'head' table for a TrueType font.
 * Allocates memory for it. Since a lot of values in the 'head' table depend on the
 * rest of the tables in the TrueType font this table should be the last one added
 * to the font.
 */
    TrueTypeTable *TrueTypeTableNew_head(sal_uInt32 fontRevision,
                                         sal_uInt16 flags,
                                         sal_uInt16 unitsPerEm,
                                         sal_uInt8  *created,
                                         sal_uInt16 macStyle,
                                         sal_uInt16 lowestRecPPEM,
                                         sal_Int16  fontDirectionHint);

/**
 * Creates a new 'hhea' table for a TrueType font.
 * Allocates memory for it and stores it in the hhea pointer.
 */
    TrueTypeTable *TrueTypeTableNew_hhea(sal_Int16  ascender,
                                         sal_Int16  descender,
                                         sal_Int16  linegap,
                                         sal_Int16  caretSlopeRise,
                                         sal_Int16  caretSlopeRun);

/**
 * Creates a new empty 'loca' table for a TrueType font.
 *
 * INTERNAL: gets called only from ProcessTables();
 */
    TrueTypeTable *TrueTypeTableNew_loca(void);

/**
 * Creates a new 'maxp' table based on an existing maxp table.
 * If maxp is 0, a new empty maxp table is created
 * size specifies the size of existing maxp table for
 * error-checking purposes
 */
    TrueTypeTable *TrueTypeTableNew_maxp(sal_uInt8 *maxp, int size);

/**
 * Creates a new empty 'glyf' table.
 */
    TrueTypeTable *TrueTypeTableNew_glyf(void);

/**
 * Creates a new empty 'cmap' table.
 */
    TrueTypeTable *TrueTypeTableNew_cmap(void);

/**
 * Creates a new 'name' table. If n != 0 the table gets populated by
 * the Name Records stored in the nr array. This function allocates
 * memory for its own copy of NameRecords, so nr array has to
 * be explicitly deallocated when it is not needed.
 */
    TrueTypeTable *TrueTypeTableNew_name(int n, NameRecord *nr);

/**
 * Creates a new 'post' table of one of the supported formats
 */
    TrueTypeTable *TrueTypeTableNew_post(sal_uInt32 format,
                                         sal_uInt32 italicAngle,
                                         sal_Int16 underlinePosition,
                                         sal_Int16 underlineThickness,
                                         sal_uInt32 isFixedPitch);


/*------------------------------------------------------------------------------
 *
 *  Table manipulation functions
 *
 *------------------------------------------------------------------------------*/


/**
 * Add a character/glyph pair to a cmap table
 */
    void cmapAdd(TrueTypeTable *, sal_uInt32 id, sal_uInt32 c, sal_uInt32 g);

/**
 * Add a glyph to a glyf table.
 *
 * @return glyphID of the glyph in the new font
 *
 * NOTE: This function does not duplicate GlyphData, so memory will be
 * deallocated in the table destructor
 */
    sal_uInt32 glyfAdd(TrueTypeTable *, GlyphData *glyphdata, TrueTypeFont *fnt);

/**
 * Query the number of glyphs currently stored in the 'glyf' table
 *
 */
    sal_uInt32 glyfCount(const TrueTypeTable *);

/**
 * Add a Name Record to a name table.
 * NOTE: This function duplicates NameRecord, so the argument
 * has to be deallocated by the caller (unlike glyfAdd)
 */
    void nameAdd(TrueTypeTable *, NameRecord *nr);



/*
 * Private Data Types
 */

    struct _TrueTypeCreator {
        sal_uInt32 tag;                         /**< TrueType file tag */
        list   tables;                      /**< List of table tags and pointers */
    };



#ifdef __cplusplus
}
#endif

#endif /* __TTCR_H */
