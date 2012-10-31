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
#include <osg/Geode>

namespace dtEntityOSG
{

   /**
    * Holds a single OSG node.
    */
   class DTENTITY_OSG_EXPORT SkyBoxComponent : public GroupComponent
   {

      typedef GroupComponent BaseClass;

   public:
      
      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId TextureUpId;
      static const dtEntity::StringId TextureDownId;
      static const dtEntity::StringId TextureNorthId;
      static const dtEntity::StringId TextureSouthId;
      static const dtEntity::StringId TextureEastId;
      static const dtEntity::StringId TextureWestId;
      
      SkyBoxComponent();
      virtual ~SkyBoxComponent();

      virtual void Finished();

      virtual dtEntity::ComponentType GetType() const { return TYPE; }

      virtual osg::Group* GetAttachmentGroup() const { return mDrawables; }
   
   private:
      osg::ref_ptr<osg::Geode> mSkyboxGeode;
      dtEntity::StringProperty mTextureUp;
      dtEntity::StringProperty mTextureDown;
      dtEntity::StringProperty mTextureNorth;
      dtEntity::StringProperty mTextureSouth;
      dtEntity::StringProperty mTextureEast;
      dtEntity::StringProperty mTextureWest;
      osg::ref_ptr<osg::Group> mDrawables;
   };

  
   //////////////////////////////////////////////////////////

   // storage only
   class DTENTITY_OSG_EXPORT SkyBoxSystem
      : public dtEntity::DefaultEntitySystem<SkyBoxComponent>
   {
   public:
      static const dtEntity::ComponentType TYPE;
      SkyBoxSystem(dtEntity::EntityManager& em);
   };

}

