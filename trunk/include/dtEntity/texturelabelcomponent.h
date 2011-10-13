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

#include <dtEntity/export.h>
#include <dtEntity/component.h>
#include <dtEntity/entity.h>
#include <dtEntity/entityid.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/osgcomponents.h>
#include <osg/Geometry>

namespace dtEntity
{      
      
   class TextureLabelSystem;

   class DT_ENTITY_EXPORT TextureLabelComponent : public dtEntity::NodeComponent
   {

      friend class TextureLabelSystem;
      typedef dtEntity::NodeComponent BaseClass;

   public:
      
      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId OffsetId;
      static const dtEntity::StringId PathId;
      static const dtEntity::StringId ColorId;
      static const dtEntity::StringId MaxSizeId;
      static const dtEntity::StringId MinSizeId;
      static const dtEntity::StringId DistanceAttenuationId;
      static const dtEntity::StringId VisibleId;
      static const dtEntity::StringId AlwaysOnTopId;
      static const dtEntity::StringId AddConnectingLineId;
      
      TextureLabelComponent();
      virtual ~TextureLabelComponent();

      virtual dtEntity::ComponentType GetType() const { return TYPE; }
      
      virtual bool IsInstanceOf(dtEntity::ComponentType id) const
      { 
         return (id == TYPE); 
      }      

      virtual void OnFinishedSettingProperties();

      virtual void OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property& prop);

      virtual void OnAddedToEntity(dtEntity::Entity& entity);
      std::string GetPath() const { return mPath.Get(); }

      // call OnFinished...() to apply
      void SetPath(const std::string& path) { mPath.Set(path); }

      osg::Geode* GetGeode() const;
      
      osg::Vec3 GetOffset()
      {
         return mOffset.Get();
      }

      osg::Vec3 GetOffset() const  { return mOffset.Get(); }
      void SetOffset(const osg::Vec3& o);
      
      void SetMaxSize(unsigned int v) { mMaxSize.Set(v); }
      unsigned int GetMaxSize() const { return mMaxSize.Get(); }

      void SetMinSize(unsigned int v) { mMinSize.Set(v); }
      unsigned int GetMinSize() const { return mMinSize.Get(); }

      void SetDistanceAttenuation(const osg::Vec3& v) { mDistanceAttenuation.Set(v); }
      osg::Vec3 GetDistanceAttenuation() const { return mDistanceAttenuation.Get(); }

      osg::Vec4 GetColor() const { return mColor.Get(); }
      void SetColor(const osg::Vec4& v);

      void SetVisible(bool v);
      bool GetVisible() const;

      bool GetAlwaysOnTop() const { return mAlwaysOnTop.Get(); }
      void SetAlwaysOnTop(bool v) { mAlwaysOnTop.Set(v); }

   private:

      osg::ref_ptr<osg::Vec4Array> mColorArray;
      osg::ref_ptr<osg::Geometry> mDotGeometry;
      dtEntity::Vec3Property mOffset;
      dtEntity::Vec4Property mColor;
      dtEntity::StringProperty mPath;
      dtEntity::UIntProperty mMinSize;
      dtEntity::UIntProperty mMaxSize;
      dtEntity::Vec3Property mDistanceAttenuation;
      dtEntity::BoolProperty mVisible;
      dtEntity::BoolProperty mAlwaysOnTop;
      TextureLabelSystem* mLabelSystem;

   };
   
   
   ////////////////////////////////////////////////////////////////////////////
   
   class DT_ENTITY_EXPORT TextureLabelSystem
      : public dtEntity::DefaultEntitySystem<TextureLabelComponent>
   {
      static const dtEntity::StringId EnabledId;
      typedef dtEntity::DefaultEntitySystem<TextureLabelComponent> BaseClass;
      
   public:

      TextureLabelSystem(dtEntity::EntityManager& em);
      virtual void OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property& prop);
      
      osg::StateSet* GetSymbolStateset(const std::string& symbolpath, TextureLabelComponent& component);

      void SetupLabel(TextureLabelComponent& component);

      void SetEnabled(bool v);
      bool GetEnabled() const { return mEnabled.Get(); }

   private:

      dtEntity::BoolProperty mEnabled;
   };
}
