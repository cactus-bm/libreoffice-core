/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: EffectMigration.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 02:51:07 $
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

#ifndef _SD_EFFECT_MIGRATION_HXX
#define _SD_EFFECT_MIGRATION_HXX

#ifndef _COM_SUN_STAR_PRESENTATION_ANIMATIONEFFECT_HPP_
#include <com/sun/star/presentation/AnimationEffect.hpp>
#endif

#ifndef _COM_SUN_STAR_PRESENTATION_ANIMATIONSPEED_HPP_
#include <com/sun/star/presentation/AnimationSpeed.hpp>
#endif

#ifndef _COM_SUN_STAR_PRESENTATION_FADEEFFECT_HPP_
#include <com/sun/star/presentation/FadeEffect.hpp>
#endif

class SdPage;
class SvxShape;
class SdAnimationInfo;
class SdrObject;

namespace sd {

/** this helper class gives various methods to convert effects from OOo 1.1
    and below to and from OOo 2.0 and above
*/
class EffectMigration
{
public:
    static ::com::sun::star::presentation::AnimationSpeed ConvertDuration( double fDuration );
    static double ConvertAnimationSpeed( ::com::sun::star::presentation::AnimationSpeed eSpeed );

    static bool ConvertPreset( const ::rtl::OUString& rPresetId, const ::rtl::OUString* pPresetSubType, ::com::sun::star::presentation::AnimationEffect& rEffect );
    static bool ConvertAnimationEffect( const ::com::sun::star::presentation::AnimationEffect& rEffect, ::rtl::OUString& rPresetId, ::rtl::OUString& rPresetSubType );

public:
    void AddEffectFromAnimationInfo( SdrObject* pObject, SdAnimationInfo* pInfo );

public:
    static void SetFadeEffect( SdPage* pPage, ::com::sun::star::presentation::FadeEffect eNewEffect);
    static ::com::sun::star::presentation::FadeEffect GetFadeEffect( const SdPage* pPage );

public:
    static void SetAnimationEffect( SvxShape* pShape, ::com::sun::star::presentation::AnimationEffect eValue );
    static ::com::sun::star::presentation::AnimationEffect GetAnimationEffect( SvxShape* pShape );
    static void SetTextAnimationEffect( SvxShape* pShape, ::com::sun::star::presentation::AnimationEffect eValue );
    static ::com::sun::star::presentation::AnimationEffect GetTextAnimationEffect( SvxShape* pShape );
    static void SetAnimationSpeed( SvxShape* pShape, ::com::sun::star::presentation::AnimationSpeed eSpeed );
    static ::com::sun::star::presentation::AnimationSpeed GetAnimationSpeed( SvxShape* pShape );
    static void SetDimColor( SvxShape* pShape, sal_Int32 nColor );
    static sal_Int32 GetDimColor( SvxShape* pShape );
    static void SetDimHide( SvxShape* pShape, sal_Bool bDimHide );
    static sal_Bool GetDimHide( SvxShape* pShape );
    static void SetDimPrevious( SvxShape* pShape, sal_Bool bDimPrevious );
    static sal_Bool GetDimPrevious( SvxShape* pShape );
    static void SetPresentationOrder( SvxShape* pShape, sal_Int32 nNewPos );
    static sal_Int32 GetPresentationOrder( SvxShape* pShape );
    static void UpdateSoundEffect( SvxShape* pShape, SdAnimationInfo* pInfo );
    static ::rtl::OUString GetSoundFile( SvxShape* pShape );
    static sal_Bool GetSoundOn( SvxShape* pShape );
};

} // end of namespace sd

#endif
