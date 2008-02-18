/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: aqua_clipboard.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2008-02-18 14:49:42 $
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
#include "precompiled_dtrans.hxx"

#ifndef _AQUA_CLIPBOARD_HXX_
#include "aqua_clipboard.hxx"
#endif

#include "DataFlavorMapping.hxx"
#include "OSXTransferable.hxx"

#ifndef COMPHELPER_MAKESEQUENCE_HXX_INCLUDED
#include "comphelper/makesequence.hxx"
#endif

#include <boost/assert.hpp>

using namespace com::sun::star::datatransfer;
using namespace com::sun::star::datatransfer::clipboard;
using namespace com::sun::star::lang;
using namespace com::sun::star::uno;
using namespace cppu;
using namespace osl;
using namespace rtl;
using namespace std;
using namespace comphelper;


@implementation EventListener;

-(EventListener*)initWithAquaClipboard: (AquaClipboard*) pcb
{
  self = [super init];

  if (self)
    {
      pAquaClipboard = pcb;
    }

  return self;
}

-(void)pasteboard:(NSPasteboard*)sender provideDataForType:(NSString*)type
{
  pAquaClipboard->provideDataForType(sender, type);
}

-(void)applicationDidBecomeActive:(NSNotification*)aNotification
{
  pAquaClipboard->applicationDidBecomeActive(aNotification);
}

@end


OUString clipboard_getImplementationName()
{
  return OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.datatransfer.clipboard.AquaClipboard"));
}

Sequence<OUString> clipboard_getSupportedServiceNames()
{
  return makeSequence(OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.datatransfer.clipboard.SystemClipboard")));
}


AquaClipboard::AquaClipboard(const Reference< XComponentContext >& context, NSPasteboard* pasteboard, bool bUseSystemPasteboard) :
  WeakComponentImplHelper4<XClipboardEx, XClipboardNotifier, XFlushableClipboard, XServiceInfo>(m_aMutex),
  mXComponentContext(context),
  mIsSystemPasteboard(bUseSystemPasteboard)
{
  Reference<XMultiComponentFactory> mrServiceMgr = mXComponentContext->getServiceManager();

  mrXMimeCntFactory = Reference<XMimeContentTypeFactory>(mrServiceMgr->createInstanceWithContext(
     OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.datatransfer.MimeContentTypeFactory")), mXComponentContext), UNO_QUERY);

  if (!mrXMimeCntFactory.is())
    {
      throw RuntimeException(OUString(
            RTL_CONSTASCII_USTRINGPARAM("AquaClipboard: Cannot create com.sun.star.datatransfer.MimeContentTypeFactory")),
            static_cast<XClipboardEx*>(this));
    }

  mpDataFlavorMapper = DataFlavorMapperPtr_t(new DataFlavorMapper(mXComponentContext));

  if (pasteboard != NULL)
    {
      mPasteboard = pasteboard;
      mIsSystemPasteboard = false;
    }
  else
    {
      mPasteboard = bUseSystemPasteboard ? [NSPasteboard generalPasteboard] :
        [NSPasteboard pasteboardWithName: NSDragPboard];

      if (mPasteboard == nil)
        {
          throw RuntimeException(OUString(
                RTL_CONSTASCII_USTRINGPARAM("AquaClipboard: Cannot create Cocoa pasteboard")),
                static_cast<XClipboardEx*>(this));
        }
    }

  [mPasteboard retain];

  mEventListener = [[EventListener alloc] initWithAquaClipboard: this];

  if (mEventListener == nil)
    {
      [mPasteboard release];

      throw RuntimeException(
            OUString(RTL_CONSTASCII_USTRINGPARAM("AquaClipboard: Cannot create pasteboard change listener")),
            static_cast<XClipboardEx*>(this));
    }

  if (mIsSystemPasteboard)
    {
      NSNotificationCenter* notificationCenter = [NSNotificationCenter defaultCenter];

      [notificationCenter addObserver: mEventListener
       selector: @selector(applicationDidBecomeActive:)
       name: @"NSApplicationDidBecomeActiveNotification"
       object: [NSApplication sharedApplication]];
    }

  mPasteboardChangeCount = [mPasteboard changeCount];
}


AquaClipboard::~AquaClipboard()
{
  if (mIsSystemPasteboard)
    {
      [[NSNotificationCenter defaultCenter] removeObserver: mEventListener];
    }

  [mEventListener release];
  [mPasteboard release];
}


Reference<XTransferable> SAL_CALL AquaClipboard::getContents() throw(RuntimeException)
{
  MutexGuard aGuard(m_aMutex);

  // Shortcut: If we are clipboard owner already we don't need
  // to drag the data through the system clipboard
  if (mXClipboardContent.is())
    {
      return mXClipboardContent;
    }

  return Reference<XTransferable>(new OSXTransferable(mXComponentContext,
                                                      mrXMimeCntFactory,
                                                      mpDataFlavorMapper,
                                                      mPasteboard));
}


void SAL_CALL AquaClipboard::setContents(const Reference<XTransferable>& xTransferable,
                                         const Reference<XClipboardOwner>& xClipboardOwner)
  throw( RuntimeException )
{
  ClearableMutexGuard aGuard(m_aMutex);

  Reference<XClipboardOwner> oldOwner(mXClipboardOwner);
  mXClipboardOwner = xClipboardOwner;

  Reference<XTransferable> oldContent(mXClipboardContent);
  mXClipboardContent = xTransferable;

  NSArray* types = mpDataFlavorMapper->flavorSequenceToTypesArray(mXClipboardContent->getTransferDataFlavors());
  mPasteboardChangeCount = [mPasteboard declareTypes: types owner: mEventListener];

  // if we are already the owner of the clipboard
  // then fire lost ownership event
  if (oldOwner.is())
    {
      fireLostClipboardOwnershipEvent(oldOwner, oldContent);
    }

  fireClipboardChangedEvent();
}


OUString SAL_CALL AquaClipboard::getName() throw( RuntimeException )
{
  return OUString();
}


sal_Int8 SAL_CALL AquaClipboard::getRenderingCapabilities() throw( RuntimeException )
{
    return 0;
}


void SAL_CALL AquaClipboard::addClipboardListener(const Reference< XClipboardListener >& listener)
  throw( RuntimeException )
{
  MutexGuard aGuard(m_aMutex);

  if (!listener.is())
     throw IllegalArgumentException(OUString(RTL_CONSTASCII_USTRINGPARAM("empty reference")),
                                   static_cast<XClipboardEx*>(this), 1);

  mClipboardListeners.push_back(listener);
}


void SAL_CALL AquaClipboard::removeClipboardListener(const Reference< XClipboardListener >& listener)
  throw( RuntimeException )
{
  MutexGuard aGuard(m_aMutex);

  if (!listener.is())
     throw IllegalArgumentException(OUString(RTL_CONSTASCII_USTRINGPARAM("empty reference")),
                                   static_cast<XClipboardEx*>(this), 1);

  mClipboardListeners.remove(listener);
}


void AquaClipboard::applicationDidBecomeActive(NSNotification* aNotification)
{
  MutexGuard aGuard(m_aMutex);

  int currentPboardChgCount = [mPasteboard changeCount];

  if (currentPboardChgCount != mPasteboardChangeCount)
    {
      mPasteboardChangeCount = currentPboardChgCount;

      // Clear clipboard content and owner and send lostOwnership
      // notification to the old clipboard owner as well as
      // ClipboardChanged notification to any clipboard listener
      Reference<XClipboardOwner> oldOwner(mXClipboardOwner);
      mXClipboardOwner = Reference<XClipboardOwner>();

      Reference<XTransferable> oldContent(mXClipboardContent);
      mXClipboardContent = Reference<XTransferable>();

      if (oldOwner.is())
        {
          fireLostClipboardOwnershipEvent(oldOwner, oldContent);
        }

      fireClipboardChangedEvent();
    }
}


void AquaClipboard::fireClipboardChangedEvent()
{
    ClearableMutexGuard aGuard(m_aMutex);

    list<Reference< XClipboardListener > > listeners(mClipboardListeners);
    ClipboardEvent aEvent;

    if (listeners.begin() != listeners.end())
      {
        aEvent = ClipboardEvent(static_cast<OWeakObject*>(this), getContents());
      }

    aGuard.clear();

    while (listeners.begin() != listeners.end())
    {
        if (listeners.front().is())
          {
            try { listeners.front()->changedContents(aEvent); }
            catch (RuntimeException&) { }
          }
        listeners.pop_front();
    }
}


void AquaClipboard::fireLostClipboardOwnershipEvent(Reference<XClipboardOwner> oldOwner, Reference<XTransferable> oldContent)
{
  BOOST_ASSERT(oldOwner.is());

  try { oldOwner->lostOwnership(static_cast<XClipboardEx*>(this), oldContent); }
  catch(RuntimeException&) { }
}


void AquaClipboard::provideDataForType(NSPasteboard* sender, NSString* type)
{
  DataProviderPtr_t dp = mpDataFlavorMapper->getDataProvider(type, mXClipboardContent);
  NSData* pBoardData = (NSData*)dp->getSystemData();
  [sender setData: pBoardData forType: type];
}


//------------------------------------------------
// XFlushableClipboard
//------------------------------------------------

void SAL_CALL AquaClipboard::flushClipboard()
  throw(RuntimeException)
{
  if (mXClipboardContent.is())
    {
          Sequence<DataFlavor> flavorList = mXClipboardContent->getTransferDataFlavors();
        sal_uInt32 nFlavors = flavorList.getLength();

        for (sal_uInt32 i = 0; i < nFlavors; i++)
          {
            NSString* sysType = mpDataFlavorMapper->openOfficeToSystemFlavor(flavorList[i]);

            if (sysType != NULL)
              {
                provideDataForType(mPasteboard, sysType);
              }
          }
    }
}


NSPasteboard* AquaClipboard::getPasteboard() const
{
  return mPasteboard;
}


//-------------------------------------------------
// XServiceInfo
//-------------------------------------------------

OUString SAL_CALL AquaClipboard::getImplementationName() throw( RuntimeException )
{
  return clipboard_getImplementationName();
}


sal_Bool SAL_CALL AquaClipboard::supportsService( const OUString& ServiceName ) throw( RuntimeException )
{
    return sal_False;
}


Sequence< OUString > SAL_CALL AquaClipboard::getSupportedServiceNames() throw( RuntimeException )
{
  return clipboard_getSupportedServiceNames();
}

