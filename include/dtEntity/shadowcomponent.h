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
#include <dtEntity/groupcomponent.h>
#include <dtEntity/stringid.h>
#include <osg/Geode>
#include <osgShadow/ShadowMap>

namespace dtEntity
{

   /**
    * Holds a single OSG node.
    */
   class DT_ENTITY_EXPORT ShadowComponent : public GroupComponent
   {
      typedef GroupComponent BaseClass;
      
   public:
      
      static const ComponentType TYPE;

      static const StringId EnabledId;
      static const StringId ShadowTechniqueId;
      static const StringId MinLightMarginId;
      static const StringId MaxFarPlaneId;
      static const StringId TexSizeId;
      static const StringId BaseTexUnitId;
      static const StringId ShadowTexUnitId;
      static const StringId ShadowTexCoordIndexId;
      static const StringId BaseTexCoordIndexId;
      static const StringId MapCountId;
      static const StringId MapResId;
      static const StringId DebugColorOnId;
      static const StringId MinNearSplitId;
      static const StringId MaxFarDistId;
      static const StringId MoveVCamFactorId;
      static const StringId PolyOffsetFactorId;
      static const StringId PolyOffsetUnitId;
      static const StringId LISPId;
      static const StringId PSSMId;
      static const StringId __SELECTED__Id;

      static const StringId ShadowTypeId;

      ShadowComponent();
      virtual ~ShadowComponent();

      void OnAddedToEntity(Entity& entity);

      virtual ComponentType GetType() const { return TYPE; }

      virtual void Finished();

      void SetShadowTechnique(const std::string name);

      void SetEnabled(bool v) { mEnabled.Set(v); }
      bool GetEnabled() const { return mEnabled.Get(); }
   private:

      Entity* mEntity;
      BoolProperty mEnabled;
      osg::ref_ptr<osgShadow::ShadowTechnique> mTechnique;

      GroupProperty mShadowType;
   };

  
   //////////////////////////////////////////////////////////

   // storage only
   class DT_ENTITY_EXPORT ShadowSystem
      : public DefaultEntitySystem<ShadowComponent>
   {
   public:
      static const StringId EnabledId;

      ShadowSystem(EntityManager& em);

      virtual void OnPropertyChanged(StringId propname, Property &prop);
      void SetEnabled(bool);
      bool GetEnabled() const { return mEnabled.Get(); }

   private:
      BoolProperty mEnabled;
   };

}
