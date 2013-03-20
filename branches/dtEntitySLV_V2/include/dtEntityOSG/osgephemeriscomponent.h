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
#include <dtEntity/dynamicproperty.h>
#include <dtEntity/stringid.h>
#include <dtEntity/scriptaccessor.h>

namespace osgEphemeris
{
   class EphemerisModel;
}

namespace dtEntityOSG
{

   ///////////////////////////////////////////////////////////////////////////
   class DTENTITY_OSG_EXPORT OSGEphemerisComponent : public GroupComponent
   {

      typedef GroupComponent BaseClass;

   public:
      
      static const dtEntity::ComponentType TYPE;
      
      OSGEphemerisComponent();
      virtual ~OSGEphemerisComponent();

      virtual dtEntity::ComponentType GetType() const { return TYPE; }
     
      virtual osg::Group* GetAttachmentGroup() const { return mDrawables; }
      void Update();
      osg::Vec4 GetSunLightPos();

      osgEphemeris::EphemerisModel* GetEphemerisModel() const { return mModel; }

   private:
      osg::ref_ptr<osg::Group> mDrawables;
      osgEphemeris::EphemerisModel* mModel;
   };

  
   ///////////////////////////////////////////////////////////////////////////
   // storage only
   class DTENTITY_OSG_EXPORT OSGEphemerisSystem
      : public dtEntity::DefaultEntitySystem<OSGEphemerisComponent>
   {
   public:

      static const dtEntity::ComponentType TYPE;
      
      OSGEphemerisSystem(dtEntity::EntityManager& em);
      ~OSGEphemerisSystem();

      void Tick(const dtEntity::Message& msg);

   private:
      dtEntity::MessageFunctor mTickFunctor;
   };
}
