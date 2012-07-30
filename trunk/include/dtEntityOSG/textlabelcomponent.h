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
#include <dtEntity/scriptaccessor.h>
#include <osgText/Text>

namespace dtEntityOSG
{      
   class Entity;
   class TextLabelSystem;
      
   class DTENTITY_OSG_EXPORT TextLabelComponent : public NodeComponent
   {

      typedef NodeComponent BaseClass;

   public:

      typedef std::vector<osg::ref_ptr<osgText::Text> >::size_type size_type;
      
      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId TextsId;
      static const dtEntity::StringId AlwaysOnTopId;
      static const dtEntity::StringId TextId;
      static const dtEntity::StringId ColorId;
      static const dtEntity::StringId BackdropColorId;
      static const dtEntity::StringId VisibleId;
      static const dtEntity::StringId HighlightedId;
      static const dtEntity::StringId OffsetId;
      static const dtEntity::StringId CharacterHeightId;
      static const dtEntity::StringId FontId;
      static const dtEntity::StringId AlignmentId;
      
      TextLabelComponent();
      virtual ~TextLabelComponent();

      virtual dtEntity::ComponentType GetType() const { return TYPE; }
      
      virtual bool IsInstanceOf(dtEntity::ComponentType id) const
      { 
         return (id == TYPE); 
      }      

      virtual void Finished();

      void OnAddedToEntity(dtEntity::Entity& entity);

      size_type GetNumTexts() const { return mTextEntries.size(); }
      void Create(size_type textid);
      void Destroy(size_type textid);

      void ClearAll();

      void SetText(size_type textid, const std::string&);
      std::string GetText(size_type textid) const;

      void SetHighlighted(size_type textid, bool);
      bool GetHighlighted(size_type textid) const;

      void SetFlashing(size_type textid, bool);
      bool GetFlashing(size_type textid) const;

      void SetColor(size_type textid, const osg::Vec4&);
      osg::Vec4 GetColor(size_type textid) const;

      void SetBackdropColor(size_type textid, const osg::Vec4&);
      osg::Vec4 GetBackdropColor(size_type textid) const;

      void SetVisible(size_type textid, bool);
      bool GetVisible(size_type textid) const;

      void SetFont(size_type textid, const std::string&);
      std::string GetFont(size_type textid) const;

      void SetOffset(size_type textid, const osg::Vec3&);
      osg::Vec3 GetOffset(size_type textid) const;

      void SetCharacterHeight(size_type textid, float);
      float GetCharacterHeight(size_type textid) const;

      void SetAlignment(size_type textid, const std::string& str);
      std::string GetAlignment(size_type textid) const;

      bool GetAlwaysOnTop() const { return mAlwaysOnTopVal; }
      void SetAlwaysOnTop(bool v);

   private:

      void SetTexts(const dtEntity::PropertyArray& texts);
      dtEntity::PropertyArray GetTexts() const;
      dtEntity::DynamicArrayProperty mTexts;
      dtEntity::ArrayProperty mTextsVal;
      dtEntity::DynamicBoolProperty mAlwaysOnTop;
      bool mAlwaysOnTopVal;
      typedef std::vector<osg::ref_ptr<osgText::Text> > TextEntries;
      TextEntries mTextEntries;
      TextLabelSystem* mTextLabelSystem;
   };
   
   
   ////////////////////////////////////////////////////////////////////////////
   
   class DTENTITY_OSG_EXPORT TextLabelSystem
      : public dtEntity::DefaultEntitySystem<TextLabelComponent>
      , public dtEntity::ScriptAccessor
   {
      static const dtEntity::StringId EnabledId;
      typedef dtEntity::DefaultEntitySystem<TextLabelComponent> BaseClass;
      
   public:

      static const dtEntity::ComponentType TYPE;

      TextLabelSystem(dtEntity::EntityManager& em);

      void SetEnabled(bool v);
      bool GetEnabled() const;

   private:

      dtEntity::DynamicBoolProperty mEnabled;
      bool mEnabledVal;
      dtEntity::Property* ScriptCreate(const dtEntity::PropertyArgs& args);
      dtEntity::Property* ScriptDestroy(const dtEntity::PropertyArgs& args);
      dtEntity::Property* ScriptSetText(const dtEntity::PropertyArgs& args);
      dtEntity::Property* ScriptSetOffset(const dtEntity::PropertyArgs& args);
      dtEntity::Property* ScriptSetAlignment(const dtEntity::PropertyArgs& args);
      dtEntity::Property* ScriptSetColor(const dtEntity::PropertyArgs& args);
      dtEntity::Property* ScriptSetBackdropColor(const dtEntity::PropertyArgs& args);
      dtEntity::Property* ScriptSetHighlighted(const dtEntity::PropertyArgs& args);

   };
}
