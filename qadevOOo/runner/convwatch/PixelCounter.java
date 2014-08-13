/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

package convwatch;

import convwatch.ImageHelper;


abstract class CountPixel
{
    int m_nCount = 0;
    public int getCount() {return m_nCount;}
    public abstract void count(int _nRGB);
}


class CountNotWhite extends CountPixel
{
    public CountNotWhite()
        {
        }

    @Override
    public void count(int pixel)
        {
            int red   = (pixel >> 16) & 0xff;
            int green = (pixel >>  8) & 0xff;
            int blue  = (pixel      ) & 0xff;

            if (red == 0xff && green == 0xff && blue == 0xff)
            {
                return;
            }
            m_nCount++;
        }
}


class CountNotBlack extends CountPixel
{
    public CountNotBlack()
        {
        }

    @Override
    public void count(int pixel)
        {
            int red   = (pixel >> 16) & 0xff;
            int green = (pixel >>  8) & 0xff;
            int blue  = (pixel      ) & 0xff;

            if (red == 0x00 && green == 0x00 && blue == 0x00)
            {
                return;
            }
            m_nCount++;
        }
}


class graphics_stuff
{
    public int stuff()
        {
// (1) decoding
            int rgba = 0; // ...; // comes from PixelGrabber, BufferedImage.getRGB etc.
            int red = (rgba >> 16) & 0xff;
            int green = (rgba >> 8) & 0xff;
            int blue = rgba & 0xff;
            int alpha = (rgba >> 24) & 0xff;
// (2) now modify red, green, blue and alpha as you like;
//     make sure that each of the four values stays in the
//     interval 0 to 255
//            ...
// (3) and encode back to an int, e.g. to give it to MemoryImageSource or
//     BufferedImage.setRGB
                rgba = (alpha << 24) | (red << 16) | (green << 8) | blue;
                return 0;
        }

    public static void countPixel(ImageHelper img, int _w, int _h, CountPixel _aPixelCounter)
        {
            for (int y = 0; y < _h; y++) {
                for (int x = 0; x < _w; x++) {
                    _aPixelCounter.count(img.getPixel(x,y));
                }
            }
        }
    public static int countNotWhitePixel(ImageHelper _aImage)
        {
            int w = _aImage.getWidth();
            int h = _aImage.getHeight();

            CountPixel aCountNotWhite = new CountNotWhite();
            countPixel(_aImage, w, h, aCountNotWhite);
            return aCountNotWhite.getCount();
        }

    public static int countNotBlackPixel(ImageHelper _aImage)
        {
            int w = _aImage.getWidth();
            int h = _aImage.getHeight();

            CountPixel aCountNotBlack = new CountNotBlack();
            countPixel(_aImage, w, h, aCountNotBlack);
            return aCountNotBlack.getCount();
        }
}



public class PixelCounter {
    private ImageHelper m_aImage;


    public int countNotWhitePixel(String _sFile)
        throws java.io.IOException
        {
            m_aImage = ImageHelper.createImageHelper(_sFile);
            int nw = graphics_stuff.countNotWhitePixel(m_aImage);
            return nw;
        }

    public int countNotBlackPixel(String _sFile)
        throws java.io.IOException
        {
            m_aImage = ImageHelper.createImageHelper(_sFile);
            int nw = graphics_stuff.countNotBlackPixel(m_aImage);
            return nw;
        }

    public static int countNotWhitePixelsFromImage(String _sFile)
        throws java.io.IOException
        {
            PixelCounter a = new PixelCounter();
            return a.countNotWhitePixel(_sFile);
        }

    public static int countNotBlackPixelsFromImage(String _sFile)
        throws java.io.IOException
        {
            PixelCounter a = new PixelCounter();
            return a.countNotBlackPixel(_sFile);
        }



}


