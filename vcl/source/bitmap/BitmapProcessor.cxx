/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

#include <vcl/BitmapProcessor.hxx>
#include <vcl/bmpacc.hxx>
#include <basegfx/color/bcolortools.hxx>

BitmapEx BitmapProcessor::createLightImage(const BitmapEx& rBitmapEx)
{
    const Size aSize(rBitmapEx.GetSizePixel());

    Bitmap aBitmap(rBitmapEx.GetBitmap());
    Bitmap aDarkBitmap(aSize, 24);

    BitmapReadAccess* pRead(aBitmap.AcquireReadAccess());
    BitmapWriteAccess* pWrite(aDarkBitmap.AcquireWriteAccess());

    if (pRead && pWrite)
    {
        for (long nY = 0; nY < aSize.Height(); ++nY)
        {
            for (long nX = 0; nX < aSize.Width(); ++nX)
            {
                BitmapColor aColor = pRead->HasPalette() ?
                                        pRead->GetPaletteColor(pRead->GetPixelIndex(nY, nX)) :
                                        pRead->GetPixel(nY, nX);
                basegfx::BColor aBColor(Color(aColor.Invert()).getBColor());
                aBColor = basegfx::tools::rgb2hsl(aBColor);

                double fHue = aBColor.getRed();
                fHue += 180.0;
                while (fHue > 360.0)
                    fHue -= 360.0;
                aBColor.setRed(fHue);

                aBColor = basegfx::tools::hsl2rgb(aBColor);
                aColor.SetRed(((aBColor.getRed()   * 255.0) + 0.5));
                aColor.SetGreen(((aBColor.getGreen() * 255.0) + 0.5));
                aColor.SetBlue(((aBColor.getBlue()  * 255.0) + 0.5));

                pWrite->SetPixel(nY, nX, aColor);
            }
        }
    }
    Bitmap::ReleaseAccess(pWrite);
    Bitmap::ReleaseAccess(pRead);

    return BitmapEx(aDarkBitmap, rBitmapEx.GetAlpha());
}

BitmapEx BitmapProcessor::createDisabledImage(const BitmapEx& rBitmapEx)
{
    const Size aSize(rBitmapEx.GetSizePixel());

    Bitmap aGrey(aSize, 8, &Bitmap::GetGreyPalette(256));
    AlphaMask aGreyAlpha(aSize);

    Bitmap aBitmap(rBitmapEx.GetBitmap());
    BitmapReadAccess* pRead(aBitmap.AcquireReadAccess());

    BitmapWriteAccess* pGrey(aGrey.AcquireWriteAccess());
    BitmapWriteAccess* pGreyAlpha(aGreyAlpha.AcquireWriteAccess());

    BitmapEx aReturnBitmap;

    if (rBitmapEx.IsTransparent())
    {
        AlphaMask aBitmapAlpha(rBitmapEx.GetAlpha());
        BitmapReadAccess* pReadAlpha(aBitmapAlpha.AcquireReadAccess());

        if (pRead && pReadAlpha && pGrey && pGreyAlpha)
        {
            BitmapColor aGreyValue(0);
            BitmapColor aGreyAlphaValue(0);

            for (int nY = 0; nY < aSize.Height(); ++nY)
            {
                for (int nX = 0; nX < aSize.Width(); ++nX)
                {
                    aGreyValue.SetIndex(pRead->GetLuminance(nY, nX));
                    pGrey->SetPixel(nY, nX, aGreyValue);

                    const BitmapColor aBitmapAlphaValue(pReadAlpha->GetPixel(nY, nX));

                    aGreyAlphaValue.SetIndex(sal_uInt8(std::min(aBitmapAlphaValue.GetIndex() + 178ul, 255ul)));
                    pGreyAlpha->SetPixel(nY, nX, aGreyAlphaValue);
                }
            }
        }
        aBitmapAlpha.ReleaseAccess(pReadAlpha);
        aReturnBitmap = BitmapEx(aGrey, aGreyAlpha);
    }
    else
    {
        if (pRead && pGrey && pGreyAlpha)
        {
            BitmapColor aGreyValue(0);
            BitmapColor aGreyAlphaValue(0);

            for (int nY = 0; nY < aSize.Height(); ++nY)
            {
                for (int nX = 0; nX < aSize.Width(); ++nX)
                {
                    aGreyValue.SetIndex(pRead->GetLuminance(nY, nX));
                    pGrey->SetPixel(nY, nX, aGreyValue);

                    aGreyAlphaValue.SetIndex(128);
                    pGreyAlpha->SetPixel(nY, nX, aGreyAlphaValue);
                }
            }
        }
        aReturnBitmap = BitmapEx(aGrey);
    }

    Bitmap::ReleaseAccess(pRead);

    Bitmap::ReleaseAccess(pGrey);
    aGreyAlpha.ReleaseAccess(pGreyAlpha);

    return aReturnBitmap;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
