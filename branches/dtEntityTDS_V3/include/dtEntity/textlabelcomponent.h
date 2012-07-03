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
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/nodecomponent.h>
#include <dtEntity/dynamicproperty.h>
#include <dtEntity/scriptaccessor.h>
#include <osgText/Text>

namespace dtEntity
{      
   class Entity;
   class TextLabelSystem;
      
   class DT_ENTITY_EXPORT TextLabelComponent : public dtEntity::NodeComponent
   {

      typedef NodeComponent BaseClass;

   public:

      typedef std::vector<osg::ref_ptr<osgText::Text> >::size_type size_type;
      
      static const ComponentType TYPE;
      static const StringId TextsId;
      static const StringId AlwaysOnTopId;
      static const StringId TextId;
      static const StringId ColorId;
      static const StringId BackdropColorId;
      static const StringId VisibleId;
      static const StringId HighlightedId;
      static const StringId OffsetId;
      static const StringId CharacterHeightId;
      static const StringId FontId;
      static const StringId AlignmentId;
      
      TextLabelComponent();
      virtual ~TextLabelComponent();

      virtual ComponentType GetType() const { return TYPE; }
      
      virtual bool IsInstanceOf(ComponentType id) const
      { 
         return (id == TYPE); 
      }      

      virtual void Finished();

      void OnAddedToEntity(Entity& entity);

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

      void SetTexts(const PropertyArray& texts);
      PropertyArray GetTexts() const;
      DynamicArrayProperty mTexts;
      ArrayProperty mTextsVal;
      DynamicBoolProperty mAlwaysOnTop;
      bool mAlwaysOnTopVal;
      typedef std::vector<osg::ref_ptr<osgText::Text> > TextEntries;
      TextEntries mTextEntries;
      TextLabelSystem* mTextLabelSystem;
   };
   
   
   ////////////////////////////////////////////////////////////////////////////
   
   class DT_ENTITY_EXPORT TextLabelSystem
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

      DynamicBoolProperty mEnabled;
      bool mEnabledVal;
      Property* ScriptCreate(const PropertyArgs& args);
      Property* ScriptDestroy(const PropertyArgs& args);
      Property* ScriptSetText(const PropertyArgs& args);
      Property* ScriptSetOffset(const PropertyArgs& args);
      Property* ScriptSetAlignment(const PropertyArgs& args);
      Property* ScriptSetColor(const PropertyArgs& args);
      Property* ScriptSetBackdropColor(const PropertyArgs& args);
      Property* ScriptSetHighlighted(const PropertyArgs& args);

   };
}
