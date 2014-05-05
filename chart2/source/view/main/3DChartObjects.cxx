/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "3DChartObjects.hxx"

namespace chart {

namespace opengl3D {

OpenGL3DRenderer* Renderable3DObject::mPrender = NULL;

Renderable3DObject::Renderable3DObject(sal_uInt32 nId):
    mnUniqueId(nId)
{
}

void Renderable3DObject::render()
{
    (void) mnUniqueId;
}

OpenGL3DRenderer* Renderable3DObject::getRender()
{
    if(mPrender==NULL)
    {
        mPrender = new OpenGL3DRenderer();
        mPrender->init();
        mPrender->Set3DSenceInfo();
    }
    return mPrender;
}


Bar::Bar(const glm::mat4& rPosition, sal_uInt32 nId)
    : Renderable3DObject(nId)
    , mbRoundedCorners(false)
    , maPos(rPosition)
{
}

void Bar::render()
{
    (void) mbRoundedCorners;
}

Line::Line(sal_uInt32 nId):
    Renderable3DObject(nId)
{
}

Text::Text(sal_uInt32 nId):
    Renderable3DObject(nId)
{
}

Rectangle::Rectangle(sal_uInt32 nId):
    Renderable3DObject(nId)
{
}

Camera::Camera():
    Renderable3DObject(0),
    maPos(10,10,-10),
    maDirection(glm::vec3(0,0,0)-maPos)
{
}

namespace temporary {

TemporaryContext::TemporaryContext():
    Renderable3DObject(-1),
    miWidth(200),
    miHeight(200)
{
}

void TemporaryContext::init()
{
    maContext.init();
    Size winSize(miWidth, miHeight);
    maContext.setWinSize(winSize);
    getRender()->SetSize(winSize);
}

void TemporaryContext::render()
{
    maContext.renderToFile();
}

}

}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
