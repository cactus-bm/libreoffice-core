/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <GL/glew.h>

#include "GL3DRenderer.hxx"

#include <vcl/opengl/OpenGLHelper.hxx>
#include <vcl/font.hxx>
#include <vcl/virdev.hxx>

#include <com/sun/star/awt/Size.hpp>

#include <StaticGeometry.h>
#include "glm/gtc/matrix_inverse.hpp"
#include <boost/checked_delete.hpp>

#define DEBUG_FBO 0

using namespace com::sun::star;

namespace chart {

namespace opengl3D {

namespace {

const int CORNER_DIVION_Y = 10;
const int CORNER_DIVION_Z = 10;

int static checkGLError(const char *file, int line)
{
    GLenum glErr;
    int retCode = 0;
    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        const char* sError = OpenGLHelper::GLErrorString(glErr);
        if (sError)
            SAL_WARN("chart2.opengl", "GL Error #" << glErr << "(" << sError << ") " << " in File " << file << " at line: " << line);
        else
            SAL_WARN("chart2.opengl", "GL Error #" << glErr << " (no message available)" << " in File " << file << " at line: " << line);
        retCode = -1;
        return retCode;
    }
    return retCode;
}

#define CHECK_GL_ERROR() checkGLError(__FILE__, __LINE__)

GLfloat texCoords[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f
};

glm::vec4 getColorAsVector(sal_uInt32 nColor)
{
    return glm::vec4(((nColor & 0x00FF0000) >> 16) / 255.0f,
            ((nColor & 0x0000FF00) >> 8) / 255.0f,
            (nColor & 0x000000FF) / 255.0f,
            (0xFF - (nColor & 0xFF000000)/255.0));
}

}

OpenGL3DRenderer::OpenGL3DRenderer():
      m_iWidth(0)
    , m_iHeight(0)
    , m_3DUBOBuffer(0)
    , m_3DActualSizeLight(0)
    , m_NormalBuffer(0)
    , m_VertexBuffer(0)
    , m_CubeVertexBuf(0)
    , m_CubeElementBuf(0)
    , m_CubeNormalBuf(0)
    , m_BoundBox(0)
    , m_BoundBoxNormal(0)
    , m_TextTexCoordBuf(0)
    , m_RoundBarMesh()
    , m_RenderVertexBuf(0)
    , m_RenderTexCoordBuf(0)
    , m_fViewAngle(30.0f)
    , m_fHeightWeight(1.0f)
    , mbPickingMode(false)
    , mnPickingFbo(0)
    , mnPickingRboDepth(0)
    , mnPickingRboColor(0)
{
    m_Polygon3DInfo.lineOnly = false;
    m_Polygon3DInfo.twoSidesLighting = false;
    m_Polygon3DInfo.vertices = NULL;
    m_Polygon3DInfo.normals = NULL;
    m_Polygon3DInfo.lineWidth = 0.001f;

    m_Extrude3DInfo.twoSidesLighting = false;

    m_RoundBarMesh.iMeshSizes = 0;
}

OpenGL3DRenderer::~OpenGL3DRenderer()
{
    // delete buffers
    glDeleteBuffers(1, &m_CubeVertexBuf);
    glDeleteBuffers(1, &m_CubeNormalBuf);
    glDeleteBuffers(1, &m_CubeElementBuf);
    glDeleteBuffers(1, &m_BoundBox);
    glDeleteBuffers(1, &m_BoundBoxNormal);
    glDeleteBuffers(1, &m_TextTexCoordBuf);
    glDeleteBuffers(1, &m_RenderTexCoordBuf);
    glDeleteBuffers(1, &m_RenderVertexBuf);
    glDeleteBuffers(1, &m_3DUBOBuffer);
    glDeleteBuffers(1, &m_VertexBuffer);
    glDeleteBuffers(1, &m_NormalBuffer);
    glDeleteBuffers(1, &m_Batch3DUBOBuffer);
    glDeleteBuffers(1, &m_3DUBOBuffer);
    glDeleteBuffers(1, &m_3DUBOBuffer);

    glDeleteFramebuffers(1, &mnPickingFbo);
    glDeleteRenderbuffers(1, &mnPickingRboDepth);
    glDeleteRenderbuffers(1, &mnPickingRboColor);
}

OpenGL3DRenderer::ShaderResources::ShaderResources()
    : m_3DProID(0)
    , m_3DProjectionID(0)
    , m_3DViewID(0)
    , m_3DModelID(0)
    , m_3DNormalMatrixID(0)
    , m_3DVertexID(0)
    , m_3DNormalID(0)
    , m_TextProID(0)
    , m_TextMatrixID(0)
    , m_TextVertexID(0)
    , m_TextTexCoordID(0)
    , m_TextTexID(0)
    , m_ScreenTextProID(0)
    , m_ScreenTextVertexID(0)
    , m_ScreenTextTexCoordID(0)
    , m_ScreenTextTexID(0)
    , m_CommonProID(0)
    , m_2DVertexID(0)
    , m_2DColorID(0)
    , m_MatrixID(0)
    , m_3DBatchProID(0)
    , m_3DBatchProjectionID(0)
    , m_3DBatchViewID(0)
    , m_3DBatchModelID(0)
    , m_3DBatchNormalMatrixID(0)
    , m_3DBatchVertexID(0)
    , m_3DBatchNormalID(0)
    , m_3DBatchColorID(0)
{
}

OpenGL3DRenderer::ShaderResources::~ShaderResources()
{
    glDeleteProgram(m_CommonProID);
    glDeleteProgram(m_TextProID);
    glDeleteProgram(m_ScreenTextProID);
    glDeleteProgram(m_3DProID);
    glDeleteProgram(m_3DBatchProID);
}

void OpenGL3DRenderer::ShaderResources::LoadShaders()
{
    m_3DProID = OpenGLHelper::LoadShaders("shape3DVertexShader", "shape3DFragmentShader");
    m_3DProjectionID = glGetUniformLocation(m_3DProID, "P");
    m_3DViewID = glGetUniformLocation(m_3DProID, "V");
    m_3DModelID = glGetUniformLocation(m_3DProID, "M");
    m_3DNormalMatrixID = glGetUniformLocation(m_3DProID, "normalMatrix");
    m_3DVertexID = glGetAttribLocation(m_3DProID, "vertexPositionModelspace");
    m_3DNormalID = glGetAttribLocation(m_3DProID, "vertexNormalModelspace");

    m_TextProID = OpenGLHelper::LoadShaders("textVertexShader", "textFragmentShader");
    m_TextMatrixID = glGetUniformLocation(m_TextProID, "MVP");
    m_TextVertexID = glGetAttribLocation(m_TextProID, "vPosition");
    m_TextTexCoordID = glGetAttribLocation(m_TextProID, "texCoord");
    m_TextTexID = glGetUniformLocation(m_TextProID, "TextTex");

    m_ScreenTextProID = OpenGLHelper::LoadShaders("screenTextVertexShader", "screenTextFragmentShader");
    m_ScreenTextVertexID = glGetAttribLocation(m_ScreenTextProID, "vPosition");
    m_ScreenTextTexCoordID = glGetAttribLocation(m_ScreenTextProID, "texCoord");
    m_ScreenTextTexID = glGetUniformLocation(m_ScreenTextProID, "TextTex");

    m_CommonProID = OpenGLHelper::LoadShaders("commonVertexShader", "commonFragmentShader");
    m_MatrixID = glGetUniformLocation(m_CommonProID, "MVP");
    m_2DVertexID = glGetAttribLocation(m_CommonProID, "vPosition");
    m_2DColorID = glGetUniformLocation(m_CommonProID, "vColor");

    m_3DBatchProID = OpenGLHelper::LoadShaders("shape3DVertexShaderBatch", "shape3DFragmentShaderBatch");
    m_3DBatchProjectionID = glGetUniformLocation(m_3DBatchProID, "P");
    m_3DBatchViewID = glGetUniformLocation(m_3DBatchProID, "V");
    m_3DBatchModelID = glGetAttribLocation(m_3DBatchProID, "M");
    m_3DBatchNormalMatrixID = glGetAttribLocation(m_3DBatchProID, "normalMatrix");
    m_3DBatchVertexID = glGetAttribLocation(m_3DBatchProID, "vertexPositionModelspace");
    m_3DBatchNormalID = glGetAttribLocation(m_3DBatchProID, "vertexNormalModelspace");
    m_3DBatchColorID = glGetAttribLocation(m_3DBatchProID, "barColor");
    CHECK_GL_ERROR();
}

OpenGL3DRenderer::PickingShaderResources::PickingShaderResources()
    : m_CommonProID(0)
    , m_2DVertexID(0)
    , m_2DColorID(0)
    , m_MatrixID(0)
{
}

OpenGL3DRenderer::PickingShaderResources::~PickingShaderResources()
{
    glDeleteProgram(m_CommonProID);
}

void OpenGL3DRenderer::PickingShaderResources::LoadShaders()
{
    m_CommonProID = OpenGLHelper::LoadShaders("pickingVertexShader", "pickingFragmentShader");
    m_MatrixID = glGetUniformLocation(m_CommonProID, "MVP");
    m_2DVertexID = glGetAttribLocation(m_CommonProID, "vPosition");
    m_2DColorID = glGetUniformLocation(m_CommonProID, "vColor");
}

void OpenGL3DRenderer::SetCameraInfo(glm::vec3 pos, glm::vec3 direction, glm::vec3 up)
{
    m_CameraInfo.cameraPos = pos;
    m_CameraInfo.cameraOrg = direction;
    m_CameraInfo.cameraUp = up;
}

void OpenGL3DRenderer::init()
{
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_MULTISAMPLE);

    ClearBuffer();

    glGenBuffers(1, &m_CubeVertexBuf);
    glGenBuffers(1, &m_CubeNormalBuf);
    glGenBuffers(1, &m_CubeElementBuf);
    glGenBuffers(1, &m_VertexBuffer);
    glGenBuffers(1, &m_NormalBuffer);
    glGenBuffers(1, &m_BatchModelMatrixBuf);
    glGenBuffers(1, &m_BatchNormalMatrixBuf);
    glGenBuffers(1, &m_BatchColorBuf);
    glGenBuffers(1, &m_BoundBox);
    glBindBuffer(GL_ARRAY_BUFFER, m_BoundBox);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boundBox), boundBox, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &m_BoundBoxNormal);
    glBindBuffer(GL_ARRAY_BUFFER, m_BoundBoxNormal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boundBoxNormal), boundBoxNormal, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_fViewAngle = 60.0f;
    m_3DProjection = glm::perspective(m_fViewAngle, (float)m_iWidth / (float)m_iHeight, 0.01f, 6000.0f);

    maResources.LoadShaders();
    maPickingResources.LoadShaders();

    glGenBuffers(1, &m_TextTexCoordBuf);
    glBindBuffer(GL_ARRAY_BUFFER, m_TextTexCoordBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &m_RenderTexCoordBuf);
    glBindBuffer(GL_ARRAY_BUFFER, m_RenderTexCoordBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coordReverseVertices), coordReverseVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &m_RenderVertexBuf);
    glBindBuffer(GL_ARRAY_BUFFER, m_RenderVertexBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    OpenGLHelper::createFramebuffer(m_iWidth, m_iHeight, mnPickingFbo, mnPickingRboDepth, mnPickingRboColor);

    m_Extrude3DInfo.rounded = false;
    CHECK_GL_ERROR();
    Init3DUniformBlock();
    InitBatch3DUniformBlock();

    glViewport(0, 0, m_iWidth, m_iHeight);
    Set3DSenceInfo(0xFFFFFF, true);
}

void OpenGL3DRenderer::SetSize(const Size& rSize)
{
    m_iWidth = rSize.Width();
    m_iHeight = rSize.Height();
}

void OpenGL3DRenderer::AddVertexData(GLuint vertexBuf)
{
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
    glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(glm::vec3), &m_Vertices[0], GL_STATIC_DRAW);
    CHECK_GL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGL3DRenderer::AddNormalData(GLuint normalBuf)
{
    glBindBuffer(GL_ARRAY_BUFFER, normalBuf);
    glBufferData(GL_ARRAY_BUFFER, m_Normals.size() * sizeof(glm::vec3), &m_Normals[0], GL_STATIC_DRAW);
    CHECK_GL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGL3DRenderer::AddIndexData(GLuint indexBuf)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuf);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned short), &m_Indices[0], GL_STATIC_DRAW);
    CHECK_GL_ERROR();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

bool OpenGL3DRenderer::GetSimilarVertexIndex(PackedVertex & packed,
    std::map<PackedVertex,unsigned short> & VertexToOutIndex,
    unsigned short & result
    )
{
    std::map<PackedVertex,unsigned short>::iterator it = VertexToOutIndex.find(packed);
    if ( it == VertexToOutIndex.end() )
    {
        return false;
    }
    else
    {
        result = it->second;
        return true;
    }
}

void OpenGL3DRenderer::SetVertex(PackedVertex &packed,
    std::map<PackedVertex,unsigned short> &VertexToOutIndex,
    std::vector<glm::vec3> &vertex,
    std::vector<glm::vec3> &normal,
    std::vector<unsigned short> &indices)
{
    unsigned short index;
    bool found = GetSimilarVertexIndex(packed, VertexToOutIndex, index);
    if ( found )
    { // A similar vertex is already in the VBO, use it instead !
        indices.push_back( index );
    }
    else
    { // If not, it needs to be added in the output data.
        vertex.push_back(packed.position);
        normal.push_back(packed.normal);
        size_t newindex = vertex.size() - 1;
        indices.push_back( newindex );
        VertexToOutIndex[ packed ] = newindex;
    }
}

void OpenGL3DRenderer::CreateActualRoundedCube(float fRadius, int iSubDivY, int iSubDivZ, float width, float height, float depth)
{
    float topThreshold = depth - 2 * fRadius;
    float bottomThreshold = fRadius;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    GenerateRoundCornerBar(vertices, normals, fRadius, iSubDivY, iSubDivZ, width, height, depth);
    std::map<PackedVertex,unsigned short> VertexToOutIndex;
    glm::vec3 actualVerteices[3];
    glm::vec3 actualNormals[3];
    std::vector<unsigned short> indices[5];
    glm::vec3 externSurNormal;
    glm::mat4 corrctCoord = glm::translate(glm::vec3(width / 2.0f, height / 2.0f, depth / 2.0f - fRadius));
    m_RoundBarMesh.topThreshold = topThreshold;
    m_RoundBarMesh.bottomThreshold = bottomThreshold;
    m_RoundBarMesh.iMeshStartIndices = m_Vertices.size();
    for (int k = 0; k < 5; k++)
    {
        m_RoundBarMesh.iElementStartIndices[k] = indices[k].size();
    }
    for (size_t i = 0; i < vertices.size(); i += 3)
    {
        for (int k = 0; k < 3; k++)
        {
            actualVerteices[k] = glm::vec3(corrctCoord * glm::vec4(vertices[i + k], 1.0));
            actualNormals[k] = normals[i + k];
        }
        float maxZ = std::max(std::max(actualVerteices[0].z, actualVerteices[1].z), actualVerteices[2].z);
        float minZ = std::min(std::min(actualVerteices[0].z, actualVerteices[1].z), actualVerteices[2].z);
        int surfaceIndex = (minZ >= topThreshold - 0.001) ? TOP_SURFACE : ((maxZ <= bottomThreshold + 0.001) ? BOTTOM_SURFACE : MIDDLE_SURFACE);
        for (int k = 0; k < 3; k++)
        {
            {
                PackedVertex packed = {actualVerteices[k], actualNormals[k]};
                SetVertex(packed, VertexToOutIndex, m_Vertices, m_Normals, indices[surfaceIndex]);
            }

            //add extern
            if ((surfaceIndex == TOP_SURFACE) || (surfaceIndex == BOTTOM_SURFACE))
            {
                actualVerteices[k].z = 0.0f;
                externSurNormal = (surfaceIndex == TOP_SURFACE) ? glm::vec3(0.0, 0.0, 1.0) : glm::vec3(0.0, 0.0, -1.0);
                int tmpSurfaceIndex = (surfaceIndex == TOP_SURFACE) ? FLAT_TOP_SURFACE : FLAT_BOTTOM_SURFACE;
                PackedVertex packed = {actualVerteices[k], externSurNormal};
                SetVertex(packed, VertexToOutIndex, m_Vertices, m_Normals, indices[tmpSurfaceIndex]);
            }
        }

    }
    //create index below
    m_RoundBarMesh.iMeshSizes = m_Vertices.size() - m_RoundBarMesh.iMeshStartIndices;
    for (int k = 0; k < 5; k++)
    {
        m_RoundBarMesh.iElementSizes[k] = indices[k].size() - m_RoundBarMesh.iElementStartIndices[k];
        m_RoundBarMesh.iElementStartIndices[k] = m_Indices.size() * sizeof(unsigned short);
        for (unsigned int IdxCnt = 0; IdxCnt < indices[k].size(); IdxCnt++)
        {
            m_Indices.push_back(indices[k][IdxCnt]);
        }
        indices[k].clear();
    }
    vertices.clear();
    normals.clear();
    VertexToOutIndex.clear();
}

int OpenGL3DRenderer::GenerateRoundCornerBar(std::vector<glm::vec3> &vertices, std::vector<glm::vec3> &normals, float fRadius, int iSubDivY, int iSubDivZ, float width, float height, float depth)
{
    //float fAddAngleY = 360.0f/float(iSubDivY), fAddAngleZ = 180.0f/float(iSubDivZ);
    float fAddAngleY = 360.0f/float(iSubDivY);
    float fAddAngleZ = 180.0f/float(iSubDivZ);
    float fCurAngleY = 0.0f;
    int iStepsY = 1;

    const float PI = float(atan(1.0)*4.0);
    int iFacesAdded = 0;
    float xOffset[] = {(width / 2 - fRadius), -(width / 2 - fRadius), -(width / 2 - fRadius), (width / 2 - fRadius), (width / 2 - fRadius)};
    float yOffset[] = {(height / 2 - fRadius), -(height / 2 - fRadius), (height / 2 - fRadius)};
    float zOffset[] = {-(depth / 2 - fRadius), -(depth / 2 - fRadius), (depth / 2 - fRadius), (depth / 2 - fRadius), -(depth / 2 - fRadius)};
    int iIndices[] = {0, 1, 2, 2, 3, 0};
    while(iStepsY <= iSubDivY)
    {
        float fNextAngleY = fCurAngleY+fAddAngleY;
        if ((fCurAngleY < 90.0f) && (fNextAngleY >= 90.0f))
        {
            fNextAngleY = 90.0f;
        }
        else if ((fCurAngleY < 180.0f) && (fNextAngleY >= 180.0f))
        {
            fNextAngleY = 180.0f;
        }
        else if ((fCurAngleY < 270.0f) && (fNextAngleY >= 270.0f))
        {
            fNextAngleY = 270.0f;
        }
        else if ((fCurAngleY < 360.0f) && (fNextAngleY >= 360.0f))
        {
            fNextAngleY = 360.0f;
        }
        float fSineY = sin(fCurAngleY/180.0f*PI), fCosY = cos(fCurAngleY/180.0f*PI);
        float fNextSineY = sin(fNextAngleY/180.0f*PI), fNextCosY = cos(fNextAngleY/180.0f*PI);
        glm::vec3 vDirY(fCosY, 0.0f, -fSineY), vNextDirY(fNextCosY, 0.0f, -fNextSineY);
        float fCurAngleZ = 0.0f;
        int iStepsZ = 1;
        int xzIndex = 0;
        if ((fCurAngleY >= 0) && (fCurAngleY < 90.0))
        {
            xzIndex = 0;
        }
        else if ((fCurAngleY >= 90) && (fCurAngleY < 180.0))
        {
            xzIndex = 1;
        }
        else if ((fCurAngleY >= 180) && (fCurAngleY < 270.0))
        {
            xzIndex = 2;
        }
        else if ((fCurAngleY >= 270) && (fCurAngleY < 360.0))
        {
            xzIndex = 3;
        }
        while(iStepsZ <= iSubDivZ)
        {
            int yIndex = 0;
            float fNextAngleZ = fCurAngleZ+fAddAngleZ;

            float fSineZ = sin(fCurAngleZ/180.0f*PI), fCosZ = cos(fCurAngleZ/180.0f*PI);
            float fNextSineZ = sin(fNextAngleZ/180.0f*PI), fNextCosZ = cos(fNextAngleZ/180.0f*PI);

            if ((fCurAngleZ < 90.0f) && (fNextAngleZ >= 90.0f))
            {
                fNextAngleZ = 90.0f;
            }

            if ((fCurAngleZ >= 0.0f) && (fCurAngleZ < 90.0f))
            {
                yIndex = 0;
            }
            else
            {
                yIndex = 1;
            }

            glm::vec3 vQuadPoints[] =
            {
                glm::vec3(vDirY.x*fSineZ*fRadius, fCosZ*fRadius, vDirY.z*fSineZ*fRadius),
                glm::vec3(vDirY.x*fNextSineZ*fRadius, fNextCosZ*fRadius, vDirY.z*fNextSineZ*fRadius),
                glm::vec3(vNextDirY.x*fNextSineZ*fRadius, fNextCosZ*fRadius, vNextDirY.z*fNextSineZ*fRadius),
                glm::vec3(vNextDirY.x*fSineZ*fRadius, fCosZ*fRadius, vNextDirY.z*fSineZ*fRadius)
            };

            glm::vec3 vActualQuadPoints[] =
            {

                glm::vec3(vQuadPoints[0].x + xOffset[xzIndex], vQuadPoints[0].y + yOffset[yIndex], vQuadPoints[0].z + zOffset[xzIndex]),
                glm::vec3(vQuadPoints[1].x + xOffset[xzIndex], vQuadPoints[1].y + yOffset[yIndex], vQuadPoints[1].z + zOffset[xzIndex]),
                glm::vec3(vQuadPoints[2].x + xOffset[xzIndex], vQuadPoints[2].y + yOffset[yIndex], vQuadPoints[2].z + zOffset[xzIndex]),
                glm::vec3(vQuadPoints[3].x + xOffset[xzIndex], vQuadPoints[3].y + yOffset[yIndex], vQuadPoints[3].z + zOffset[xzIndex])
            };

            glm::vec3 vNormals[] =
            {
                glm::normalize(vQuadPoints[0]),
                glm::normalize(vQuadPoints[1]),
                glm::normalize(vQuadPoints[2]),
                glm::normalize(vQuadPoints[3])
            };
            for (int i = 0; i < 6; i++)
            {
                int index = iIndices[i];
                vertices.push_back(vActualQuadPoints[index]);
                normals.push_back(vNormals[index]);
            }
            iFacesAdded += 2; // Keep count of added faces
            if (((fCurAngleY < 90.0) && (fNextAngleY >= 90.0)) ||
                ((fCurAngleY < 180.0) && (fNextAngleY >= 180.0)) ||
                ((fCurAngleY < 270.0) && (fNextAngleY >= 270.0)) ||
                ((fCurAngleY < 360.0) && (fNextAngleY >= 360.0)))
            {
                glm::vec3 vXZQuadNextPoints[] =
                {
                    glm::vec3(vQuadPoints[3].x + xOffset[xzIndex], vQuadPoints[3].y + yOffset[yIndex], vQuadPoints[3].z + zOffset[xzIndex]),
                    glm::vec3(vQuadPoints[2].x + xOffset[xzIndex], vQuadPoints[2].y + yOffset[yIndex], vQuadPoints[2].z + zOffset[xzIndex]),
                    glm::vec3(vQuadPoints[2].x + xOffset[xzIndex + 1], vQuadPoints[2].y + yOffset[yIndex], vQuadPoints[2].z + zOffset[xzIndex + 1]),
                    glm::vec3(vQuadPoints[3].x + xOffset[xzIndex + 1], vQuadPoints[3].y + yOffset[yIndex], vQuadPoints[3].z + zOffset[xzIndex + 1])
                };
                glm::vec3 vXZNextNormals[] =
                {
                    glm::normalize(vQuadPoints[3]),
                    glm::normalize(vQuadPoints[2]),
                    glm::normalize(vQuadPoints[2]),
                    glm::normalize(vQuadPoints[3])
                };
                for (int i = 0; i < 6; i++)
                {
                    int index = iIndices[i];
                    vertices.push_back(vXZQuadNextPoints[index]);
                    normals.push_back(vXZNextNormals[index]);
                }
                iFacesAdded += 2;
            }
            if ((fCurAngleZ < 90.0) && (fNextAngleZ >= 90.0))
            {
                glm::vec3 vYQuadNextPoints[] =
                {
                    glm::vec3(vQuadPoints[1].x + xOffset[xzIndex], vQuadPoints[1].y + yOffset[yIndex], vQuadPoints[1].z + zOffset[xzIndex]),
                    glm::vec3(vQuadPoints[1].x + xOffset[xzIndex], vQuadPoints[1].y + yOffset[yIndex + 1], vQuadPoints[1].z + zOffset[xzIndex]),
                    glm::vec3(vQuadPoints[2].x + xOffset[xzIndex], vQuadPoints[2].y + yOffset[yIndex + 1], vQuadPoints[2].z + zOffset[xzIndex]),
                    glm::vec3(vQuadPoints[2].x + xOffset[xzIndex], vQuadPoints[2].y + yOffset[yIndex], vQuadPoints[2].z + zOffset[xzIndex])
                };
                glm::vec3 vYNextNormals[] =
                {
                    glm::normalize(vQuadPoints[1]),
                    glm::normalize(vQuadPoints[1]),
                    glm::normalize(vQuadPoints[2]),
                    glm::normalize(vQuadPoints[2])
                };
                for (int i = 0; i < 6; i++)
                {
                    int index = iIndices[i];
                    vertices.push_back(vYQuadNextPoints[index]);
                    normals.push_back(vYNextNormals[index]);
                }
                iFacesAdded += 2;
            }
            iStepsZ++;
            if ((iStepsZ > iSubDivZ) && (fCurAngleZ < 180.0f))
            {
                iStepsZ--;
            }
            fCurAngleZ = fNextAngleZ;
        }
        iStepsY++;
        if ((iStepsY > iSubDivY) && (fCurAngleY < 360.0f))
        {
            iStepsY--;
        }
        fCurAngleY = fNextAngleY;
    }
    //draw the rectangle face
    //top surface
    glm::vec3 vTopPoints[] =
    {
        glm::vec3(xOffset[0], height / 2, zOffset[0]),
        glm::vec3(xOffset[1], height / 2, zOffset[1]),
        glm::vec3(xOffset[2], height / 2, zOffset[2]),
        glm::vec3(xOffset[3], height / 2, zOffset[3])
    };
    glm::vec3 vTopNormal = glm::vec3(0.0f, 1.0f, 0.0f);
    for (int i = 0; i < 6; i++)
    {
        int index = iIndices[i];
        vertices.push_back(vTopPoints[index]);
        normals.push_back(vTopNormal);
    }
    iFacesAdded += 2;
    //bottom surface
    glm::vec3 vBottomPoints[] =
    {
        glm::vec3(xOffset[3], -height / 2, zOffset[3]),
        glm::vec3(xOffset[2], -height / 2, zOffset[2]),
        glm::vec3(xOffset[1], -height / 2, zOffset[1]),
        glm::vec3(xOffset[0], -height / 2, zOffset[0])
    };
    glm::vec3 vBottomNormal = glm::vec3(0.0f, -1.0f, 0.0f);
    for (int i = 0; i < 6; i++)
    {
        int index = iIndices[i];
        vertices.push_back(vBottomPoints[index]);
        normals.push_back(vBottomNormal);
    }
    iFacesAdded += 2;
    //left surface
    glm::vec3 vLeftPoints[] =
    {
        glm::vec3(-width / 2, yOffset[0], zOffset[0]),
        glm::vec3(-width / 2, yOffset[1], zOffset[1]),
        glm::vec3(-width / 2, yOffset[1], zOffset[2]),
        glm::vec3(-width / 2, yOffset[0], zOffset[3])
    };
    glm::vec3 vLeftNormal = glm::vec3(-1.0f, 0.0f, 0.0f);
    for (int i = 0; i < 6; i++)
    {
        int index = iIndices[i];
        vertices.push_back(vLeftPoints[index]);
        normals.push_back(vLeftNormal);
    }
    //right surface
    glm::vec3 vRightPoints[] =
    {
        glm::vec3(width / 2, yOffset[0], zOffset[3]),
        glm::vec3(width / 2, yOffset[1], zOffset[2]),
        glm::vec3(width / 2, yOffset[1], zOffset[1]),
        glm::vec3(width / 2, yOffset[0], zOffset[0])
    };
    glm::vec3 vRightNormal = glm::vec3(1.0f, 0.0f, 0.0f);
    for (int i = 0; i < 6; i++)
    {
        int index = iIndices[i];
        vertices.push_back(vRightPoints[index]);
        normals.push_back(vRightNormal);
    }
    iFacesAdded += 2;
    //front surface
    glm::vec3 vFrontPoints[] =
    {
        glm::vec3(xOffset[0], yOffset[0], depth / 2),
        glm::vec3(xOffset[1], yOffset[0], depth / 2),
        glm::vec3(xOffset[2], yOffset[1], depth / 2),
        glm::vec3(xOffset[3], yOffset[1], depth / 2)
    };
    glm::vec3 vFrontNormal = glm::vec3(0.0f, 0.0f, 1.0f);
    for (int i = 0; i < 6; i++)
    {
        int index = iIndices[i];
        vertices.push_back(vFrontPoints[index]);
        normals.push_back(vFrontNormal);
    }
    //back surface
    glm::vec3 vBackPoints[] =
    {
        glm::vec3(xOffset[0], yOffset[1], -depth / 2),
        glm::vec3(xOffset[1], yOffset[1], -depth / 2),
        glm::vec3(xOffset[2], yOffset[0], -depth / 2),
        glm::vec3(xOffset[3], yOffset[0], -depth / 2)
    };
    glm::vec3 vBackNormal = glm::vec3(0.0f, 0.0f, -1.0f);
    for (int i = 0; i < 6; i++)
    {
        int index = iIndices[i];
        vertices.push_back(vBackPoints[index]);
        normals.push_back(vBackNormal);
    }
    iFacesAdded += 2;
    return iFacesAdded;
}

void OpenGL3DRenderer::RenderLine3D(Polygon3DInfo &polygon)
{
    glUseProgram(maResources.m_CommonProID);
    PosVecf3 trans = {0.0f, 0, 0.0};
    PosVecf3 angle = {0.0f, 0.0f, 0.0f};
    PosVecf3 scale = {1.0f, 1.0f, m_fHeightWeight};
    MoveModelf(trans, angle, scale);

    m_3DMVP = m_3DProjection * m_3DView * m_Model;

    for (size_t i = 0; i < polygon.verticesList.size(); i++)
    {
        //move the circle to the pos, and scale using the xScale and Y scale
        Vertices3D *pointList = polygon.verticesList[i];
        //if line only, using the common shader to render

        //fill vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, pointList->size() * sizeof(glm::vec3), &pointList[0][0], GL_STATIC_DRAW);
        if(mbPickingMode)
            glUniform4fv(maResources.m_2DColorID, 1, &polygon.id[0]);
        else
            glUniform4fv(maResources.m_2DColorID, 1, &polygon.polygonColor[0]);
        glUniformMatrix4fv(maResources.m_MatrixID, 1, GL_FALSE, &m_3DMVP[0][0]);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(maResources.m_2DVertexID);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
        glVertexAttribPointer(maResources.m_2DVertexID, // attribute
                                3,                  // size
                                GL_FLOAT,           // type
                                GL_FALSE,           // normalized?
                                0,                  // stride
                                (void*)0            // array buffer offset
                                );

        glLineWidth(polygon.lineWidth);
        glDrawArrays(GL_LINE_STRIP, 0, pointList->size());
        glDisableVertexAttribArray(maResources.m_2DVertexID);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glUseProgram(0);
}

void OpenGL3DRenderer::RenderPolygon3D(Polygon3DInfo &polygon)
{
    size_t verticesNum = polygon.verticesList.size();
    size_t normalsNum = polygon.normalsList.size();
    //check whether the number of vertices and normals are equal
    if (verticesNum != normalsNum)
    {
        return ;
    }
    //update ubo
    Update3DUniformBlock();
    glBindBuffer(GL_UNIFORM_BUFFER, m_3DUBOBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, m_3DActualSizeLight, sizeof(MaterialParameters), &polygon.material);
    CHECK_GL_ERROR();
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    if(mbPickingMode)
    {
        glUseProgram(maPickingResources.m_CommonProID);
    }
    else
    {
        glUseProgram(maResources.m_3DProID);
        glUniformMatrix4fv(maResources.m_3DViewID, 1, GL_FALSE, &m_3DView[0][0]);
        glUniformMatrix4fv(maResources.m_3DProjectionID, 1, GL_FALSE, &m_3DProjection[0][0]);
    }
    for (size_t i = 0; i < verticesNum; i++)
    {
        //move the circle to the pos, and scale using the xScale and Y scale
        Vertices3D *pointList = polygon.verticesList[i];
        Normals3D *normalList = polygon.normalsList[i];
        PosVecf3 trans = {0.0f, 0.0f, 0.0};
        PosVecf3 angle = {0.0f, 0.0f, 0.0f};
        PosVecf3 scale = {1.0f, 1.0f, m_fHeightWeight};
        MoveModelf(trans, angle, scale);
        glm::mat3 normalMatrix(m_Model);
        glm::mat3 normalInverseTranspos = glm::inverseTranspose(normalMatrix);

        //render to fbo
        //fill vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, pointList->size() * sizeof(glm::vec3), &pointList[0][0], GL_STATIC_DRAW);
        CHECK_GL_ERROR();

        if(!mbPickingMode)
        {
            //fill normal buffer
            glBindBuffer(GL_ARRAY_BUFFER, m_NormalBuffer);
            glBufferData(GL_ARRAY_BUFFER, normalList->size() * sizeof(glm::vec3), &normalList[0][0], GL_STATIC_DRAW);
            glUniformMatrix4fv(maResources.m_3DModelID, 1, GL_FALSE, &m_Model[0][0]);
            glUniformMatrix3fv(maResources.m_3DNormalMatrixID, 1, GL_FALSE, &normalInverseTranspos[0][0]);
        }
        else
        {
            glm::mat4 aMVP = m_3DProjection * m_3DView * m_Model;
            glUniformMatrix4fv(maPickingResources.m_MatrixID, 1, GL_FALSE, &aMVP[0][0]);
            glUniform4fv(maResources.m_2DColorID, 1, &polygon.id[0]);
        }
        GLint maVertexID = mbPickingMode ? maPickingResources.m_2DVertexID : maResources.m_3DVertexID;
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(maVertexID);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
        glVertexAttribPointer(maVertexID, // attribute
                                3,                  // size
                                GL_FLOAT,           // type
                                GL_FALSE,           // normalized?
                                0,                  // stride
                                (void*)0            // array buffer offset
                                );
        if(!mbPickingMode)
        {
            // 2nd attribute buffer : normals
            glEnableVertexAttribArray(maResources.m_3DNormalID);
            glBindBuffer(GL_ARRAY_BUFFER, m_NormalBuffer);
            glVertexAttribPointer(maResources.m_3DNormalID, // attribute
                    3,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
                    );
        }
        glDrawArrays(GL_POLYGON, 0, pointList->size());
        glDisableVertexAttribArray(maVertexID);
        if(!mbPickingMode)
            glDisableVertexAttribArray(maResources.m_3DNormalID);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glUseProgram(0);
}

namespace {

template< typename T >
struct DeletePointer
{
    void operator()(T* p)
    {
        delete p;
    }
};

}

void OpenGL3DRenderer::ReleasePolygonShapes()
{
    for (size_t i = 0; i < m_Polygon3DInfoList.size(); i++)
    {
        Polygon3DInfo &polygon = m_Polygon3DInfoList[i];
        std::for_each(polygon.verticesList.begin(),
                polygon.verticesList.end(), DeletePointer<Vertices3D>());
        std::for_each(polygon.normalsList.begin(),
                polygon.normalsList.end(), DeletePointer<Normals3D>());
        delete polygon.vertices;
        delete polygon.normals;
    }
    m_Polygon3DInfoList.clear();
}

void OpenGL3DRenderer::RenderPolygon3DObject()
{
    glDepthMask(GL_FALSE);
    for (size_t i = 0; i < m_Polygon3DInfoList.size(); i++)
    {
        Polygon3DInfo &polygon = m_Polygon3DInfoList[i];
        if (polygon.lineOnly || (!polygon.fillStyle))
        {
            //just use the common shader is ok for lines
            RenderLine3D(polygon);
        }
        else
        {
            RenderPolygon3D(polygon);
        }
    }
    glDepthMask(GL_TRUE);
    return;
}

void OpenGL3DRenderer::Set3DSenceInfo(sal_uInt32 nColor, bool twoSidesLighting)
{
    m_Polygon3DInfo.material.twoSidesLighting = twoSidesLighting;

    m_LightsInfo.ambient = getColorAsVector(nColor);

    m_LightsInfo.lightNum = 0;
    SetLightInfo(true, 0xFFFFFF, glm::vec4(1.0, 1.0, 1.0, 0.0));
}

void OpenGL3DRenderer::SetLightInfo(bool lightOn, sal_uInt32 nColor, const glm::vec4& direction)
{
    if (m_LightsInfo.lightNum >= MAX_LIGHT_NUM)
    {
        return;
    }

    if (lightOn)
    {
        m_LightsInfo.light[m_LightsInfo.lightNum].lightColor = getColorAsVector(nColor);
        m_LightsInfo.light[m_LightsInfo.lightNum].positionWorldspace = direction;
        m_LightsInfo.light[m_LightsInfo.lightNum].lightPower = 1.0;
        m_LightsInfo.lightNum++;
    }
}

void OpenGL3DRenderer::AddShapePolygon3DObject(sal_uInt32 nColor, bool lineOnly, sal_uInt32 nLineColor,long fillStyle, sal_uInt32 specular, sal_uInt32 nUniqueId)
{
    m_Polygon3DInfo.polygonColor = getColorAsVector(nColor);
    m_Polygon3DInfo.id = getColorAsVector(nUniqueId);
    m_Polygon3DInfo.material.materialColor = m_Polygon3DInfo.polygonColor;//material color seems to be the same for all parts, so we use the polygon color
    //line or Polygon
    m_Polygon3DInfo.lineOnly = lineOnly;


    // if line only, use line color
    if (m_Polygon3DInfo.lineOnly)
    {
        m_Polygon3DInfo.polygonColor = getColorAsVector(nLineColor);
    }

    //fillStyle
    m_Polygon3DInfo.fillStyle= fillStyle;


    //material specular;
    m_Polygon3DInfo.material.specular = getColorAsVector(specular);

    m_Polygon3DInfo.material.diffuse = glm::vec4(1.0, 1.0, 1.0, 1.0);

    m_Polygon3DInfo.material.ambient = glm::vec4(0.2, 0.2, 0.2, 1.0);

    m_Polygon3DInfo.material.shininess = 1.0f;
}

void OpenGL3DRenderer::EndAddShapePolygon3DObject()
{
    m_Polygon3DInfoList.push_back(m_Polygon3DInfo);
    m_Polygon3DInfo.normals = NULL;
    m_Polygon3DInfo.vertices = NULL;
    // TODO: moggi: memory leak???
    m_Polygon3DInfo.verticesList.clear();
    m_Polygon3DInfo.normalsList.clear();
}

void OpenGL3DRenderer::AddPolygon3DObjectNormalPoint(float x, float y, float z)
{
    if (m_Polygon3DInfo.fillStyle)
    {
        if (!m_Polygon3DInfo.normals)
        {
            m_Polygon3DInfo.normals = new Normals3D;
        }
        m_Polygon3DInfo.normals->push_back(glm::vec3(x, y, z));
    }
}

void OpenGL3DRenderer::EndAddPolygon3DObjectNormalPoint()
{
    m_Polygon3DInfo.normalsList.push_back(m_Polygon3DInfo.normals);
    m_Polygon3DInfo.normals = NULL;
}

void OpenGL3DRenderer::AddPolygon3DObjectPoint(float x, float y, float z)
{
    if (!m_Polygon3DInfo.vertices)
    {
        m_Polygon3DInfo.vertices = new Vertices3D;
    }
    //float actualX = x - (float)m_iWidth / 2;
    //float actualY = y  - (float)m_iHeight / 2;
    float actualX = x;
    float actualY = y;
    float actualZ = z;
    m_Polygon3DInfo.vertices->push_back(glm::vec3(actualX, actualY, actualZ));
}

void OpenGL3DRenderer::EndAddPolygon3DObjectPoint()
{
    m_Polygon3DInfo.verticesList.push_back(m_Polygon3DInfo.vertices);
    m_Polygon3DInfo.vertices = NULL;
}

void OpenGL3DRenderer::AddShape3DExtrudeObject(bool roundedCorner, sal_uInt32 nColor, sal_uInt32 specular, const glm::mat4& modelMatrix, sal_uInt32 nUniqueId)
{
    m_Extrude3DInfo.id = getColorAsVector(nUniqueId);
    glm::vec4 tranform = modelMatrix * glm::vec4(0.0, 0.0, 0.0, 1.0);
    glm::vec4 DirX = modelMatrix * glm::vec4(1.0, 0.0, 0.0, 0.0);
    glm::vec4 DirY = modelMatrix * glm::vec4(0.0, 1.0, 0.0, 0.0);
    glm::vec4 DirZ = modelMatrix * glm::vec4(0.0, 0.0, 1.0, 0.0);
    m_Extrude3DInfo.xScale = glm::length(DirX);
    m_Extrude3DInfo.yScale = glm::length(DirY);
    m_Extrude3DInfo.zScale = glm::length(DirZ);
    glm::mat4 transformMatrixInverse = glm::inverse(glm::translate(glm::vec3(tranform)));
    glm::mat4 scaleMatrixInverse = glm::inverse(glm::scale(glm::vec3(m_Extrude3DInfo.xScale, m_Extrude3DInfo.yScale, m_Extrude3DInfo.zScale)));
    m_Extrude3DInfo.rotation = transformMatrixInverse * modelMatrix * scaleMatrixInverse;

    //color
    m_Extrude3DInfo.extrudeColor = getColorAsVector(nColor);
    m_Extrude3DInfo.material.materialColor = m_Extrude3DInfo.extrudeColor;//material color seems to be the same for all parts, so we use the polygon color

    //material specular;
    m_Extrude3DInfo.material.specular = getColorAsVector(specular);

    m_Extrude3DInfo.material.diffuse = glm::vec4(1.0, 1.0, 1.0, 1.0);

    m_Extrude3DInfo.material.ambient = glm::vec4(0.2, 0.2, 0.2, 1.0);

    m_Extrude3DInfo.material.shininess = 1.0f;
    m_Extrude3DInfo.xTransform = tranform.x;
    m_Extrude3DInfo.yTransform = tranform.y;
    m_Extrude3DInfo.zTransform = tranform.z;
    float width = 1.0f;
    float height = m_Extrude3DInfo.yScale / m_Extrude3DInfo.xScale;
    float radius = height > 0.2f ? 0.2f : height / 4.0f;
    float depth = 1 + 2 * radius;
    bool NORoundedCube = (radius > (width / 2)) || (radius > (height / 2)) || (radius > (depth / 2));
    if (!NORoundedCube && roundedCorner && (m_RoundBarMesh.iMeshSizes == 0))
    {
        CreateActualRoundedCube(radius, CORNER_DIVION_Y, CORNER_DIVION_Z, width, height, depth);
        AddVertexData(m_CubeVertexBuf);
        AddNormalData(m_CubeNormalBuf);
        AddIndexData(m_CubeElementBuf);
        for (int j = 0; j < 5; j++)
        {
            m_Extrude3DInfo.startIndex[j] = m_RoundBarMesh.iElementStartIndices[j];
            m_Extrude3DInfo.size[j] = m_RoundBarMesh.iElementSizes[j];
        }
        m_Vertices.clear();
        m_Normals.clear();
        m_Indices.clear();
        m_Extrude3DInfo.rounded = true;
    }
    m_Batchmaterial = m_Extrude3DInfo.material;
}

void OpenGL3DRenderer::EndAddShape3DExtrudeObject()
{
    m_Extrude3DList.push_back(m_Extrude3DInfo);
}

void OpenGL3DRenderer::Init3DUniformBlock()
{
    if(mbPickingMode)
        return;

    GLuint a3DLightBlockIndex = glGetUniformBlockIndex(maResources.m_3DProID, "GlobalLights");
    GLuint a3DMaterialBlockIndex = glGetUniformBlockIndex(maResources.m_3DProID, "GlobalMaterialParameters");

    if ((GL_INVALID_INDEX == a3DLightBlockIndex) || (GL_INVALID_INDEX == a3DMaterialBlockIndex))
    {
        return;
    }
    int nUniformBufferAlignSize = 1;
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &nUniformBufferAlignSize);
    GLint nBlockDataSizeLight = 0, nBlockDataSizeMertrial = 0;
    glGetActiveUniformBlockiv(maResources.m_3DProID, a3DLightBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &nBlockDataSizeLight);
    glGetActiveUniformBlockiv(maResources.m_3DProID, a3DMaterialBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &nBlockDataSizeMertrial);
    CHECK_GL_ERROR();
    glGenBuffers(1, &m_3DUBOBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, m_3DUBOBuffer);
    CHECK_GL_ERROR();
    m_3DActualSizeLight = ((nBlockDataSizeLight / nUniformBufferAlignSize) + std::min(nBlockDataSizeLight % nUniformBufferAlignSize, 1)) * nUniformBufferAlignSize;
//    cout << "nBlockDataSizeMertrial = " << nBlockDataSizeMertrial << ", nBlockDataSizeLight = " << nBlockDataSizeLight << ", m_3DActualSizeLight = " << m_3DActualSizeLight << endl;
    int dataSize = m_3DActualSizeLight + nBlockDataSizeMertrial;
    glBufferData(GL_UNIFORM_BUFFER, dataSize, NULL, GL_DYNAMIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_3DUBOBuffer, 0, nBlockDataSizeLight);
    CHECK_GL_ERROR();
    glUniformBlockBinding(maResources.m_3DProID, a3DLightBlockIndex, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 1, m_3DUBOBuffer, ((nBlockDataSizeLight / nUniformBufferAlignSize) + std::min(nBlockDataSizeLight % nUniformBufferAlignSize, 1)) * nUniformBufferAlignSize, nBlockDataSizeMertrial);
    glUniformBlockBinding(maResources.m_3DProID, a3DMaterialBlockIndex, 1);
    //for the light source uniform, we must calc the offset of each element
    CHECK_GL_ERROR();
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGL3DRenderer::InitBatch3DUniformBlock()
{
    if(mbPickingMode)
        return;

    GLuint a3DLightBlockIndex = glGetUniformBlockIndex(maResources.m_3DBatchProID, "GlobalLights");
    GLuint a3DMaterialBlockIndex = glGetUniformBlockIndex(maResources.m_3DBatchProID, "GlobalMaterialParameters");

    if ((GL_INVALID_INDEX == a3DLightBlockIndex) || (GL_INVALID_INDEX == a3DMaterialBlockIndex))
    {
        return;
    }
    int nUniformBufferAlignSize = 1;
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &nUniformBufferAlignSize);
    GLint nBlockDataSizeLight = 0, nBlockDataSizeMertrial = 0;
    glGetActiveUniformBlockiv(maResources.m_3DBatchProID, a3DLightBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &nBlockDataSizeLight);
    glGetActiveUniformBlockiv(maResources.m_3DBatchProID, a3DMaterialBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &nBlockDataSizeMertrial);
    CHECK_GL_ERROR();
    glGenBuffers(1, &m_Batch3DUBOBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, m_Batch3DUBOBuffer);
    CHECK_GL_ERROR();
    m_Batch3DActualSizeLight = ((nBlockDataSizeLight / nUniformBufferAlignSize) + std::min(nBlockDataSizeLight % nUniformBufferAlignSize, 1)) * nUniformBufferAlignSize;
//    cout << "nBlockDataSizeMertrial = " << nBlockDataSizeMertrial << ", nBlockDataSizeLight = " << nBlockDataSizeLight << ", m_3DActualSizeLight = " << m_3DActualSizeLight << endl;
    int dataSize = m_Batch3DActualSizeLight + nBlockDataSizeMertrial;
    glBufferData(GL_UNIFORM_BUFFER, dataSize, NULL, GL_DYNAMIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 2, m_Batch3DUBOBuffer, 0, nBlockDataSizeLight);
    CHECK_GL_ERROR();
    glUniformBlockBinding(maResources.m_3DBatchProID, a3DLightBlockIndex, 2);

    glBindBufferRange(GL_UNIFORM_BUFFER, 3, m_Batch3DUBOBuffer, ((nBlockDataSizeLight / nUniformBufferAlignSize) + std::min(nBlockDataSizeLight % nUniformBufferAlignSize, 1)) * nUniformBufferAlignSize, nBlockDataSizeMertrial);
    glUniformBlockBinding(maResources.m_3DBatchProID, a3DMaterialBlockIndex, 3);
    //for the light source uniform, we must calc the offset of each element
    CHECK_GL_ERROR();
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGL3DRenderer::UpdateBatch3DUniformBlock()
{
    if(mbPickingMode)
        return;

    glBindBuffer(GL_UNIFORM_BUFFER, m_Batch3DUBOBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GLint), &m_LightsInfo.lightNum);
    CHECK_GL_ERROR();
    //current std140 alignment: 16
    glBufferSubData(GL_UNIFORM_BUFFER, 16, sizeof(glm::vec4), &m_LightsInfo.ambient[0]);
    CHECK_GL_ERROR();
    //current std140 alignment: 16
    glBufferSubData(GL_UNIFORM_BUFFER, 32, sizeof(LightSource) * MAX_LIGHT_NUM, &m_LightsInfo.light);
    CHECK_GL_ERROR();
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGL3DRenderer::Update3DUniformBlock()
{
    if(mbPickingMode)
        return;

    glBindBuffer(GL_UNIFORM_BUFFER, m_3DUBOBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GLint), &m_LightsInfo.lightNum);
    CHECK_GL_ERROR();
    //current std140 alignment: 16
    glBufferSubData(GL_UNIFORM_BUFFER, 16, sizeof(glm::vec4), &m_LightsInfo.ambient[0]);
    CHECK_GL_ERROR();
    //current std140 alignment: 16
    glBufferSubData(GL_UNIFORM_BUFFER, 32, sizeof(LightSource) * MAX_LIGHT_NUM, &m_LightsInfo.light);
    CHECK_GL_ERROR();
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGL3DRenderer::RenderExtrudeFlatSurface(const Extrude3DInfo& extrude3D, int surIndex)
{
    float xyScale = extrude3D.xScale;
    PosVecf3 trans = {extrude3D.xTransform,
                      extrude3D.yTransform,
                      extrude3D.zTransform};
    glm::mat4 aTranslationMatrix = glm::translate(glm::vec3(trans.x, trans.y, trans.z));
    glm::mat4 flatScale = glm::scale(glm::vec3(xyScale, xyScale, xyScale));
    m_Model = aTranslationMatrix * extrude3D.rotation * flatScale;
    if(!mbPickingMode)
    {
        glm::mat3 normalMatrix(m_Model);
        glm::mat3 normalInverseTranspos = glm::inverseTranspose(normalMatrix);
        glUniformMatrix4fv(maResources.m_3DModelID, 1, GL_FALSE, &m_Model[0][0]);
        glUniformMatrix3fv(maResources.m_3DNormalMatrixID, 1, GL_FALSE, &normalInverseTranspos[0][0]);
    }
    else
    {
        glm::mat4 aMVP = m_3DProjection * m_3DView * m_Model;
        glUniformMatrix4fv(maPickingResources.m_MatrixID, 1, GL_FALSE, &aMVP[0][0]);
        glUniform4fv(maResources.m_2DColorID, 1, &extrude3D.id[0]);
    }

    glDrawElements(GL_TRIANGLES, extrude3D.size[surIndex], GL_UNSIGNED_SHORT, reinterpret_cast<GLvoid*>(extrude3D.startIndex[surIndex]));
}

void OpenGL3DRenderer::RenderExtrudeBottomSurface(const Extrude3DInfo& extrude3D)
{
    float xyScale = extrude3D.xScale;
    float zScale = extrude3D.zScale;
    float actualZTrans = zScale - m_RoundBarMesh.bottomThreshold * xyScale;
    PosVecf3 trans = {extrude3D.xTransform,
                      extrude3D.yTransform,
                      extrude3D.zTransform};
    //PosVecf3 angle = {0.0f, 0.0f, 0.0f};
    if (actualZTrans < 0.0f)
    {
        // the height of rounded corner is higher than the cube than use the org scale matrix
   //     yScale /= (float)(1 + BOTTOM_THRESHOLD);
        zScale /= (float)(m_RoundBarMesh.bottomThreshold);
        PosVecf3 scale = {xyScale, xyScale, zScale};
        glm::mat4 aTranslationMatrix = glm::translate(glm::vec3(trans.x, trans.y, trans.z));
        glm::mat4 aScaleMatrix = glm::scale(glm::vec3(scale.x, scale.y, scale.z));
        m_Model = aTranslationMatrix * extrude3D.rotation * aScaleMatrix;
    }
    else
    {
        glm::mat4 topTrans = glm::translate(glm::vec3(0.0, 0.0, actualZTrans));
        glm::mat4 topScale = glm::scale(glm::vec3(xyScale, xyScale, xyScale));
        glm::mat4 aTranslationMatrix = glm::translate(glm::vec3(trans.x, trans.y, trans.z));
        m_Model = aTranslationMatrix * extrude3D.rotation * topTrans * topScale;
    }

    if(!mbPickingMode)
    {
        glm::mat3 normalMatrix(m_Model);
        glm::mat3 normalInverseTranspos = glm::inverseTranspose(normalMatrix);
        glUniformMatrix4fv(maResources.m_3DModelID, 1, GL_FALSE, &m_Model[0][0]);
        glUniformMatrix3fv(maResources.m_3DNormalMatrixID, 1, GL_FALSE, &normalInverseTranspos[0][0]);
    }
    else
    {
        glm::mat4 aMVP = m_3DProjection * m_3DView * m_Model;
        glUniformMatrix4fv(maPickingResources.m_MatrixID, 1, GL_FALSE, &aMVP[0][0]);
        glUniform4fv(maResources.m_2DColorID, 1, &extrude3D.id[0]);
    }
    glDrawElements(GL_TRIANGLES, extrude3D.size[BOTTOM_SURFACE], GL_UNSIGNED_SHORT, reinterpret_cast<GLvoid*>(extrude3D.startIndex[BOTTOM_SURFACE]));
}

void OpenGL3DRenderer::RenderExtrudeMiddleSurface(const Extrude3DInfo& extrude3D)
{
    float xyScale = extrude3D.xScale;
    float zScale = extrude3D.zScale;
    float actualZScale = zScale - m_RoundBarMesh.bottomThreshold * xyScale;
    PosVecf3 trans = {extrude3D.xTransform,
                      extrude3D.yTransform,
                      extrude3D.zTransform};
    if (actualZScale < 0.0f)
    {
        // the height of rounded corner is higher than the cube than use the org scale matrix
 //       yScale /= (float)(1 + BOTTOM_THRESHOLD);
 //       PosVecf3 scale = {xzScale, yScale, xzScale};
 //       MoveModelf(trans, angle, scale);
          return ;
    }
    else
    {
        glm::mat4 scale = glm::scale(glm::vec3(xyScale, xyScale,actualZScale));
        glm::mat4 aTranslationMatrix = glm::translate(glm::vec3(trans.x, trans.y, trans.z));
        m_Model = aTranslationMatrix * extrude3D.rotation * scale;
    }

    if (extrude3D.reverse)
    {
        glm::mat4 reverseMatrix = glm::translate(glm::vec3(0.0, 0.0, -1.0));
        m_Model = m_Model * reverseMatrix;
    }

    if(!mbPickingMode)
    {
        glm::mat3 normalMatrix(m_Model);
        glm::mat3 normalInverseTranspos = glm::inverseTranspose(normalMatrix);
        glUniformMatrix4fv(maResources.m_3DModelID, 1, GL_FALSE, &m_Model[0][0]);
        glUniformMatrix3fv(maResources.m_3DNormalMatrixID, 1, GL_FALSE, &normalInverseTranspos[0][0]);
    }
    else
    {
        glm::mat4 aMVP = m_3DProjection * m_3DView * m_Model;
        glUniformMatrix4fv(maPickingResources.m_MatrixID, 1, GL_FALSE, &aMVP[0][0]);
        glUniform4fv(maResources.m_2DColorID, 1, &extrude3D.id[0]);
    }
    glDrawElements(GL_TRIANGLES, extrude3D.size[MIDDLE_SURFACE], GL_UNSIGNED_SHORT, reinterpret_cast<GLvoid*>(extrude3D.startIndex[MIDDLE_SURFACE]));
}

void OpenGL3DRenderer::RenderExtrudeTopSurface(const Extrude3DInfo& extrude3D)
{
    float xyScale = extrude3D.xScale;
    float zScale = extrude3D.zScale;
    float actualZTrans = zScale - m_RoundBarMesh.bottomThreshold * xyScale;
    PosVecf3 trans = {extrude3D.xTransform,
                      extrude3D.yTransform,
                      extrude3D.zTransform};
    if (actualZTrans < 0.0f)
    {
        // the height of rounded corner is higher than the cube than use the org scale matrix
        //yScale /= (float)(1 + BOTTOM_THRESHOLD);
        zScale /= (float)(m_RoundBarMesh.bottomThreshold);
        glm::mat4 orgTrans = glm::translate(glm::vec3(0.0, 0.0, -1.0));
        glm::mat4 scale = glm::scale(glm::vec3(xyScale, xyScale, zScale));
        //MoveModelf(trans, angle, scale);
        glm::mat4 aTranslationMatrix = glm::translate(glm::vec3(trans.x, trans.y, trans.z));
        m_Model = aTranslationMatrix * extrude3D.rotation * scale * orgTrans;
    }
    else
    {
        // use different matrices for different parts
        glm::mat4 orgTrans = glm::translate(glm::vec3(0.0, 0.0, -1.0));
        glm::mat4 topTrans = glm::translate(glm::vec3(0.0, 0.0, actualZTrans));
        glm::mat4 topScale = glm::scale(glm::vec3(xyScale, xyScale, xyScale));
        glm::mat4 aTranslationMatrix = glm::translate(glm::vec3(trans.x, trans.y, trans.z));
        m_Model = aTranslationMatrix * extrude3D.rotation * topTrans * topScale * orgTrans;
    }

    if(!mbPickingMode)
    {
        glm::mat3 normalMatrix(m_Model);
        glm::mat3 normalInverseTranspos = glm::inverseTranspose(normalMatrix);
        glUniformMatrix4fv(maResources.m_3DModelID, 1, GL_FALSE, &m_Model[0][0]);
        glUniformMatrix3fv(maResources.m_3DNormalMatrixID, 1, GL_FALSE, &normalInverseTranspos[0][0]);
    }
    else
    {
        glm::mat4 aMVP = m_3DProjection * m_3DView * m_Model;
        glUniformMatrix4fv(maPickingResources.m_MatrixID, 1, GL_FALSE, &aMVP[0][0]);
        glUniform4fv(maResources.m_2DColorID, 1, &extrude3D.id[0]);
    }
    glDrawElements(GL_TRIANGLES, extrude3D.size[TOP_SURFACE], GL_UNSIGNED_SHORT, reinterpret_cast<GLvoid*>(extrude3D.startIndex[TOP_SURFACE]));
    RenderExtrudeFlatSurface(extrude3D, FLAT_BOTTOM_SURFACE);
}

void OpenGL3DRenderer::RenderNonRoundedBar(const Extrude3DInfo& extrude3D)
{
    float xScale = extrude3D.xScale;
    float yScale = extrude3D.yScale;
    float zScale = extrude3D.zScale;
    glm::mat4 transformMatrix = glm::translate(glm::vec3(extrude3D.xTransform, extrude3D.yTransform, extrude3D.zTransform));
    glm::mat4 scaleMatrix = glm::scale(glm::vec3(xScale, yScale, zScale));
    m_Model = transformMatrix * extrude3D.rotation * scaleMatrix;
    if (extrude3D.reverse)
    {
        glm::mat4 reverseMatrix = glm::translate(glm::vec3(0.0, 0.0, -1.0));
        m_Model = m_Model * reverseMatrix;
    }

    if(!mbPickingMode)
    {
        glm::mat3 normalMatrix(m_Model);
        glm::mat3 normalInverseTranspos = glm::inverseTranspose(normalMatrix);
        glUniformMatrix4fv(maResources.m_3DModelID, 1, GL_FALSE, &m_Model[0][0]);
        glUniformMatrix3fv(maResources.m_3DNormalMatrixID, 1, GL_FALSE, &normalInverseTranspos[0][0]);
    }
    else
    {
        glm::mat4 aMVP = m_3DProjection * m_3DView * m_Model;
        glUniformMatrix4fv(maPickingResources.m_MatrixID, 1, GL_FALSE, &aMVP[0][0]);
        glUniform4fv(maResources.m_2DColorID, 1, &extrude3D.id[0]);
    }
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void OpenGL3DRenderer::RenderExtrudeSurface(const Extrude3DInfo& extrude3D)
{
    RenderExtrudeMiddleSurface(extrude3D);
    // check reverse flag to decide whether to render the top middle
    if (extrude3D.reverse)
    {
        RenderExtrudeBottomSurface(extrude3D);
        RenderExtrudeFlatSurface(extrude3D, FLAT_TOP_SURFACE);
    }
    else
    {
        RenderExtrudeTopSurface(extrude3D);
        RenderExtrudeFlatSurface(extrude3D, FLAT_BOTTOM_SURFACE);
    }
}
void OpenGL3DRenderer::ReleaseExtrude3DShapes()
{
    m_Extrude3DList.clear();
}

void OpenGL3DRenderer::RenderExtrude3DObject()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    if(mbPickingMode)
    {
        glUseProgram(maPickingResources.m_CommonProID);
    }
    else
    {
        Update3DUniformBlock();
        glUseProgram(maResources.m_3DProID);
        glUniformMatrix4fv(maResources.m_3DViewID, 1, GL_FALSE, &m_3DView[0][0]);
        glUniformMatrix4fv(maResources.m_3DProjectionID, 1, GL_FALSE, &m_3DProjection[0][0]);
    }
    CHECK_GL_ERROR();
    size_t extrude3DNum = m_Extrude3DList.size();
    for (size_t i = 0; i < extrude3DNum; i++)
    {
        Extrude3DInfo extrude3DInfo = m_Extrude3DList[i];
        GLuint vertexBuf = extrude3DInfo.rounded ? m_CubeVertexBuf : m_BoundBox;
        GLuint normalBuf = extrude3DInfo.rounded ? m_CubeNormalBuf : m_BoundBoxNormal;

        if(mbPickingMode)
            glUniform4fv(maResources.m_2DColorID, 1, &extrude3DInfo.id[0]);
        // 1st attribute buffer : vertices

        GLint aVertexID = mbPickingMode ? maPickingResources.m_2DVertexID : maResources.m_3DVertexID;
        glEnableVertexAttribArray(aVertexID);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
        glVertexAttribPointer(aVertexID, // attribute
                                3,                  // size
                                GL_FLOAT,           // type
                                GL_FALSE,           // normalized?
                                0,                  // stride
                                (void*)0            // array buffer offset
                                );

        if(!mbPickingMode)
        {
            // 2nd attribute buffer : normals
            glEnableVertexAttribArray(maResources.m_3DNormalID);
            glBindBuffer(GL_ARRAY_BUFFER, normalBuf);
            glVertexAttribPointer(maResources.m_3DNormalID, // attribute
                    3,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
                    );
        }
        extrude3DInfo.zTransform *= m_fHeightWeight;
        extrude3DInfo.zScale *= m_fHeightWeight;
        if(!mbPickingMode)
        {
            glBindBuffer(GL_UNIFORM_BUFFER, m_3DUBOBuffer);
            glBufferSubData(GL_UNIFORM_BUFFER, m_3DActualSizeLight, sizeof(MaterialParameters), &extrude3DInfo.material);
            CHECK_GL_ERROR();
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
        extrude3DInfo.reverse = 0;
        if (extrude3DInfo.rounded)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_CubeElementBuf);
            RenderExtrudeSurface(extrude3DInfo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
        else
        {
            RenderNonRoundedBar(extrude3DInfo);
        }
        glDisableVertexAttribArray(aVertexID);
        if(!mbPickingMode)
            glDisableVertexAttribArray(maResources.m_3DNormalID);
    }
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisable(GL_CULL_FACE);
}

void OpenGL3DRenderer::CreateScreenTextTexture(
        const boost::shared_array<sal_uInt8> &bitmapBuf,
        ::Size maSizePixels,
        glm::vec2 vTopLeft, glm::vec2 vBottomRight, sal_uInt32 nUniqueId)
{
    long bmpWidth = maSizePixels.Width();
    long bmpHeight = maSizePixels.Height();

    TextInfo aTextInfo;
    aTextInfo.id = getColorAsVector(nUniqueId);
    aTextInfo.vertex[0] = vTopLeft.x;
    aTextInfo.vertex[1] = vTopLeft.y;
    aTextInfo.vertex[2] = 0;

    aTextInfo.vertex[3] = vBottomRight.x;
    aTextInfo.vertex[4] = vTopLeft.y;
    aTextInfo.vertex[5] = 0;

    aTextInfo.vertex[9] = vTopLeft.x;
    aTextInfo.vertex[10] = vBottomRight.y;
    aTextInfo.vertex[11] = 0;

    aTextInfo.vertex[6] = vBottomRight.x;
    aTextInfo.vertex[7] = vBottomRight.y;
    aTextInfo.vertex[8] = 0;

    CHECK_GL_ERROR();
    glGenTextures(1, &aTextInfo.texture);
    CHECK_GL_ERROR();
    glBindTexture(GL_TEXTURE_2D, aTextInfo.texture);
    CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    CHECK_GL_ERROR();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmpWidth, bmpHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmapBuf.get());
    CHECK_GL_ERROR();
    glBindTexture(GL_TEXTURE_2D, 0);
    CHECK_GL_ERROR();
    m_ScreenTextInfoList.push_back(aTextInfo);
}

void OpenGL3DRenderer::CreateTextTexture(const boost::shared_array<sal_uInt8> &bitmapBuf,
                                         ::Size maSizePixels,
                                         glm::vec3 vTopLeft,glm::vec3 vTopRight,
                                         glm::vec3 vBottomRight, glm::vec3 vBottomLeft,
                                         sal_uInt32 nUniqueId)
{
    long bmpWidth = maSizePixels.Width();
    long bmpHeight = maSizePixels.Height();

    TextInfo aTextInfo;
    aTextInfo.id = getColorAsVector(nUniqueId);
    aTextInfo.vertex[0] = vTopLeft.x;
    aTextInfo.vertex[1] = vTopLeft.y;
    aTextInfo.vertex[2] = vTopLeft.z * m_fHeightWeight;

    aTextInfo.vertex[3] = vTopRight.x;
    aTextInfo.vertex[4] = vTopRight.y;
    aTextInfo.vertex[5] = vTopRight.z * m_fHeightWeight;

    aTextInfo.vertex[9] = vBottomLeft.x;
    aTextInfo.vertex[10] = vBottomLeft.y;
    aTextInfo.vertex[11] = vBottomLeft.z * m_fHeightWeight;

    aTextInfo.vertex[6] = vBottomRight.x;
    aTextInfo.vertex[7] = vBottomRight.y;
    aTextInfo.vertex[8] = vBottomRight.z * m_fHeightWeight;

    CHECK_GL_ERROR();
    glGenTextures(1, &aTextInfo.texture);
    CHECK_GL_ERROR();
    glBindTexture(GL_TEXTURE_2D, aTextInfo.texture);
    CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    CHECK_GL_ERROR();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmpWidth, bmpHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmapBuf.get());
    CHECK_GL_ERROR();
    glBindTexture(GL_TEXTURE_2D, 0);
    CHECK_GL_ERROR();
    m_TextInfoList.push_back(aTextInfo);
}

void OpenGL3DRenderer::ReleaseTextShapes()
{
    for (size_t i = 0; i < m_TextInfoList.size(); i++)
    {
        TextInfo &textInfo = m_TextInfoList[i];
        glDeleteTextures(1, &textInfo.texture);
    }
    m_TextInfoList.clear();
}

void OpenGL3DRenderer::ReleaseScreenTextShapes()
{
    for (size_t i = 0; i < m_ScreenTextInfoList.size(); i++)
    {
        TextInfo &textInfo = m_ScreenTextInfoList[i];
        glDeleteTextures(1, &textInfo.texture);
    }
    m_ScreenTextInfoList.clear();
}

void OpenGL3DRenderer::RenderScreenTextShape()
{
    CHECK_GL_ERROR();
    for (size_t i = 0; i < m_ScreenTextInfoList.size(); i++)
    {
        TextInfo &textInfo = m_ScreenTextInfoList[i];
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
        CHECK_GL_ERROR();
        glBufferData(GL_ARRAY_BUFFER, sizeof(textInfo.vertex), textInfo.vertex, GL_STATIC_DRAW);
        CHECK_GL_ERROR();
        glUseProgram(maResources.m_ScreenTextProID);

        CHECK_GL_ERROR();

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(maResources.m_ScreenTextVertexID);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
        glVertexAttribPointer(
            maResources.m_ScreenTextVertexID,
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
            );
        //tex coord
        CHECK_GL_ERROR();
        glEnableVertexAttribArray(maResources.m_ScreenTextTexCoordID);
        glBindBuffer(GL_ARRAY_BUFFER, m_TextTexCoordBuf);
        glVertexAttribPointer(
            maResources.m_ScreenTextTexCoordID,
            2,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
            );
        //texture
        CHECK_GL_ERROR();
        glBindTexture(GL_TEXTURE_2D, textInfo.texture);
        CHECK_GL_ERROR();
        glUniform1i(maResources.m_ScreenTextTexID, 0);
        CHECK_GL_ERROR();
        //TODO: moggi: get rid fo GL_QUADS
        glDrawArrays(GL_QUADS, 0, 4);
        CHECK_GL_ERROR();
        glDisableVertexAttribArray(maResources.m_ScreenTextTexCoordID);
        CHECK_GL_ERROR();
        glDisableVertexAttribArray(maResources.m_ScreenTextVertexID);
        CHECK_GL_ERROR();
        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);
    }
    CHECK_GL_ERROR();
}

void OpenGL3DRenderer::RenderTextShape()
{
    CHECK_GL_ERROR();
    for (size_t i = 0; i < m_TextInfoList.size(); i++)
    {
        TextInfo &textInfo = m_TextInfoList[i];
        PosVecf3 trans = {0, 0, 0};
        PosVecf3 angle = {0.0f, 0.0f, 0.0f};
        PosVecf3 scale = {1.0, 1.0, 1.0f};
        MoveModelf(trans, angle, scale);
        glm::mat4 aMVP = m_3DProjection * m_3DView * m_Model;
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
        CHECK_GL_ERROR();
        glBufferData(GL_ARRAY_BUFFER, sizeof(textInfo.vertex), textInfo.vertex, GL_STATIC_DRAW);
        CHECK_GL_ERROR();
        glUseProgram(maResources.m_TextProID);

        CHECK_GL_ERROR();
        glUniformMatrix4fv(maResources.m_TextMatrixID, 1, GL_FALSE, &aMVP[0][0]);
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(maResources.m_TextVertexID);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
        glVertexAttribPointer(
            maResources.m_TextVertexID,
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
            );
        //tex coord
        CHECK_GL_ERROR();
        glEnableVertexAttribArray(maResources.m_TextTexCoordID);
        glBindBuffer(GL_ARRAY_BUFFER, m_TextTexCoordBuf);
        glVertexAttribPointer(
            maResources.m_TextTexCoordID,
            2,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
            );
        //texture
        CHECK_GL_ERROR();
        glBindTexture(GL_TEXTURE_2D, textInfo.texture);
        CHECK_GL_ERROR();
        glUniform1i(maResources.m_TextTexID, 0);
        CHECK_GL_ERROR();
        //TODO: moggi: get rid fo GL_QUADS
        glDrawArrays(GL_QUADS, 0, 4);
        CHECK_GL_ERROR();
        glDisableVertexAttribArray(maResources.m_TextTexCoordID);
        CHECK_GL_ERROR();
        glDisableVertexAttribArray(maResources.m_TextVertexID);
        CHECK_GL_ERROR();
        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);
    }
    CHECK_GL_ERROR();
}

void OpenGL3DRenderer::CreateSceneBoxView()
{
    m_3DView = glm::lookAt(m_CameraInfo.cameraPos,
               m_CameraInfo.cameraOrg,
               m_CameraInfo.cameraUp);
}

void OpenGL3DRenderer::ClearBuffer()
{
    static bool bOldRender = getenv("OLDRENDER");
    if (!bOldRender) // gradient background
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);

        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBegin (GL_QUADS);
        glColor3f(0.3,0.3,0.3);
        glVertex3f (-1.0f, -1.0f, -1.0f);
        glVertex3f (1.0f, -1.0f, -1.0f);

        glColor3f(0.0,0.0,0.0);
        glVertex3f (1.0f, 1.0f, -1.0f);
        glVertex3f (-1.0f, 1.0f, -1.0f);
        glEnd ();

        glEnable(GL_LIGHTING);
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glClearDepth(1.0f);
        glClearColor (1.0, 0.5, 0.5, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}

void OpenGL3DRenderer::ProcessUnrenderedShape(bool bNewScene)
{
    glViewport(0, 0, m_iWidth, m_iHeight);
    ClearBuffer();
    CreateSceneBoxView();
    //Polygon
    RenderPolygon3DObject();
    //Shape3DExtrudeObject
    if(mbPickingMode)
        RenderExtrude3DObject();
    else
        RenderBatchBars(bNewScene);
    //render text
    RenderTextShape();
    // render screen text
    RenderScreenTextShape();
#if DEBUG_FBO
    OUString aFileName = OUString("D://shaderout_") + OUString::number(m_iWidth) + "_" + OUString::number(m_iHeight) + ".png";
    OpenGLHelper::renderToFile(m_iWidth, m_iHeight, aFileName);
#endif
}

void OpenGL3DRenderer::MoveModelf(PosVecf3& trans,PosVecf3& angle,PosVecf3& scale)
{
    glm::mat4 aTranslationMatrix = glm::translate(glm::vec3(trans.x, trans.y, trans.z));
    glm::mat4 aScaleMatrix = glm::scale(glm::vec3(scale.x, scale.y, scale.z));
    glm::mat4 aRotationMatrix = glm::eulerAngleYXZ(angle.y, angle.x, angle.z);
    m_Model = aTranslationMatrix * aRotationMatrix * aScaleMatrix;
}

void OpenGL3DRenderer::SetPickingMode(bool bPickingMode)
{
    mbPickingMode = bPickingMode;
    if(mbPickingMode)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, mnPickingFbo);
        glDisable(GL_MULTISAMPLE);
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glEnable(GL_MULTISAMPLE);
    }
}

sal_uInt32 OpenGL3DRenderer::GetPixelColorFromPoint(long nX, long nY)
{
    static sal_uInt32 nId = 0;
    OUString aFileName = OUString("/home/moggi/work/picking_") + OUString::number(nId++) + ".png";
    OpenGLHelper::renderToFile(m_iWidth, m_iHeight, aFileName);
    boost::scoped_array<sal_uInt8> buf(new sal_uInt8[4]);
    glReadPixels(nX, m_iHeight-nY, 1, 1, GL_BGRA, GL_UNSIGNED_BYTE, buf.get());
    Color aColor(255-buf[3], buf[2], buf[1], buf[0]);
    return aColor.GetColor();
}

void OpenGL3DRenderer::ReleaseBatchBarInfo()
{
    for (int i = 0; i < 3; i++)
    {
        m_BarSurface[i].modelMatrixList.clear();
        m_BarSurface[i].normalMatrixList.clear();
        m_BarSurface[i].colorList.clear();
    }
}

void OpenGL3DRenderer::ReleaseShapes()
{
    ReleasePolygonShapes();
    ReleaseExtrude3DShapes();
    ReleaseTextShapes();
    ReleaseScreenTextShapes();
    ReleaseBatchBarInfo();
}

void OpenGL3DRenderer::GetBatchMiddleInfo(Extrude3DInfo &extrude3D)
{
    float xyScale = extrude3D.xScale;
    float zScale = extrude3D.zScale;
    float actualZScale = zScale - m_RoundBarMesh.bottomThreshold * xyScale;
    PosVecf3 trans = {extrude3D.xTransform,
                      extrude3D.yTransform,
                      extrude3D.zTransform};
    if (actualZScale < 0.0f)
    {
          return ;
    }
    else
    {
        glm::mat4 scale = glm::scale(xyScale, xyScale,actualZScale);
        glm::mat4 aTranslationMatrix = glm::translate(glm::vec3(trans.x, trans.y, trans.z));
        m_Model = aTranslationMatrix * extrude3D.rotation * scale;
    }
    glm::mat3 normalMatrix(m_Model);
    glm::mat3 normalInverseTranspos = glm::inverseTranspose(normalMatrix);
    m_BarSurface[MIDDLE_SURFACE].modelMatrixList.push_back(m_Model);
    m_BarSurface[MIDDLE_SURFACE].normalMatrixList.push_back(normalInverseTranspos);
    m_BarSurface[MIDDLE_SURFACE].colorList.push_back(extrude3D.material.materialColor);
}

void OpenGL3DRenderer::GetBatchTopAndFlatInfo(Extrude3DInfo &extrude3D)
{
    float xyScale = extrude3D.xScale;
    float zScale = extrude3D.zScale;
    float actualZTrans = zScale - m_RoundBarMesh.bottomThreshold * xyScale;
    PosVecf3 trans = {extrude3D.xTransform,
                      extrude3D.yTransform,
                      extrude3D.zTransform};
    glm::mat4 orgTrans = glm::translate(glm::vec3(0.0, 0.0, -1.0));
    if (actualZTrans < 0.0f)
    {
        // the height of rounded corner is higher than the cube than use the org scale matrix
        //yScale /= (float)(1 + BOTTOM_THRESHOLD);
        zScale /= (float)(m_RoundBarMesh.bottomThreshold);
        glm::mat4 scale = glm::scale(xyScale, xyScale, zScale);
        //MoveModelf(trans, angle, scale);
        glm::mat4 aTranslationMatrix = glm::translate(glm::vec3(trans.x, trans.y, trans.z));
        m_Model = aTranslationMatrix * extrude3D.rotation * scale * orgTrans;
    }
    else
    {
        // use different matrices for different parts
        glm::mat4 topTrans = glm::translate(glm::vec3(0.0, 0.0, actualZTrans));
        glm::mat4 topScale = glm::scale(xyScale, xyScale, xyScale);
        glm::mat4 aTranslationMatrix = glm::translate(glm::vec3(trans.x, trans.y, trans.z));
        m_Model = aTranslationMatrix * extrude3D.rotation * topTrans * topScale * orgTrans;
    }
    glm::mat3 normalMatrix(m_Model);
    glm::mat3 normalInverseTranspos = glm::inverseTranspose(normalMatrix);
    m_BarSurface[TOP_SURFACE].modelMatrixList.push_back(m_Model);
    m_BarSurface[TOP_SURFACE].normalMatrixList.push_back(normalInverseTranspos);
    m_BarSurface[TOP_SURFACE].colorList.push_back(extrude3D.material.materialColor);

    glm::mat4 aTranslationMatrix = glm::translate(glm::vec3(trans.x, trans.y, trans.z));
    glm::mat4 flatScale = glm::scale(xyScale, xyScale, xyScale);
    m_Model = aTranslationMatrix * extrude3D.rotation * flatScale;
    normalMatrix = glm::mat3(m_Model);
    normalInverseTranspos = glm::inverseTranspose(normalMatrix);

    m_BarSurface[FLAT_BOTTOM_SURFACE].modelMatrixList.push_back(m_Model);
    m_BarSurface[FLAT_BOTTOM_SURFACE].normalMatrixList.push_back(normalInverseTranspos);
    m_BarSurface[FLAT_BOTTOM_SURFACE].colorList.push_back(extrude3D.material.materialColor);
}

void OpenGL3DRenderer::GetBatchBarsInfo()
{
    for (size_t i = 0; i < m_Extrude3DList.size(); i++)
    {
        Extrude3DInfo &extrude3DInfo = m_Extrude3DList[i];
        extrude3DInfo.zTransform *= m_fHeightWeight;
        extrude3DInfo.zScale *= m_fHeightWeight;
        if (m_Extrude3DInfo.rounded)
        {
            GetBatchTopAndFlatInfo(extrude3DInfo);
            GetBatchMiddleInfo(extrude3DInfo);
        }
        else
        {
            glm::mat4 transformMatrix = glm::translate(glm::vec3(extrude3DInfo.xTransform, extrude3DInfo.yTransform, extrude3DInfo.zTransform));
            glm::mat4 scaleMatrix = glm::scale(extrude3DInfo.xScale, extrude3DInfo.yScale, extrude3DInfo.zScale);
            m_Model = transformMatrix * extrude3DInfo.rotation * scaleMatrix;
            glm::mat3 normalMatrix(m_Model);
            glm::mat3 normalInverseTranspos = glm::inverseTranspose(normalMatrix);
            m_BarSurface[0].modelMatrixList.push_back(m_Model);
            m_BarSurface[0].normalMatrixList.push_back(normalInverseTranspos);
            m_BarSurface[0].colorList.push_back(extrude3DInfo.material.materialColor);
        }
    }
}

void OpenGL3DRenderer::RenderBatchBars(bool bNewScene)
{
    if(bNewScene)
        GetBatchBarsInfo();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glUseProgram(maResources.m_3DBatchProID);
    UpdateBatch3DUniformBlock();
    glBindBuffer(GL_UNIFORM_BUFFER, m_Batch3DUBOBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, m_Batch3DActualSizeLight, sizeof(MaterialParameters), &m_Batchmaterial);
    CHECK_GL_ERROR();
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glUniformMatrix4fv(maResources.m_3DBatchViewID, 1, GL_FALSE, &m_3DView[0][0]);
    glUniformMatrix4fv(maResources.m_3DBatchProjectionID, 1, GL_FALSE, &m_3DProjection[0][0]);
    CHECK_GL_ERROR();
    GLuint vertexBuf = m_Extrude3DInfo.rounded ? m_CubeVertexBuf : m_BoundBox;
    GLuint normalBuf = m_Extrude3DInfo.rounded ? m_CubeNormalBuf : m_BoundBoxNormal;
    //vertex
    glEnableVertexAttribArray(maResources.m_3DBatchVertexID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
    glVertexAttribPointer(maResources.m_3DBatchVertexID, // attribute
                          3,                  // size
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );
    //normal
    glEnableVertexAttribArray(maResources.m_3DBatchNormalID);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuf);
    glVertexAttribPointer(maResources.m_3DBatchNormalID, // attribute
                            3,                  // size
                            GL_FLOAT,           // type
                            GL_FALSE,           // normalized?
                            0,                  // stride
                            (void*)0            // array buffer offset
                            );

    for (unsigned int i = 0; i < 4 ; i++)
    {
        glEnableVertexAttribArray(maResources.m_3DBatchModelID + i);
        glBindBuffer(GL_ARRAY_BUFFER, m_BatchModelMatrixBuf);
        glVertexAttribPointer(maResources.m_3DBatchModelID + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), reinterpret_cast<GLvoid*>(sizeof(GLfloat) * i * 4));
        glVertexAttribDivisor(maResources.m_3DBatchModelID + i, 1);
    }

    for (unsigned int i = 0; i < 3 ; i++)
    {
        glEnableVertexAttribArray(maResources.m_3DBatchNormalMatrixID + i);
        glBindBuffer(GL_ARRAY_BUFFER, m_BatchNormalMatrixBuf);
        glVertexAttribPointer(maResources.m_3DBatchNormalMatrixID + i, 3, GL_FLOAT, GL_FALSE, sizeof(glm::mat3), reinterpret_cast<GLvoid*>(sizeof(GLfloat) * i * 3));
        glVertexAttribDivisor(maResources.m_3DBatchNormalMatrixID + i, 1);
    }
    glEnableVertexAttribArray(maResources.m_3DBatchColorID);
    glBindBuffer(GL_ARRAY_BUFFER, m_BatchColorBuf);
    glVertexAttribPointer(maResources.m_3DBatchColorID , 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), 0);
    glVertexAttribDivisor(maResources.m_3DBatchColorID, 1);
    if (m_Extrude3DInfo.rounded)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_CubeElementBuf);
        for (int i = 0; i < 3; i++)
        {
            glBindBuffer(GL_ARRAY_BUFFER, m_BatchModelMatrixBuf);
            glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * m_BarSurface[i].modelMatrixList.size(), &m_BarSurface[i].modelMatrixList[0][0], GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, m_BatchNormalMatrixBuf);
            glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat3) * m_BarSurface[i].normalMatrixList.size(), &m_BarSurface[i].normalMatrixList[0][0], GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, m_BatchColorBuf);
            glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * m_BarSurface[i].colorList.size(), &m_BarSurface[i].colorList[0], GL_DYNAMIC_DRAW);
            glDrawElementsInstancedBaseVertex(GL_TRIANGLES,
                                              m_Extrude3DInfo.size[i],
                                              GL_UNSIGNED_SHORT,
                                              reinterpret_cast<GLvoid*>(m_Extrude3DInfo.startIndex[i]),
                                              m_BarSurface[i].modelMatrixList.size(),
                                              0);
        }
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_BatchModelMatrixBuf);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * m_BarSurface[0].modelMatrixList.size(), &m_BarSurface[0].modelMatrixList[0][0], GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, m_BatchNormalMatrixBuf);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat3) * m_BarSurface[0].normalMatrixList.size(), &m_BarSurface[0].normalMatrixList[0][0], GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, m_BatchColorBuf);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * m_BarSurface[0].colorList.size(), &m_BarSurface[0].colorList[0], GL_DYNAMIC_DRAW);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, m_BarSurface[0].modelMatrixList.size());
    }
    glDisableVertexAttribArray(maResources.m_3DBatchVertexID);
    glDisableVertexAttribArray(maResources.m_3DBatchNormalID);
    glDisableVertexAttribArray(maResources.m_3DBatchColorID);
    glVertexAttribDivisor(maResources.m_3DBatchColorID, 0);
    for (unsigned int i = 0; i < 4 ; i++)
    {
        glDisableVertexAttribArray(maResources.m_3DBatchModelID + i);
        glVertexAttribDivisor(maResources.m_3DBatchModelID + i, 0);
    }
    for (unsigned int i = 0; i < 3 ; i++)
    {
        glDisableVertexAttribArray(maResources.m_3DBatchNormalMatrixID + i);
        glVertexAttribDivisor(maResources.m_3DBatchNormalMatrixID + i, 0);
    }
    glUseProgram(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisable(GL_CULL_FACE);
}
}
}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
