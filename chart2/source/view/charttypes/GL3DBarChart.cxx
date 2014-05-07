/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <GL3DBarChart.hxx>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "3DChartObjects.hxx"
#include "GL3DRenderer.hxx"

namespace chart {

GL3DBarChart::GL3DBarChart(const std::vector<VDataSeries*>& rDataSeries,
        OpenGLContext& rContext):
    maDataSeries(rDataSeries),
    mpRenderer(new opengl3D::OpenGL3DRenderer()),
    mrContext(rContext)
{
}

GL3DBarChart::~GL3DBarChart()
{
}

void GL3DBarChart::create3DShapes()
{
    const float nBarSizeX = 10;
    const float nBarSizeY = 10;
    const float nBarDistanceX = nBarSizeX / 2;
    const float nBarDistanceY = nBarSizeY / 2;

    maShapes.clear();
    maShapes.push_back(new opengl3D::Camera(mpRenderer.get()));
    sal_Int32 nSeriesIndex = 0;
    sal_uInt32 nId = 1;
    for(std::vector<VDataSeries*>::const_iterator itr = maDataSeries.begin(),
            itrEnd = maDataSeries.end(); itr != itrEnd; ++itr)
    {
        VDataSeries* pDataSeries = *itr;
        sal_Int32 nPointCount = pDataSeries->getTotalPointCount();
        for(sal_Int32 nIndex = 0; nIndex < nPointCount; ++nIndex)
        {
            float nVal = pDataSeries->getYValue(nIndex);
            float nXPos = nIndex * (nBarSizeX + nBarDistanceX);
            float nYPos = nSeriesIndex * (nBarSizeY + nBarDistanceY);

            glm::mat4 aBarPosition;
            aBarPosition = glm::scale(aBarPosition, nBarSizeX, nBarSizeY, nVal);
            aBarPosition = glm::translate(aBarPosition, nXPos, nYPos, nVal/2);

            maShapes.push_back(new opengl3D::Bar(mpRenderer.get(), aBarPosition, nId++));
        }

        ++nSeriesIndex;
    }
}

void GL3DBarChart::render()
{
    mrContext.makeCurrent();
    for(boost::ptr_vector<opengl3D::Renderable3DObject>::iterator itr = maShapes.begin(),
            itrEnd = maShapes.end(); itr != itrEnd; ++itr)
    {
        itr->render();
    }
    mrContext.swapBuffers();
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
