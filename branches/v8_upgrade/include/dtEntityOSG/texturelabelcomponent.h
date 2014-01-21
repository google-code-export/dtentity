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

#include <dtEntityOSG/export.h>
#include <dtEntity/component.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntityOSG/nodecomponent.h>
#include <dtEntity/dynamicproperty.h>
#include <osg/Geometry>
#include <osg/Program>

namespace dtEntityOSG
{      

   class TextureLabelSystem;

   class DTENTITY_OSG_EXPORT TextureLabelComponent : public NodeComponent
   {

      friend class TextureLabelSystem;
      typedef NodeComponent BaseClass;

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

      virtual void Finished();

      virtual void OnAddedToEntity(dtEntity::Entity& entity);
      std::string GetPath() const { return mPath.Get(); }

      // call OnFinished...() to apply
      void SetPath(const std::string& path) { mPath.Set(path); }

      osg::Geode* GetGeode() const;
      
      osg::Vec3 GetOffset() const  { return mOffsetVal; }
      void SetOffset(const osg::Vec3& o);
      
      void SetMaxSize(unsigned int v) { mMaxSize.Set(v); }
      unsigned int GetMaxSize() const { return mMaxSize.Get(); }

      void SetMinSize(unsigned int v) { mMinSize.Set(v); }
      unsigned int GetMinSize() const { return mMinSize.Get(); }

      void SetDistanceAttenuation(const osg::Vec3& v) { mDistanceAttenuation.Set(v); }
      osg::Vec3 GetDistanceAttenuation() const { return mDistanceAttenuation.Get(); }

      osg::Vec4 GetColor() const { return mColorVal; }
      void SetColor(const osg::Vec4& v);

      void SetVisible(bool v);
      bool GetVisible() const;

      bool GetAlwaysOnTop() const { return mAlwaysOnTop.Get(); }
      void SetAlwaysOnTop(bool v) { mAlwaysOnTop.Set(v); }

      osg::Uniform* GetColorUniform() const { return mColorUniform; }
   private:

      osg::ref_ptr<osg::Vec4Array> mColorArray;
      osg::ref_ptr<osg::Geometry> mDotGeometry;
      dtEntity::DynamicVec3Property mOffset;
      osg::Vec3 mOffsetVal;
      dtEntity::DynamicVec4Property mColor;
      osg::Vec4 mColorVal;
      dtEntity::StringProperty mPath;
      dtEntity::UIntProperty mMinSize;
      dtEntity::UIntProperty mMaxSize;
      dtEntity::Vec3Property mDistanceAttenuation;
      dtEntity::DynamicBoolProperty mVisible;
      dtEntity::BoolProperty mAlwaysOnTop;
      TextureLabelSystem* mLabelSystem;
      osg::ref_ptr<osg::Uniform> mColorUniform;
   };
   
   
   ////////////////////////////////////////////////////////////////////////////
   
   class DTENTITY_OSG_EXPORT TextureLabelSystem
      : public dtEntity::DefaultEntitySystem<TextureLabelComponent>
   {
      static const dtEntity::StringId EnabledId;
      typedef dtEntity::DefaultEntitySystem<TextureLabelComponent> BaseClass;
      
   public:

      static const dtEntity::ComponentType TYPE;

      TextureLabelSystem(dtEntity::EntityManager& em);
      ~TextureLabelSystem();

      osg::StateSet* GetSymbolStateset(const std::string& symbolpath, TextureLabelComponent& component);

      void SetupLabel(TextureLabelComponent& component);

      void SetEnabled(bool v);
      bool GetEnabled() const;

   private:

      dtEntity::DynamicBoolProperty mEnabled;
      bool mEnabledVal;
      osg::ref_ptr<osg::Program> mProgram;
   };
}
