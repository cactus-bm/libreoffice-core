/*************************************************************************
 *
 *  $RCSfile: FileOpenDlg.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: tra $ $Date: 2001-10-04 11:10:45 $
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

#ifndef _FILEOPENDLG_HXX_
#define _FILEOPENDLG_HXX_

//------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring>
#endif

#ifndef _AUTO_BUFFER_HXX_
#include "..\misc\AutoBuffer.hxx"
#endif

#include <memory>

#include <windows.h>

#ifndef _GETFILENAMEWRAPPER_HXX_
#include "getfilenamewrapper.hxx"
#endif

// because we don't want to import the new W2k platform skd
// into our build environment if have stolen the definition
// for the new OPENFILENAME structure from the new headers

#if (_WIN32_WINNT >= 0x0500)

#ifndef CDSIZEOF_STRUCT
#define CDSIZEOF_STRUCT(structname, member)  (((int)((LPBYTE)(&((structname*)0)->member) - ((LPBYTE)((structname*)0)))) + sizeof(((structname*)0)->member))
#endif

typedef struct _tagOFNW {
   DWORD        lStructSize;
   HWND         hwndOwner;
   HINSTANCE    hInstance;
   LPCWSTR      lpstrFilter;
   LPWSTR       lpstrCustomFilter;
   DWORD        nMaxCustFilter;
   DWORD        nFilterIndex;
   LPWSTR       lpstrFile;
   DWORD        nMaxFile;
   LPWSTR       lpstrFileTitle;
   DWORD        nMaxFileTitle;
   LPCWSTR      lpstrInitialDir;
   LPCWSTR      lpstrTitle;
   DWORD        Flags;
   WORD         nFileOffset;
   WORD         nFileExtension;
   LPCWSTR      lpstrDefExt;
   LPARAM       lCustData;
   LPOFNHOOKPROC lpfnHook;
   LPCWSTR      lpTemplateName;
#ifdef _MAC
   LPEDITMENU   lpEditInfo;
   LPCSTR       lpstrPrompt;
#endif
#if (_WIN32_WINNT >= 0x0500)
   void *       pvReserved;
   DWORD        dwReserved;
   DWORD        FlagsEx;
#endif // (_WIN32_WINNT >= 0x0500)
} _OPENFILENAMEW, *_LPOPENFILENAMEW;

#define _OPENFILENAME_SIZE_VERSION_400W  CDSIZEOF_STRUCT(OPENFILENAMEW,lpTemplateName)

#endif // (_WIN32_WINNT >= 0x0500)


//-------------------------------------------------------------
// A simple wrapper class around the Win32 GetOpenFileName API.
// This class is not thread-safe and only one instance at a
// time is allowed
//-------------------------------------------------------------

class CFileOpenDialog
{
public:
    typedef sal_Unicode* POSITION;

public:

    // ctor
    // bFileOpenDialog idicates if we want a FileOpen or FileSave
    // dialog
    // dwFlags see OPENFILENAME
    // dwTemplateId - an ID for custom templates
    // hInstance    - an instance handle for the module
    // which provides the custom template, unused if dwTemplateId
    // is 0
    CFileOpenDialog(
        sal_Bool bFileOpenDialog = sal_True,
        sal_uInt32 dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        sal_uInt32 dwTemplateId = 0,
        HINSTANCE hInstance = 0 );

    virtual ~CFileOpenDialog( );

    virtual void SAL_CALL setTitle( const rtl::OUString& aTitle );

    // to set a filter string using the M$ format
    // e.g. FltName\0*.txt;*.rtf\0...\0\0
    void SAL_CALL setFilter( const rtl::OUString& aFilter );

    // set the index of the current filter when the
    // dialog is about to shown, the index starts with 1
    // the function succeeded if the given filter index
    // is greater than zero and is a valid position
    // within filter string that was previously set
    sal_Bool SAL_CALL setFilterIndex( sal_uInt32 aIndex );

    // get the index of the currently selected filter
    // the index of the returned filter starts with 1
    sal_uInt32 SAL_CALL getSelectedFilterIndex( ) const;

    // set the name and optional the path of the
    // file that will be initially be shown when
    // the dialog will be displayed
    virtual void SAL_CALL setDefaultName( const rtl::OUString& aName );

    // set the initial directory
    virtual void SAL_CALL setDisplayDirectory( const rtl::OUString& aDirectory );

    // returns only the path of the selected file
    virtual rtl::OUString SAL_CALL getLastDisplayDirectory( ) const;

    // returns the full file name including drive letter, path
    // file name and file extension
    virtual rtl::OUString SAL_CALL getFullFileName( ) const;

    // returns the file name and the file extension without
    // drive letter and path
    rtl::OUString SAL_CALL getFileName( ) const;

    // returns the file extension of the selected file
    rtl::OUString SAL_CALL getFileExtension( );

    // set a default extension, only the first three letters of
    // the given extension will be used; the given extension
    // should not contain a '.'
    void SAL_CALL setDefaultFileExtension( const rtl::OUString& aExtension );

    // enables or disables the multiselection mode for
    // the FileOpen/FileSave dialog
    void SAL_CALL setMultiSelectionMode( sal_Bool bMode );

    // returns whether multi-selection mode is enabled or not
    sal_Bool SAL_CALL getMultiSelectionMode( ) const;

    // sets the label of the specified control
    // returns true on success or false if the control id
    // is invalid or another error occured
    sal_Bool SAL_CALL setControlLabel( sal_Int16 ElementID, const rtl::OUString& aLabel );

    // enables or disables the specified control
    virtual void SAL_CALL enableControl( sal_Int16 ElementID, sal_Bool bEnabled );

    // shows or hides a control ( bShow = true shows the control
    // and bShow = false hides the control )
    // returns true on success and false if the control id
    // is invalid or another error occured
    sal_Bool SAL_CALL showControl( sal_Int16 ElementID, sal_Bool bShow );

    // starts enumerating the selected file names if
    // the multiselection mode is enabled
    POSITION SAL_CALL beginEnumFileNames( );

    // returns true if multiselection mode is enabled and
    // another file name was copied to aNextFileName
    // else returns false
    // on success the function returns the complete
    // file name including drive letter and path
    // pos is an in-out parameter and receives the next
    // position when there are further file names or
    // 0 if there aren't
    sal_Bool SAL_CALL getNextFileName(
        /*in|out*/ POSITION& pos, rtl::OUString& aNextFileName );

    // shows the dialog, calls preModal before
    // showing the dialog and postModal after
    // showing the dialog
    // the method returns:
    //  0 - when the dialog was canceled by the user
    //  1 - when the dialog was closed with ok
    // -1 - when an error occured
    sal_Int16 SAL_CALL doModal( );

    // returns the last dialog error that occured
    sal_uInt32 SAL_CALL getLastDialogError( ) const;

    // retrievs the currently selected file
    // including path and drive information
    // can be called only if the dialog is
    // already displayed
    rtl::OUString SAL_CALL getCurrentFilePath( ) const;

    // retrievs the currently selected folder
    rtl::OUString SAL_CALL getCurrentFolderPath( ) const;

    // retrievs the currently selected file name
    // without drive and path
    rtl::OUString SAL_CALL getCurrentFileName( ) const;

protected:
    // have to be overwritten when subclasses
    // want to do special pre- and post-modal
    // processing

    // if preModal return true processing will
    // continue else doModal exit without showing
    // a dialog and returns -1
    virtual sal_Bool SAL_CALL preModal( );

    // post modal processing
    // the function should accept only values returned from
    // doModal and act appropriately
    virtual void SAL_CALL postModal( sal_Int16 nDialogResult );

    // message handler, to be overwritten by subclasses
    virtual sal_uInt32 SAL_CALL onShareViolation( const rtl::OUString& aPathName );
    virtual sal_uInt32 SAL_CALL onFileOk();
    virtual void SAL_CALL onSelChanged( HWND hwndListBox );
    virtual void SAL_CALL onHelp( );

    // only called back if OFN_EXPLORER is set
    virtual void SAL_CALL onInitDone();
    virtual void SAL_CALL onFolderChanged();
    virtual void SAL_CALL onTypeChanged( sal_uInt32 nFilterIndex );

    // call base class method first when overloading
    virtual void SAL_CALL onInitDialog( HWND hwndDlg, HWND hwndChild );

    virtual sal_uInt32 SAL_CALL onCtrlCommand( HWND hwndDlg, sal_uInt16 ctrlId, sal_uInt16 notifyCode );

    sal_uInt32 SAL_CALL onWMNotify( HWND hwndChild, LPOFNOTIFYW lpOfNotify );

    // we use non-virtual functions to do necessary work before
    // calling the virtual funtions (see Gamma: Template method)
    void SAL_CALL handleInitDialog( HWND hwndDlg, HWND hwndChild );

protected:

    // handle to the window of the
    // FileOpen/FileSave dialog
    // will be set on message
    // WM_INITDIALOG, before this
    // value is undefined
    HWND    m_hwndFileOpenDlg;
    HWND    m_hwndFileOpenDlgChild;

    _OPENFILENAMEW  m_ofn;

    // we connect the instance with the dialog window using
    // SetProp, with this function we can reconnect from
    // callback functions to this instance
    static CFileOpenDialog* SAL_CALL getCurrentInstance( HWND hwnd );

    void SAL_CALL centerPositionToParent( ) const;

private:
    // FileOpen or FileSaveDialog
    BOOL            m_bFileOpenDialog;
    rtl::OUString   m_dialogTitle;
    rtl::OUString   m_displayDirectory;
    rtl::OUString   m_defaultExtension;

    CAutoUnicodeBuffer  m_filterBuffer;
    CAutoUnicodeBuffer  m_fileNameBuffer;
    CAutoUnicodeBuffer  m_fileTitleBuffer;
    CAutoUnicodeBuffer  m_helperBuffer;

    ::std::auto_ptr< CGetFileNameWrapper > m_GetFileNameWrapper;

    DLGPROC             m_pfnBaseDlgProc;

    // callback function
    static unsigned int CALLBACK ofnHookProc(
        HWND hChildDlg, // handle to child dialog box
        unsigned int uiMsg,     // message identifier
        WPARAM wParam,  // message parameter
        LPARAM lParam   // message parameter
    );

    // we have to subclass the dialog in order
    // to clean up the window property we are
    // using to connect the window with a class
    // instance in WM_NCDESTROY
    static unsigned int CALLBACK BaseDlgProc(
        HWND hWnd, WORD wMessage, WPARAM wParam, LPARAM lParam );

private:

    // avoid copy and assignment
    CFileOpenDialog( const CFileOpenDialog& );
    CFileOpenDialog& operator=( const CFileOpenDialog& );
};

#endif
