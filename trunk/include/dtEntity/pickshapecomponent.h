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
#include <dtEntity/export.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/nodecomponent.h>
#include <dtEntity/property.h>
#include <dtEntity/stringid.h>

namespace osg
{
   class Box;
}

namespace dtEntity
{

   ///////////////////////////////////////////////////////////////////////////

   /**
    * Adds a pickable shape to the transform of the entity
    */
   class DT_ENTITY_EXPORT PickShapeComponent
      : public NodeComponent
   {
   public:

      typedef NodeComponent BaseClass;

      static const ComponentType TYPE;
      static const StringId MinBoundsId;
      static const StringId MaxBoundsId;
      static const StringId VisibleId;
    
      PickShapeComponent();
      virtual ~PickShapeComponent();

      virtual ComponentType GetType() const { return TYPE; }


      virtual void OnPropertyChanged(StringId propname, Property& prop);

      void SetMinBounds(const osg::Vec3& v);
      osg::Vec3 GetMinBounds() const { return mMinBounds.Get(); }

      void SetMaxBounds(const osg::Vec3& v);
      osg::Vec3 GetMaxBounds() const { return mMaxBounds.Get(); }

      void SetVisible(bool v);
      bool GetVisible() const;

   private:
     
      void UpdatePickShape();

      // path to loaded script file
      StringProperty mMeshPathProperty;
      Vec3Property mMinBounds;
      Vec3Property mMaxBounds;
      BoolProperty mVisible;
      osg::ref_ptr<osg::Box> mBox;

   };


   //////////////////////////////////////////////////////////

   class DT_ENTITY_EXPORT PickShapeSystem
      : public DefaultEntitySystem<PickShapeComponent>
   {
   public:

      static const ComponentType TYPE;

      PickShapeSystem(EntityManager& em);
      ~PickShapeSystem();
      virtual void OnPropertyChanged(StringId propname, Property& prop);

   private:
   };

}
