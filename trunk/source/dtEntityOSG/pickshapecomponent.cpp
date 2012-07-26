/*
* dtEntity Game and Simulation Engine
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* Martin Scheffler
*/

#include <dtEntityOSG/pickshapecomponent.h>

#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/nodemasks.h>
#include <assert.h>
#include <osg/Geode>
#include <osg/ShapeDrawable>

namespace dtEntityOSG
{

   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId PickShapeComponent::TYPE(dtEntity::SID("PickShape"));
   const dtEntity::StringId PickShapeComponent::MinBoundsId(dtEntity::SID("MinBounds"));
   const dtEntity::StringId PickShapeComponent::MaxBoundsId(dtEntity::SID("MaxBounds"));
   const dtEntity::StringId PickShapeComponent::VisibleId(dtEntity::SID("Visible"));
   
   ////////////////////////////////////////////////////////////////////////////
   PickShapeComponent::PickShapeComponent()
      : BaseClass(new osg::Geode())
      , mMinBounds(
           dtEntity::DynamicVec3Property::SetValueCB(this, &PickShapeComponent::SetMinBounds),
           dtEntity::DynamicVec3Property::GetValueCB(this, &PickShapeComponent::GetMinBounds)
        )
      , mMaxBounds(
           dtEntity::DynamicVec3Property::SetValueCB(this, &PickShapeComponent::SetMaxBounds),
           dtEntity::DynamicVec3Property::GetValueCB(this, &PickShapeComponent::GetMaxBounds)
        )
      , mVisible(
           dtEntity::DynamicBoolProperty::SetValueCB(this, &PickShapeComponent::SetVisible),
           dtEntity::DynamicBoolProperty::GetValueCB(this, &PickShapeComponent::GetVisible)
        )
      , mBox(new osg::Box())      
      , mMinBoundsVal(osg::Vec3(-0.5f, -0.5f, -0.5f))
      , mMaxBoundsVal(osg::Vec3(0.5f, 0.5f, 0.5f))
   {
      Register(MinBoundsId, &mMinBounds);
      Register(MaxBoundsId, &mMaxBounds);
      Register(VisibleId, &mVisible);
      
      mVisible.Set(false);

      osg::Geode* geode = static_cast<osg::Geode*>(GetNode());
      geode->setName("PickShapeComponent");
      SetVisible(false);

      UpdatePickShape();
   }

   ////////////////////////////////////////////////////////////////////////////
   PickShapeComponent::~PickShapeComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void PickShapeComponent::SetMinBounds(const osg::Vec3& v)
   {
      mMinBoundsVal = v;
      UpdatePickShape();
   }

   ////////////////////////////////////////////////////////////////////////////
   void PickShapeComponent::SetMaxBounds(const osg::Vec3& v)
   {
      mMaxBoundsVal = v;
      UpdatePickShape();
   }

   ////////////////////////////////////////////////////////////////////////////
   void PickShapeComponent::UpdatePickShape()
   {
      const osg::Vec3& min = mMinBoundsVal;
      const osg::Vec3& max = mMaxBoundsVal;
      osg::Vec3 dif = max - min;
      mBox->setCenter((max + min) * 0.5f);
      mBox->setHalfLengths(dif * 0.5f);

      osg::ShapeDrawable* drawable = new osg::ShapeDrawable(mBox.get());
      drawable->setColor(osg::Vec4(0.2f, 0.8f, 0.2f, 1));
      osg::Geode* geode = static_cast<osg::Geode*>(GetNode());
      geode->removeDrawables(0, geode->getNumDrawables());
      geode->addDrawable(drawable);
   }

   ////////////////////////////////////////////////////////////////////////////
   void PickShapeComponent::SetVisible(bool v)
   {
      if(v)
      {
         GetNode()->setNodeMask(dtEntity::NodeMasks::VISIBLE | dtEntity::NodeMasks::PICKABLE);
      }
      else
      {
         GetNode()->setNodeMask(dtEntity::NodeMasks::PICKABLE);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   bool PickShapeComponent::GetVisible() const
   {
      return ((GetNode()->getNodeMask() & dtEntity::NodeMasks::VISIBLE) != 0);
   }

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId PickShapeSystem::TYPE(dtEntity::SID("PickShape"));

   ////////////////////////////////////////////////////////////////////////////
   PickShapeSystem::PickShapeSystem(dtEntity::EntityManager& em)
      : dtEntity::DefaultEntitySystem<PickShapeComponent>(em)
   {

   }

   ////////////////////////////////////////////////////////////////////////////
   PickShapeSystem::~PickShapeSystem()
   {
   }

}
