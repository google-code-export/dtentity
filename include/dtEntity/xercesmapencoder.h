/* -*-c++-*-
* Delta3D Open Source Game and Simulation Engine
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

#ifndef DELTA_ENTITY_XERCES_MAP
#define DELTA_ENTITY_XERCES_MAP

#include <dtEntity/export.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/mapencoder.h>
#include <dtEntity/property.h>
#include <osg/ref_ptr>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <osg/Timer>

namespace dtEntity
{

   struct XMLNames;
   class Spawner;
   class MapSystem;

   class DT_ENTITY_EXPORT XercesMapEncoder
         : public MapEncoder
   {
      
   public:

      XercesMapEncoder(EntityManager& em);
      virtual ~XercesMapEncoder();

      virtual bool LoadMapFromFile(const std::string& path);

      // empty destination means overwrite mapPath
      virtual bool SaveMapToFile(const std::string& mapPath, const std::string& destination = "");

      virtual bool LoadSceneFromFile(const std::string& path);

      // empty dest means overwrite original file
      virtual bool SaveSceneToFile(const std::string& path);

      void SetDocument(xercesc::DOMDocument* doc) { mDocument = doc; }
      xercesc::DOMDocument* GetDocument() const { return mDocument; }
   

      bool StartEntitySystem(StringId componentType);
	   void ParseMap(xercesc::DOMElement* element, const std::string& mapPath);
	   void ParseSpawner(xercesc::DOMElement* element, const std::string& mapName);
      void ParseEntity(xercesc::DOMElement* element, const std::string& mapName);
      void CreateComponent(xercesc::DOMElement* element, EntityId entityId);
      void SetupComponent(xercesc::DOMElement* element, EntityId entityId);
      void SetupProperty(xercesc::DOMElement* element, PropertyContainer& component);
      void AddComponentToSpawner(xercesc::DOMElement* element, Spawner& spawner);
      bool ParseMessage(xercesc::DOMElement* element, dtEntity::Message*& msg, osg::Timer_t& time);

      Property* ParseProperty(xercesc::DOMElement* element);
      BoolProperty* ParseBoolProperty(xercesc::DOMElement* element);
      CharProperty* ParseCharProperty(xercesc::DOMElement* element);
      IntProperty* ParseIntProperty(xercesc::DOMElement* element);
      DoubleProperty* ParseDoubleProperty(xercesc::DOMElement* element);
      FloatProperty* ParseFloatProperty(xercesc::DOMElement* element);
      MatrixProperty* ParseMatrixProperty(xercesc::DOMElement* element);
      Vec2dProperty* ParseVec2Property(xercesc::DOMElement* element);
      Vec3dProperty* ParseVec3Property(xercesc::DOMElement* element);
      Vec4dProperty* ParseVec4Property(xercesc::DOMElement* element);
      QuatProperty* ParseQuatProperty(xercesc::DOMElement* element);
      StringProperty* ParseStringProperty(xercesc::DOMElement* element);
      UIntProperty* ParseUIntProperty(xercesc::DOMElement* element);
      ArrayProperty* ParseArrayProperty(xercesc::DOMElement* element);
      GroupProperty* ParseGroupProperty(xercesc::DOMElement* element);


      void SerializeSpawner(xercesc::DOMElement* parent, Spawner* spawner);
      void SerializeEntity(xercesc::DOMElement* parent, EntityId eid);
      void SerializeEntitySystem(xercesc::DOMElement* parent, const EntitySystem* es);
      void SerializeMessage(xercesc::DOMElement* parent, const Message* msg, osg::Timer_t timeval);
      void SerializeMap(xercesc::DOMElement* parent, const std::string& path);
      void SerializeComponent(xercesc::DOMElement* parent, const Component* component, const PropertyContainer& defaults);
      void SerializeProperty(xercesc::DOMElement* parent, const std::string& name, const Property* prop);
      void SerializeBoolProperty(xercesc::DOMElement* parent, const std::string& name, const BoolProperty* prop);
      void SerializeCharProperty(xercesc::DOMElement* parent, const std::string& name, const CharProperty* prop);
      void SerializeDoubleProperty(xercesc::DOMElement* parent, const std::string& name, const DoubleProperty* prop);
      void SerializeIntProperty(xercesc::DOMElement* parent, const std::string& name, const IntProperty* prop);
      void SerializeUIntProperty(xercesc::DOMElement* parent, const std::string& name, const UIntProperty* prop);
      void SerializeFloatProperty(xercesc::DOMElement* parent, const std::string& name, const FloatProperty* prop);
      void SerializeMatrixProperty(xercesc::DOMElement* parent, const std::string& name, const MatrixProperty* prop);
      void SerializeVec2Property(xercesc::DOMElement* parent, const std::string& name, const osg::Vec2d& v);
      void SerializeVec3Property(xercesc::DOMElement* parent, const std::string& name, const osg::Vec3d& v);
      void SerializeVec4Property(xercesc::DOMElement* parent, const std::string& name, const osg::Vec4d& v);
      void SerializeQuatProperty(xercesc::DOMElement* parent, const std::string& name, const QuatProperty* prop);
      void SerializeStringProperty(xercesc::DOMElement* parent, const std::string& name, const StringProperty* prop);
      void SerializeStringIdProperty(xercesc::DOMElement* parent, const std::string& name, const StringIdProperty* prop);
      void SerializeArrayProperty(xercesc::DOMElement* parent, const std::string& name, const ArrayProperty* prop);
      void SerializeGroupProperty(xercesc::DOMElement* parent, const std::string& name, const GroupProperty* prop);


      void SetupEntitySystem(xercesc::DOMElement* element);
      void FindMapsToLoad(xercesc::DOMElement* element, std::list<std::string>& mapsToLoad);
      void ParseScene(xercesc::DOMElement* element, std::list<std::string>& mapsToLoad);

   private:
      EntityManager* mEntityManager;
      MapSystem* mMapSystem;
      XMLNames* mNames;
      xercesc::DOMDocument* mDocument;

   };
}

#endif // DELTA_ENTITY_XERCES_MAP
