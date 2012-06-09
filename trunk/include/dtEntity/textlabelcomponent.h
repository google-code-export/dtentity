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

      unsigned int GetNumTexts() const { return mTextEntries.size(); }
      void Create(unsigned int textid);
      void Destroy(unsigned int textid);

      void ClearAll();

      void SetText(unsigned int textid, const std::string&);
      std::string GetText(unsigned int textid) const;

      void SetHighlighted(unsigned int textid, bool);
      bool GetHighlighted(unsigned int textid) const;

      void SetFlashing(unsigned int textid, bool);
      bool GetFlashing(unsigned int textid) const;

      void SetColor(unsigned int textid, const osg::Vec4&);
      osg::Vec4 GetColor(unsigned int textid) const;

      void SetBackdropColor(unsigned int textid, const osg::Vec4&);
      osg::Vec4 GetBackdropColor(unsigned int textid) const;

      void SetVisible(unsigned int textid, bool);
      bool GetVisible(unsigned int textid) const;

      void SetFont(unsigned int textid, const std::string&);
      std::string GetFont(unsigned int textid) const;

      void SetOffset(unsigned int textid, const osg::Vec3&);
      osg::Vec3 GetOffset(unsigned int textid) const;

      void SetCharacterHeight(unsigned int textid, float);
      float GetCharacterHeight(unsigned int textid) const;

      void SetAlignment(unsigned int textid, const std::string& str);
      std::string GetAlignment(unsigned int textid) const;

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

      void OnPropertyChanged(dtEntity::StringId propname, dtEntity::Property& prop);

      void SetEnabled(bool v);
      bool GetEnabled() const { return mEnabled.Get(); }

   private:
      dtEntity::BoolProperty mEnabled;

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
