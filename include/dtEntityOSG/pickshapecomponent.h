#pragma once

/* -*-c++-*-
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

#include <osg/ref_ptr>
#include <dtEntityOSG/export.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/dynamicproperty.h>
#include <dtEntity/nodecomponent.h>
#include <dtEntity/property.h>
#include <dtEntity/stringid.h>

namespace osg
{
   class Box;
}

namespace dtEntityOSG
{

   ///////////////////////////////////////////////////////////////////////////

   /**
    * Adds a pickable shape to the transform of the entity
    */
   class DTENTITY_OSG_EXPORT PickShapeComponent
      : public dtEntity::NodeComponent
   {
   public:

      typedef dtEntity::NodeComponent BaseClass;

      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId MinBoundsId;
      static const dtEntity::StringId MaxBoundsId;
      static const dtEntity::StringId VisibleId;
    
      PickShapeComponent();
      virtual ~PickShapeComponent();

      virtual dtEntity::ComponentType GetType() const { return TYPE; }

      void SetMinBounds(const osg::Vec3& v);
      osg::Vec3 GetMinBounds() const { return mMinBoundsVal; }

      void SetMaxBounds(const osg::Vec3& v);
      osg::Vec3 GetMaxBounds() const { return mMaxBoundsVal; }

      void SetVisible(bool v);
      bool GetVisible() const;

   private:
     
      void UpdatePickShape();

      // path to loaded script file
      dtEntity::StringProperty mMeshPathProperty;
      dtEntity::DynamicVec3Property mMinBounds;
      dtEntity::DynamicVec3Property mMaxBounds;
      dtEntity::DynamicBoolProperty mVisible;
      osg::ref_ptr<osg::Box> mBox;
      osg::Vec3 mMinBoundsVal;
      osg::Vec3 mMaxBoundsVal;

   };


   //////////////////////////////////////////////////////////

   class DTENTITY_OSG_EXPORT PickShapeSystem
      : public dtEntity::DefaultEntitySystem<PickShapeComponent>
   {
   public:

      static const dtEntity::ComponentType TYPE;

      PickShapeSystem(dtEntity::EntityManager& em);
      ~PickShapeSystem();
   private:
   };

}
