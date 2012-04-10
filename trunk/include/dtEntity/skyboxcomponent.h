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

namespace dtEntity
{

   /**
    * Holds a single OSG node.
    */
   class DT_ENTITY_EXPORT SkyBoxComponent : public GroupComponent
   {

      typedef GroupComponent BaseClass;

   public:
      
      static const ComponentType TYPE;
      static const StringId TextureUpId;
      static const StringId TextureDownId;
      static const StringId TextureNorthId;
      static const StringId TextureSouthId;
      static const StringId TextureEastId;
      static const StringId TextureWestId;
      
      SkyBoxComponent();
      virtual ~SkyBoxComponent();

      virtual ComponentType GetType() const { return TYPE; }

      virtual void OnPropertyChanged(StringId propname, Property& prop);

      virtual osg::Group* GetAttachmentGroup() const { return mDrawables; }
   
   private:
      osg::ref_ptr<osg::Geode> mSkyboxGeode;
      StringProperty mTextureUp;
      StringProperty mTextureDown;
      StringProperty mTextureNorth;
      StringProperty mTextureSouth;
      StringProperty mTextureEast;
      StringProperty mTextureWest;
      osg::ref_ptr<osg::Group> mDrawables;
   };

  
   //////////////////////////////////////////////////////////

   // storage only
   class DT_ENTITY_EXPORT SkyBoxSystem
      : public DefaultEntitySystem<SkyBoxComponent>
   {
   public:
      SkyBoxSystem(EntityManager& em);
   };

}

