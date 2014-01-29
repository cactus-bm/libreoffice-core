/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <GL/glew.h>
#include <vector>
#include "OpenGLRender.hxx"
#include <vcl/bitmapex.hxx>
#include <vcl/bmpacc.hxx>
#include <vcl/graph.hxx>
#include <com/sun/star/awt/XBitmap.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/graphic/XGraphic.hpp>
#include <comphelper/InlineContainer.hxx>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/drawing/CircleKind.hpp>
#include <com/sun/star/drawing/DoubleSequence.hpp>
#include <com/sun/star/drawing/FlagSequence.hpp>
#include <com/sun/star/drawing/FillStyle.hpp>
#include <com/sun/star/drawing/LineStyle.hpp>
#include <com/sun/star/drawing/NormalsKind.hpp>
#include <com/sun/star/drawing/PointSequence.hpp>
#include <com/sun/star/drawing/PolygonKind.hpp>
#include <com/sun/star/drawing/PolyPolygonBezierCoords.hpp>
#include <com/sun/star/drawing/ProjectionMode.hpp>
#include <com/sun/star/drawing/ShadeMode.hpp>
#include <com/sun/star/drawing/TextFitToSizeType.hpp>
#include <com/sun/star/drawing/TextureProjectionMode.hpp>
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/uno/Any.hxx>
#include <editeng/unoprnms.hxx>
#include <vcl/virdev.hxx>
#include <vcl/dibtools.hxx>
#include <vcl/bmpacc.hxx>
#include <vcl/svapp.hxx>

#include <boost/scoped_array.hpp>

using namespace com::sun::star;

using namespace std;

#define DEBUG_PNG 0
#define BMP_HEADER_LEN 54

#if DEBUG_PNG
#include <vcl/pngwrite.hxx>
#endif

#define OPENGL_SHADER( ... )# __VA_ARGS__

#define GL_PI 3.14159f

#if defined( _WIN32 )
#define WGL_SAMPLE_BUFFERS_ARB   0x2041
#define WGL_SAMPLES_ARB          0x2042
#endif

//begin shaders

#if DEBUG_POSITIONING

const char* DebugVertexShader = OPENGL_SHADER (

attribute vec3 vPosition;
uniform vec4 vColor;
varying vec4 fragmentColor;

void main()
{
    gl_Position = vec4(vPosition, 1);
}

);

const char* DebugFragmentShader = OPENGL_SHADER (

varying vec4 fragmentColor;

void main()
{
    gl_FragColor = vec4(1.0, 1.0, 0.0, 0.5);
}

);

#endif

const char *CommonFragmemtShader = OPENGL_SHADER (

varying vec4 fragmentColor;

void main()
{
    gl_FragColor = fragmentColor;
}

);

const char *CommonVertexShader = OPENGL_SHADER (

attribute vec3 vPosition;
uniform mat4 MVP;
uniform vec4 vColor;
varying vec4 fragmentColor;

void main()
{
    gl_Position =  MVP * vec4(vPosition, 1);
    fragmentColor = vColor;
}

);


const char *BackgroundFragmemtShader = OPENGL_SHADER (

varying vec4 fragmentColor;

void main()
{
    gl_FragColor = fragmentColor;
}

);

const char *BackgroundVertexShader = OPENGL_SHADER (

attribute vec3 vPosition;
uniform mat4 MVP;
attribute vec4 vColor;
varying vec4 fragmentColor;

void main()
{
    gl_Position =  MVP * vec4(vPosition, 1);
    fragmentColor = vColor;
}

);


const char *RenderFragmentShader = OPENGL_SHADER (

uniform sampler2D RenderTex;
varying vec2 vTexCoord;

void main()
{
    gl_FragColor = vec4(texture2D(RenderTex, vTexCoord).rgb, 1.0);
}

);

const char *RenderVertexShader = OPENGL_SHADER (

attribute vec4 vPosition;
attribute vec2 texCoord;
varying vec2 vTexCoord;

void main()
{
    gl_Position =  vPosition;
    vTexCoord = texCoord;
}

);

const char *TextFragmentShader = OPENGL_SHADER (
uniform sampler2D TextTex;
varying vec2 vTexCoord;
void main()
{
    gl_FragColor = vec4(texture2D(TextTex, vTexCoord).rgba);
}

);

const char *TextVertexShader = OPENGL_SHADER (

attribute vec3 vPosition;
uniform mat4 MVP;
attribute vec2 texCoord;
varying vec2 vTexCoord;
void main()
{
    gl_Position =  MVP * vec4(vPosition, 1);
    vTexCoord = texCoord;
}

);

// end shaders

static GLfloat squareVertices[] = {
    -1.0f, -1.0f, -1.0,
    1.0f, -1.0f, -1.0,
    1.0f,  1.0f, -1.0,
    -1.0f,  1.0f, -1.0
};

static GLfloat coordVertices[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
};

#if 0
static const GLfloat g_vertex_buffer_data[] = {
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f
};

// One color for each vertex. They were generated randomly.
static const GLfloat g_color_buffer_data[] = {
    0.583f,  0.771f,  0.014f,
    0.609f,  0.115f,  0.436f,
    0.327f,  0.483f,  0.844f,
    0.822f,  0.569f,  0.201f,
    0.435f,  0.602f,  0.223f,
    0.310f,  0.747f,  0.185f,
    0.597f,  0.770f,  0.761f,
    0.559f,  0.436f,  0.730f,
    0.359f,  0.583f,  0.152f,
    0.483f,  0.596f,  0.789f,
    0.559f,  0.861f,  0.639f,
    0.195f,  0.548f,  0.859f,
    0.014f,  0.184f,  0.576f,
    0.771f,  0.328f,  0.970f,
    0.406f,  0.615f,  0.116f,
    0.676f,  0.977f,  0.133f,
    0.971f,  0.572f,  0.833f,
    0.140f,  0.616f,  0.489f,
    0.997f,  0.513f,  0.064f,
    0.945f,  0.719f,  0.592f,
    0.543f,  0.021f,  0.978f,
    0.279f,  0.317f,  0.505f,
    0.167f,  0.620f,  0.077f,
    0.347f,  0.857f,  0.137f,
    0.055f,  0.953f,  0.042f,
    0.714f,  0.505f,  0.345f,
    0.783f,  0.290f,  0.734f,
    0.722f,  0.645f,  0.174f,
    0.302f,  0.455f,  0.848f,
    0.225f,  0.587f,  0.040f,
    0.517f,  0.713f,  0.338f,
    0.053f,  0.959f,  0.120f,
    0.393f,  0.621f,  0.362f,
    0.673f,  0.211f,  0.457f,
    0.820f,  0.883f,  0.371f,
    0.982f,  0.099f,  0.879f
};
#endif

int static checkGLError(const char *file, int line)
{
    GLenum glErr;
    int retCode = 0;
    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        const GLubyte* sError = gluErrorString(glErr);

        if (sError)
            SAL_WARN("chart2.opengl", "GL Error #" << glErr << "(" << gluErrorString(glErr) << ") " << " in File " << file << " at line: " << line);
        else
            SAL_WARN("chart2.opengl", "GL Error #" << glErr << " (no message available)" << " in File " << file << " at line: " << line);

        retCode = -1;
        return retCode;
    }
    return retCode;
}


#define CHECK_GL_ERROR() checkGLError(__FILE__, __LINE__)

#define CHECK_GL_FRAME_BUFFER_STATUS() \
    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);\
    if( status != GL_FRAMEBUFFER_COMPLETE ) {\
        SAL_WARN("chart2.opengl", "OpenGL error: " << status );\
        return -1;\
    }

GLint OpenGLRender::LoadShaders(const char *vertexShader,const char *fragmentShader)
{
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    char const * VertexSourcePointer = vertexShader;
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    if ( !Result )
    {
        glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if ( InfoLogLength > 0 )
        {
            std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
            glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
            VertexShaderErrorMessage.push_back('\0');
            SAL_WARN("chart2.opengl", "vertex shader compile failed : " << &VertexShaderErrorMessage[0]);
        }
        else
            SAL_WARN("chart2.opengl", "vertex shader compile failed without error log");

        return 0;
    }

    // Compile Fragment Shader
    char const * FragmentSourcePointer = fragmentShader;
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    if ( !Result )
    {
        glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if ( InfoLogLength > 0 )
        {
            std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
            glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
            FragmentShaderErrorMessage.push_back('\0');
            SAL_WARN("chart2.opengl", "fragment shader compile failed : " << &FragmentShaderErrorMessage[0]);
        }
        else
            SAL_WARN("chart2.opengl", "fragment shader compile failed without error log");


        return 0;
    }

    // Link the program
    GLint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    if ( !Result )
    {
        glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if ( InfoLogLength > 0 )
        {
            std::vector<char> ProgramErrorMessage(InfoLogLength+1);
            glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
            ProgramErrorMessage.push_back('\0');
            SAL_WARN("chart2.opengl", "Shader Program failed : " << &ProgramErrorMessage[0]);
        }
        else
            SAL_WARN("chart2.opengl", "shader program link failed without error log");

        return 0;
    }

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

int OpenGLRender::InitOpenGL(GLWindow aWindow)
{
    glWin = aWindow;
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        SAL_WARN("chart2.opengl", "Failed to initialize GLEW");
        return -1;
    }

    // These guys don't just check support but setup the vtables.
    if (glewIsSupported("framebuffer_object") != GLEW_OK)
    {
        SAL_WARN("chart2.opengl", "GL stack has no framebuffer support");
        return -1;
    }

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
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

    glClearColor (m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Init the Projection matrix
    m_Projection = glm::ortho(0.f, float(m_iWidth), 0.f, float(m_iHeight), -1.f, 1.f);
    m_View       = glm::lookAt(glm::vec3(0,0,1), // Camera is at (4,3,-3), in World Space
                               glm::vec3(0,0,0), // and looks at the origin
                               glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                               );
    m_MVP = m_Projection * m_View * m_Model;
    glGenBuffers(1, &m_VertexBuffer);
    glGenBuffers(1, &m_ColorBuffer);

    m_RenderProID = LoadShaders(RenderVertexShader, RenderFragmentShader);
    m_RenderVertexID = glGetAttribLocation(m_RenderProID, "vPosition");
    m_RenderTexCoordID = glGetAttribLocation(m_RenderProID, "texCoord");
    m_RenderTexID = glGetUniformLocation(m_RenderProID, "RenderTex");

    m_CommonProID = LoadShaders(CommonVertexShader, CommonFragmemtShader);
    m_MatrixID = glGetUniformLocation(m_CommonProID, "MVP");
    m_2DVertexID = glGetAttribLocation(m_CommonProID, "vPosition");
    m_2DColorID = glGetUniformLocation(m_CommonProID, "vColor");
    CHECK_GL_ERROR();

#if DEBUG_POSITIONING
    m_DebugProID = LoadShaders(DebugVertexShader, DebugFragmentShader);
    m_DebugVertexID = glGetAttribLocation(m_DebugProID, "vPosition");
#endif
    CHECK_GL_ERROR();

    m_BackgroundProID = LoadShaders(BackgroundVertexShader, BackgroundFragmemtShader);
    m_BackgroundMatrixID = glGetUniformLocation(m_BackgroundProID, "MVP");
    m_BackgroundVertexID = glGetAttribLocation(m_BackgroundProID, "vPosition");
    m_BackgroundColorID = glGetAttribLocation(m_BackgroundProID, "vColor");

    m_TextProID = LoadShaders(TextVertexShader, TextFragmentShader);
    m_TextMatrixID = glGetUniformLocation(m_TextProID, "MVP");
    m_TextVertexID = glGetAttribLocation(m_TextProID, "vPosition");
    m_TextTexCoordID = glGetAttribLocation(m_TextProID, "texCoord");
    m_TextTexID = glGetUniformLocation(m_TextProID, "TextTex");

    CHECK_GL_ERROR();

    glGenBuffers(1, &m_RenderVertexBuf);
    glBindBuffer(GL_ARRAY_BUFFER, m_RenderVertexBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &m_RenderTexCoordBuf);
    glBindBuffer(GL_ARRAY_BUFFER, m_RenderTexCoordBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coordVertices), coordVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &m_TextTexCoordBuf);
    glBindBuffer(GL_ARRAY_BUFFER, m_TextTexCoordBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coordVertices), coordVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

#if defined( WNT )
    SwapBuffers(glWin.hDC);
#elif defined( UNX )
    glXSwapBuffers(glWin.dpy, glWin.win);
#endif
    glFlush();
    glEnable(GL_LIGHTING);
    GLfloat light_direction[] = { 0.0 , 0.0 , 1.0 };
    GLfloat materialDiffuse[] = { 1.0 , 1.0 , 1.0 , 1.0};
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_direction);
    glMaterialfv(GL_FRONT,GL_DIFFUSE,materialDiffuse);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    return 0;
}

BitmapEx OpenGLRender::GetAsBitmap()
{
    boost::scoped_array<sal_uInt8> buf(new sal_uInt8[m_iWidth * m_iHeight * 4]);
    glReadPixels(0, 0, m_iWidth, m_iHeight, GL_BGRA, GL_UNSIGNED_BYTE, buf.get());

    Bitmap aBitmap( Size(m_iWidth, m_iHeight), 24 );
    AlphaMask aAlpha( Size(m_iWidth, m_iHeight) );

    {
        Bitmap::ScopedWriteAccess pWriteAccess( aBitmap );
        AlphaMask::ScopedWriteAccess pAlphaWriteAccess( aAlpha );

        size_t nCurPos = 0;
        for( int y = 0; y < m_iHeight; ++y)
        {
            Scanline pScan = pWriteAccess->GetScanline(y);
            Scanline pAlphaScan = pAlphaWriteAccess->GetScanline(y);
            for( int x = 0; x < m_iWidth; ++x )
            {
                *pScan++ = buf[nCurPos];
                *pScan++ = buf[nCurPos+1];
                *pScan++ = buf[nCurPos+2];

                nCurPos += 3;
                *pAlphaScan++ = static_cast<sal_uInt8>( 255 - buf[nCurPos++] );
            }
        }
    }

    BitmapEx aBmp(aBitmap, aAlpha);

#if DEBUG_PNG // debug PNG writing
    static int nIdx = 0;
    OUString aName = OUString( "file:///home/moggi/Documents/work/" ) + OUString::number( nIdx++ ) + ".png";
    try {
        vcl::PNGWriter aWriter( aBmp );
        SvFileStream sOutput( aName, STREAM_WRITE );
        aWriter.Write( sOutput );
        sOutput.Close();
    } catch (...) {
        SAL_WARN("chart2.opengl", "Error writing png to " << aName);
    }
#endif

    return aBmp;
}

int OpenGLRender::SetLine2DShapePoint(float x, float y, int listLength)
{
    if (m_Line2DPointList.empty())
    {
        m_Line2DPointList.reserve(listLength*3);
    }
    float actualX = (x / OPENGL_SCALE_VALUE);
    float actualY = (y / OPENGL_SCALE_VALUE);
    m_Line2DPointList.push_back(actualX);
    m_Line2DPointList.push_back(actualY);
    m_Line2DPointList.push_back(m_fZStep);

    if (m_Line2DPointList.size() == size_t(listLength * 3))
    {
        m_Line2DShapePointList.push_back(m_Line2DPointList);
        m_Line2DPointList.clear();
    }
    return 0;
}

int OpenGLRender::RenderLine2FBO(int)
{
    m_fZStep += 0.001;
    CHECK_GL_ERROR();
    glLineWidth(m_fLineWidth);
    size_t listNum = m_Line2DShapePointList.size();
    PosVecf3 trans = {0.0f, 0.0f, 0.0f};
    PosVecf3 angle = {0.0f, 0.0f, 0.0f};
    PosVecf3 scale = {1.0f, 1.0f, 1.0f};
    MoveModelf(trans, angle, scale);
    m_MVP = m_Projection * m_View * m_Model;
    for (size_t i = 0; i < listNum; i++)
    {
        Line2DPointList &pointList = m_Line2DShapePointList.front();
        //fill vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
        CHECK_GL_ERROR();
        glBufferData(GL_ARRAY_BUFFER, pointList.size() * sizeof(float), &pointList[0], GL_STATIC_DRAW);
        CHECK_GL_ERROR();
        // Use our shader
        glUseProgram(m_CommonProID);
        CHECK_GL_ERROR();

        glUniform4fv(m_2DColorID, 1, &m_2DColor[0]);
        CHECK_GL_ERROR();
        glUniformMatrix4fv(m_MatrixID, 1, GL_FALSE, &m_MVP[0][0]);
        //CHECK_GL_ERROR();

        // 1rst attribute buffer : vertices
        CHECK_GL_ERROR();
        glVertexAttribPointer(
            m_2DVertexID,
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
            );
        glEnableVertexAttribArray(m_2DVertexID);
        glDrawArrays(GL_LINE_STRIP, 0, pointList.size()/3); // 12*3 indices starting at 0 -> 12 triangles
        CHECK_GL_ERROR();
        glUseProgram(0);
        glDisableVertexAttribArray(m_2DVertexID);
        CHECK_GL_ERROR();
        m_Line2DShapePointList.pop_front();
    }
    GLenum status;
    CHECK_GL_ERROR();
    CHECK_GL_FRAME_BUFFER_STATUS();
    return 0;
}

#if DEBUG_POSITIONING
void OpenGLRender::renderDebug()
{
    CHECK_GL_ERROR();

    GLfloat vertices[4][3] = {
        {-0.9, -0.9, 0 },
        {-0.6, -0.2, 0 },
        {0.3, 0.3, 0 },
        {0.9, 0.9, 0 } };

    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    CHECK_GL_ERROR();
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    CHECK_GL_ERROR();
    glUseProgram(m_DebugProID);
    CHECK_GL_ERROR();
    glVertexAttribPointer(m_DebugVertexID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    CHECK_GL_ERROR();
    glEnableVertexAttribArray(m_DebugVertexID);

    glDrawArrays(GL_LINE_STRIP, 0, 3);
    CHECK_GL_ERROR();
    glDisableVertexAttribArray(m_DebugVertexID);

    CHECK_GL_ERROR();
}
#endif

void OpenGLRender::prepareToRender()
{
    glViewport(0, 0, m_iWidth, m_iHeight);
    if (!m_FboID)
    {
        // create a texture object
        CreateTextureObj(m_iWidth, m_iHeight);
        //create render buffer object
        CreateRenderObj(m_iWidth, m_iHeight);
        //create fbo
        CreateFrameBufferObj();
        if (mbArbMultisampleSupported)
        {
            CreateMultiSampleFrameBufObj();
        }
    }
    //bind fbo
    if (mbArbMultisampleSupported)
    {
        glBindFramebuffer(GL_FRAMEBUFFER,m_frameBufferMS);
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FboID);
    }

    // Clear the screen
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_fZStep = 0;
}

void OpenGLRender::renderToBitmap()
{
    if (mbArbMultisampleSupported)
    {
        GLenum status;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_frameBufferMS);
        status = glCheckFramebufferStatus(GL_READ_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            SAL_INFO("chart2.opengl", "The frame buffer status is not complete!");
        }
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FboID);
        status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            SAL_INFO("chart2.opengl", "The frame buffer status is not complete!");
        }
        glBlitFramebuffer(0, 0 ,m_iWidth, m_iHeight, 0, 0,m_iWidth ,m_iHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        glBindFramebuffer(GL_READ_FRAMEBUFFER,0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, m_FboID);

#if RENDER_TO_FILE
    char fileName[256] = {0};
    sprintf(fileName, "D:\\shaderout_%d_%d.bmp", m_iWidth, m_iHeight);
    sal_uInt8 *buf = (sal_uInt8 *)malloc(m_iWidth * m_iHeight * 3 + BMP_HEADER_LEN);
    CreateBMPHeader(buf, m_iWidth, m_iHeight);
    glReadPixels(0, 0, m_iWidth, m_iHeight, GL_BGR, GL_UNSIGNED_BYTE, buf + BMP_HEADER_LEN);
    FILE *pfile = fopen(fileName,"wb");
    fwrite(buf,m_iWidth * m_iHeight * 3 + BMP_HEADER_LEN, 1, pfile);
    free(buf);
    fclose(pfile);
#else
    Graphic aGraphic( GetAsBitmap() );
    uno::Reference< awt::XBitmap> xBmp( aGraphic.GetXGraphic(), uno::UNO_QUERY );
    uno::Reference < beans::XPropertySet > xPropSet ( mxRenderTarget, uno::UNO_QUERY );
    xPropSet->setPropertyValue("Graphic", uno::makeAny(aGraphic.GetXGraphic()));
    mxRenderTarget->setSize(awt::Size(m_iWidth*OPENGL_SCALE_VALUE, m_iHeight*OPENGL_SCALE_VALUE));
    mxRenderTarget->setPosition(awt::Point(0,0));
#endif
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#if defined( WNT )
    SwapBuffers(glWin.hDC);
#elif defined( UNX )
    glXSwapBuffers(glWin.dpy, glWin.win);
#endif
    glFlush();
}

int OpenGLRender::RenderTexture2FBO(GLuint TexID)
{
    CHECK_GL_ERROR();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDepthMask(GL_FALSE);
    glUseProgram(m_RenderProID);
    glEnableVertexAttribArray(m_RenderVertexID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RenderVertexBuf);
    glVertexAttribPointer(
        m_RenderVertexID,                  // attribute. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
        );
    glEnableVertexAttribArray(m_RenderTexCoordID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RenderTexCoordBuf);
    glVertexAttribPointer(
        m_RenderTexCoordID,                  // attribute. No particular reason for 0, but must match the layout in the shader.
        2,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
        );
    glBindTexture(GL_TEXTURE_2D, TexID);
    glUniform1i(m_RenderTexID, 0);
    //TODO: moggi: get rid fo GL_QUADS
    glDrawArrays(GL_QUADS, 0, 4);
    glDisableVertexAttribArray(m_RenderTexCoordID);
    glDisableVertexAttribArray(m_RenderVertexID);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    glDepthMask(GL_TRUE);
    CHECK_GL_ERROR();
    return 0;
}


int OpenGLRender::RenderTexture(GLuint TexID)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_RenderProID);

    glEnableVertexAttribArray(m_RenderVertexID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RenderVertexBuf);
    glVertexAttribPointer(
        m_RenderVertexID,                  // attribute. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
        );
    glEnableVertexAttribArray(m_RenderTexCoordID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RenderTexCoordBuf);
    glVertexAttribPointer(
        m_RenderTexCoordID,                  // attribute. No particular reason for 0, but must match the layout in the shader.
        2,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
        );
    glBindTexture(GL_TEXTURE_2D, TexID);
    glUniform1i(m_RenderTexID, 0);
    //TODO: moggi: get rid fo GL_QUADS
    glDrawArrays(GL_QUADS, 0, 4);
    glDisableVertexAttribArray(m_RenderTexCoordID);
    glDisableVertexAttribArray(m_RenderVertexID);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
#if defined( WNT )
    SwapBuffers(glWin.hDC);
#elif defined( UNX )
    glXSwapBuffers(glWin.dpy, glWin.win);
#endif
    glFlush();
    return 0;
}


int OpenGLRender::CreateTextureObj(int width, int height)
{
    glGenTextures(1, &m_TextureObj);
    glBindTexture(GL_TEXTURE_2D, m_TextureObj);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    CHECK_GL_ERROR();
    glBindTexture(GL_TEXTURE_2D, 0);
    return 0;
}

int OpenGLRender::CreateRenderObj(int width, int height)
{
    glGenRenderbuffers(1, &m_RboID);
    CHECK_GL_ERROR();
    glBindRenderbuffer(GL_RENDERBUFFER, m_RboID);
    CHECK_GL_ERROR();
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    CHECK_GL_ERROR();
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    CHECK_GL_ERROR();
    return 0;
}

int OpenGLRender::MoveModelf(PosVecf3 trans, PosVecf3 angle, PosVecf3 scale)
{
    glm::mat4 aTranslationMatrix = glm::translate(glm::vec3(trans.x, trans.y, trans.z));
    glm::mat4 aScaleMatrix = glm::scale(glm::vec3(scale.x, scale.y, scale.z));
    glm::mat4 aRotationMatrix = glm::eulerAngleYXZ(angle.y, angle.x, angle.z);
    m_Model = aTranslationMatrix * aRotationMatrix * aScaleMatrix;
    return 0;
}

int OpenGLRender::CreateFrameBufferObj()
{
    GLenum status;
    // create a framebuffer object, you need to delete them when program exits.
    glGenFramebuffers(1, &m_FboID);
    CHECK_GL_FRAME_BUFFER_STATUS();
    glBindFramebuffer(GL_FRAMEBUFFER, m_FboID);
    glBindTexture(GL_TEXTURE_2D, m_TextureObj);
    // attach a texture to FBO color attachement point
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureObj, 0);
    CHECK_GL_FRAME_BUFFER_STATUS();
    glBindTexture(GL_TEXTURE_2D, 0);
    // attach a renderbuffer to depth attachment point
    glBindRenderbuffer(GL_RENDERBUFFER, m_RboID);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RboID);
    CHECK_GL_FRAME_BUFFER_STATUS();
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return 0;
}

void OpenGLRender::Release()
{
    glDeleteBuffers(1, &m_VertexBuffer);
    glDeleteBuffers(1, &m_ColorBuffer);
    glDeleteBuffers(1, &m_RenderVertexBuf);
    glDeleteBuffers(1, &m_RenderTexCoordBuf);
    glDeleteProgram(m_RenderProID);
    glDeleteProgram(m_CommonProID);
    glDeleteProgram(m_TextProID);
    glDeleteProgram(m_BackgroundProID);
    glDeleteFramebuffers(1, &m_FboID);
    glDeleteTextures(1, &m_TextureObj);
    glDeleteRenderbuffers(1, &m_RboID);
#if defined( WNT )
    wglMakeCurrent(NULL, NULL);
#elif defined( UNX )

    glXMakeCurrent(glWin.dpy, None, NULL);
    if( glGetError() != GL_NO_ERROR ) {
        SAL_INFO("chart2.opengl", "glError: " << (char *)gluErrorString(glGetError()));
    }
    glXDestroyContext(glWin.dpy, glWin.ctx);
    glWin.ctx = NULL;
    glWin.win = 0;

#endif
}

OpenGLRender::OpenGLRender(uno::Reference< drawing::XShape > xTarget):
    m_Model(glm::mat4(1.0f)),
    m_TextureObj(0),
    m_FboID(0),
    m_RboID(0),
    m_iWidth(0),
    m_iHeight(0),
    mxRenderTarget(xTarget),
    mbArbMultisampleSupported(false),
    m_2DColor(glm::vec4(1.0, 0.0, 0.0, 1.0)),
    m_TextVertexID(0),
    m_TextTexCoordID(1),
    m_ClearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
{
    //TODO: moggi: use STL
    memset(&m_Bubble2DCircle, 0, sizeof(m_Bubble2DCircle));

    m_iArbMultisampleFormat = 0;

    //TODO: moggi: use STL
    for (size_t i = 0; i < sizeof(m_BackgroundColor) / sizeof(float); i++)
    {
        m_BackgroundColor[i] = 1.0;
    }

    mxRenderTarget->setPosition(awt::Point(0,0));
}

OpenGLRender::~OpenGLRender()
{
    Release();
}

void OpenGLRender::SetWidth(int width)
{
    m_iWidth = width;
    m_Projection = glm::ortho(0.f, float(m_iWidth), 0.f, float(m_iHeight), -4.f, 3.f);
}

void OpenGLRender::SetHeight(int height)
{
    m_iHeight = height;
    m_Projection = glm::ortho(0.f, float(m_iWidth), 0.f, float(m_iHeight), -4.f, 3.f);
}

#if RENDER_TO_FILE
int OpenGLRender::CreateBMPHeader(sal_uInt8 *bmpHeader, int xsize, int ysize)
{
    unsigned char header[BMP_HEADER_LEN] = {
        0x42, 0x4d, 0, 0, 0, 0, 0, 0, 0, 0,
        54, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0
    };

    long file_size = (long)xsize * (long)ysize * 3 + 54;
    header[2] = (unsigned char)(file_size &0x000000ff);
    header[3] = (file_size >> 8) & 0x000000ff;
    header[4] = (file_size >> 16) & 0x000000ff;
    header[5] = (file_size >> 24) & 0x000000ff;

    long width = xsize;
    header[18] = width & 0x000000ff;
    header[19] = (width >> 8) &0x000000ff;
    header[20] = (width >> 16) &0x000000ff;
    header[21] = (width >> 24) &0x000000ff;

    long height = -ysize;
    header[22] = height &0x000000ff;
    header[23] = (height >> 8) &0x000000ff;
    header[24] = (height >> 16) &0x000000ff;
    header[25] = (height >> 24) &0x000000ff;
    memcpy(bmpHeader, header, BMP_HEADER_LEN);
    return 0;
}
#endif

void OpenGLRender::SetLine2DColor(sal_uInt8 r, sal_uInt8 g, sal_uInt8 b)
{
    m_2DColor = glm::vec4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, m_fAlpha);
}

void OpenGLRender::SetLine2DWidth(int width)
{
    m_fLineWidth = std::max((float)width / OPENGL_SCALE_VALUE, 0.001f);
}

#if defined( _WIN32 )
static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        return 0;
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
    case WM_DESTROY:
        return 0;
    case WM_KEYDOWN:
        switch(wParam)
        {
        case VK_ESCAPE:
            PostQuitMessage(0);
            return 0;

        case VK_SPACE:
            break;
        }
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
}

bool OpenGLRender::InitMultisample(PIXELFORMATDESCRIPTOR pfd)
{
    HWND hWnd = NULL;
    //create a temp windwo to check whether support multi-sample, if support, get the format
    if (InitTempWindow(&hWnd, m_iWidth, m_iHeight, pfd) < 0)
    {
        SAL_WARN("chart2.opengl", "Can't create temp window to test");
        return false;
    }

    // See If The String Exists In WGL!
    if (!WGLisExtensionSupported("WGL_ARB_multisample"))
    {
        mbArbMultisampleSupported = false;
        SAL_WARN("chart2.opengl", "Device doesn't support multi sample");
        return false;
    }
    // Get Our Pixel Format
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
    if (!wglChoosePixelFormatARB)
    {
        mbArbMultisampleSupported = false;
        return false;
    }
    // Get Our Current Device Context
    HDC hDC = GetDC(hWnd);

    int pixelFormat;
    int valid;
    UINT    numFormats;
    float   fAttributes[] = {0,0};
    // These Attributes Are The Bits We Want To Test For In Our Sample
    // Everything Is Pretty Standard, The Only One We Want To
    // Really Focus On Is The SAMPLE BUFFERS ARB And WGL SAMPLES
    // These Two Are Going To Do The Main Testing For Whether Or Not
    // We Support Multisampling On This Hardware.
    int iAttributes[] =
    {
        WGL_DRAW_TO_WINDOW_ARB,GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB,GL_TRUE,
        WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB,
        WGL_COLOR_BITS_ARB,24,
        WGL_ALPHA_BITS_ARB,8,
        WGL_DEPTH_BITS_ARB,16,
        WGL_STENCIL_BITS_ARB,0,
        WGL_DOUBLE_BUFFER_ARB,GL_TRUE,
        WGL_SAMPLE_BUFFERS_ARB,GL_TRUE,
        WGL_SAMPLES_ARB,8,
        0,0
    };
    // First We Check To See If We Can Get A Pixel Format For 4 Samples
    valid = wglChoosePixelFormatARB(hDC, iAttributes, fAttributes, 1, &pixelFormat, &numFormats);
    // If We Returned True, And Our Format Count Is Greater Than 1
    if (valid && numFormats >= 1)
    {
        mbArbMultisampleSupported = true;
        m_iArbMultisampleFormat = pixelFormat;
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(glWin.hRC);
        ReleaseDC(hWnd, glWin.hDC);
        DestroyWindow(hWnd);
        return mbArbMultisampleSupported;
    }
    // Our Pixel Format With 4 Samples Failed, Test For 2 Samples
    iAttributes[19] = 2;
    valid = wglChoosePixelFormatARB(hDC, iAttributes, fAttributes, 1, &pixelFormat, &numFormats);
    if (valid && numFormats >= 1)
    {
        mbArbMultisampleSupported = true;
        m_iArbMultisampleFormat = pixelFormat;
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(glWin.hRC);
        ReleaseDC(hWnd, glWin.hDC);
        DestroyWindow(hWnd);
        return mbArbMultisampleSupported;
    }
    // Return The Valid Format
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(glWin.hRC);
    ReleaseDC(hWnd, glWin.hDC);
    DestroyWindow(hWnd);
    return  mbArbMultisampleSupported;
}
#endif

bool OpenGLRender::GetMSAASupport()
{
    return mbArbMultisampleSupported;
}

int OpenGLRender::GetMSAAFormat()
{
    return m_iArbMultisampleFormat;
}

#if defined( _WIN32 )
//TODO: moggi: why the hell do we need another implementation here?
int OpenGLRender::InitTempWindow(HWND *hwnd, int width, int height, PIXELFORMATDESCRIPTOR inPfd)
{
    PIXELFORMATDESCRIPTOR  pfd = inPfd;
    int  pfmt;
    int ret;
    WNDCLASS wc;
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = wc.cbWndExtra = 0;
    wc.hInstance = NULL;
    wc.hIcon = NULL;
    wc.hCursor = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = (LPCSTR)"GLRenderer";
    RegisterClass(&wc);
    *hwnd = CreateWindow(wc.lpszClassName, NULL, WS_DISABLED, 0, 0, width, height, NULL, NULL, wc.hInstance, NULL);
    glWin.hDC = GetDC(*hwnd);
    pfmt = ChoosePixelFormat(glWin.hDC, &pfd);
    if (!pfmt)
    {
        return -1;
    }
    ret = SetPixelFormat(glWin.hDC, pfmt, &pfd);
    if(!ret)
    {
        return -1;
    }
    glWin.hRC = wglCreateContext(glWin.hDC);
    if(!(glWin.hRC))
    {
        return -1;
    }
    ret = wglMakeCurrent(glWin.hDC, glWin.hRC);
    if(!ret)
    {
        return -1;
    }
    return 0;
}

bool OpenGLRender::WGLisExtensionSupported(const char *extension)
{
    const size_t extlen = strlen(extension);
    const char *supported = NULL;

    // Try To Use wglGetExtensionStringARB On Current DC, If Possible
    PROC wglGetExtString = wglGetProcAddress("wglGetExtensionsStringARB");

    if (wglGetExtString)
        supported = ((char*(__stdcall*)(HDC))wglGetExtString)(wglGetCurrentDC());
    // If That Failed, Try Standard Opengl Extensions String
    if (supported == NULL)
        supported = (char*)glGetString(GL_EXTENSIONS);
    // If That Failed Too, Must Be No Extensions Supported
    if (supported == NULL)
        return 0;

    // Begin Examination At Start Of String, Increment By 1 On False Match
    for (const char* p = supported; ; p++)
    {
        // Advance p Up To The Next Possible Match
        p = strstr(p, extension);

        if (p == NULL)
            return 0; // No Match

        // Make Sure That Match Is At The Start Of The String Or That
        // The Previous Char Is A Space, Or Else We Could Accidentally
        // Match "wglFunkywglExtension" With "wglExtension"

        // Also, Make Sure That The Following Character Is Space Or NULL
        // Or Else "wglExtensionTwo" Might Match "wglExtension"
        if ((p==supported || p[-1]==' ') && (p[extlen]=='\0' || p[extlen]==' '))
            return 1; // Match
    }
    return 1;
}
#endif

void OpenGLRender::SetColor(sal_uInt32 color)
{
    sal_uInt8 r = (color & 0x00FF0000) >> 16;
    sal_uInt8 g = (color & 0x0000FF00) >> 8;
    sal_uInt8 b = (color & 0x000000FF);
    m_2DColor = glm::vec4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, m_fAlpha);
}

int OpenGLRender::CreateMultiSampleFrameBufObj()
{
    glGenFramebuffers(1, &m_frameBufferMS);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferMS);

    glGenRenderbuffers(1, &m_renderBufferColorMS);
    glBindRenderbuffer(GL_RENDERBUFFER, m_renderBufferColorMS);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8,  GL_RGB, m_iWidth, m_iHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_renderBufferColorMS);

    glGenRenderbuffers(1, &m_renderBufferDepthMS);
    glBindRenderbuffer(GL_RENDERBUFFER, m_renderBufferDepthMS);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_DEPTH_COMPONENT24, m_iWidth, m_iHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_renderBufferDepthMS);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return 0;
}

int OpenGLRender::Create2DCircle(int detail)
{
    float angle;
    int idx = 2;
    if (detail <= 0)
    {
        return -1;
    }
    m_Bubble2DCircle.bufLen = 2 * (detail + 3)* sizeof(float);
    m_Bubble2DCircle.pointBuf = (float *)malloc(m_Bubble2DCircle.bufLen);
    memset(m_Bubble2DCircle.pointBuf, 0, m_Bubble2DCircle.bufLen);
    for(angle = 2.0f * GL_PI; angle > -(2.0f * GL_PI / detail); angle -= (2.0f * GL_PI / detail))
    {
        m_Bubble2DCircle.pointBuf[idx++] = sin(angle);
        m_Bubble2DCircle.pointBuf[idx++] = cos(angle);
    }
    return 0;
}

int OpenGLRender::Bubble2DShapePoint(float x, float y, float directionX, float directionY)
{
    //check whether to create the circle data
    if (!m_Bubble2DCircle.pointBuf)
    {
        Create2DCircle(100);
    }

    float actualX = (x / OPENGL_SCALE_VALUE);
    float actualY = (y / OPENGL_SCALE_VALUE);
    Bubble2DPointList aBubble2DPointList;
    aBubble2DPointList.x = actualX;
    aBubble2DPointList.y = actualY;
    aBubble2DPointList.xScale = directionX / OPENGL_SCALE_VALUE;
    aBubble2DPointList.yScale = directionY / OPENGL_SCALE_VALUE;

    m_Bubble2DShapePointList.push_back(aBubble2DPointList);
    return 0;
}

int OpenGLRender::RenderBubble2FBO(int)
{
    CHECK_GL_ERROR();
    size_t listNum = m_Bubble2DShapePointList.size();
    for (size_t i = 0; i < listNum; i++)
    {
        //move the circle to the pos, and scale using the xScale and Y scale
        Bubble2DPointList &pointList = m_Bubble2DShapePointList.front();
        PosVecf3 trans = {pointList.x, pointList.y, m_fZStep};
        PosVecf3 angle = {0.0f, 0.0f, 0.0f};
        PosVecf3 scale = {pointList.xScale, pointList.yScale, 1.0f};
        MoveModelf(trans, angle, scale);
        m_MVP = m_Projection * m_View * m_Model;
        //render to fbo
        //fill vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
        if (!m_Bubble2DCircle.pointBuf)
        {
            Create2DCircle(100);
        }
        glBufferData(GL_ARRAY_BUFFER, m_Bubble2DCircle.bufLen, m_Bubble2DCircle.pointBuf, GL_STATIC_DRAW);

        glUseProgram(m_CommonProID);

        glUniform4fv(m_2DColorID, 1, &m_2DColor[0]);

        glUniformMatrix4fv(m_MatrixID, 1, GL_FALSE, &m_MVP[0][0]);
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(m_2DVertexID);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
        glVertexAttribPointer(
            m_2DVertexID,                  // attribute. No particular reason for 0, but must match the layout in the shader.
            2,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
            );
        glDrawArrays(GL_TRIANGLE_FAN, 0, m_Bubble2DCircle.bufLen / sizeof(float) / 2);
        glDisableVertexAttribArray(m_2DVertexID);
        glUseProgram(0);
        m_Bubble2DShapePointList.pop_front();
    }
    //if use MSAA, we should copy the data to the FBO texture
    GLenum fbResult = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if( fbResult != GL_FRAMEBUFFER_COMPLETE )
    {
        SAL_WARN("chart2.opengl", "error");
        return -1;
    }
    CHECK_GL_ERROR();
    return 0;
}

void OpenGLRender::SetTransparency(sal_uInt32 transparency)
{
    m_fAlpha = (float)transparency / 255.0;
    m_2DColor = glm::vec4(m_2DColor.r, m_2DColor.g, m_2DColor.b, m_fAlpha);
}

int OpenGLRender::RectangleShapePoint(float x, float y, float directionX, float directionY)
{
    //check whether to create the circle data
    float actualX = x / OPENGL_SCALE_VALUE;
    float actualY = y / OPENGL_SCALE_VALUE;
    float actualSizeX = directionX / OPENGL_SCALE_VALUE;
    float actualSizeY = directionY / OPENGL_SCALE_VALUE;
    RectanglePointList aRectangle;

    aRectangle.points[0] = actualX;
    aRectangle.points[1] = actualY;
    aRectangle.points[2] = m_fZStep;
    aRectangle.points[3] = actualX + actualSizeX;
    aRectangle.points[4] = actualY;
    aRectangle.points[5] = m_fZStep;
    aRectangle.points[6] = actualX + actualSizeX;
    aRectangle.points[7] = actualY + actualSizeY;
    aRectangle.points[8] = m_fZStep;
    aRectangle.points[9] = actualX;
    aRectangle.points[10] = actualY + actualSizeY;
    aRectangle.points[11] = m_fZStep;

    m_RectangleShapePointList.push_back(aRectangle);
    return 0;
}


int OpenGLRender::RenderRectangleShape(bool bBorder, bool bFill)
{
    m_fZStep += 0.001;
    size_t listNum = m_RectangleShapePointList.size();
    for (size_t i = 0; i < listNum; i++)
    {
        //move the circle to the pos, and scale using the xScale and Y scale
        RectanglePointList &pointList = m_RectangleShapePointList.front();
        PosVecf3 trans = {0, 0, 0};
        PosVecf3 angle = {0.0f, 0.0f, 0.0f};
        PosVecf3 scale = {1, 1, 1.0f};
        MoveModelf(trans, angle, scale);
        m_MVP = m_Projection * m_View * m_Model;

        //render to fbo
        //fill vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pointList.points), pointList.points, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, m_ColorBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_BackgroundColor), m_BackgroundColor, GL_STATIC_DRAW);
        glUseProgram(m_BackgroundProID);

        glUniformMatrix4fv(m_BackgroundMatrixID, 1, GL_FALSE, &m_MVP[0][0]);
        if(bFill)
        {
            glEnableVertexAttribArray(m_BackgroundVertexID);
            glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
            glVertexAttribPointer(
                    m_BackgroundVertexID,                  // attribute. No particular reason for 0, but must match the layout in the shader.
                    3,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
                    );

            // 2nd attribute buffer : color
            glEnableVertexAttribArray(m_BackgroundColorID);
            glBindBuffer(GL_ARRAY_BUFFER, m_ColorBuffer);
            glVertexAttribPointer(
                    m_BackgroundColorID,                  // attribute. No particular reason for 0, but must match the layout in the shader.
                    4,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
                    );
            //TODO: moggi: get rid of GL_QUADS
            glDrawArrays(GL_QUADS, 0, 4);
            glDisableVertexAttribArray(m_BackgroundVertexID);
            glDisableVertexAttribArray(m_BackgroundColorID);
        }
        if(bBorder)
        {
            SetBackGroundColor(COL_BLACK, COL_BLACK);

            glBindBuffer(GL_ARRAY_BUFFER, m_ColorBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(m_BackgroundColor), m_BackgroundColor, GL_STATIC_DRAW);
            // 1rst attribute buffer : vertices
            glEnableVertexAttribArray(m_BackgroundVertexID);
            glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
            glVertexAttribPointer(
                    m_BackgroundVertexID,                  // attribute. No particular reason for 0, but must match the layout in the shader.
                    3,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
                    );

            // 2nd attribute buffer : color
            glEnableVertexAttribArray(m_BackgroundColorID);
            glBindBuffer(GL_ARRAY_BUFFER, m_ColorBuffer);
            glVertexAttribPointer(
                    m_BackgroundColorID,                  // attribute. No particular reason for 0, but must match the layout in the shader.
                    4,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
                    );
            glDrawArrays(GL_LINE_LOOP, 0, 4);
            glDisableVertexAttribArray(m_BackgroundVertexID);
            glDisableVertexAttribArray(m_BackgroundColorID);
        }
        glDisableVertexAttribArray(m_BackgroundVertexID);
        glDisableVertexAttribArray(m_BackgroundColorID);
        glUseProgram(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        m_RectangleShapePointList.pop_front();
    }
    CHECK_GL_ERROR();
    return 0;
}


int OpenGLRender::CreateTextTexture(::rtl::OUString textValue, sal_uInt32 color, const Font& rFont, awt::Point aPos, awt::Size aSize, long rotation)
{
    VirtualDevice aDevice(*Application::GetDefaultDevice(), 0, 0);
    aDevice.Erase();
    Rectangle aRect;
    aDevice.SetFont(rFont);
    aDevice.GetTextBoundRect(aRect, textValue);
    int screenWidth = (aRect.BottomRight().X() + 3) & ~3;
    int screenHeight = (aRect.BottomRight().Y() + 3) & ~3;
    aDevice.SetOutputSizePixel(Size(screenWidth * 3, screenHeight));
    aDevice.SetBackground(Wallpaper(COL_TRANSPARENT));
    aDevice.DrawText(Point(0, 0), textValue);
    int bmpWidth = (aRect.Right() - aRect.Left() + 3) & ~3;
    int bmpHeight = (aRect.Bottom() - aRect.Top() + 3) & ~3;
    BitmapEx aBitmapEx(aDevice.GetBitmapEx(aRect.TopLeft(), Size(bmpWidth, bmpHeight)));

#if DEBUG_PNG // debug PNG writing
    static int nIdx = 0;
    OUString aName = OUString( "file:///home/moggi/Documents/work/text" ) + OUString::number( nIdx++ ) + ".png";
    try {
        vcl::PNGWriter aWriter( aBitmapEx );
        SvFileStream sOutput( aName, STREAM_WRITE );
        aWriter.Write( sOutput );
        sOutput.Close();
    } catch (...) {
        SAL_WARN("slideshow.opengl", "Error writing png to " << aName);
    }
#endif

    Bitmap aBitmap (aBitmapEx.GetBitmap());
    AlphaMask aAlpha (aBitmapEx.GetAlpha());
    boost::scoped_array<sal_uInt8> bitmapBuf(new sal_uInt8[4* bmpWidth * bmpHeight ]);
    Bitmap::ScopedReadAccess pReadAccces( aBitmap );
    AlphaMask::ScopedReadAccess pAlphaReadAccess( aAlpha );

    size_t i = 0;
    for (long ny = 0; ny < bmpHeight; ny++)
    {
        Scanline pAScan = pAlphaReadAccess->GetScanline(ny);
        for(long nx = 0; nx < bmpWidth; nx++)
        {
            BitmapColor aCol = pReadAccces->GetColor( ny, nx );
            bitmapBuf[i++] = aCol.GetRed();
            bitmapBuf[i++] = aCol.GetGreen();
            bitmapBuf[i++] = aCol.GetBlue();
            bitmapBuf[i++] = *pAScan++;
        }
    }

    TextInfo aTextInfo;
    aTextInfo.x = (float)(aPos.X + aSize.Width / 2) / OPENGL_SCALE_VALUE;
    aTextInfo.y = (float)(aPos.Y + aSize.Height / 2) / OPENGL_SCALE_VALUE;
    aTextInfo.z = m_fZStep;
    aTextInfo.rotation = -(double)rotation * GL_PI / 18000.0f;
    aTextInfo.vertex[0] = (float)(-aSize.Width / 2) / OPENGL_SCALE_VALUE;
    aTextInfo.vertex[1] = (float)(-aSize.Height / 2) / OPENGL_SCALE_VALUE;

    aTextInfo.vertex[2] = (float)(aSize.Width / 2) / OPENGL_SCALE_VALUE;
    aTextInfo.vertex[3] = (float)(-aSize.Height / 2) / OPENGL_SCALE_VALUE;

    aTextInfo.vertex[4] = (float)(aSize.Width / 2) / OPENGL_SCALE_VALUE;
    aTextInfo.vertex[5] = (float)(aSize.Height / 2) / OPENGL_SCALE_VALUE;

    aTextInfo.vertex[6] = (float)(-aSize.Width / 2) / OPENGL_SCALE_VALUE;
    aTextInfo.vertex[7] = (float)(aSize.Height / 2) / OPENGL_SCALE_VALUE;

    //if has ratotion, we must re caculate the central pos
    if (!rtl::math::approxEqual(0, rotation))
    {
        //use left top
        double r = sqrt((double)(aSize.Width * aSize.Width + aSize.Height * aSize.Height)) / 2;
        double sinOrgAngle =  aTextInfo.vertex[1] / r / 2;
        double cosOrgAngle = aTextInfo.vertex[0] / r / 2;
        double sinDiataAngle = sin(aTextInfo.rotation);
        double cosDiataAngle = cos(aTextInfo.rotation);
        double x = r * (cosOrgAngle * cosDiataAngle - sinOrgAngle * sinDiataAngle);
        double y = r * (sinOrgAngle * cosDiataAngle + cosOrgAngle * sinDiataAngle);
        double diataX = x - aTextInfo.vertex[0];
        double diataY = y - aTextInfo.vertex[1];
        aTextInfo.x = aTextInfo.x - diataX;
        aTextInfo.y = aTextInfo.y - diataY;

    }

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
    aDevice.Erase();
    return 0;
}

int OpenGLRender::RenderTextShape()
{
    CHECK_GL_ERROR();
    m_fZStep += 0.01f;
    size_t listNum = m_TextInfoList.size();
    for (size_t i = 0; i < listNum; i++)
    {
        TextInfo &textInfo = m_TextInfoList.front();
        PosVecf3 trans = {textInfo.x, textInfo.y, textInfo.z};
        PosVecf3 angle = {0.0f, 0.0f, float(textInfo.rotation)};
        PosVecf3 scale = {1.0, 1.0, 1.0f};
        MoveModelf(trans, angle, scale);
        m_MVP = m_Projection * m_View * m_Model;
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(textInfo.vertex), textInfo.vertex, GL_STATIC_DRAW);
        glUseProgram(m_TextProID);

        glUniformMatrix4fv(m_TextMatrixID, 1, GL_FALSE, &m_MVP[0][0]);
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(m_TextVertexID);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
        glVertexAttribPointer(
            m_TextVertexID,                  // attribute. No particular reason for 0, but must match the layout in the shader.
            2,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
            );
        //tex coord
        glEnableVertexAttribArray(m_TextTexCoordID);
        glBindBuffer(GL_ARRAY_BUFFER, m_TextTexCoordBuf);
        glVertexAttribPointer(
            m_TextTexCoordID,                  // attribute. No particular reason for 0, but must match the layout in the shader.
            2,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
            );
        //texture
        glBindTexture(GL_TEXTURE_2D, textInfo.texture);
        glUniform1i(m_TextTexID, 0);
        //TODO: moggi: get rid fo GL_QUADS
        glDrawArrays(GL_QUADS, 0, 4);
        glDisableVertexAttribArray(m_TextTexCoordID);
        glDisableVertexAttribArray(m_TextVertexID);
        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);
        glDeleteTextures(1, &textInfo.texture);
        m_TextInfoList.pop_front();
    }
    CHECK_GL_ERROR();
    return 0;
}

int OpenGLRender::SetArea2DShapePoint(float x, float y, int listLength)
{
    if (m_Area2DPointList.empty())
    {
        m_Area2DPointList.reserve(listLength);
    }
    float actualX = (x / OPENGL_SCALE_VALUE);
    float actualY = (y / OPENGL_SCALE_VALUE);
    m_Area2DPointList.push_back(actualX);
    m_Area2DPointList.push_back(actualY);
    m_Area2DPointList.push_back(m_fZStep);

    if (m_Area2DPointList.size() == size_t(listLength * 3))
    {
        m_Area2DShapePointList.push_back(m_Area2DPointList);
        m_Area2DPointList.clear();
    }
    return 0;
}

int OpenGLRender::RenderArea2DShape()
{
    CHECK_GL_ERROR();

    glDisable(GL_MULTISAMPLE);
    size_t listNum = m_Area2DShapePointList.size();
    PosVecf3 trans = {0.0f, 0.0f, 0.0f};
    PosVecf3 angle = {0.0f, 0.0f, 0.0f};
    PosVecf3 scale = {1.0f, 1.0f, 1.0f};
    MoveModelf(trans, angle, scale);
    m_MVP = m_Projection * m_View * m_Model;
    for (size_t i = 0; i < listNum; ++i)
    {
        Area2DPointList &pointList = m_Area2DShapePointList.front();
        //fill vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, pointList.size() * sizeof(float), &pointList[0], GL_STATIC_DRAW);
        // Use our shader
        glUseProgram(m_CommonProID);

        glUniform4fv(m_2DColorID, 1, &m_2DColor[0]);

        glUniformMatrix4fv(m_MatrixID, 1, GL_FALSE, &m_MVP[0][0]);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(m_2DVertexID);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
        glVertexAttribPointer(
            m_2DVertexID,                  // attribute. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
            );
        glDrawArrays(GL_POLYGON, 0, pointList.size() / 3); // 12*3 indices starting at 0 -> 12 triangles
        glDisableVertexAttribArray(m_2DVertexID);
        glUseProgram(0);
        m_Area2DShapePointList.pop_front();
    }
    glEnable(GL_MULTISAMPLE);
    m_fZStep += 0.01f;

    CHECK_GL_ERROR();

    return 0;
}

void OpenGLRender::SetBackGroundColor(sal_uInt32 color1, sal_uInt32 color2)
{
    sal_uInt8 r = (color1 & 0x00FF0000) >> 16;
    sal_uInt8 g = (color1 & 0x0000FF00) >> 8;
    sal_uInt8 b = (color1 & 0x000000FF);

    m_BackgroundColor[0] = (float)r / 255.0f;
    m_BackgroundColor[1] = (float)g / 255.0f;
    m_BackgroundColor[2] = (float)b / 255.0f;
    m_BackgroundColor[3] = m_fAlpha;

    m_BackgroundColor[4] = (float)r / 255.0f;
    m_BackgroundColor[5] = (float)g / 255.0f;
    m_BackgroundColor[6] = (float)b / 255.0f;
    m_BackgroundColor[7] = m_fAlpha;

    r = (color2 & 0x00FF0000) >> 16;
    g = (color2 & 0x0000FF00) >> 8;
    b = (color2 & 0x000000FF);

    m_BackgroundColor[8] = (float)r / 255.0f;
    m_BackgroundColor[9] = (float)g / 255.0f;
    m_BackgroundColor[10] = (float)b / 255.0f;
    m_BackgroundColor[11] = 1.0;

    m_BackgroundColor[12] = (float)r / 255.0f;
    m_BackgroundColor[13] = (float)g / 255.0f;
    m_BackgroundColor[14] = (float)b / 255.0f;
    m_BackgroundColor[15] = 1.0;
    SAL_INFO("chart2.opengl", "color1 = " << color1 << ", color2 = " << color2);

}

void OpenGLRender::SetChartTransparencyGradient(long transparencyGradient)
{
    if (transparencyGradient == 1)
    {
        m_BackgroundColor[11] = 0.0;
        m_BackgroundColor[15] = 0.0;
    }
}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
