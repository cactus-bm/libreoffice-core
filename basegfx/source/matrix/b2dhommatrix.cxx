/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: b2dhommatrix.cxx,v $
 *
 *  $Revision: 1.16 $
 *
 *  last change: $Author: vg $ $Date: 2007-02-05 12:50:26 $
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
#include "precompiled_basegfx.hxx"

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif

#ifndef INCLUDED_RTL_INSTANCE_HXX
#include <rtl/instance.hxx>
#endif

#ifndef _BGFX_MATRIX_B2DHOMMATRIX_HXX
#include <basegfx/matrix/b2dhommatrix.hxx>
#endif

#ifndef _HOMMATRIX_TEMPLATE_HXX
#include <hommatrixtemplate.hxx>
#endif

#ifndef _BGFX_MATRIX_B3DHOMMATRIX_HXX
#include <basegfx/matrix/b3dhommatrix.hxx>
#endif

#ifndef _BGFX_TUPLE_B3DTUPLE_HXX
#include <basegfx/tuple/b3dtuple.hxx>
#endif

#ifndef _BGFX_TUPLE_B2DTUPLE_HXX
#include <basegfx/tuple/b2dtuple.hxx>
#endif

#ifndef _BGFX_VECTOR_B2DVECTOR_HXX
#include <basegfx/vector/b2dvector.hxx>
#endif

namespace basegfx
{
    class Impl2DHomMatrix : public ::basegfx::internal::ImplHomMatrixTemplate< 3 >
    {
    };

    namespace { struct IdentityMatrix : public rtl::Static< B2DHomMatrix::ImplType,
                                                            IdentityMatrix > {}; }

    B2DHomMatrix::B2DHomMatrix() :
        mpImpl( IdentityMatrix::get() ) // use common identity matrix
    {
    }

    B2DHomMatrix::B2DHomMatrix(const B2DHomMatrix& rMat) :
        mpImpl(rMat.mpImpl)
    {
    }

    B2DHomMatrix::~B2DHomMatrix()
    {
    }

    B2DHomMatrix& B2DHomMatrix::operator=(const B2DHomMatrix& rMat)
    {
        mpImpl = rMat.mpImpl;
        return *this;
    }

    void B2DHomMatrix::makeUnique()
    {
        mpImpl.make_unique();
    }

    double B2DHomMatrix::get(sal_uInt16 nRow, sal_uInt16 nColumn) const
    {
        return mpImpl->get(nRow, nColumn);
    }

    void B2DHomMatrix::set(sal_uInt16 nRow, sal_uInt16 nColumn, double fValue)
    {
        mpImpl->set(nRow, nColumn, fValue);
    }

    bool B2DHomMatrix::isLastLineDefault() const
    {
        return mpImpl->isLastLineDefault();
    }

    bool B2DHomMatrix::isIdentity() const
    {
        if(mpImpl.same_object(IdentityMatrix::get()))
            return true;

        return mpImpl->isIdentity();
    }

    void B2DHomMatrix::identity()
    {
        mpImpl = IdentityMatrix::get();
    }

    bool B2DHomMatrix::isInvertible() const
    {
        return mpImpl->isInvertible();
    }

    bool B2DHomMatrix::invert()
    {
        Impl2DHomMatrix aWork(*mpImpl);
        sal_uInt16* pIndex = new sal_uInt16[mpImpl->getEdgeLength()];
        sal_Int16 nParity;

        if(aWork.ludcmp(pIndex, nParity))
        {
            mpImpl->doInvert(aWork, pIndex);
            delete[] pIndex;

            return true;
        }

        delete[] pIndex;
        return false;
    }

    bool B2DHomMatrix::isNormalized() const
    {
        return mpImpl->isNormalized();
    }

    void B2DHomMatrix::normalize()
    {
        if(!const_cast<const B2DHomMatrix*>(this)->mpImpl->isNormalized())
            mpImpl->doNormalize();
    }

    double B2DHomMatrix::determinant() const
    {
        return mpImpl->doDeterminant();
    }

    double B2DHomMatrix::trace() const
    {
        return mpImpl->doTrace();
    }

    void B2DHomMatrix::transpose()
    {
        mpImpl->doTranspose();
    }

    B2DHomMatrix& B2DHomMatrix::operator+=(const B2DHomMatrix& rMat)
    {
        mpImpl->doAddMatrix(*rMat.mpImpl);
        return *this;
    }

    B2DHomMatrix& B2DHomMatrix::operator-=(const B2DHomMatrix& rMat)
    {
        mpImpl->doSubMatrix(*rMat.mpImpl);
        return *this;
    }

    B2DHomMatrix& B2DHomMatrix::operator*=(double fValue)
    {
        const double fOne(1.0);

        if(!fTools::equal(fOne, fValue))
            mpImpl->doMulMatrix(fValue);

        return *this;
    }

    B2DHomMatrix& B2DHomMatrix::operator/=(double fValue)
    {
        const double fOne(1.0);

        if(!fTools::equal(fOne, fValue))
            mpImpl->doMulMatrix(1.0 / fValue);

        return *this;
    }

    B2DHomMatrix& B2DHomMatrix::operator*=(const B2DHomMatrix& rMat)
    {
        if(!rMat.isIdentity())
            mpImpl->doMulMatrix(*rMat.mpImpl);

        return *this;
    }

    bool B2DHomMatrix::operator==(const B2DHomMatrix& rMat) const
    {
        if(mpImpl.same_object(rMat.mpImpl))
            return true;

        return mpImpl->isEqual(*rMat.mpImpl);
    }

    bool B2DHomMatrix::operator!=(const B2DHomMatrix& rMat) const
    {
        return !(*this == rMat);
    }

    void B2DHomMatrix::rotate(double fRadiant)
    {
        if(!fTools::equalZero(fRadiant))
        {
            double fSin;
            double fCos;

            // is the rotation angle an approximate multiple of pi/2?
            // If yes, force fSin/fCos to -1/0/1, to maintain
            // orthogonality (which might also be advantageous for the
            // other cases, but: for multiples of pi/2, the exact
            // values _can_ be attained. It would be largely
            // unintuitive, if a 180 degrees rotation would introduce
            // slight roundoff errors, instead of exactly mirroring
            // the coordinate system).
            if( fTools::equalZero( fmod( fRadiant, F_PI2 ) ) )
            {
                // determine quadrant
                const sal_Int32 nQuad(
                    (4 + fround( 4/F_2PI*fmod( fRadiant, F_2PI ) )) % 4 );
                switch( nQuad )
                {
                    case 0: // -2pi,0,2pi
                        fSin = 0.0;
                        fCos = 1.0;
                        break;

                    case 1: // -3/2pi,1/2pi
                        fSin = 1.0;
                        fCos = 0.0;
                        break;

                    case 2: // -pi,pi
                        fSin = 0.0;
                        fCos = -1.0;
                        break;

                    case 3: // -1/2pi,3/2pi
                        fSin = -1.0;
                        fCos = 0.0;
                        break;

                    default:
                        OSL_ENSURE( false,
                                    "B2DHomMatrix::rotate(): Impossible case reached" );
                }
            }
            else
            {
                // TODO(P1): Maybe use glibc's sincos here (though
                // that's kinda non-portable...)
                fSin = sin(fRadiant);
                fCos = cos(fRadiant);
            }

            Impl2DHomMatrix aRotMat;

            aRotMat.set(0, 0, fCos);
            aRotMat.set(1, 1, fCos);
            aRotMat.set(1, 0, fSin);
            aRotMat.set(0, 1, -fSin);

            mpImpl->doMulMatrix(aRotMat);
        }
    }

    void B2DHomMatrix::translate(double fX, double fY)
    {
        if(!fTools::equalZero(fX) || !fTools::equalZero(fY))
        {
            Impl2DHomMatrix aTransMat;

            aTransMat.set(0, 2, fX);
            aTransMat.set(1, 2, fY);

            mpImpl->doMulMatrix(aTransMat);
        }
    }

    void B2DHomMatrix::scale(double fX, double fY)
    {
        const double fOne(1.0);

        if(!fTools::equal(fOne, fX) || !fTools::equal(fOne, fY))
        {
            Impl2DHomMatrix aScaleMat;

            aScaleMat.set(0, 0, fX);
            aScaleMat.set(1, 1, fY);

            mpImpl->doMulMatrix(aScaleMat);
        }
    }

    void B2DHomMatrix::shearX(double fSx)
    {
        const double fOne(1.0);

        if(!fTools::equal(fOne, fSx))
        {
            Impl2DHomMatrix aShearXMat;

            aShearXMat.set(0, 1, fSx);

            mpImpl->doMulMatrix(aShearXMat);
        }
    }

    void B2DHomMatrix::shearY(double fSy)
    {
        const double fOne(1.0);

        if(!fTools::equal(fOne, fSy))
        {
            Impl2DHomMatrix aShearYMat;

            aShearYMat.set(1, 0, fSy);

            mpImpl->doMulMatrix(aShearYMat);
        }
    }

    // Decomposition
    bool B2DHomMatrix::decompose(B2DTuple& rScale, B2DTuple& rTranslate, double& rRotate, double& rShearX) const
    {
        // when perspective is used, decompose is not made here
        if(!mpImpl->isLastLineDefault())
            return false;

        // test for rotation and shear
        if(fTools::equalZero(get(0, 1))
           && fTools::equalZero(get(1, 0)))
        {
            // no rotation and shear, direct value extraction
            rRotate = rShearX = 0.0;

            // copy scale values
            rScale.setX(get(0, 0));
            rScale.setY(get(1, 1));

            // copy translation values
            rTranslate.setX(get(0, 2));
            rTranslate.setY(get(1, 2));

            return true;
        }
        else
        {
            // test if shear is zero. That's the case, if the unit vectors in the matrix
            // are perpendicular -> scalar is zero
            const ::basegfx::B2DVector aUnitVecX(get(0, 0), get(1, 0));
            const ::basegfx::B2DVector aUnitVecY(get(0, 1), get(1, 1));

            if(fTools::equalZero(aUnitVecX.scalar(aUnitVecY)))
            {
                // no shear, direct value extraction
                rShearX = 0.0;

                // calculate rotation
                rRotate = atan2(aUnitVecX.getY(), aUnitVecX.getX());

                // calculate scale values
                rScale.setX(aUnitVecX.getLength());
                rScale.setY(aUnitVecY.getLength());

                // copy translation values
                rTranslate.setX(get(0, 2));
                rTranslate.setY(get(1, 2));

                return true;
            }
            else
            {
                // If determinant is zero, decomposition is not possible
                if(0.0 == determinant())
                    return false;

                // copy 2x2 matrix and translate vector to 3x3 matrix
                ::basegfx::B3DHomMatrix a3DHomMat;

                a3DHomMat.set(0, 0, get(0, 0));
                a3DHomMat.set(0, 1, get(0, 1));
                a3DHomMat.set(1, 0, get(1, 0));
                a3DHomMat.set(1, 1, get(1, 1));
                a3DHomMat.set(0, 3, get(0, 2));
                a3DHomMat.set(1, 3, get(1, 2));

                ::basegfx::B3DTuple r3DScale, r3DTranslate, r3DRotate, r3DShear;

                if(a3DHomMat.decompose(r3DScale, r3DTranslate, r3DRotate, r3DShear))
                {
                    // copy scale values
                    rScale.setX(r3DScale.getX());
                    rScale.setY(r3DScale.getY());

                    // copy shear
                    rShearX = r3DShear.getX();

                    // copy rotate
                    rRotate = r3DRotate.getZ();

                    // copy translate
                    rTranslate.setX(r3DTranslate.getX());
                    rTranslate.setY(r3DTranslate.getY());

                    return true;
                }
            }
        }

        return false;
    }
} // end of namespace basegfx

// eof
