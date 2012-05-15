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

#include <dtEntity/pickshapecomponent.h>

#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/nodemasks.h>
#include <assert.h>
#include <osg/Geode>


namespace dtEntity
{

   ////////////////////////////////////////////////////////////////////////////
   const StringId PickShapeComponent::TYPE(dtEntity::SID("PickShape"));
   const StringId PickShapeComponent::MinBoundsId(dtEntity::SID("MinBounds"));
   const StringId PickShapeComponent::MaxBoundsId(dtEntity::SID("MaxBounds"));
   const StringId PickShapeComponent::VisibleId(dtEntity::SID("Visible"));
   
   ////////////////////////////////////////////////////////////////////////////
   PickShapeComponent::PickShapeComponent()
      : BaseClass(new osg::Geode())
      , mBox(new osg::Box())
   {
      Register(MinBoundsId, &mMinBounds);
      Register(MaxBoundsId, &mMaxBounds);
      Register(VisibleId, &mVisible);

      mMinBounds.Set(osg::Vec3(-0.5f, -0.5f, -0.5f));
      mMaxBounds.Set(osg::Vec3(0.5f, 0.5f, 0.5f));
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
   void PickShapeComponent::OnPropertyChanged(StringId propname, Property& prop)
   {
      BaseClass::OnPropertyChanged(propname, prop);

      if(propname == MinBoundsId)
      {
         SetMinBounds(prop.Vec3Value());
      }
      else if(propname == MaxBoundsId)
      {
         SetMaxBounds(prop.Vec3Value());
      }
      else if(propname == VisibleId)
      {
         SetVisible(prop.BoolValue());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void PickShapeComponent::SetMinBounds(const osg::Vec3& v)
   {
      mMinBounds.Set(v);
      UpdatePickShape();
   }

   ////////////////////////////////////////////////////////////////////////////
   void PickShapeComponent::SetMaxBounds(const osg::Vec3& v)
   {
      mMaxBounds.Set(v);
      UpdatePickShape();
   }

   ////////////////////////////////////////////////////////////////////////////
   void PickShapeComponent::UpdatePickShape()
   {
      osg::Vec3 min = mMinBounds.Get();
      osg::Vec3 max = mMaxBounds.Get();
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
         GetNode()->setNodeMask(NodeMasks::VISIBLE | NodeMasks::PICKABLE);
      }
      else
      {
         GetNode()->setNodeMask(NodeMasks::PICKABLE);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   bool PickShapeComponent::GetVisible() const
   {
      return ((GetNode()->getNodeMask() & NodeMasks::VISIBLE) != 0);
   }

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   const StringId PickShapeSystem::TYPE(dtEntity::SID("PickShape"));

   ////////////////////////////////////////////////////////////////////////////
   PickShapeSystem::PickShapeSystem(EntityManager& em)
      : dtEntity::DefaultEntitySystem<PickShapeComponent>(em)
   {

   }

   ////////////////////////////////////////////////////////////////////////////
   PickShapeSystem::~PickShapeSystem()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void PickShapeSystem::OnPropertyChanged(StringId propname, Property& prop)
   {

   }
}
