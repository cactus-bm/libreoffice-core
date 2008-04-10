/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: OGLTrans_TransitionImpl.hxx,v $
 * $Revision: 1.4 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
#ifndef INCLUDED_OGLTRANS_TRANSITIONIMPL_HXX_
#define INCLUDED_OGLTRANS_TRANSITIONIMPL_HXX_

#include <basegfx/vector/b2dvector.hxx>
#include <basegfx/vector/b3dvector.hxx>

#include <vector>
#include <GL/gl.h>

class Primitive;
class Operation;
class SceneObject;

/** OpenGL 3D Transition class. It implicitly is constructed from XOGLTransition

    This class is capable of making itself into many difference transitions. It holds Primitives and Operations on those primitives.
*/
class OGLTransitionImpl
{
public:
    OGLTransitionImpl() :
        maLeavingSlidePrimitives(),
        maEnteringSlidePrimitives(),
        maSceneObjects()
    {}

    ~OGLTransitionImpl();

    void prepare();
    void display( double nTime, ::sal_Int32 glLeavingSlideTex, ::sal_Int32 glEnteringSlideTex, double SlideWidth, double SlideHeight, double DispWidth, double DispHeight);
    void finish();

    void makeOutsideCubeFaceToLeft();
    void makeInsideCubeFaceToLeft();
    void makeNByMTileFlip( ::sal_uInt16 n, ::sal_uInt16 m );
    void makeRevolvingCircles( ::sal_uInt16 nCircles , ::sal_uInt16 nPointsOnCircles );
    void makeHelix( ::sal_uInt16 nRows );
    void makeFallLeaving();
    void makeTurnAround();
    void makeTurnDown();
    void makeIris();
    void makeRochade();

private:
    /** clears all the primitives and operations
    */
    void clear();

    /** All the primitives that use the leaving slide texture
    */
    std::vector<Primitive> maLeavingSlidePrimitives;

    /** All the primitives that use the leaving slide texture
    */
    std::vector<Primitive> maEnteringSlidePrimitives;

    /** All the surrounding scene objects
    */
    std::vector<SceneObject*> maSceneObjects;

    /** All the operations that should be applied to both leaving and entering slide primitives. These operations will be called in the order they were pushed back in. In OpenGL this effectively uses the operations in the opposite order they were pushed back.
    */
    std::vector<Operation*> OverallOperations;
};

class SceneObject
{
public:
    SceneObject();

    virtual void prepare() {};
    virtual void display(double nTime, double SlideWidth, double SlideHeight, double DispWidth, double DispHeight);
    virtual void finish() {};

    void pushPrimitive (const Primitive &p);

protected:
    /** All the surrounding scene primitives
    */
    std::vector<Primitive> maPrimitives;
};

class Iris : public SceneObject
{
public:
    Iris ();

    virtual void prepare();
    virtual void display(double nTime, double SlideWidth, double SlideHeight, double DispWidth, double DispHeight);
    virtual void finish();

private:

    GLuint maTexture;
};

/** This class is a list of Triangles that will share Operations, and could possibly share
*/
class Primitive
{
public:
    Primitive() {}
    // making copy constructor explicit makes the class un-suitable for use with stl containers
    Primitive(const Primitive& rvalue);
    ~Primitive();

    void display(double nTime, double SlideWidthScale, double SlideHeightScale);
    const Primitive& operator=(const Primitive& rvalue);

    /** PushBack a vertex,normal, and tex coord. Each SlideLocation is where on the slide is mapped to this location ( from (0,0) to (1,1)  ). This will make sure the correct aspect ratio is used, and helps to make slides begin and end at the correct position. (0,0) is the top left of the slide, and (1,1) is the bottom right.

    @param SlideLocation0
    Location of first Vertex on slide

    @param SlideLocation1
    Location of second Vertex on slide

    @param SlideLocation2
    Location of third Vertex on slide

    */
    void pushTriangle(const basegfx::B2DVector& SlideLocation0,const basegfx::B2DVector& SlideLocation1,const basegfx::B2DVector& SlideLocation2);

    /** clear all the vertices, normals, tex coordinates, and normals
    */
    void clearTriangles();

    /** guards against directly changing the vertices

        @return
        the list of vertices
    */
    const std::vector<basegfx::B3DVector>& getVertices() const {return Vertices;}

    /** guards against directly changing the vertices
    */
    const std::vector<basegfx::B3DVector>& getNormals() const {return Normals;}

    /** guards against directly changing the vertices

        @return
        the list of Texture Coordinates

    */
    const std::vector<basegfx::B2DVector>& getTexCoords() const {return TexCoords;}

    /** list of Operations to be performed on this primitive.These operations will be called in the order they were pushed back in. In OpenGL this effectively uses the operations in the opposite order they were pushed back.

        @return
        the list of Operations

    */
    std::vector<Operation*> Operations;

private:
    /** list of vertices
    */
    std::vector<basegfx::B3DVector> Vertices;

    /** list of Normals
    */
    std::vector<basegfx::B3DVector> Normals;

    /** list of Texture Coordinates
    */
    std::vector<basegfx::B2DVector> TexCoords;
};

/** This class is to be derived to make any operation (tranform) you may need in order to construct your transitions
*/
class Operation
{
public:
    Operation(){}
    virtual ~Operation(){}

    /** Should this operation be interpolated . If TRUE, the transform will smoothly move from making no difference from t = 0.0 to nT0 to being completely transformed from t = nT1 to 1. If FALSE, the transform will be inneffectual from t = 0 to nT0, and completely transformed from t = nT0 to 1.
    */
    bool bInterpolate;

    /** time to begin the transformation
    */
    double nT0;

    /** time to finish the transformation
    */
    double nT1;
public:
    /** this is the function that is called to give the Operation to OpenGL.

        @param t
        time from t = 0 to t = 1

        @param SlideWidthScale
        width of slide divided by width of window

        @param SlideHeightScale
        height of slide divided by height of window

    */
    virtual void interpolate(double t,double SlideWidthScale,double SlideHeightScale) = 0;

    /** return a copy of this operation
    */
    virtual Operation* clone() = 0;
};

/** this class is a generic CounterClockWise(CCW) rotation with an axis angle
*/
class SRotate: public Operation
{
public:
    void interpolate(double t,double SlideWidthScale,double SlideHeightScale);
    virtual SRotate* clone();

    /** Constructor

        @param Axis
        axis to rotate about

        @param Origin
        position that rotation axis runs through

        @param Angle
        angle in radians of CCW rotation

        @param bInter
        see Operation

        @param T0
        transformation starting time

        @param T1
        transformation ending time

    */
    SRotate(const basegfx::B3DVector& Axis,const basegfx::B3DVector& Origin,double Angle,bool bInter, double T0, double T1);
    ~SRotate(){}
private:
    /** axis to rotate CCW about
    */
    basegfx::B3DVector axis;

    /** position that rotation axis runs through
    */
    basegfx::B3DVector origin;

    /** angle in radians of CCW rotation
    */
    double angle;
};

/** scaling transformation
*/
class SScale: public Operation
{
public:
    void interpolate(double t,double SlideWidthScale,double SlideHeightScale);
    SScale* clone();

    /** Constructor

        @param Scale
        amount to scale by

        @param Origin
        position that rotation axis runs through

        @param bInter
        see Operation

        @param T0
        transformation starting time

        @param T1
        transformation ending time

    */
    SScale(const basegfx::B3DVector& Scale, const basegfx::B3DVector& Origin,bool bInter, double T0, double T1);
    ~SScale(){}
private:
    basegfx::B3DVector scale;
    basegfx::B3DVector origin;
};

/** translation transformation
*/
class STranslate: public Operation
{
public:
    void interpolate(double t,double SlideWidthScale,double SlideHeightScale);
    STranslate* clone();

    /** Constructor

        @param Vector
        vector to translate

        @param bInter
        see Operation

        @param T0
        transformation starting time

        @param T1
        transformation ending time

    */
    STranslate(const basegfx::B3DVector& Vector,bool bInter, double T0, double T1);
    ~STranslate(){}
private:
    /** vector to translate by
    */
    basegfx::B3DVector vector;
};

/** translation transformation
*/
class SEllipseTranslate: public Operation
{
public:
    void interpolate(double t,double SlideWidthScale,double SlideHeightScale);
    SEllipseTranslate* clone();

    /** Constructor

        @param Vector
        vector to translate

        @param bInter
        see Operation

        @param T0
        transformation starting time

        @param T1
        transformation ending time

    */
    SEllipseTranslate(double dWidth, double dHeight, double dStartPosition, double dEndPosition, bool bInter, double T0, double T1);
    ~SEllipseTranslate(){}
private:
    /** width and length of the ellipse
     */
    double width, height;

    /** start and end position on the ellipse <0,1>
     */
    double startPosition;
    double endPosition;
};

/** Same as SRotate, except the depth is scaled by the width of the slide divided by the width of the window.
*/
class RotateAndScaleDepthByWidth: public Operation
{
public:
    void interpolate(double t,double SlideWidthScale,double SlideHeightScale);
    RotateAndScaleDepthByWidth* clone();

    RotateAndScaleDepthByWidth(const basegfx::B3DVector& Axis,const basegfx::B3DVector& Origin,double Angle,bool bInter, double T0, double T1);
    ~RotateAndScaleDepthByWidth(){}
private:
    basegfx::B3DVector axis;
    basegfx::B3DVector origin;
    double angle;
};

#endif // INCLUDED_SLIDESHOW_TRANSITION_HXX_

