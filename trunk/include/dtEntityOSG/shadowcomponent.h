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
#include <dtEntityOSG/groupcomponent.h>
#include <dtEntity/stringid.h>
#include <osgShadow/ShadowTechnique>

namespace dtEntityOSG
{

   /**
    * Holds a single OSG node.
    */
   class DTENTITY_OSG_EXPORT ShadowComponent : public GroupComponent
   {
      typedef GroupComponent BaseClass;
      
   public:
      
      static const dtEntity::ComponentType TYPE;

      static const dtEntity::StringId EnabledId;
      static const dtEntity::StringId ShadowTechniqueId;
      static const dtEntity::StringId MinLightMarginId;
      static const dtEntity::StringId MaxFarPlaneId;
      static const dtEntity::StringId TexSizeId;
      static const dtEntity::StringId BaseTexUnitId;
      static const dtEntity::StringId ShadowTexUnitId;
      static const dtEntity::StringId ShadowTexCoordIndexId;
      static const dtEntity::StringId BaseTexCoordIndexId;
      static const dtEntity::StringId MapCountId;
      static const dtEntity::StringId MapResId;
      static const dtEntity::StringId DebugColorOnId;
      static const dtEntity::StringId MinNearSplitId;
      static const dtEntity::StringId MaxFarDistId;
      static const dtEntity::StringId MoveVCamFactorId;
      static const dtEntity::StringId PolyOffsetFactorId;
      static const dtEntity::StringId PolyOffsetUnitId;
      static const dtEntity::StringId LISPId;
      static const dtEntity::StringId PSSMId;
      static const dtEntity::StringId __SELECTED__Id;

      static const dtEntity::StringId ShadowTypeId;

      ShadowComponent();
      virtual ~ShadowComponent();

      void OnAddedToEntity(dtEntity::Entity& entity);

      virtual dtEntity::ComponentType GetType() const { return TYPE; }

      virtual void Finished();

      void SetShadowTechnique(const std::string name);

      void SetEnabled(bool v) { mEnabled.Set(v); }
      bool GetEnabled() const { return mEnabled.Get(); }
   private:

      dtEntity::Entity* mEntity;
      dtEntity::BoolProperty mEnabled;
      osg::ref_ptr<osgShadow::ShadowTechnique> mTechnique;

      dtEntity::GroupProperty mShadowType;
   };

  
   //////////////////////////////////////////////////////////

   // storage only
   class DTENTITY_OSG_EXPORT ShadowSystem
      : public dtEntity::DefaultEntitySystem<ShadowComponent>
   {
   public:
      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId EnabledId;

      ShadowSystem(dtEntity::EntityManager& em);
      virtual ~ShadowSystem();

      void SetEnabled(bool);
      bool GetEnabled() const { return mEnabledVal; }

   private:
      dtEntity::DynamicBoolProperty mEnabled;
      bool mEnabledVal;
   };

}
