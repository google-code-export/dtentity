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

#include <dtEntity/rapidxmlmapencoder.h>

#include <dtEntity/basemessages.h>
#include <dtEntity/component.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/message.h>
#include <dtEntity/spawner.h>
#include <osg/Matrix>
#include <osg/Quat>
#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgDB/ReadFile>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <rapidxml_print.hpp>
#include <rapidxml_utils.hpp>

namespace dtEntity
{
  using namespace rapidxml;

   ////////////////////////////////////////////////////////////////////////////////
   template <class T>
   bool fromString(T& t, const std::string& s, std::ios_base& (*f)(std::ios_base&) = std::dec)
   {
     std::istringstream iss(s);
     return !(iss >> f >> t).fail();
   }

   ////////////////////////////////////////////////////////////////////////////////
   char* ToString(xml_document<>& doc, double  v)
   {
      std::ostringstream os;
      os.precision(10);
      os << std::fixed << v;
      return doc.allocate_string(os.str().c_str());
   }
 
   ////////////////////////////////////////////////////////////////////////////////
   RapidXMLMapEncoder::RapidXMLMapEncoder(EntityManager& em)
      : mEntityManager(&em)
   {
      em.GetEntitySystem(MapComponent::TYPE, mMapSystem);
	}
   
   ////////////////////////////////////////////////////////////////////////////////
   RapidXMLMapEncoder::~RapidXMLMapEncoder()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool StartEntitySystem(EntityManager& em, StringId componentType)
   {
      if(em.HasEntitySystem(componentType))
      {
         return true;
      }

      MapSystem* mapSystem;
      em.GetEntitySystem(MapComponent::TYPE, mapSystem);

      ComponentPluginManager& pluginManager = mapSystem->GetPluginManager();
      if(!pluginManager.FactoryExists(componentType))
      {
         return false;
      }
      return pluginManager.StartEntitySystem(componentType);
   }

   ////////////////////////////////////////////////////////////////////////////////
   BoolProperty* ParseBoolProperty(xml_node<>* element)
   {
      for(xml_attribute<>* attr = element->first_attribute();
           attr; attr = attr->next_attribute())
      {
         if(strcmp(attr->name(), "value") == 0 && strcmp(attr->value(), "true") == 0)
         {
            return new BoolProperty(true);
         }
      }
      return new BoolProperty(false);
   }

   ////////////////////////////////////////////////////////////////////////////////
   CharProperty* ParseCharProperty(xml_node<>* element)
   {
      CharProperty* p = new CharProperty();
      for(xml_attribute<>* attr = element->first_attribute();
           attr; attr = attr->next_attribute())
      {
         if(strcmp(attr->name(), "value") == 0)
         {
            p->SetString(attr->value());
         }
      }
      return p;
   }

   ////////////////////////////////////////////////////////////////////////////////
   IntProperty* ParseIntProperty(xml_node<>* element)
   {
      IntProperty* p = new IntProperty();
      for(xml_attribute<>* attr = element->first_attribute();
           attr; attr = attr->next_attribute())
      {
         if(strcmp(attr->name(), "value") == 0)
         {
            p->SetString(attr->value());
         }
      }
      return p;
   }

   ////////////////////////////////////////////////////////////////////////////////
   FloatProperty* ParseFloatProperty(xml_node<>* element)
   {
      FloatProperty* p = new FloatProperty();
      for(xml_attribute<>* attr = element->first_attribute();
           attr; attr = attr->next_attribute())
      {
         if(strcmp(attr->name(), "value") == 0)
         {
            p->SetString(attr->value());
         }
      }
      return p;
   }

   ////////////////////////////////////////////////////////////////////////////////
   DoubleProperty* ParseDoubleProperty(xml_node<>* element)
   {
      DoubleProperty* p = new DoubleProperty();
      for(xml_attribute<>* attr = element->first_attribute();
           attr; attr = attr->next_attribute())
      {
         if(strcmp(attr->name(), "value") == 0)
         {
            p->SetString(attr->value());
         }
      }
      return p;
   }

   ////////////////////////////////////////////////////////////////////////////////
   MatrixProperty* ParseMatrixProperty(xml_node<>* element)
   {
      MatrixProperty* p = new MatrixProperty();
      p->SetString(element->value());
      return p;
   }

   ////////////////////////////////////////////////////////////////////////////////
   UIntProperty* ParseUIntProperty(xml_node<>* element)
   {
      UIntProperty* p = new UIntProperty();
      for(xml_attribute<>* attr = element->first_attribute();
           attr; attr = attr->next_attribute())
      {
         if(strcmp(attr->name(), "value") == 0)
         {
            p->SetString(attr->value());
         }
      }
      return p;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Vec2dProperty* ParseVec2Property(xml_node<>* element)
   {
      double x = 0, y = 0;
      for(xml_attribute<>* attr = element->first_attribute();
           attr; attr = attr->next_attribute())
      {
         if(strcmp(attr->name(), "x") == 0)
         {
           fromString<double>(x, attr->value());
         }
         else if(strcmp(attr->name(), "y") == 0)
         {
           fromString<double>(y, attr->value());
         }
      }
      return new Vec2dProperty(osg::Vec2d(x, y));
   }

   ////////////////////////////////////////////////////////////////////////////////
   Vec3dProperty* ParseVec3Property(xml_node<>* element)
   {
      double x = 0, y = 0, z = 0;
      for(xml_attribute<>* attr = element->first_attribute();
           attr; attr = attr->next_attribute())
      {
         if(strcmp(attr->name(), "x") == 0)
         {
           fromString<double>(x, attr->value());
         }
         else if(strcmp(attr->name(), "y") == 0)
         {
           fromString<double>(y, attr->value());
         }
         else if(strcmp(attr->name(), "z") == 0)
         {
           fromString<double>(z, attr->value());
         }
      }
      return new Vec3dProperty(osg::Vec3d(x, y, z));
   }

   ////////////////////////////////////////////////////////////////////////////////
   Vec4dProperty* ParseVec4Property(xml_node<>* element)
   {
      double x = 0, y = 0, z = 0, w = 0;
      for(xml_attribute<>* attr = element->first_attribute();
           attr; attr = attr->next_attribute())
      {
         if(strcmp(attr->name(), "x") == 0)
         {
           fromString<double>(x, attr->value());
         }
         else if(strcmp(attr->name(), "y") == 0)
         {
           fromString<double>(y, attr->value());
         }
         else if(strcmp(attr->name(), "z") == 0)
         {
           fromString<double>(z, attr->value());
         }
         else if(strcmp(attr->name(), "w") == 0)
         {
           fromString<double>(w, attr->value());
         }
      }
      return new Vec4dProperty(osg::Vec4d(x, y, z, w));
   }

   ////////////////////////////////////////////////////////////////////////////////
   QuatProperty* ParseQuatProperty(xml_node<>* element)
   {
      double x = 0, y = 0, z = 0, w = 0;
      for(xml_attribute<>* attr = element->first_attribute();
           attr; attr = attr->next_attribute())
      {
         if(strcmp(attr->name(), "x") == 0)
         {
           fromString<double>(x, attr->value());
         }
         else if(strcmp(attr->name(), "y") == 0)
         {
           fromString<double>(y, attr->value());
         }
         else if(strcmp(attr->name(), "z") == 0)
         {
           fromString<double>(z, attr->value());
         }
         else if(strcmp(attr->name(), "w") == 0)
         {
           fromString<double>(w, attr->value());
         }
      }
      return new QuatProperty(osg::Quat(x, y, z, w));
   }

   ////////////////////////////////////////////////////////////////////////////////
   StringProperty* ParseStringProperty(xml_node<>* element)
   {
      return new StringProperty(element->value());
   }

   ////////////////////////////////////////////////////////////////////////////////
   ArrayProperty* ParseArrayProperty(xml_node<>* element)
   {
      ArrayProperty* arrayproperty = new ArrayProperty();

      for(xml_node<>* currentNode(element->first_node());
          currentNode != NULL; currentNode = currentNode->next_sibling())
      {
         if(currentNode->type() == node_element)
         {
            Property* prop = RapidXMLMapEncoder::ParseProperty(currentNode);
            if(prop != NULL)
            {
               arrayproperty->Add(prop);
            }
         }
      }
      return arrayproperty;
   }

   ////////////////////////////////////////////////////////////////////////////////
   GroupProperty* ParseGroupProperty(xml_node<>* element)
   {
      DynamicPropertyContainer subprops;

      for(xml_node<>* currentNode(element->first_node());
          currentNode != NULL; currentNode = currentNode->next_sibling())
      {
         if(currentNode->type() == node_element)
         {
            for(xml_attribute<>* attr = currentNode->first_attribute();
                 attr; attr = attr->next_attribute())
            {
               if(strcmp("name", attr->name()) == 0)
               {
                  Property* prop = RapidXMLMapEncoder::ParseProperty(currentNode);
                  if(prop != NULL)
                  {
                     subprops.AddProperty(SID(attr->value()), *prop);
                     delete prop;
                  }
               }
            }

         }
      }

      PropertyContainer::PropertyMap pmap;
      subprops.GetProperties(pmap);
      PropertyGroup groupvals;
      for(PropertyContainer::PropertyMap::iterator i = pmap.begin(); i != pmap.end(); ++i)
      {
         groupvals[i->first] = i->second;
      }
      return new GroupProperty(groupvals);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Property* RapidXMLMapEncoder::ParseProperty(xml_node<>* element)
   {
      const char* name = element->name();
      if(strncmp("array", name, 5) == 0)
      {
         return ParseArrayProperty(element);
      }
      else if(strncmp("bool", name, 4) == 0)
      {
         return ParseBoolProperty(element);
      }
      else if(strncmp("char", name, 4) == 0)
      {
         return ParseCharProperty(element);
      }
      else if(strncmp("double", name, 6) == 0)
      {
         return ParseDoubleProperty(element);
      }
      else if(strncmp("float", name, 5) == 0)
      {
         return ParseFloatProperty(element);
      }
      else if(strncmp("group", name, 5) == 0)
      {
         return ParseGroupProperty(element);
      }
      else if(strncmp("int", name, 3) == 0)
      {
         return ParseIntProperty(element);
      }
      else if(strncmp("matrix", name, 6) == 0)
      {
         return ParseMatrixProperty(element);
      }
      else if(strncmp("quat", name, 4) == 0)
      {
         return ParseQuatProperty(element);
      }
      else if(strncmp("string", name, 6) == 0)
      {
         return ParseStringProperty(element);
      }
      else if(strncmp("uint", name, 4) == 0)
      {
         return ParseUIntProperty(element);
      }
      else if(strncmp("vec2", name, 4) == 0)
      {
         return ParseVec2Property(element);
      }
      else if(strncmp("vec3", name, 4) == 0)
      {
         return ParseVec3Property(element);
      }
      else if(strncmp("vec4", name, 4) == 0)
      {
         return ParseVec4Property(element);
      }
      LOG_ERROR("Could not parse property, unknown type!");
      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CreateComponent(EntityManager& em, xml_node<>* element, EntityId entityId, const std::string& mapname)
   {
      for(xml_attribute<>* attr = element->first_attribute();
           attr; attr = attr->next_attribute())
      {
         if(strcmp(attr->name(), "type") == 0)
         {
            StringId componentType = SID(attr->value());
            Component* component;
            bool found = em.GetComponent(entityId, componentType, component);
            if(!found)
            {
               bool success = StartEntitySystem(em, componentType);
               if(!success)
               {
                  LOG_WARNING("In Map " << mapname << ": Cannot add component, no entity system of this type registered: " << GetStringFromSID(componentType));
                  return;
               }
               em.CreateComponent(entityId, componentType, component);
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SetupComponent(EntityManager& em, xml_node<>* element, EntityId entityId, const std::string& mapname)
   {
      StringId componentType;
      for(xml_attribute<>* attr = element->first_attribute();
           attr; attr = attr->next_attribute())
      {
         if(strcmp(attr->name(), "type") == 0)
         {
            componentType = SID(attr->value());
         }
      }

      Component* component;
      bool found = em.GetComponent(entityId, componentType, component);
      if(!found)
      {
         return;
      }

      for(xml_node<>* currentNode(element->first_node());
          currentNode != NULL; currentNode = currentNode->next_sibling())
      {
         if(currentNode->type() == node_element)
         {
            dtEntity::StringId namesid;
            for(xml_attribute<>* attr = currentNode->first_attribute();
                 attr; attr = attr->next_attribute())
            {
               if(strcmp(attr->name(), "name") == 0)
               {
                  namesid = SID(attr->value());
               }
            }
            Property* property = RapidXMLMapEncoder::ParseProperty(currentNode);
            if(property != NULL)
            {
               Property* toset = component->Get(namesid);
               if(toset == NULL)
               {
                  LOG_WARNING("In Map " << mapname << ": Property " << GetStringFromSID(namesid) 
					  << " does not exist in component "
					  << GetStringFromSID(componentType));
               }
               else
               {
                  toset->SetFrom(*property);
                  component->OnPropertyChanged(namesid, *toset);
               }
               delete property;
            }
         }
      }

      component->Finished();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ParseEntity(EntityManager& em, xml_node<>* element, const std::string& mapName)
   {

      dtEntity::Entity* newentity;
      bool success = em.CreateEntity(newentity);
      assert(success);


      for(xml_attribute<>* attr = element->first_attribute();
           attr; attr = attr->next_attribute())
      {
         if(strcmp(attr->name(), "spawner") == 0)
         {
            std::string spawnername = attr->value();
            if(spawnername != "")
            {
               Spawner* spawner;
               MapSystem* mapSystem;
               em.GetEntitySystem(MapComponent::TYPE, mapSystem);
               bool found = mapSystem->GetSpawner(spawnername, spawner);
               if(!found)
               {
                  LOG_ERROR("Spawner not found: " + std::string(attr->value()));
                  return;
               }

               success = spawner->Spawn(*newentity);
               assert(success);
            }
         }
      }

      for(xml_node<>* currentNode(element->first_node());
          currentNode != NULL; currentNode = currentNode->next_sibling())
      {
         if(currentNode->type() == node_element)
         {
            if(strcmp(currentNode->name(), "component") == 0)
            {
               CreateComponent(em, currentNode, newentity->GetId(), mapName);
            }
         }
      }

      for(xml_node<>* currentNode(element->first_node());
          currentNode != NULL; currentNode = currentNode->next_sibling())
      {
         if(currentNode->type() == node_element)
         {
            if(strcmp(currentNode->name(), "component") == 0)
            {
               SetupComponent(em, currentNode, newentity->GetId(), mapName);
            }
         }
      }

      // Make sure entity has a map component
      MapComponent* mc;
      if(!em.GetComponent(newentity->GetId(), mc))
      {
         em.CreateComponent(newentity->GetId(), mc);
      }
      mc->SetMapName(mapName);

      em.AddToScene(newentity->GetId());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AddComponentToSpawner(xml_node<>* element, Spawner& spawner)
   {
      std::string typestr;

      for(xml_attribute<>* attr = element->first_attribute();
           attr; attr = attr->next_attribute())
      {
         if(strcmp(attr->name(), "type") == 0)
         {
            typestr = attr->value();
         }
      }
      if(typestr.empty())
      {
         return;
      }
      DynamicPropertyContainer props;

      for(xml_node<>* currentNode(element->first_node());
          currentNode != NULL; currentNode = currentNode->next_sibling())
      {
         if(currentNode->type() == node_element)
         {
            std::string name;
            for(xml_attribute<>* attr = currentNode->first_attribute();
                 attr; attr = attr->next_attribute())
            {
               if(strcmp(attr->name(), "name") == 0)
               {
                  name = attr->value();
               }
            }
            Property* property = RapidXMLMapEncoder::ParseProperty(currentNode);

            if(property != NULL)
            {
               props.AddProperty(SID(name), *property);
               delete property;
            }
         }
      }
      spawner.AddComponent(SID(typestr), props);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ParseSpawner(EntityManager& em, xml_node<>* element, const std::string& mapName)
   {
       MapSystem* mapSystem;
       em.GetEntitySystem(MapComponent::TYPE, mapSystem);

       std::string name = "";
       std::string parent = "";
       std::string guiCategory = "";
       std::string addToSpawnerStore = "";
       std::string iconPath = "";

       for(xml_attribute<>* attr = element->first_attribute();
            attr; attr = attr->next_attribute())
       {
          if(strcmp(attr->name(), "name") == 0)
          {
             name = attr->value();
          }
          else if(strcmp(attr->name(), "parent") == 0)
          {
             parent = attr->value();
          }
          else if(strcmp(attr->name(), "guicategory") == 0)
          {
             guiCategory = attr->value();
          }
          else if(strcmp(attr->name(), "addtospawnerstore") == 0)
          {
             addToSpawnerStore = attr->value();
          }
          else if(strcmp(attr->name(), "iconpath") == 0)
          {
             iconPath = attr->value();
          }
      }

      Spawner* spawner;
      if(parent.empty())
      {
         spawner = new Spawner(name, mapName);
      }
      else
      {
         Spawner* parentspawner;
         bool found = mapSystem->GetSpawner(parent, parentspawner);
         if(!found)
         {
            LOG_ERROR("Cannot initialize spawner: Parent spawner not found. Name: " + parent);
            return;
         }
         spawner = new Spawner(name, mapName, parentspawner);
      }

      if(!guiCategory.empty())
      {
         spawner->SetGUICategory(guiCategory);
      }

      if(!iconPath.empty())
      {
         spawner->SetIconPath(iconPath);
      }

      if(!addToSpawnerStore.empty())
      {
         spawner->SetAddToSpawnerStore(addToSpawnerStore == "true" || addToSpawnerStore == "TRUE");
      }

      for(xml_node<>* currentNode(element->first_node());
          currentNode != NULL; currentNode = currentNode->next_sibling())
      {
         if(currentNode->type() == node_element)
         {
            AddComponentToSpawner(currentNode, *spawner);
         }
      }


      // add meta data component to entity
      DynamicPropertyContainer mapComponentProps;
      if(spawner->HasComponent(MapComponent::TYPE))
      {
         mapComponentProps = spawner->GetComponentValues(MapComponent::TYPE);
      }

      mapComponentProps.AddProperty(MapComponent::SpawnerNameId, StringIdProperty(SID(name)));
      mapComponentProps.AddProperty(MapComponent::MapNameId, StringProperty(mapName));
      spawner->AddComponent(MapComponent::TYPE, mapComponentProps);
      mapSystem->AddSpawner(*spawner);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SetupEntitySystem(EntityManager& em, xml_node<>* element, const std::string& filename)
   {
      StringId componentType;
      for(xml_attribute<>* attr = element->first_attribute();
           attr; attr = attr->next_attribute())
      {
         if(strcmp(attr->name(), "type") == 0)
         {
            componentType = SID(attr->value());
         }
      }

      if(!em.HasEntitySystem(componentType))
      {
         bool success = StartEntitySystem(em, componentType);
         if(!success)
         {
            LOG_WARNING("In scene " << filename << ": Cannot setup entity system. Component type not found: " + GetStringFromSID(componentType));
            return;
         }
      }
      EntitySystem* es = em.GetEntitySystem(componentType);
      assert(es != NULL);

      for(xml_node<>* currentNode(element->first_node());
          currentNode != NULL; currentNode = currentNode->next_sibling())
      {
         if(currentNode->type() == node_element)
         {
            //SetupProperty(currentElement, *es);

            std::string name;
            for(xml_attribute<>* attr = currentNode->first_attribute();
                 attr; attr = attr->next_attribute())
            {
               if(strcmp(attr->name(), "name") == 0)
               {
                  name = attr->value();
               }
            }

            dtEntity::StringId namesid = SID(name);
            Property* property = RapidXMLMapEncoder::ParseProperty(currentNode);
            if(property != NULL)
            {
               Property* toset = es->Get(namesid);
               if(toset == NULL)
               {
                  LOG_WARNING("Property " << name << " does not exist in entity system "
                     << GetStringFromSID(componentType));
               }
               else
               {
                  toset->SetFrom(*property);
                  es->OnPropertyChanged(namesid, *toset);
               }
               delete property;
            }
         }
      }
      es->Finished();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FindMapsToLoad(EntityManager& em, xml_node<>* element, std::list<std::string>& mapsToLoad)
   {
      for(xml_node<>* currentNode(element->first_node());
          currentNode != NULL; currentNode = currentNode->next_sibling())
      {
         if(currentNode->type() == node_element)
         {
            if(strcmp(currentNode->name(), "map") == 0)
            {
               for(xml_attribute<>* attr = currentNode->first_attribute();
                    attr; attr = attr->next_attribute())
               {
                  if(strcmp(attr->name(), "path") == 0)
                  {
                     mapsToLoad.push_back(attr->value());
                  }
               }
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ParseMap(EntityManager& em, xml_node<>* element, const std::string& mapPath)
   {

      for(xml_node<>* currentNode(element->first_node());
          currentNode != NULL; currentNode = currentNode->next_sibling())
      {
         if(currentNode->type() == node_element)
         {
            if(strcmp("entity", currentNode->name()) == 0)
            {
               ParseEntity(em, currentNode, mapPath);
            }
            else if(strcmp("spawner", currentNode->name()) == 0)
            {
               ParseSpawner(em, currentNode, mapPath);
            }
         }
      }
   }


   ////////////////////////////////////////////////////////////////////////////////
   void ParseScene(EntityManager& em, xml_node<>* element, std::list<std::string>& mapsToLoad, const std::string& sceneName)
   {
      // first load all libraries
      for(xml_node<>* rootLibNode(element->first_node());
         rootLibNode != NULL; rootLibNode = rootLibNode->next_sibling())
      {
         if(rootLibNode->type() == node_element)
         {
            if(strcmp(rootLibNode->name(), "libraries") == 0)
            {
               MapSystem* mapSystem;
               em.GetEntitySystem(MapComponent::TYPE, mapSystem);
               ComponentPluginManager& pluginManager = mapSystem->GetPluginManager();

               // parse all child element as they contain library names
               for(xml_node<>* currLibNode(rootLibNode->first_node());
                  currLibNode != NULL; currLibNode = currLibNode->next_sibling())
               {
                  if(currLibNode->type() == node_element)
                  {
                     if(strcmp(currLibNode->name(), "library") == 0)
                     {
                        std::string fullLibName =
                           osgDB::Registry::instance()->createLibraryNameForNodeKit(currLibNode->value());
                        pluginManager.AddPlugin(fullLibName, true);
                     }
                  }
               }
            }
         }
      }

      // then parse entity systems
      for(xml_node<>* currentNode(element->first_node());
          currentNode != NULL; currentNode = currentNode->next_sibling())
      {
         if(currentNode->type() == node_element)
         {
            if(strcmp(currentNode->name(), "entitysystem") == 0)
            {
               SetupEntitySystem(em, currentNode, sceneName);
            }
         }
      }


      // then load maps
      for(xml_node<>* currentNode(element->first_node());
          currentNode != NULL; currentNode = currentNode->next_sibling())
      {
         if(currentNode->type() == node_element)
         {
            // Found node which is an Element. Re-cast node as element
            if(strcmp(currentNode->name(), "maps") == 0)
            {
               FindMapsToLoad(em, currentNode, mapsToLoad);
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   struct Names
   {
      Names(xml_document<>& doc)
      {
         mAddToSpawnerStore = doc.allocate_string("addtospawnerstore");
         mComponent = doc.allocate_string("component");
         mEntity = doc.allocate_string("entity");
         mEntitySystem = doc.allocate_string("entitysystem");
         mFalse = doc.allocate_string("false");
         mGuiCategory = doc.allocate_string("guicategory");
         mIconPath = doc.allocate_string("iconpath");
         mLibraries = doc.allocate_string("libraries");
         mLibrary = doc.allocate_string("library");
         mMap = doc.allocate_string("map");
         mMaps = doc.allocate_string("maps");
         mName = doc.allocate_string("name");
         mPath = doc.allocate_string("path");
         mParent = doc.allocate_string("parent");
         mScene = doc.allocate_string("scene");
         mSpawner = doc.allocate_string("spawner");
         mTrue = doc.allocate_string("true");
         mType = doc.allocate_string("type");
         mValue = doc.allocate_string("value");

         mBoolProperty = doc.allocate_string("boolproperty");
         mCharProperty = doc.allocate_string("charproperty");
         mIntProperty = doc.allocate_string("intproperty");
         mUIntProperty = doc.allocate_string("uintproperty");
         mDoubleProperty = doc.allocate_string("doubleproperty");
         mFloatProperty = doc.allocate_string("floatproperty");
         mMatrixProperty = doc.allocate_string("matrixproperty");
         mVec2Property = doc.allocate_string("vec2property");
         mVec3Property = doc.allocate_string("vec3property");
         mVec4Property = doc.allocate_string("vec4property");
         mQuatProperty = doc.allocate_string("quatproperty");
         mStringProperty = doc.allocate_string("stringproperty");
         mArrayProperty = doc.allocate_string("arrayproperty");
         mGroupProperty = doc.allocate_string("groupproperty");

         mX = doc.allocate_string("x");
         mY = doc.allocate_string("y");
         mZ = doc.allocate_string("z");
         mW = doc.allocate_string("w");
      }

      char* mAddToSpawnerStore;
      char* mComponent;
      char* mEntity;
      char* mEntitySystem;
      char* mFalse;
      char* mGuiCategory;
      char* mIconPath;
      char* mLibrary;
      char* mLibraries;
      char* mMap;
      char* mMaps;
      char* mName;
      char* mPath;
      char* mParent;
      char* mScene;
      char* mSpawner;
      char* mTrue;
      char* mType;
      char* mValue;

      char* mX;
      char* mY;
      char* mZ;
      char* mW;

      char* mBoolProperty;
      char* mCharProperty;
      char* mIntProperty;
      char* mUIntProperty;
      char* mDoubleProperty;
      char* mFloatProperty;
      char* mMatrixProperty;
      char* mVec2Property;
      char* mVec3Property;
      char* mVec4Property;
      char* mQuatProperty;
      char* mStringProperty;
      char* mArrayProperty;
      char* mGroupProperty;

   };

   ////////////////////////////////////////////////////////////////////////////////
   // fwd decl
   xml_node<>* SerializeProperty(xml_document<>& doc, const Names& names,  const char* name, const Property* prop);

   ////////////////////////////////////////////////////////////////////////////////
   void RapidXMLMapEncoder::SerializeProperties(xml_document<>& doc, xml_node<>* parent,  const std::map<std::string, const Property*>& props)
   {
      Names names(doc);
      std::map<std::string, const Property*>::const_iterator i;
      for(i = props.begin(); i != props.end(); ++i)
      {
         std::string name = i->first;
         const Property* prop = i->second;

         xml_node<>* node = SerializeProperty(doc, names, doc.allocate_string(name.c_str()), prop);
         parent->append_node(node);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   xml_node<>* SerializePropertyFromString(xml_document<>& doc, const Names& names, const char* tagname, const Property* prop)
   {
      xml_node<>* entity = doc.allocate_node(node_element, tagname);
      entity->append_attribute(doc.allocate_attribute(names.mValue, doc.allocate_string(prop->StringValue().c_str())));
      return entity;
   }

   ////////////////////////////////////////////////////////////////////////////////
   xml_node<>* SerializeVec2Property(xml_document<>& doc, const Names& names, const osg::Vec2d& v)
   {
      xml_node<>* entity = doc.allocate_node(node_element, names.mVec2Property);
      entity->append_attribute(doc.allocate_attribute(names.mX, ToString(doc, v[0])));
      entity->append_attribute(doc.allocate_attribute(names.mY, ToString(doc, v[1])));
      return entity;
   }

   ////////////////////////////////////////////////////////////////////////////////
   xml_node<>* SerializeVec3Property(xml_document<>& doc, const Names& names, const osg::Vec3d& v)
   {
      xml_node<>* entity = doc.allocate_node(node_element, names.mVec3Property);
      entity->append_attribute(doc.allocate_attribute(names.mX, ToString(doc, v[0])));
      entity->append_attribute(doc.allocate_attribute(names.mY, ToString(doc, v[1])));
      entity->append_attribute(doc.allocate_attribute(names.mZ, ToString(doc, v[2])));
      return entity;
   }

   ////////////////////////////////////////////////////////////////////////////////
   xml_node<>* SerializeVec4Property(xml_document<>& doc, const Names& names, const osg::Vec4d& v)
   {
      xml_node<>* entity = doc.allocate_node(node_element, names.mVec4Property);
      entity->append_attribute(doc.allocate_attribute(names.mX, ToString(doc, v[0])));
      entity->append_attribute(doc.allocate_attribute(names.mY, ToString(doc, v[1])));
      entity->append_attribute(doc.allocate_attribute(names.mZ, ToString(doc, v[2])));
      entity->append_attribute(doc.allocate_attribute(names.mW, ToString(doc, v[3])));
      return entity;
   }

   ////////////////////////////////////////////////////////////////////////////////
   xml_node<>* SerializeQuatProperty(xml_document<>& doc, const Names& names, const osg::Quat& v)
   {
      xml_node<>* entity = doc.allocate_node(node_element, names.mQuatProperty);
      entity->append_attribute(doc.allocate_attribute(names.mX, ToString(doc, v[0])));
      entity->append_attribute(doc.allocate_attribute(names.mY, ToString(doc, v[1])));
      entity->append_attribute(doc.allocate_attribute(names.mZ, ToString(doc, v[2])));
      entity->append_attribute(doc.allocate_attribute(names.mW, ToString(doc, v[3])));
      return entity;
   }

   ////////////////////////////////////////////////////////////////////////////////
   xml_node<>* SerializeStringProperty(xml_document<>& doc, const Names& names, const std::string& v)
   {
      xml_node<>* entity = doc.allocate_node(node_element, names.mStringProperty);
      entity->value(doc.allocate_string(v.c_str()));
      return entity;
   }

   ////////////////////////////////////////////////////////////////////////////////
   xml_node<>* SerializeArrayProperty(xml_document<>& doc, const Names& names, const ArrayProperty* prop)
   {
      xml_node<>* entity = doc.allocate_node(node_element, names.mArrayProperty);

      PropertyArray arr = prop->Get();

      for(unsigned int i = 0; i < arr.size(); ++i)
      {
         std::ostringstream os;
         os << i;
         entity->append_node(SerializeProperty(doc, names, doc.allocate_string(os.str().c_str()), arr[i]));
      }
      return entity;
   }

   ////////////////////////////////////////////////////////////////////////////////
   xml_node<>* SerializeGroupProperty(xml_document<>& doc, const Names& names,const GroupProperty* prop)
   {

      xml_node<>* entity = doc.allocate_node(node_element, names.mGroupProperty);

      PropertyGroup grp = prop->Get();

      std::map<std::string, const Property*> sorted;
      for(PropertyGroup::iterator i = grp.begin(); i != grp.end(); ++i)
      {
         sorted[GetStringFromSID(i->first)] = i->second;
      }

      for(std::map<std::string, const Property*>::const_iterator i = sorted.begin(); i != sorted.end(); ++i)
      {
         entity->append_node(SerializeProperty(doc, names, doc.allocate_string(i->first.c_str()), i->second));
      }
      return entity;
   }

   ////////////////////////////////////////////////////////////////////////////////
   xml_node<>* SerializeProperty(xml_document<>& doc, const Names& names,  const char* name, const Property* prop)
   {
      xml_node<>* propelem;
      switch(prop->GetType())
      {
      case DataType::BOOL: propelem = SerializePropertyFromString(doc, names, names.mBoolProperty, prop); break;
      case DataType::CHAR: propelem = SerializePropertyFromString(doc, names, names.mCharProperty, prop); break;
      case DataType::INT: propelem = SerializePropertyFromString(doc, names, names.mIntProperty, prop); break;
      case DataType::UINT: propelem = SerializePropertyFromString(doc, names, names.mUIntProperty, prop); break;
      case DataType::DOUBLE: propelem = SerializePropertyFromString(doc, names, names.mDoubleProperty, prop); break;
      case DataType::FLOAT: propelem = SerializePropertyFromString(doc, names, names.mFloatProperty, prop); break;
      case DataType::MATRIX: propelem = SerializePropertyFromString(doc, names, names.mMatrixProperty, prop); break;
      case DataType::VEC2:
      case DataType::VEC2D: propelem = SerializeVec2Property(doc, names, prop->Vec2dValue()); break;
      case DataType::VEC3:
      case DataType::VEC3D: propelem = SerializeVec3Property(doc, names, prop->Vec3dValue()); break;
      case DataType::VEC4:
      case DataType::VEC4D: propelem = SerializeVec4Property(doc, names, prop->Vec4dValue()); break;
      case DataType::QUAT: propelem = SerializeQuatProperty(doc, names, prop->QuatValue()); break;
      case DataType::STRING:
      case DataType::STRINGID: propelem = SerializeStringProperty(doc, names, prop->StringValue()); break;
      case DataType::ARRAY: propelem = SerializeArrayProperty(doc, names, static_cast<const ArrayProperty*>(prop)); break;
      case DataType::GROUP: propelem = SerializeGroupProperty(doc, names, static_cast<const GroupProperty*>(prop)); break;
      default:
         {
            LOG_ERROR("Unknown property type, cannot serialize");
            return NULL;
         }
      }

      xml_attribute<>* nameattr = doc.allocate_attribute(names.mName, name);
      propelem->prepend_attribute(nameattr);
      return propelem;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SerializeSpawner(xml_document<>& doc, const Names& names, xml_node<>* parent, Spawner* spawner)
   {

      xml_node<>* entity = doc.allocate_node(node_element, names.mSpawner);

      parent->append_node(entity);

      xml_attribute<>* nameattr = doc.allocate_attribute(names.mName, doc.allocate_string(spawner->GetName().c_str()));
      entity->append_attribute(nameattr);

      xml_attribute<>* guicatattr = doc.allocate_attribute(names.mGuiCategory, doc.allocate_string(spawner->GetGUICategory().c_str()));
      entity->append_attribute(guicatattr);

      xml_attribute<>* storeattr = doc.allocate_attribute(names.mAddToSpawnerStore, spawner->GetAddToSpawnerStore() ? names.mTrue :  names.mFalse);
      entity->append_attribute(storeattr);

      xml_attribute<>* iconpathattr = doc.allocate_attribute(names.mIconPath, doc.allocate_string(spawner->GetIconPath().c_str()));
      entity->append_attribute(iconpathattr);

      if(spawner->GetParent() != NULL)
      {
         xml_attribute<>* parentattr = doc.allocate_attribute(names.mParent, doc.allocate_string(spawner->GetParent()->GetName().c_str()));
         entity->append_attribute(parentattr);
      }

      Spawner::ComponentProperties spawnerprops;
      spawner->GetAllComponentProperties(spawnerprops);

      // write components sorted by component type name
      std::map<std::string, DynamicPropertyContainer> sorted;
      Spawner::ComponentProperties::const_iterator j;
      for(j = spawnerprops.begin(); j != spawnerprops.end(); ++j)
      {
         // no need to save map component, is redundant
         if(j->first == MapComponent::TYPE)
         {
            continue;
         }
         sorted[GetStringFromSID(j->first)] = j->second;
      }


      std::map<std::string, DynamicPropertyContainer>::const_iterator i;
      for(i = sorted.begin(); i != sorted.end(); ++i)
      {
         std::string tname = i->first;

         DynamicPropertyContainer props = i->second;

         xml_node<>* compelem = doc.allocate_node(node_element, names.mComponent);
         entity->append_node(compelem);

         xml_attribute<>* typeattr = doc.allocate_attribute(names.mType, doc.allocate_string(tname.c_str()));
         compelem->append_attribute(typeattr);

         PropertyContainer::ConstPropertyMap p;
         props.GetProperties(p);

         std::map<std::string, const Property*> sorted;
         for(PropertyContainer::ConstPropertyMap::const_iterator j = p.begin(); j != p.end(); ++j)
         {
            sorted[GetStringFromSID(j->first)] = j->second;
         }

         for(std::map<std::string, const Property*>::const_iterator j = sorted.begin(); j != sorted.end(); ++j)
         {
            compelem->append_node(SerializeProperty(doc, names, doc.allocate_string(j->first.c_str()), j->second));
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SerializeComponent(xml_document<>& doc, const Names& names, xml_node<>* parent, const Component* component, const PropertyContainer& defaults)
   {
      xml_node<>* element = doc.allocate_node(node_element, names.mComponent);
      parent->append_node(element);

      std::string tname = GetStringFromSID(component->GetType());

      xml_attribute<>* typeattr = doc.allocate_attribute(names.mType, doc.allocate_string(tname.c_str()));
      element->append_attribute(typeattr);

      PropertyContainer::ConstPropertyMap props;
      component->GetProperties(props);

      // write properties sorted by property name
      std::map<std::string, const Property*> sorted;
      PropertyContainer::ConstPropertyMap::const_iterator i;
      for(i = props.begin(); i != props.end(); ++i)
      {
         sorted[GetStringFromSID(i->first)] = i->second;
      }

      std::map<std::string, const Property*>::const_iterator j;
      for(j = sorted.begin(); j != sorted.end(); ++j)
      {
         // only save property to map if it is changed from its default
         const Property* deflt = defaults.Get(SID(j->first));
         if(!((*deflt) == (*j->second)))
         {
            element->append_node(SerializeProperty(doc, names, doc.allocate_string(j->first.c_str()), j->second));
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SerializeEntity(EntityManager& em, xml_document<>& doc, const Names& names, xml_node<>* parent, EntityId eid)
   {
      MapSystem* mapSystem;
      em.GetEntitySystem(MapComponent::TYPE, mapSystem);

      // get spawner of entity so that only property values are changed that are not identical
      // to the values from the spawner
      Spawner::ComponentProperties spawnerprops;

      MapComponent* mapcomp = NULL;
      if(em.GetComponent(eid, mapcomp))
      {
         std::string spawnername = mapcomp->GetSpawnerName();
         if(spawnername != "")
         {
            Spawner* spawner;
            if(mapSystem->GetSpawner(spawnername, spawner))
            {
               spawner->GetAllComponentPropertiesRecursive(spawnerprops);
            }
         }
      }

      std::vector<const Component*> comps;
      em.GetComponents(eid, comps);

      // write components sorted by component type name
      std::map<std::string, const Component*> sorted;
      std::vector<const Component*>::const_iterator i;
      for(i = comps.begin(); i != comps.end(); ++i)
      {
         sorted[GetStringFromSID((*i)->GetType())] = *i;
      }

      xml_node<>* entity = doc.allocate_node(node_element, names.mEntity);
      parent->append_node(entity);

      if(mapcomp != NULL && mapcomp->GetSpawnerName() != "")
      {
         xml_attribute<>* attr = doc.allocate_attribute(names.mSpawner, doc.allocate_string(mapcomp->GetSpawnerName().c_str()));
         entity->append_attribute(attr);
      }

      std::map<std::string, const Component*>::const_iterator j;
      for(j = sorted.begin(); j != sorted.end(); ++j)
      {
         EntitySystem* es = em.GetEntitySystem(j->second->GetType());

         if(es->StoreComponentToMap(eid))
         {
            // Get default component values. If entity was created from a spawner
            // then overwrite component values with values from spawner
            DynamicPropertyContainer defaultprops = es->GetComponentProperties();
            Spawner::ComponentProperties::iterator it = spawnerprops.find(j->second->GetType());
            if(it != spawnerprops.end())
            {
               defaultprops += (it->second);
            }

            SerializeComponent(doc, names, entity, j->second, defaultprops);
         }
      }
   }


   ////////////////////////////////////////////////////////////////////////////////
   void SerializeEntitySystem(xml_document<>& doc, const Names& names, xml_node<>* parent, const EntitySystem* es)
   {
      xml_node<>* entity = doc.allocate_node(node_element, names.mEntitySystem);
      parent->append_node(entity);

      std::string tname = GetStringFromSID(es->GetComponentType());
      xml_attribute<>* attr = doc.allocate_attribute(names.mType, doc.allocate_string(tname.c_str()));
      entity->append_attribute(attr);

      PropertyContainer::ConstPropertyMap props;
      es->GetProperties(props);

      std::map<std::string, const Property*> sorted;
      PropertyContainer::ConstPropertyMap::const_iterator i;
      for(i = props.begin(); i != props.end(); ++i)
      {
         sorted[GetStringFromSID(i->first)] = i->second;
      }

      std::map<std::string, const Property*>::const_iterator j;
      for(j = sorted.begin(); j != sorted.end(); ++j)
      {
         entity->append_node(SerializeProperty(doc, names, doc.allocate_string(j->first.c_str()), j->second));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SerializeMap(xml_document<>& doc, const Names& names, xml_node<>* parent, const std::string& path)
   {
      xml_node<>* entity = doc.allocate_node(node_element, names.mMap);
      parent->append_node(entity);

      xml_attribute<>* attr = doc.allocate_attribute(names.mPath, doc.allocate_string(path.c_str()));
      entity->append_attribute(attr);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool RapidXMLMapEncoder::LoadMapFromFile(const std::string& path)
   {

      const std::string absPath = osgDB::findDataFile(path);
      if(absPath == "")
      {
         LOG_ERROR("Map not found: " + path);
         return false;
      }

      file<> file(absPath.c_str());
		bool success = true;

      try
      {
         xml_document<> doc;    // character type defaults to char
         doc.parse<0>(file.data());

			xml_node<>* mapnode = doc.first_node("map");
			if(mapnode == NULL)
			{
				return false;
			}
			ParseMap(*mEntityManager, mapnode, path);
		}
		catch(const std::exception& ex)
		{
			LOG_ERROR("XML Parsing error: "+ std::string(ex.what()));
			success = false;
		}

		return success;

	}

   ////////////////////////////////////////////////////////////////////////////////
   bool RapidXMLMapEncoder::LoadSceneFromFile(const std::string& path)
   {
      const std::string absPath = osgDB::findDataFile(path);
      if(absPath == "")
      {
         return false;
      }
      std::list<std::string> mapsToLoad;

      file<> file(absPath.c_str());
      try
      {
         xml_document<> doc;    // character type defaults to char
         doc.parse<0>(file.data());

			xml_node<>* scenenode = doc.first_node("scene");
			ParseScene(*mEntityManager, scenenode, mapsToLoad, path);

		}
		catch(const std::exception& ex)
		{
			LOG_ERROR("XML Parsing error: "+ std::string(ex.what()));
			return false;
		}

      for(std::list<std::string>::iterator i = mapsToLoad.begin(); i != mapsToLoad.end(); ++i)
      {
         mMapSystem->LoadMap(*i);
      }

      return true;
   }


   ////////////////////////////////////////////////////////////////////////////////
   bool RapidXMLMapEncoder::SaveMapToFile(const std::string& path, const std::string& p_dest)
   {

      xml_document<> doc;
      Names names(doc);

      xml_node<>* mapelem = doc.allocate_node(node_element, names.mMap);
      doc.append_node(mapelem);
      
      // write spawners to map
      {
         // We have to take care to write child spawners after their parents
         std::map<std::string, Spawner*> spawnerset;
         std::map<std::string, Spawner*> spawners;
         mMapSystem->GetAllSpawners(spawners);
         std::map<std::string, Spawner*>::const_iterator i;
         for(i = spawners.begin(); i != spawners.end(); ++i)
         {
            if(i->second->GetMapName() == path)
            {
               spawnerset[i->first] = i->second;
            }
         }

         while(!spawnerset.empty())
         {
            Spawner* candidate = spawnerset.begin()->second;
            while(true)
            {
               Spawner* parent = candidate->GetParent();
               if(parent != NULL && spawnerset.find(parent->GetName()) != spawnerset.end())
               {
                  candidate = candidate->GetParent();
               }
               else
               {
                  break;
               }
            }

            spawnerset.erase(candidate->GetName());
            SerializeSpawner(doc, names, mapelem, candidate);
         }
      }

      // write entities in the order of their unique ids

      {
         MapComponent* mapcomp;
         std::map<std::string, dtEntity::EntityId> sorted;
         std::vector<EntityId>::const_iterator i;

         std::vector<EntityId> eids;
         mMapSystem->GetEntitiesInMap(path, eids);
         for(i = eids.begin(); i != eids.end(); ++i)
         {
            if(mEntityManager->GetComponent(*i, mapcomp) && mapcomp->GetSaveWithMap())
            {
               sorted[mapcomp->GetUniqueId()] = *i;
            }
         }

         std::map<std::string, dtEntity::EntityId>::const_iterator j;
         for(j = sorted.begin(); j != sorted.end(); ++j)
         {
            SerializeEntity(*mEntityManager, doc, names, mapelem, j->second);
         }
      }
     
      std::ofstream of(p_dest.c_str());
	   if(of.fail())
	   {
	 	  LOG_ERROR("Cannot open file for writing: " << p_dest);
 		  return false;
      }
      of << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\" ?>\n";
      of << doc;
      of.close();
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool RapidXMLMapEncoder::SaveSceneToFile(const std::string& path)
   {
      xml_document<> doc;
      xml_node<>* sceneelem = doc.allocate_node(node_element, "scene");
      doc.append_node(sceneelem);
      Names names(doc);

      // first output the list of plugins to load with scene
      xml_node<>* libsElem = doc.allocate_node(node_element, names.mLibraries);
      sceneelem->append_node(libsElem);
      MapSystem* mapSystem;
      mEntityManager->GetEntitySystem(MapComponent::TYPE, mapSystem);
      ComponentPluginManager& pluginManager = mapSystem->GetPluginManager();

      const std::map<std::string, bool>& pluginList = pluginManager.GetLoadedPlugins();
      std::map<std::string, bool>::const_iterator itr;
      for(itr = pluginList.begin(); itr != pluginList.end(); itr++)
      {
         if (itr->second == true)
         {
            // this plugin must be saved to file: add it
            std::string currLib(itr->first);
            xml_node<>* currLibElem = doc.allocate_node(node_element, names.mLibrary);
            libsElem->append_node(currLibElem);
            currLibElem->value(doc.allocate_string(currLib.c_str()));
         }
      }

      std::vector<const EntitySystem*> es;
      mEntityManager->GetEntitySystems(es);
      std::vector<const EntitySystem*>::const_iterator i;
      for(i = es.begin(); i != es.end(); ++i)
      {
         if((*i)->StorePropertiesToScene())
         {
            SerializeEntitySystem(doc, names, sceneelem, *i);
         }
      }

      std::vector<std::string> maps = mMapSystem->GetLoadedMaps();
      if(!maps.empty())
      {
         typedef std::map<unsigned int, std::string> Ordered;
         Ordered ordered;

         for(std::vector<std::string>::iterator j = maps.begin(); j != maps.end(); ++j)
         {
            ordered[mapSystem->GetMapSaveOrder(*j)] = *j;
         }

         xml_node<>* mapselem = doc.allocate_node(node_element, names.mMaps);
         sceneelem->append_node(mapselem);
         Ordered::iterator k;
         for(k = ordered.begin(); k != ordered.end(); ++k)
         {
            SerializeMap(doc, names, mapselem, k->second);
         }
      }

      std::ofstream of(path.c_str());

		if(of.fail())
		{
		  LOG_ERROR("Cannot open file for writing: " << path);
		  return false;
		}
      of << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\" ?>\n";
      of << doc;
      of.close();

      return true;
   }   
}
