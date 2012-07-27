/*
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

#include <dtEntityOSG/textlabelcomponent.h>

#include <dtEntity/nodemasks.h>
#include <dtEntity/stringid.h>
#include <iostream>
#include <osg/BlendFunc>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Image>
#include <osg/Point>
#include <osg/PointSprite>
#include <osg/Texture2D>
#include <osg/StateSet>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osg/Multisample>
#include <osg/PolygonOffset>
#include <osgText/Text>

namespace dtEntityOSG
{

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId TextLabelComponent::TYPE(dtEntity::SID("TextLabel"));
   const dtEntity::StringId TextLabelComponent::TextsId(dtEntity::SID("Texts"));
   const dtEntity::StringId TextLabelComponent::AlwaysOnTopId(dtEntity::SID("AlwaysOnTop"));

   const dtEntity::StringId TextLabelComponent::TextId(dtEntity::SID("Text"));
   const dtEntity::StringId TextLabelComponent::ColorId(dtEntity::SID("Color"));
   const dtEntity::StringId TextLabelComponent::BackdropColorId(dtEntity::SID("BackdropColor"));
   const dtEntity::StringId TextLabelComponent::VisibleId(dtEntity::SID("Visible"));
   const dtEntity::StringId TextLabelComponent::HighlightedId(dtEntity::SID("Highlighted"));
   const dtEntity::StringId TextLabelComponent::OffsetId(dtEntity::SID("Offset"));
   const dtEntity::StringId TextLabelComponent::CharacterHeightId(dtEntity::SID("CharacterHeight"));
   const dtEntity::StringId TextLabelComponent::FontId(dtEntity::SID("Font"));
   const dtEntity::StringId TextLabelComponent::AlignmentId(dtEntity::SID("Alignment"));


   ////////////////////////////////////////////////////////////////////////////
   TextLabelComponent::TextLabelComponent()
      : dtEntity::NodeComponent(new osg::Geode())      
      , mTexts(
           dtEntity::DynamicArrayProperty::SetValueCB(this, &TextLabelComponent::SetTexts),
           dtEntity::DynamicArrayProperty::GetValueCB(this, &TextLabelComponent::GetTexts)
        )
      , mAlwaysOnTop(
         dtEntity::DynamicBoolProperty::SetValueCB(this, &TextLabelComponent::SetAlwaysOnTop),
         dtEntity::DynamicBoolProperty::GetValueCB(this, &TextLabelComponent::GetAlwaysOnTop)
        )
      , mAlwaysOnTopVal(true)
      , mTextLabelSystem(NULL)
   {
      GetNode()->setName("TextLabelComponent");
      GetNode()->setNodeMask(dtEntity::NodeMasks::VISIBLE);
      Register(TextsId, &mTexts);
      Register(AlwaysOnTopId, &mAlwaysOnTop);

      osg::StateSet* ss = GetNode()->getOrCreateStateSet();
      ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
      ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
      ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
      ss->setRenderBinDetails(90, "DepthSortedBin");
      ss->setMode(GL_FOG, osg::StateAttribute::OFF);
      GetNode()->setInitialBound(osg::BoundingBox(osg::Vec3(-1, -1, -1), osg::Vec3(1, 1, 1)));
      GetNode()->setCullingActive(false);
   }
    
   ////////////////////////////////////////////////////////////////////////////
   TextLabelComponent::~TextLabelComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void TextLabelComponent::OnAddedToEntity(dtEntity::Entity& entity)
   {
      BaseClass::OnAddedToEntity(entity);
      entity.GetEntityManager().GetEntitySystem(TYPE, mTextLabelSystem);
   }

   ////////////////////////////////////////////////////////////////////////////
   void TextLabelComponent::SetTexts(const dtEntity::PropertyArray& texts)
   {
      mTextsVal.Set(texts);

      ClearAll();
      size_t count = 0;
      const dtEntity::PropertyArray& grps = mTextsVal.Get();
      dtEntity::PropertyArray::const_iterator i;
      for(i = grps.begin(); i != grps.end(); ++i)
      {
         dtEntity::PropertyGroup props = (*i)->GroupValue();
         Create(count);

         std::string text = props[TextId]->StringValue();
         SetText(count, text);

         osg::Vec4 color = props[ColorId]->Vec4Value();
         SetColor(count, color);

         osg::Vec4 bgcolor = props[BackdropColorId]->Vec4Value();
         SetBackdropColor(count, bgcolor);

         bool visible = props[VisibleId]->BoolValue();
         SetVisible(count, visible);

         bool highlighted = props[HighlightedId]->BoolValue();
         SetHighlighted(count, highlighted);

         osg::Vec3 offset = props[OffsetId]->Vec3Value();
         SetOffset(count, offset);

         float charheight = props[CharacterHeightId]->FloatValue();
         SetCharacterHeight(count, charheight);

         std::string font = props[FontId]->StringValue();
         SetFont(count, font);

         std::string align = props[AlignmentId]->StringValue();
         SetAlignment(count, align);
         ++count;
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::PropertyArray TextLabelComponent::GetTexts() const
   {
      return mTextsVal.Get();
   }

   ////////////////////////////////////////////////////////////////////////////
   void TextLabelComponent::Finished()
   {
      BaseClass::Finished();
      for(unsigned int j = 0; j < GetNumTexts(); ++j)
      {
         SetVisible(j, mTextLabelSystem->GetEnabled());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void TextLabelComponent::Create(size_type textid)
   {
      bool visible = true;
      if(mTextLabelSystem != NULL && !mTextLabelSystem->GetEnabled())
      {
         visible = false;
      }
      osg::Geode* geode = static_cast<osg::Geode*>(GetNode());

      if(mTextEntries.size() < textid + 1 )
      {
         mTextEntries.resize(textid + 1);
      }

      if(mTextEntries[textid].valid())
      {
         return;
      }

      osgText::Text* text = new osgText::Text();

      mTextEntries[textid] = text;

      text->setCharacterSizeMode(osgText::TextBase::SCREEN_COORDS);

      text->setAlignment(osgText::TextBase::CENTER_CENTER);
      text->setAxisAlignment(osgText::TextBase::SCREEN);
      text->setFont("fonts/FreeSans.ttf");
      text->setBackdropType(osgText::Text::NONE);
      text->setBackdropColor(osg::Vec4(1,0.2f,0,1));
      text->setPosition(osg::Vec3(0, 0, 0));
      text->setCharacterSize(22);
      text->setDataVariance(osg::Object::DYNAMIC);
      text->setInitialBound(
        osg::BoundingBox(osg::Vec3(-1, -1, -1), osg::Vec3(1, 1, 1))
      );

      if(visible)
      {
         geode->addDrawable(text);
      }

      while(mTextsVal.Get().size() < textid + 1)
      {
         dtEntity::PropertyGroup props;
         dtEntity::StringProperty text(""); 
         props[TextId] = &text;

         dtEntity::Vec4Property color(osg::Vec4(1, 1, 1, 1)); 
         props[ColorId] = &color;         

         dtEntity::Vec4Property bgcolor(osg::Vec4(1, 0, 0, 1)); 
         props[BackdropColorId] = &bgcolor;

         dtEntity::BoolProperty pvisible(visible);
         props[VisibleId] = &pvisible;

         dtEntity::BoolProperty highlighted(false);
         props[HighlightedId] = &highlighted;

         dtEntity::Vec3Property offset(osg::Vec3(0, 0, 0));
         props[OffsetId] = &offset;

         dtEntity::FloatProperty charheight(30.0f);
         props[CharacterHeightId] = &charheight;
         
         dtEntity::StringProperty font("TrueTypeFonts/FreeSans.ttf");
         props[FontId] = &font;

         dtEntity::StringProperty alignment("CENTER_CENTER");
         props[AlignmentId] = &alignment;

         dtEntity::GroupProperty* grp = new dtEntity::GroupProperty(props);
         mTextsVal.Add(grp);
      }
   }


   ////////////////////////////////////////////////////////////////////////////
   void TextLabelComponent::Destroy(size_type textid)
   {
      if(mTextEntries.size() <= textid || !mTextEntries[textid].valid())
      {
         return;
      }
      osg::Geode* geode = static_cast<osg::Geode*>(GetNode());
      geode->removeDrawable(mTextEntries[textid]);
      mTextEntries[textid] = NULL;

   }

   ////////////////////////////////////////////////////////////////////////////
   void TextLabelComponent::ClearAll()
   {
      osg::Geode* geode = static_cast<osg::Geode*>(GetNode());
      geode->removeDrawables(0, geode->getNumDrawables());
      mTextEntries.clear();
   }

   ////////////////////////////////////////////////////////////////////////////
   void TextLabelComponent::SetVisible(size_type textid, bool enabled)
   {
      if(mTextEntries.size() < textid || !mTextEntries[textid].valid())
      {
         return;
      }

      osg::Geode* geode = static_cast<osg::Geode*>(GetNode());

      osgText::Text* txt = mTextEntries[textid];
      if(enabled)
      {
         if(txt->getNumParents() == 0 || txt->getParent(0) != geode)
         {
            bool success = geode->addDrawable(txt);
            assert(success);
         }
      }
      else
      {
         if(txt->getNumParents() != 0 && txt->getParent(0) == geode)
         {
            bool success = geode->removeDrawable(txt);

            assert(success);
         }
      }

      dtEntity::PropertyArray arr = mTexts.Get();
      assert(arr.size() > textid);
      dtEntity::PropertyGroup props = arr[textid]->GroupValue();
      assert(props[VisibleId]->GetDataType() == dtEntity::DataType::BOOL);
      static_cast<dtEntity::BoolProperty*>(props[VisibleId])->Set(enabled);
   }

   ////////////////////////////////////////////////////////////////////////////
   bool TextLabelComponent::GetVisible(size_type textid) const
   {
      if(!mTextEntries[textid].valid())
      {
         return false;
      }
      return (GetNode() == mTextEntries[textid]->getParent(0));
   }

   ////////////////////////////////////////////////////////////////////////////
   void TextLabelComponent::SetText(size_type textid, const std::string& txt)
   {
      if(mTextEntries.size() < textid || !mTextEntries[textid].valid())
      {
         return;
      }
      mTextEntries[textid]->setText(txt);

      dtEntity::PropertyArray arr = mTexts.Get();
      assert(arr.size() > textid);
      dtEntity::PropertyGroup props = arr[textid]->GroupValue();
      assert(props[TextId]->GetDataType() == dtEntity::DataType::STRING);
      static_cast<dtEntity::StringProperty*>(props[TextId])->Set(txt);
   }

   ////////////////////////////////////////////////////////////////////////////
   std::string TextLabelComponent::GetText(size_type textid) const
   {
      if(!mTextEntries[textid].valid())
      {
         return "Error";
      }
      return mTextEntries[textid]->getText().createUTF8EncodedString();
   }

   ////////////////////////////////////////////////////////////////////////////
   void TextLabelComponent::SetHighlighted(size_type textid, bool h)
   {
      if(!mTextEntries[textid].valid())
      {
         return;
      }
      mTextEntries[textid]->setBackdropType (h ? osgText::Text::OUTLINE : osgText::Text::NONE);

      dtEntity::PropertyArray arr = mTexts.Get();
      assert(arr.size() > textid);
      dtEntity::PropertyGroup props = arr[textid]->GroupValue();
      assert(props[HighlightedId]->GetDataType() == dtEntity::DataType::BOOL);
      static_cast<dtEntity::BoolProperty*>(props[HighlightedId])->Set(h);
   }

   ////////////////////////////////////////////////////////////////////////////
   bool TextLabelComponent::GetHighlighted(size_type textid) const
   {
      if(!mTextEntries[textid].valid())
      {
         return false;
      }
      return mTextEntries[textid]->getBackdropType() == osgText::Text::OUTLINE;
   }

   ////////////////////////////////////////////////////////////////////////////
   void TextLabelComponent::SetColor(size_type textid, const osg::Vec4& c)
   {
      if(!mTextEntries[textid].valid())
      {
         return;
      }
      mTextEntries[textid]->setColor(c);

      dtEntity::PropertyArray arr = mTexts.Get();
      assert(arr.size() > textid);
      dtEntity::PropertyGroup props = arr[textid]->GroupValue();
      props[ColorId]->SetVec4(c);      
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Vec4 TextLabelComponent::GetColor(size_type textid) const
   {
      if(!mTextEntries[textid].valid())
      {
         return osg::Vec4();
      }
      return mTextEntries[textid]->getColor();
   }

   ////////////////////////////////////////////////////////////////////////////
   void TextLabelComponent::SetOffset(size_type textid, const osg::Vec3& v)
   {
      if(!mTextEntries[textid].valid())
      {
         return;
      }
      mTextEntries[textid]->setPosition(v);

      dtEntity::PropertyArray arr = mTexts.Get();
      assert(arr.size() > textid);
      dtEntity::PropertyGroup props = arr[textid]->GroupValue();
      props[OffsetId]->SetVec3(v);
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Vec3 TextLabelComponent::GetOffset(size_type textid) const
   {
      if(!mTextEntries[textid].valid())
      {
         return osg::Vec3();
      }
      return mTextEntries[textid]->getPosition();
   }

   ////////////////////////////////////////////////////////////////////////////
   void TextLabelComponent::SetCharacterHeight(size_type textid, float v)
   {
      if(!mTextEntries[textid].valid())
      {
         return;
      }
      mTextEntries[textid]->setCharacterSize(v, mTextEntries[textid]->getCharacterAspectRatio());

      dtEntity::PropertyArray arr = mTexts.Get();
      assert(arr.size() > textid);
      dtEntity::PropertyGroup props = arr[textid]->GroupValue();
      props[CharacterHeightId]->SetFloat(v);
   }

   ////////////////////////////////////////////////////////////////////////////
   float TextLabelComponent::GetCharacterHeight(size_type textid) const
   {
      if(!mTextEntries[textid].valid())
      {
         return 0;
      }
      return mTextEntries[textid]->getCharacterHeight();
   }

   ////////////////////////////////////////////////////////////////////////////
   void TextLabelComponent::SetBackdropColor(size_type textid, const osg::Vec4& c)
   {
      if(!mTextEntries[textid].valid())
      {
         return;
      }
      mTextEntries[textid]->setBackdropColor(c);

      dtEntity::PropertyArray arr = mTexts.Get();
      assert(arr.size() > textid);
      dtEntity::PropertyGroup props = arr[textid]->GroupValue();
      props[BackdropColorId]->SetVec4(c);
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Vec4 TextLabelComponent::GetBackdropColor(size_type textid) const
   {
      if(!mTextEntries[textid].valid())
      {
         return osg::Vec4();
      }
      return mTextEntries[textid]->getBackdropColor();
   }


   ////////////////////////////////////////////////////////////////////////////
   void TextLabelComponent::SetFont(size_type textid, const std::string& f)
   {
      if(!mTextEntries[textid].valid())
      {
         return;
      }
      mTextEntries[textid]->setFont(f);

      dtEntity::PropertyArray arr = mTexts.Get();
      assert(arr.size() > textid);
      dtEntity::PropertyGroup props = arr[textid]->GroupValue();
      assert(props[FontId]->GetDataType() == dtEntity::DataType::STRING);
      static_cast<dtEntity::StringProperty*>(props[FontId])->Set(f);
   }


   ////////////////////////////////////////////////////////////////////////////
   std::string TextLabelComponent::GetFont(size_type textid) const
   {
      if(!mTextEntries[textid].valid())
      {
         return "Error";
      }
      return mTextEntries[textid]->getFont()->getFileName();
   }

   ////////////////////////////////////////////////////////////////////////////
   void TextLabelComponent::SetAlignment(size_type textid, const std::string& align)
   {
      if(!mTextEntries[textid].valid())
      {
         return;
      }

      if(align == "LEFT_TOP")
      {
         mTextEntries[textid]->setAlignment(osgText::TextBase::LEFT_TOP);
      }
      else if(align == "LEFT_CENTER")
      {
         mTextEntries[textid]->setAlignment(osgText::TextBase::LEFT_CENTER);
      }
      else if(align == "LEFT_BOTTOM")
      {
         mTextEntries[textid]->setAlignment(osgText::TextBase::LEFT_BOTTOM);
      }
      else if(align == "CENTER_TOP")
      {
         mTextEntries[textid]->setAlignment(osgText::TextBase::CENTER_TOP);
      }
      else if(align == "CENTER_CENTER")
      {
         mTextEntries[textid]->setAlignment(osgText::TextBase::CENTER_CENTER);
      }
      else if(align == "CENTER_BOTTOM")
      {
         mTextEntries[textid]->setAlignment(osgText::TextBase::CENTER_BOTTOM);
      }
      else if(align == "RIGHT_TOP")
      {
         mTextEntries[textid]->setAlignment(osgText::TextBase::RIGHT_TOP);
      }
      else if(align == "RIGHT_CENTER")
      {
         mTextEntries[textid]->setAlignment(osgText::TextBase::RIGHT_CENTER);
      }
      else if(align == "RIGHT_BOTTOM")
      {
         mTextEntries[textid]->setAlignment(osgText::TextBase::RIGHT_BOTTOM);
      }
      else if(align == "LEFT_BASE_LINE")
      {
         mTextEntries[textid]->setAlignment(osgText::TextBase::LEFT_BASE_LINE);
      }
      else if(align == "CENTER_BASE_LINE")
      {
         mTextEntries[textid]->setAlignment(osgText::TextBase::CENTER_BASE_LINE);
      }
      else if(align == "RIGHT_BASE_LINE")
      {
         mTextEntries[textid]->setAlignment(osgText::TextBase::RIGHT_BASE_LINE);
      }
      else if(align == "LEFT_BOTTOM_BASE_LINE")
      {
         mTextEntries[textid]->setAlignment(osgText::TextBase::LEFT_BOTTOM_BASE_LINE);
      }
      else if(align == "CENTER_BOTTOM_BASE_LINE")
      {
         mTextEntries[textid]->setAlignment(osgText::TextBase::CENTER_BOTTOM_BASE_LINE);
      }
      else if(align == "RIGHT_BOTTOM_BASE_LINE")
      {
         mTextEntries[textid]->setAlignment(osgText::TextBase::RIGHT_BOTTOM_BASE_LINE);
      }

      dtEntity::PropertyArray arr = mTexts.Get();
      assert(arr.size() > textid);
      dtEntity::PropertyGroup props = arr[textid]->GroupValue();
      assert(props[AlignmentId]->GetDataType() == dtEntity::DataType::STRING);
      static_cast<dtEntity::StringProperty*>(props[AlignmentId])->Set(align);
   }


   ////////////////////////////////////////////////////////////////////////////
   std::string TextLabelComponent::GetAlignment(size_type textid) const
   {
      if(!mTextEntries[textid].valid())
      {
         return "Error";
      }
      switch(mTextEntries[textid]->getAlignment())
      {
      case osgText::TextBase::LEFT_TOP: return "LEFT_TOP";
         case osgText::TextBase::LEFT_CENTER: return "LEFT_CENTER";
         case osgText::TextBase::LEFT_BOTTOM: return "LEFT_BOTTOM";
         case osgText::TextBase::CENTER_TOP: return "CENTER_TOP";
         case osgText::TextBase::CENTER_CENTER: return "CENTER_CENTER";
         case osgText::TextBase::CENTER_BOTTOM: return "CENTER_BOTTOM";

         case osgText::TextBase::RIGHT_TOP: return "RIGHT_TOP";
         case osgText::TextBase::RIGHT_CENTER: return "RIGHT_CENTER";
         case osgText::TextBase::RIGHT_BOTTOM: return "RIGHT_BOTTOM";
        
         case osgText::TextBase::LEFT_BASE_LINE: return "LEFT_BASE_LINE";
         case osgText::TextBase::CENTER_BASE_LINE: return "CENTER_BASE_LINE";
         case osgText::TextBase::RIGHT_BASE_LINE: return "RIGHT_BASE_LINE";
    
         case osgText::TextBase::LEFT_BOTTOM_BASE_LINE: return "LEFT_BOTTOM_BASE_LINE";
         case osgText::TextBase::CENTER_BOTTOM_BASE_LINE: return "CENTER_BOTTOM_BASE_LINE";
         case osgText::TextBase::RIGHT_BOTTOM_BASE_LINE: return "RIGHT_BOTTOM_BASE_LINE";
         default: return "UNKNOWN";
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void TextLabelComponent::SetAlwaysOnTop(bool v)
   { 
      mAlwaysOnTopVal = v;
      osg::StateSet* ss = GetNode()->getOrCreateStateSet();
      ss->setMode(GL_DEPTH_TEST, v ? osg::StateAttribute::OFF : osg::StateAttribute::ON);
   }

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId TextLabelSystem::TYPE(dtEntity::SID("TextLabel"));
   const dtEntity::StringId TextLabelSystem::EnabledId(dtEntity::SID("Enabled"));
   
   ///////////////
   TextLabelSystem::TextLabelSystem(dtEntity::EntityManager& em)
      : dtEntity::DefaultEntitySystem<TextLabelComponent>(em)
      , mEnabled(
           dtEntity::DynamicBoolProperty::SetValueCB(this, &TextLabelSystem::SetEnabled),
           dtEntity::DynamicBoolProperty::GetValueCB(this, &TextLabelSystem::GetEnabled)
        )
      , mEnabledVal(true)
   {
      Register(EnabledId, &mEnabled);

      AddScriptedMethod("create", dtEntity::ScriptMethodFunctor(this, &TextLabelSystem::ScriptCreate));
      AddScriptedMethod("destroy", dtEntity::ScriptMethodFunctor(this, &TextLabelSystem::ScriptDestroy));
      AddScriptedMethod("setText", dtEntity::ScriptMethodFunctor(this, &TextLabelSystem::ScriptSetText));
      AddScriptedMethod("setOffset", dtEntity::ScriptMethodFunctor(this, &TextLabelSystem::ScriptSetOffset));
      AddScriptedMethod("setAlignment", dtEntity::ScriptMethodFunctor(this, &TextLabelSystem::ScriptSetAlignment));
      AddScriptedMethod("setColor", dtEntity::ScriptMethodFunctor(this, &TextLabelSystem::ScriptSetColor));
      AddScriptedMethod("setBackdropColor", dtEntity::ScriptMethodFunctor(this, &TextLabelSystem::ScriptSetBackdropColor));
      AddScriptedMethod("setHighlighted", dtEntity::ScriptMethodFunctor(this, &TextLabelSystem::ScriptSetHighlighted));
   }  

   ////////////////////////////////////////////////////////////////////////////
   bool TextLabelSystem::GetEnabled() const
   {
      return mEnabledVal;
   }

   ////////////////////////////////////////////////////////////////////////////
   void TextLabelSystem::SetEnabled(bool v)
   {
      mEnabledVal = v;
      for(ComponentStore::iterator i = mComponents.begin(); i != mComponents.end(); ++i)
      {
         for(unsigned int j = 0; j < i->second->GetNumTexts(); ++j)
         {
            i->second->SetVisible(j, v);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::Property* TextLabelSystem::ScriptCreate(const dtEntity::PropertyArgs& args)
   {
      dtEntity::EntityId id = args[0]->UIntValue();
      TextLabelComponent* c = GetComponent(id);
      if(c)
      {
         c->Create(args[1]->UIntValue());
      }
      
      return new dtEntity::BoolProperty(true);
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::Property* TextLabelSystem::ScriptDestroy(const dtEntity::PropertyArgs& args)
   {
      dtEntity::EntityId id = args[0]->UIntValue();
      TextLabelComponent* c = GetComponent(id);
      if(c)
      {
         c->Destroy(args[1]->UIntValue());
      }
      
      return new dtEntity::BoolProperty(true);
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::Property* TextLabelSystem::ScriptSetText(const dtEntity::PropertyArgs& args)
   {
      dtEntity::EntityId id = args[0]->UIntValue();
      TextLabelComponent* c = GetComponent(id);
      if(c)
      {
         c->SetText(args[1]->UIntValue(), args[2]->StringValue());
      }
      
      return new dtEntity::BoolProperty(true);
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::Property* TextLabelSystem::ScriptSetOffset(const dtEntity::PropertyArgs& args)
   {
      dtEntity::EntityId id = args[0]->UIntValue();
      TextLabelComponent* c = GetComponent(id);
      if(c)
      {
         c->SetOffset(args[1]->UIntValue(), args[2]->Vec3Value());
      }
      
      return new dtEntity::BoolProperty(true);
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::Property* TextLabelSystem::ScriptSetAlignment(const dtEntity::PropertyArgs& args)
   {
      dtEntity::EntityId id = args[0]->UIntValue();
      TextLabelComponent* c = GetComponent(id);
      if(c)
      {
         c->SetAlignment(args[1]->UIntValue(), args[2]->StringValue());
      }
      
      return new dtEntity::BoolProperty(true);
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::Property* TextLabelSystem::ScriptSetColor(const dtEntity::PropertyArgs& args)
   {
      dtEntity::EntityId id = args[0]->UIntValue();
      TextLabelComponent* c = GetComponent(id);
      if(c)
      {
         c->SetColor(args[1]->UIntValue(), args[2]->Vec4Value());
      }
      
      return new dtEntity::BoolProperty(true);
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::Property* TextLabelSystem::ScriptSetBackdropColor(const dtEntity::PropertyArgs& args)
   {
      dtEntity::EntityId id = args[0]->UIntValue();
      TextLabelComponent* c = GetComponent(id);
      if(c)
      {
         c->SetBackdropColor(args[1]->UIntValue(), args[2]->Vec4Value());
      }
      
      return new dtEntity::BoolProperty(true);
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::Property* TextLabelSystem::ScriptSetHighlighted(const dtEntity::PropertyArgs& args)
   {
      dtEntity::EntityId id = args[0]->UIntValue();
      TextLabelComponent* c = GetComponent(id);
      if(c)
      {
         c->SetHighlighted(args[1]->UIntValue(), args[2]->BoolValue());
      }
      
      return new dtEntity::BoolProperty(true);
   }
}
