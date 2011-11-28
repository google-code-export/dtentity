/*
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

#include <dtEntity/xercesmapencoder.h>

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
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMText.hpp> 
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace xercesc;

namespace dtEntity
{
 
   static bool sXercesInitialized = false;

   // lifted from http://www.codeguru.com/forum/showthread.php?t=231054
   template <class T>
   bool fromString(T& t, const std::string& s, std::ios_base& (*f)(std::ios_base&) = std::dec)
   {
     std::istringstream iss(s);
     return !(iss >> f >> t).fail();
   }

   /////////////////////////////////////////////////////////////////////////////

   /**
    * Utility methods for using strings, often for XML purposes.
    * This is a simple class that lets us do easy (though not terribly efficient)
    * trancoding of string data to XMLCh.
    */
   class StringToXMLConverter
   {
      public:
         StringToXMLConverter(const std::string& charData): mXmlForm(NULL)
         {
            mXmlForm = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(charData.c_str());
         }

         ~StringToXMLConverter()
         {
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&mXmlForm);
         }

         /**
          * @return the XMLCh string as a char*
          */
         const XMLCh* ToXmlString()
         {
            return mXmlForm;
         }
      private:
         XMLCh* mXmlForm;
         StringToXMLConverter(const StringToXMLConverter&) {}
         StringToXMLConverter& operator=(const StringToXMLConverter&) { return *this;}
   };

   /////////////////////////////////////////////////////////////////////////////

   void LogNotDefined(const std::string& compname, const std::string& propname)
   {
      std::ostringstream os; 
      os << "Could not set property " << propname << " of component ";
      os << compname << ", does not exist!";
      LOG_ERROR(os.str());
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   std::string ToString(unsigned int i)
   {
      // add some leading zeros to keep alphabetical ordering
      std::ostringstream os; os << std::setfill('0') << std::setw(6) << i; return os.str();
   }

   ////////////////////////////////////////////////////////////////////////////////
   struct XMLNames
   {
      XMLCh* mAddToSpawnerStore;
      XMLCh* mComponentType;
      XMLCh* mGUICategory;
      XMLCh* mH;
      XMLCh* mHandle;
      XMLCh* mIconPath;
      XMLCh* mName;
      XMLCh* mP;
      XMLCh* mPath;
      XMLCh* mParent;
      XMLCh* mR;
      XMLCh* mSpawner;
      XMLCh* mString;
      XMLCh* mTime;
      XMLCh* mType;
      XMLCh* mUniqueId;
      XMLCh* mValue;
      XMLCh* mW;
      XMLCh* mX;
      XMLCh* mY;
      XMLCh* mZ;


      XMLCh* mArrayPropertyTag;
      XMLCh* mBoolPropertyTag;
      XMLCh* mCharPropertyTag;
      XMLCh* mComponentTag;
      XMLCh* mDoublePropertyTag;
      XMLCh* mEntityTag;
      XMLCh* mEntitySystemTag;
      XMLCh* mGroupPropertyTag;
      XMLCh* mIntPropertyTag;
      XMLCh* mFloatPropertyTag;
      XMLCh* mMapTag;	 
      XMLCh* mMapsTag;	 
      XMLCh* mMatrixPropertyTag;
      XMLCh* mMessageTag;
      XMLCh* mPropertyTag;
      XMLCh* mQuatPropertyTag;
      XMLCh* mSpawnerTag;
      XMLCh* mStringPropertyTag;
      XMLCh* mUIntPropertyTag;
      XMLCh* mVec2PropertyTag;
      XMLCh* mVec3PropertyTag;
      XMLCh* mVec4PropertyTag;

      XMLNames()
      {
        
         // Tags and attributes used in XML file.
         mAddToSpawnerStore = XMLString::transcode("addtospawnerstore");
         mArrayPropertyTag = XMLString::transcode("arrayproperty");
         mBoolPropertyTag = XMLString::transcode("boolproperty");
         mComponentTag = XMLString::transcode("component");
         mCharPropertyTag = XMLString::transcode("charproperty");
         mDoublePropertyTag = XMLString::transcode("doubleproperty");
         mFloatPropertyTag = XMLString::transcode("floatproperty");
         mEntityTag = XMLString::transcode("entity");
         mEntitySystemTag = XMLString::transcode("entitysystem");
         mGroupPropertyTag = XMLString::transcode("groupproperty");
         mGUICategory = XMLString::transcode("guicategory");
         mIconPath = XMLString::transcode("iconpath");
         mIntPropertyTag = XMLString::transcode("intproperty");
         mUIntPropertyTag = XMLString::transcode("uintproperty");
         mMapTag = XMLString::transcode("map");
         mMapsTag = XMLString::transcode("maps");
         mMatrixPropertyTag = XMLString::transcode("matrixproperty");
         mMessageTag = XMLString::transcode("message");
         mPropertyTag = XMLString::transcode("property");
         mQuatPropertyTag = XMLString::transcode("quatproperty");
         mSpawnerTag = XMLString::transcode("spawner");
         mStringPropertyTag = XMLString::transcode("stringproperty");
         mVec2PropertyTag = XMLString::transcode("vec2property");
         mVec3PropertyTag = XMLString::transcode("vec3property");
         mVec4PropertyTag = XMLString::transcode("vec4property");
         
         mName = XMLString::transcode("name");
         mPath = XMLString::transcode("path");
         mParent = XMLString::transcode("parent");
         mX = XMLString::transcode("x");
         mY = XMLString::transcode("y");
         mZ = XMLString::transcode("z");
         mW = XMLString::transcode("w");
         mH = XMLString::transcode("h");
         mP = XMLString::transcode("p");
         mR = XMLString::transcode("r");
         mString = XMLString::transcode("string");
         mSpawner = XMLString::transcode("spawner");
         mTime = XMLString::transcode("time");
         mType = XMLString::transcode("type");
         mHandle = XMLString::transcode("handle");
         mComponentType = XMLString::transcode("componenttype");
         mUniqueId = XMLString::transcode("uniqueid");
         mValue = XMLString::transcode("value");
      }

      ~XMLNames()
      {
         try
         {
            XMLString::release(&mAddToSpawnerStore);
            XMLString::release(&mGUICategory);
            XMLString::release(&mIconPath);
            XMLString::release(&mX);
            XMLString::release(&mY);
            XMLString::release(&mZ);
            XMLString::release(&mW);
            XMLString::release(&mH);
            XMLString::release(&mP);
            XMLString::release(&mPath);
            XMLString::release(&mParent);
            XMLString::release(&mR);
            XMLString::release(&mParent);
            XMLString::release(&mString);
            XMLString::release(&mSpawner);
            XMLString::release(&mTime);
            XMLString::release(&mType);
            XMLString::release(&mHandle);
            XMLString::release(&mUniqueId);
            XMLString::release(&mComponentType);
            XMLString::release(&mValue);
            XMLString::release(&mName);
         
            XMLString::release(&mMapTag);	 
            XMLString::release(&mMapsTag);	 
            XMLString::release(&mSpawnerTag);	 
            XMLString::release(&mEntityTag);
            XMLString::release(&mEntitySystemTag);
            XMLString::release(&mPropertyTag);
            XMLString::release(&mArrayPropertyTag);
            XMLString::release(&mBoolPropertyTag);
            XMLString::release(&mCharPropertyTag);
            XMLString::release(&mDoublePropertyTag);
            XMLString::release(&mIntPropertyTag);
            XMLString::release(&mFloatPropertyTag);
            XMLString::release(&mGroupPropertyTag);
            XMLString::release(&mMessageTag);
            XMLString::release(&mMatrixPropertyTag);
            XMLString::release(&mVec2PropertyTag);
            XMLString::release(&mVec3PropertyTag);
            XMLString::release(&mVec4PropertyTag);
            XMLString::release(&mQuatPropertyTag);
            XMLString::release(&mStringPropertyTag);
            XMLString::release(&mUIntPropertyTag);
            XMLString::release(&mComponentTag);
         }
         catch( ... )
         {
            LOG_ERROR("Unknown exception encountered in TagNamesdtor");
         }

      }
   };

   ////////////////////////////////////////////////////////////////////////////////
   class ParseErrorHandler : public xercesc::DefaultHandler {

      std::string mPath;

   public:
      ParseErrorHandler(const std::string& path)
         : mPath(path)
      {
      }

       void warning(const SAXParseException& e)
       {
          std::ostringstream os;
          os << "XML warning, file " << mPath << " line " << e.getLineNumber() << ": " << XMLString::transcode(e.getMessage( ));
          LOG_ERROR(os.str());
       }
       void error(const SAXParseException& e)
       {
          std::ostringstream os;
          os << "XML error, file " << mPath << " line " << e.getLineNumber() << ": " << XMLString::transcode(e.getMessage( ));
          LOG_ERROR(os.str());
       }
       void fatalError(const SAXParseException& e) { 
       
          std::ostringstream os;
          os << "XML fatal error, file " << mPath << " line " << e.getLineNumber() << ": " << XMLString::transcode(e.getMessage( ));
          LOG_ERROR(os.str());
       }
   };

   ////////////////////////////////////////////////////////////////////////////////
   XercesMapEncoder::XercesMapEncoder(EntityManager& em)
      : mEntityManager(&em)
   {
      if(!sXercesInitialized)
      {
        // make sure xerces is initialized
        xercesc::XMLPlatformUtils::Initialize();
        sXercesInitialized = true;
      }
      mNames = new XMLNames();
      bool success = em.GetEntitySystem(MapComponent::TYPE, mMapSystem);
      assert(success);
	}
   
   ////////////////////////////////////////////////////////////////////////////////
   XercesMapEncoder::~XercesMapEncoder()
   {
      delete mNames;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool XercesMapEncoder::StartEntitySystem(StringId componentType)
   {
      if(mEntityManager->HasEntitySystem(componentType))
      {
         return true;
      }

      ComponentPluginManager& pluginManager = mMapSystem->GetPluginManager();
      if(!pluginManager.FactoryExists(componentType))
      {
         return false;
      }
      return pluginManager.StartEntitySystem(componentType);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::ParseMap(DOMElement* element, const std::string& mapPath)
   {

       for(DOMNode* currentNode (element->getFirstChild()); 
            currentNode != NULL; 
            currentNode = currentNode->getNextSibling())
      {
         if( currentNode->getNodeType() &&  // true is not NULL
             currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
         {
            // Found node which is an Element. Re-cast node as element
            DOMElement* currentElement = dynamic_cast<xercesc::DOMElement*>(currentNode);
            if( XMLString::equals(currentElement->getTagName(), mNames->mEntityTag))
            {
               ParseEntity(currentElement, mapPath);
            }
            else if( XMLString::equals(currentElement->getTagName(), mNames->mSpawnerTag))
            {
               ParseSpawner(currentElement, mapPath);
            }
         }
      }
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::ParseEntity(DOMElement* element, const std::string& mapName)
   {
     dtEntity::Entity* newentity;
      bool success = mEntityManager->CreateEntity(newentity);
      assert(success);

      if(element->getAttribute(mNames->mSpawner))
      {
         std::string spawnername = XMLString::transcode(element->getAttribute(mNames->mSpawner));
         if(spawnername != "")
         {
            Spawner* spawner;
            bool found = mMapSystem->GetSpawner(spawnername, spawner);
            if(!found)
            {
               LOG_ERROR("Spawner not found: " + spawnername);
               return;
            }

            success = spawner->Spawn(*newentity);
            assert(success);
         }
      }

       for(DOMNode* currentNode (element->getFirstChild()); 
            currentNode != NULL; 
            currentNode = currentNode->getNextSibling())
       {
         if( currentNode->getNodeType() &&
             currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
         {
            // Found node which is an Element. Re-cast node as element
            DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >(currentNode);
            if( XMLString::equals(currentElement->getTagName(), mNames->mComponentTag))
            {
               CreateComponent(currentElement, newentity->GetId());
            }
         }
      }

      // Then set their properties
      for(DOMNode* currentNode (element->getFirstChild()); 
            currentNode != NULL; 
            currentNode = currentNode->getNextSibling())
      {
         if( currentNode->getNodeType() &&
             currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
         {
            // Found node which is an Element. Re-cast node as element
            DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >(currentNode);
            if( XMLString::equals(currentElement->getTagName(), mNames->mComponentTag))
            {
               SetupComponent(currentElement, newentity->GetId());
            }
         }
      }

      // Make sure entity has a map component
      MapComponent* mc;
      if(!mEntityManager->GetComponent(newentity->GetId(), mc))
      {
         mEntityManager->CreateComponent(newentity->GetId(), mc);
      }
      mc->SetMapName(mapName);

      mEntityManager->AddToScene(newentity->GetId());
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool XercesMapEncoder::ParseMessage(DOMElement* element, dtEntity::Message*& msg, osg::Timer_t& time)
   {
      std::string timestr = XMLString::transcode(element->getAttribute(mNames->mTime));
      fromString(time, timestr);
      std::string tname = XMLString::transcode(element->getAttribute(mNames->mType));
      
      bool success = mEntityManager->CreateMessage(SID(tname), msg);
      if(!success)
      {
         LOG_ERROR("Message not registered with entity manager: " + tname);
         return false;
      }
      for(DOMNode* currentNode (element->getFirstChild()); 
            currentNode != NULL; 
            currentNode = currentNode->getNextSibling())
      {
         if( currentNode->getNodeType() &&
             currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
         {
            // Found node which is an Element. Re-cast node as element
            DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >(currentNode);
            std::string name = XMLString::transcode(currentElement->getAttribute(mNames->mName));
            dtEntity::StringId namesid = SID(name);
            Property* property = ParseProperty(currentElement);
            if(property != NULL)
            {
               Property* toset = msg->Get(namesid);
               if(toset == NULL)
               {
                  std::ostringstream os;
                  os << "Property " << name << " does not exist in message " << tname;
                  LOG_WARNING(os.str());
               }
               else
               {
                  toset->SetFrom(*property);
               }
               delete property;
            }
         }
      }
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::ParseSpawner(DOMElement* element, const std::string& mapName)
   {
      std::string name = XMLString::transcode(element->getAttribute(mNames->mName));
      std::string parent = XMLString::transcode(element->getAttribute(mNames->mParent));
      std::string guiCategory = XMLString::transcode(element->getAttribute(mNames->mGUICategory));
      std::string addToSpawnerStore = XMLString::transcode(element->getAttribute(mNames->mAddToSpawnerStore));
      std::string iconPath = XMLString::transcode(element->getAttribute(mNames->mIconPath));

      Spawner* spawner;
      if(parent == "")
      {
         spawner = new Spawner(name, mapName);
      }
      else
      {
         Spawner* parentspawner;
         bool found = mMapSystem->GetSpawner(parent, parentspawner);
         if(!found)
         {
            LOG_ERROR("Cannot initialize spawner: Parent spawner not found. Name: " + parent);
            return;
         }
         spawner = new Spawner(name, mapName, parentspawner);
      }

      if(guiCategory != "")
      {
         spawner->SetGUICategory(guiCategory);
      }

      if(iconPath != "")
      {
         spawner->SetIconPath(iconPath);
      }

      if(addToSpawnerStore != "")
      {
         spawner->SetAddToSpawnerStore(addToSpawnerStore == "true" || addToSpawnerStore == "TRUE");
      }

      for(DOMNode* currentNode (element->getFirstChild()); 
            currentNode != NULL; 
            currentNode = currentNode->getNextSibling())
      {
         if(currentNode->getNodeType() && currentNode->getNodeType() == DOMNode::ELEMENT_NODE)
         {
            DOMElement* currentElement = dynamic_cast<xercesc::DOMElement*>(currentNode);
            if(XMLString::equals(currentElement->getTagName(), mNames->mComponentTag))
            {
               AddComponentToSpawner(currentElement, *spawner);
            }
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
      mMapSystem->AddSpawner(*spawner);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::AddComponentToSpawner(DOMElement* element, Spawner& spawner)
   {
      std::string typestr = XMLString::transcode(element->getAttribute(mNames->mType));

      DynamicPropertyContainer props;
      for(DOMNode* currentNode (element->getFirstChild()); 
            currentNode != NULL; 
            currentNode = currentNode->getNextSibling())
      {
         if(currentNode->getNodeType() && currentNode->getNodeType() == DOMNode::ELEMENT_NODE)
         {
            DOMElement* currentElement = dynamic_cast<xercesc::DOMElement*>(currentNode);
            std::string name = XMLString::transcode(currentElement->getAttribute(mNames->mName));
            Property* property = ParseProperty(currentElement);

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
   void XercesMapEncoder::CreateComponent(DOMElement* element, EntityId entityId)
   {
      std::string tname = XMLString::transcode(element->getAttribute(mNames->mType));
      StringId componentType = SID(tname);
      Component* component;
      bool found = mEntityManager->GetComponent(entityId, componentType, component);
      if(!found)
      {
         bool success = StartEntitySystem(componentType);
         if(!success)
         {
            LOG_WARNING("Cannot add component, no entity system of this type registered: " + GetStringFromSID(componentType));
            return;
         }
         mEntityManager->CreateComponent(entityId, componentType, component);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::SetupComponent(DOMElement* element, EntityId entityId)
   {
      std::string tname = XMLString::transcode(element->getAttribute(mNames->mType));
      StringId componentType = SID(tname);
      Component* component;
      bool found = mEntityManager->GetComponent(entityId, componentType, component);
      if(!found)
      {
         return;
      }
      
      for(DOMNode* currentNode (element->getFirstChild()); 
            currentNode != NULL; 
            currentNode = currentNode->getNextSibling())
      {
         if( currentNode->getNodeType() &&
             currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
         {
            // Found node which is an Element. Re-cast node as element
            DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >(currentNode);
            std::string name = XMLString::transcode(currentElement->getAttribute(mNames->mName));
            dtEntity::StringId namesid = SID(name);
            Property* property = ParseProperty(currentElement);
            if(property != NULL)
            {
               Property* toset = component->Get(namesid);
               if(toset == NULL)
               {
                  std::ostringstream os;
                  os << "Property " << name << " does not exist in component " << tname;
                  LOG_WARNING(os.str());
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
      component->OnFinishedSettingProperties();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Property* XercesMapEncoder::ParseProperty(DOMElement* element)
   {
      if(XMLString::equals(element->getTagName(), mNames->mArrayPropertyTag))
      {
         return ParseArrayProperty(element);
      }
      else if(XMLString::equals(element->getTagName(), mNames->mBoolPropertyTag))
      {
         return ParseBoolProperty(element);
      }
      else if(XMLString::equals(element->getTagName(), mNames->mCharPropertyTag))
      {
         return ParseCharProperty(element);
      }
      else if(XMLString::equals(element->getTagName(), mNames->mDoublePropertyTag))
      {
         return ParseDoubleProperty(element);
      }
      else if(XMLString::equals(element->getTagName(), mNames->mFloatPropertyTag))
      {
         return ParseFloatProperty(element);
      }
      else if(XMLString::equals(element->getTagName(), mNames->mGroupPropertyTag))
      {
         return ParseGroupProperty(element);
      }
      else if(XMLString::equals(element->getTagName(), mNames->mIntPropertyTag))
      {
         return ParseIntProperty(element);
      }
      else if(XMLString::equals(element->getTagName(), mNames->mMatrixPropertyTag))
      {
         return ParseMatrixProperty(element);
      }
      else if(XMLString::equals(element->getTagName(), mNames->mQuatPropertyTag))
      {
         return ParseQuatProperty(element);
      }
      else if(XMLString::equals(element->getTagName(), mNames->mStringPropertyTag))
      {
         return ParseStringProperty(element);
      }
      else if(XMLString::equals(element->getTagName(), mNames->mUIntPropertyTag))
      {
         return ParseUIntProperty(element);
      }
      else if(XMLString::equals(element->getTagName(), mNames->mVec2PropertyTag))
      {
         return ParseVec2Property(element);
      }
      else if(XMLString::equals(element->getTagName(), mNames->mVec3PropertyTag))
      {
         return ParseVec3Property(element);
      }
      else if(XMLString::equals(element->getTagName(), mNames->mVec4PropertyTag))
      {
         return ParseVec4Property(element);
      }
      LOG_ERROR("Could not parse property, unknown type!");
      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   BoolProperty* XercesMapEncoder::ParseBoolProperty(DOMElement* element)
   {
      std::string boolval = XMLString::transcode(element->getAttribute(mNames->mValue));
      bool val = (boolval == "true" || boolval == "TRUE");
      return new BoolProperty(val);
   }

   ////////////////////////////////////////////////////////////////////////////////
   CharProperty* XercesMapEncoder::ParseCharProperty(DOMElement* element)
   {
      std::string val = XMLString::transcode(element->getAttribute(mNames->mValue));
      CharProperty* p = new CharProperty();
      p->SetString(val);
      return p;
   }

   ////////////////////////////////////////////////////////////////////////////////
   IntProperty* XercesMapEncoder::ParseIntProperty(DOMElement* element)
   {
      std::string intval = XMLString::transcode(element->getAttribute(mNames->mValue));
      int val;
      fromString(val, intval);
      return new IntProperty(val);
   }

   ////////////////////////////////////////////////////////////////////////////////
   FloatProperty* XercesMapEncoder::ParseFloatProperty(DOMElement* element)
   {
      std::string floatval = XMLString::transcode(element->getAttribute(mNames->mValue));
      float val;
      fromString(val, floatval);
      return new FloatProperty(val);
   }

   ////////////////////////////////////////////////////////////////////////////////
   DoubleProperty* XercesMapEncoder::ParseDoubleProperty(DOMElement* element)
   {
      std::string doubleval = XMLString::transcode(element->getAttribute(mNames->mValue));
      double val;
      fromString(val, doubleval);
      return new DoubleProperty(val);
   }

   ////////////////////////////////////////////////////////////////////////////////
   MatrixProperty* XercesMapEncoder::ParseMatrixProperty(DOMElement* element)
   {
      std::string matrixval = XMLString::transcode(element->getTextContent());
      std::istringstream is(matrixval);

      float v0, v1, v2, v3;
      float v4, v5, v6, v7;
      float v8, v9, v10, v11;
      float v12, v13, v14, v15;
      is >> v0;
      is >> v1;
      is >> v2;
      is >> v3;
      is >> v4;
      is >> v5;
      is >> v6;
      is >> v7;
      is >> v8;
      is >> v9;
      is >> v10;
      is >> v11;
      is >> v12;
      is >> v13;
      is >> v14;
      is >> v15;
      osg::Matrix mat;
      mat.set(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15);
      return new MatrixProperty(mat);
   }

   ////////////////////////////////////////////////////////////////////////////////
   UIntProperty* XercesMapEncoder::ParseUIntProperty(DOMElement* element)
   {
      std::string uintval = XMLString::transcode(element->getAttribute(mNames->mValue));
      unsigned int val;
      fromString(val, uintval);
      return new UIntProperty(val);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Vec2dProperty* XercesMapEncoder::ParseVec2Property(DOMElement* element)
   {
      std::string valx = XMLString::transcode(element->getAttribute(mNames->mX));
      std::string valy = XMLString::transcode(element->getAttribute(mNames->mY));
      double x, y;
      fromString<double>(x, valx);
      fromString<double>(y, valy);
      return new Vec2dProperty(osg::Vec2(x, y));
   }

   ////////////////////////////////////////////////////////////////////////////////
   Vec3dProperty* XercesMapEncoder::ParseVec3Property(DOMElement* element)
   {
      std::string valx = XMLString::transcode(element->getAttribute(mNames->mX));
      std::string valy = XMLString::transcode(element->getAttribute(mNames->mY));
      std::string valz = XMLString::transcode(element->getAttribute(mNames->mZ));
      double x, y, z;
      fromString<double>(x, valx);
      fromString<double>(y, valy);
      fromString<double>(z, valz);
      return new Vec3dProperty(osg::Vec3(x, y, z));
   }

   ////////////////////////////////////////////////////////////////////////////////
   Vec4dProperty* XercesMapEncoder::ParseVec4Property(DOMElement* element)
   {
      std::string valx = XMLString::transcode(element->getAttribute(mNames->mX));
      std::string valy = XMLString::transcode(element->getAttribute(mNames->mY));
      std::string valz = XMLString::transcode(element->getAttribute(mNames->mZ));
      std::string valw = XMLString::transcode(element->getAttribute(mNames->mW));
      double x, y, z, w;
      fromString<double>(x, valx);
      fromString<double>(y, valy);
      fromString<double>(z, valz);
      fromString<double>(w, valw);
      return new Vec4dProperty(osg::Vec4(x, y, z, w));
   }

   ////////////////////////////////////////////////////////////////////////////////
   QuatProperty* XercesMapEncoder::ParseQuatProperty(DOMElement* element)
   {
      std::string valx = XMLString::transcode(element->getAttribute(mNames->mX));
      std::string valy = XMLString::transcode(element->getAttribute(mNames->mY));
      std::string valz = XMLString::transcode(element->getAttribute(mNames->mZ));
      std::string valw = XMLString::transcode(element->getAttribute(mNames->mW));
      double x, y, z, w;
      fromString<double>(x, valx);
      fromString<double>(y, valy);
      fromString<double>(z, valz);
      fromString<double>(w, valw);
      return new QuatProperty(osg::Quat(x, y, z, w));
   }

   ////////////////////////////////////////////////////////////////////////////////
   StringProperty* XercesMapEncoder::ParseStringProperty(DOMElement* element)
   {
      std::string val = XMLString::transcode(element->getTextContent());
      return new StringProperty(val);
   }

   ////////////////////////////////////////////////////////////////////////////////
   ArrayProperty* XercesMapEncoder::ParseArrayProperty(DOMElement* element)
   {
      ArrayProperty* arrayproperty = new ArrayProperty();

      for(DOMNode* currentNode (element->getFirstChild()); 
            currentNode != NULL; 
            currentNode = currentNode->getNextSibling())
      {
         if(currentNode->getNodeType() && currentNode->getNodeType() == DOMNode::ELEMENT_NODE)
         {
            DOMElement* currentElement = dynamic_cast<xercesc::DOMElement*>(currentNode);

            Property* prop = ParseProperty(currentElement);
            if(prop != NULL)
            {
               arrayproperty->Add(prop);
            }
         }
      }
      return arrayproperty;
   }

   ////////////////////////////////////////////////////////////////////////////////
   GroupProperty* XercesMapEncoder::ParseGroupProperty(DOMElement* element)
   {
      DynamicPropertyContainer subprops;

      for(DOMNode* currentNode (element->getFirstChild()); 
            currentNode != NULL; 
            currentNode = currentNode->getNextSibling())
      {
         if(currentNode->getNodeType() && currentNode->getNodeType() == DOMNode::ELEMENT_NODE)
         {
            DOMElement* currentElement = dynamic_cast<xercesc::DOMElement*>(currentNode);
            std::string name = XMLString::transcode(currentElement->getAttribute(mNames->mName));
            Property* prop = ParseProperty(currentElement);
            if(prop != NULL)
            {
               subprops.AddProperty(SID(name), *prop);
               delete prop;
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
   void XercesMapEncoder::SerializeSpawner(DOMElement* parent, Spawner* spawner)
   {

      DOMElement* entity = mDocument->createElement(mNames->mSpawnerTag);
      parent->appendChild(entity);

      entity->setAttribute(mNames->mName, StringToXMLConverter(spawner->GetName()).ToXmlString());
      entity->setAttribute(mNames->mGUICategory, StringToXMLConverter(spawner->GetGUICategory()).ToXmlString());
      entity->setAttribute(mNames->mAddToSpawnerStore,
                           StringToXMLConverter(spawner->GetAddToSpawnerStore() ? "true" : "false")
                           .ToXmlString());
      entity->setAttribute(mNames->mIconPath, StringToXMLConverter(spawner->GetIconPath()).ToXmlString());

      if(spawner->GetParent() != NULL)
      {
         entity->setAttribute(mNames->mParent, StringToXMLConverter(spawner->GetParent()->GetName()).ToXmlString());
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

         DOMElement* compelem = mDocument->createElement(mNames->mComponentTag);
         entity->appendChild(compelem);

         compelem->setAttribute(mNames->mType, StringToXMLConverter(tname).ToXmlString());

         PropertyContainer::ConstPropertyMap p;
         props.GetProperties(p);

         std::map<std::string, const Property*> sorted;
         for(PropertyContainer::ConstPropertyMap::const_iterator j = p.begin(); j != p.end(); ++j)
         {
            sorted[GetStringFromSID(j->first)] = j->second;
         }

         for(std::map<std::string, const Property*>::const_iterator j = sorted.begin(); j != sorted.end(); ++j)
         {
            SerializeProperty(compelem, j->first, j->second);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::SerializeEntity(DOMElement* parent, EntityId eid)
   {
      // get spawner of entity so that only property values are changed that are not identical
      // to the values from the spawner
      Spawner::ComponentProperties spawnerprops;

      MapComponent* mapcomp = NULL;
      if(mEntityManager->GetComponent(eid, mapcomp))
      {
         std::string spawnername = mapcomp->GetSpawnerName();
         if(spawnername != "")
         {
            Spawner* spawner;
            if(mMapSystem->GetSpawner(spawnername, spawner))
            {
               spawner->GetAllComponentPropertiesRecursive(spawnerprops);
            }
         }
      }

      std::list<const Component*> comps;
      mEntityManager->GetComponents(eid, comps);

      // write components sorted by component type name
      std::map<std::string, const Component*> sorted;
      std::list<const Component*>::const_iterator i;
      for(i = comps.begin(); i != comps.end(); ++i)
      {
         sorted[GetStringFromSID((*i)->GetType())] = *i;
      }

      DOMElement* entity = mDocument->createElement(mNames->mEntityTag);
      parent->appendChild(entity);

      if(mapcomp != NULL && mapcomp->GetSpawnerName() != "")
      {
         entity->setAttribute(mNames->mSpawner, StringToXMLConverter(mapcomp->GetSpawnerName()).ToXmlString());
      }

      std::map<std::string, const Component*>::const_iterator j;
      for(j = sorted.begin(); j != sorted.end(); ++j)
      {
         EntitySystem* es = mEntityManager->GetEntitySystem(j->second->GetType());

         if(es->StoreComponentsToMap())
         {
            // Get default component values. If entity was created from a spawner
            // then overwrite component values with values from spawner
            DynamicPropertyContainer defaultprops = es->GetComponentProperties();
            Spawner::ComponentProperties::iterator it = spawnerprops.find(j->second->GetType());
            if(it != spawnerprops.end())
            {
               defaultprops += (it->second);
            }

            SerializeComponent(entity, j->second, defaultprops);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::SerializeComponent(DOMElement* parent, const Component* component, const PropertyContainer& defaults)
   {
      DOMElement* compelem = mDocument->createElement(mNames->mComponentTag);
      parent->appendChild(compelem);

      std::string tname = GetStringFromSID(component->GetType());
      compelem->setAttribute(mNames->mType, StringToXMLConverter(tname).ToXmlString());

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
         if(!((*deflt) == (*j->second)) || SID(j->first) == MapComponent::SpawnerNameId)
         {
            SerializeProperty(compelem, j->first, j->second);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::SerializeProperty(DOMElement* parent, const std::string& nstring, const Property* prop)
   {
      switch(prop->GetType())
      {
      case DataType::BOOL: SerializeBoolProperty(parent, nstring, static_cast<const BoolProperty*>(prop)); break;
      case DataType::CHAR: SerializeCharProperty(parent, nstring, static_cast<const CharProperty*>(prop)); break;
      case DataType::INT: SerializeIntProperty(parent, nstring, static_cast<const IntProperty*>(prop)); break;
      case DataType::UINT: SerializeUIntProperty(parent, nstring, static_cast<const UIntProperty*>(prop)); break;
      case DataType::DOUBLE: SerializeDoubleProperty( parent, nstring, static_cast<const DoubleProperty*>(prop)); break;
      case DataType::FLOAT: SerializeFloatProperty(parent, nstring, static_cast<const FloatProperty*>(prop)); break;
      case DataType::MATRIX: SerializeMatrixProperty(parent, nstring, static_cast<const MatrixProperty*>(prop)); break;
      case DataType::VEC2:  
      case DataType::VEC2D: SerializeVec2Property(parent, nstring, prop->Vec2dValue()); break;
      case DataType::VEC3:  
      case DataType::VEC3D: SerializeVec3Property(parent, nstring, prop->Vec3dValue()); break;
      case DataType::VEC4: 
      case DataType::VEC4D: SerializeVec4Property(parent, nstring, prop->Vec4dValue()); break;
      case DataType::QUAT: SerializeQuatProperty(parent, nstring, static_cast<const QuatProperty*>(prop)); break;
      case DataType::STRING: SerializeStringProperty(parent, nstring, static_cast<const StringProperty*>(prop)); break;
      case DataType::STRINGID: SerializeStringIdProperty(parent, nstring, static_cast<const StringIdProperty*>(prop)); break;
      case DataType::ARRAY: SerializeArrayProperty(parent, nstring, static_cast<const ArrayProperty*>(prop)); break;
      case DataType::GROUP: SerializeGroupProperty(parent, nstring, static_cast<const GroupProperty*>(prop)); break;
      default: LOG_ERROR("Unknown property type, cannot serialize");
      }

   }

   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::SerializeBoolProperty(DOMElement* parent, const std::string& name, const BoolProperty* prop)
   {
      DOMElement* propelem = mDocument->createElement(mNames->mBoolPropertyTag);
      std::string val = prop->Get() ? "true" : "false";
      propelem->setAttribute(mNames->mName, StringToXMLConverter(name).ToXmlString());
      propelem->setAttribute(mNames->mValue, StringToXMLConverter(val).ToXmlString());
      parent->appendChild(propelem);
   }

   
   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::SerializeCharProperty(DOMElement* parent, const std::string& name, const CharProperty* prop)
   {
      DOMElement* propelem = mDocument->createElement(mNames->mCharPropertyTag);
      std::string val = prop->StringValue();
      propelem->setAttribute(mNames->mName, StringToXMLConverter(name).ToXmlString());
      propelem->setAttribute(mNames->mValue, StringToXMLConverter(val).ToXmlString());
      parent->appendChild(propelem);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::SerializeDoubleProperty(DOMElement* parent, const std::string& name, const DoubleProperty* prop)
   {
      DOMElement* propelem = mDocument->createElement(mNames->mDoublePropertyTag);
      std::ostringstream os;
      os.precision(10);
      os << std::fixed << prop->Get();
      propelem->setAttribute(mNames->mName, StringToXMLConverter(name).ToXmlString());
      propelem->setAttribute(mNames->mValue, StringToXMLConverter(os.str()).ToXmlString());
      parent->appendChild(propelem);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::SerializeIntProperty(DOMElement* parent, const std::string& name, const IntProperty* prop)
   {
      DOMElement* propelem = mDocument->createElement(mNames->mIntPropertyTag);
      std::ostringstream os;
      os << prop->Get();
      propelem->setAttribute(mNames->mName, StringToXMLConverter(name).ToXmlString());
      propelem->setAttribute(mNames->mValue, StringToXMLConverter(os.str()).ToXmlString());
      parent->appendChild(propelem);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::SerializeUIntProperty(DOMElement* parent, const std::string& name, const UIntProperty* prop)
   {
      DOMElement* propelem = mDocument->createElement(mNames->mUIntPropertyTag);
      std::ostringstream os;
      os << prop->Get();
      propelem->setAttribute(mNames->mName, StringToXMLConverter(name).ToXmlString());
      propelem->setAttribute(mNames->mValue, StringToXMLConverter(os.str()).ToXmlString());
      parent->appendChild(propelem);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::SerializeFloatProperty(DOMElement* parent, const std::string& name, const FloatProperty* prop)
   {
      DOMElement* propelem = mDocument->createElement(mNames->mFloatPropertyTag);
      std::ostringstream os;
      os << std::fixed << prop->Get();
      propelem->setAttribute(mNames->mName, StringToXMLConverter(name).ToXmlString());
      propelem->setAttribute(mNames->mValue, StringToXMLConverter(os.str()).ToXmlString());
      parent->appendChild(propelem);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::SerializeMatrixProperty(DOMElement* parent, const std::string& name, const MatrixProperty* prop)
   {
      DOMElement* propelem = mDocument->createElement(mNames->mMatrixPropertyTag);
      osg::Matrix mat = prop->Get();
      std::ostringstream os;
      os.precision(10);
      os << std::fixed;
      os << mat(0, 0) << " ";
      os << mat(0, 1) << " ";
      os << mat(0, 2) << " ";
      os << mat(0, 3) << " ";
      os << mat(1, 0) << " ";
      os << mat(1, 1) << " ";
      os << mat(1, 2) << " ";
      os << mat(1, 3) << " ";
      os << mat(2, 0) << " ";
      os << mat(2, 1) << " ";
      os << mat(2, 2) << " ";
      os << mat(2, 3) << " ";
      os << mat(3, 0) << " ";
      os << mat(3, 1) << " ";
      os << mat(3, 2) << " " ;
      os << mat(3, 3);
      propelem->setAttribute(mNames->mName, StringToXMLConverter(name).ToXmlString());
      propelem->setTextContent(StringToXMLConverter(os.str()).ToXmlString());
      parent->appendChild(propelem);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::SerializeVec2Property(DOMElement* parent, const std::string& name, const osg::Vec2d& v)
   {
      DOMElement* propelem = mDocument->createElement(mNames->mVec2PropertyTag);
      std::ostringstream os0, os1;
      os0.precision(10);
      os1.precision(10);
      os0 << std::fixed << v[0];
      os1 << std::fixed << v[1];
      propelem->setAttribute(mNames->mName, StringToXMLConverter(name).ToXmlString());
      propelem->setAttribute(mNames->mX, StringToXMLConverter(os0.str()).ToXmlString());
      propelem->setAttribute(mNames->mY, StringToXMLConverter(os1.str()).ToXmlString());
      parent->appendChild(propelem);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::SerializeVec3Property(DOMElement* parent, const std::string& name, const osg::Vec3d& v)
   {
      DOMElement* propelem = mDocument->createElement(mNames->mVec3PropertyTag);
      std::ostringstream os0, os1, os2;
      os0.precision(10);
      os1.precision(10);
      os2.precision(10);
      os0 << std::fixed << v[0];
      os1 << std::fixed << v[1];
      os2 << std::fixed << v[2];
      propelem->setAttribute(mNames->mName, StringToXMLConverter(name).ToXmlString());
      propelem->setAttribute(mNames->mX, StringToXMLConverter(os0.str()).ToXmlString());
      propelem->setAttribute(mNames->mY, StringToXMLConverter(os1.str()).ToXmlString());
      propelem->setAttribute(mNames->mZ, StringToXMLConverter(os2.str()).ToXmlString());
      parent->appendChild(propelem);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::SerializeVec4Property(DOMElement* parent, const std::string& name, const osg::Vec4d& v)
   {
      DOMElement* propelem = mDocument->createElement(mNames->mVec4PropertyTag);
      std::ostringstream os0, os1, os2, os3;
      os0.precision(10);
      os1.precision(10);
      os2.precision(10);
      os3.precision(10);
      os0 << std::fixed << v[0];
      os1 << std::fixed << v[1];
      os2 << std::fixed << v[2];
      os3 << std::fixed << v[3];
      propelem->setAttribute(mNames->mName, StringToXMLConverter(name).ToXmlString());
      propelem->setAttribute(mNames->mX, StringToXMLConverter(os0.str()).ToXmlString());
      propelem->setAttribute(mNames->mY, StringToXMLConverter(os1.str()).ToXmlString());
      propelem->setAttribute(mNames->mZ, StringToXMLConverter(os2.str()).ToXmlString());
      propelem->setAttribute(mNames->mW, StringToXMLConverter(os3.str()).ToXmlString());
      parent->appendChild(propelem);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::SerializeQuatProperty(DOMElement* parent, const std::string& name, const QuatProperty* prop)
   {
      DOMElement* propelem = mDocument->createElement(mNames->mQuatPropertyTag);
      std::ostringstream os0, os1, os2, os3;
      os0.precision(10);
      os1.precision(10);
      os2.precision(10);
      os3.precision(10);

      osg::Quat q = prop->Get();
      os0 << std::fixed << q[0];
      os1 << std::fixed << q[1];
      os2 << std::fixed << q[2];
      os3 << std::fixed << q[3];
      propelem->setAttribute(mNames->mName, StringToXMLConverter(name).ToXmlString());
      propelem->setAttribute(mNames->mX, StringToXMLConverter(os0.str()).ToXmlString());
      propelem->setAttribute(mNames->mY, StringToXMLConverter(os1.str()).ToXmlString());
      propelem->setAttribute(mNames->mZ, StringToXMLConverter(os2.str()).ToXmlString());
      propelem->setAttribute(mNames->mW, StringToXMLConverter(os3.str()).ToXmlString());
      parent->appendChild(propelem);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::SerializeStringProperty(DOMElement* parent, const std::string& name, const StringProperty* prop)
   {
      DOMElement* propelem = mDocument->createElement(mNames->mStringPropertyTag);
      propelem->setAttribute(mNames->mName, StringToXMLConverter(name).ToXmlString());
      propelem->setTextContent(StringToXMLConverter(prop->Get()).ToXmlString());
      parent->appendChild(propelem);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::SerializeStringIdProperty(DOMElement* parent, const std::string& name, const StringIdProperty* prop)
   {
      DOMElement* propelem = mDocument->createElement(mNames->mStringPropertyTag);
      propelem->setAttribute(mNames->mName, StringToXMLConverter(name).ToXmlString());
      propelem->setTextContent(StringToXMLConverter(GetStringFromSID(prop->Get())).ToXmlString());
      parent->appendChild(propelem);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::SerializeArrayProperty(DOMElement* parent, const std::string& name, const ArrayProperty* prop)
   {
      DOMElement* propelem = mDocument->createElement(mNames->mArrayPropertyTag);
      propelem->setAttribute(mNames->mName, StringToXMLConverter(name).ToXmlString());

      PropertyArray arr = prop->Get();


      for(unsigned int i = 0; i < arr.size(); ++i)
      {
         SerializeProperty(propelem, ToString(i), arr[i]);
      }
      parent->appendChild(propelem);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::SerializeGroupProperty(DOMElement* parent, const std::string& name, const GroupProperty* prop)
   {
      DOMElement* propelem = mDocument->createElement(mNames->mGroupPropertyTag);
      propelem->setAttribute(mNames->mName, StringToXMLConverter(name).ToXmlString());

      PropertyGroup grp = prop->Get();

      std::map<std::string, const Property*> sorted;
      for(PropertyGroup::iterator i = grp.begin(); i != grp.end(); ++i)
      {
         sorted[GetStringFromSID(i->first)] = i->second;
      }

      for(std::map<std::string, const Property*>::const_iterator i = sorted.begin(); i != sorted.end(); ++i)
      {
         SerializeProperty(propelem, i->first, i->second);
      }

      parent->appendChild(propelem);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::SerializeEntitySystem(DOMElement* parent, const EntitySystem* es)
   {
      DOMElement* eselem = mDocument->createElement(mNames->mEntitySystemTag);
      parent->appendChild(eselem);

      std::string tname = GetStringFromSID(es->GetComponentType());
      eselem->setAttribute(mNames->mType, StringToXMLConverter(tname).ToXmlString());

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
         SerializeProperty(eselem, j->first, j->second);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::SerializeMessage(xercesc::DOMElement* parent, const Message* msg, osg::Timer_t timeval)
   {
      DOMElement* eselem = mDocument->createElement(mNames->mMessageTag);
      parent->appendChild(eselem);

      std::ostringstream os; os << timeval;
      std::string tname = GetStringFromSID(msg->GetType());

      eselem->setAttribute(mNames->mType, StringToXMLConverter(tname).ToXmlString());
      eselem->setAttribute(mNames->mTime, StringToXMLConverter(os.str()).ToXmlString());

      PropertyContainer::ConstPropertyMap props;
      msg->GetProperties(props);

      std::map<std::string, const Property*> sorted;
      PropertyContainer::ConstPropertyMap::const_iterator i;
      for(i = props.begin(); i != props.end(); ++i)
      {
         sorted[GetStringFromSID(i->first)] = i->second;
      }

      std::map<std::string, const Property*>::const_iterator j;
      for(j = sorted.begin(); j != sorted.end(); ++j)
      {
         SerializeProperty(eselem, j->first, j->second);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::SerializeMap(DOMElement* parent, const std::string& path)
   {
      DOMElement* elem = mDocument->createElement(mNames->mMapTag);
      parent->appendChild(elem);
      elem->setAttribute(mNames->mPath, StringToXMLConverter(path).ToXmlString());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::SetupEntitySystem(DOMElement* element)
   {
      std::string tname = XMLString::transcode(element->getAttribute(mNames->mType));
      StringId componentType = SID(tname);

      if(!mEntityManager->HasEntitySystem(componentType))
      {
         bool success = StartEntitySystem(componentType);
         if(!success)
         {
            LOG_WARNING("Cannot setup entity system. Component type not found: " + tname);
            return;
         }
      }
      EntitySystem* es = mEntityManager->GetEntitySystem(componentType);
      assert(es != NULL);

      for(DOMNode* currentNode (element->getFirstChild()); 
            currentNode != NULL; 
            currentNode = currentNode->getNextSibling())
      {
         if( currentNode->getNodeType() &&
             currentNode->getNodeType() == DOMNode::ELEMENT_NODE )
         {
            DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >(currentNode);
            //SetupProperty(currentElement, *es);

            std::string name = XMLString::transcode(currentElement->getAttribute(mNames->mName));
            dtEntity::StringId namesid = SID(name);
            Property* property = ParseProperty(currentElement);
            if(property != NULL)
            {
               Property* toset = es->Get(namesid);
               if(toset == NULL)
               {
                  std::ostringstream os;
                  os << "Property " << name << " does not exist in entity system ";
                  os << GetStringFromSID(componentType);
                  LOG_WARNING(os.str());
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
      es->OnFinishedSettingProperties();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::FindMapsToLoad(DOMElement* element, std::list<std::string>& mapsToLoad)
   {
      for(DOMNode* currentNode (element->getFirstChild()); 
            currentNode != NULL; 
            currentNode = currentNode->getNextSibling())
      {
         if( currentNode->getNodeType() &&
             currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
         {
            // Found node which is an Element. Re-cast node as element
            DOMElement* currentElement = dynamic_cast<xercesc::DOMElement*>(currentNode);
            if(XMLString::equals(currentElement->getTagName(), mNames->mMapTag))
            {
               std::string path = XMLString::transcode(currentElement->getAttribute(mNames->mPath));
               mapsToLoad.push_back(path);
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void XercesMapEncoder::ParseScene(DOMElement* element, std::list<std::string>& mapsToLoad)
   {

     for(DOMNode* currentNode (element->getFirstChild()); 
            currentNode != NULL; 
            currentNode = currentNode->getNextSibling())
      {
         if(currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
         {
            // Found node which is an Element. Re-cast node as element
            DOMElement* currentElement = dynamic_cast<xercesc::DOMElement*>(currentNode);
            if(XMLString::equals(currentElement->getTagName(), mNames->mEntitySystemTag))
            {
               SetupEntitySystem(currentElement);
            }
         }
      }

      // then load maps
      for(DOMNode* currentNode (element->getFirstChild()); 
            currentNode != NULL; 
            currentNode = currentNode->getNextSibling())
      {
         if(currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
         {
            // Found node which is an Element. Re-cast node as element
            DOMElement* currentElement = dynamic_cast<xercesc::DOMElement*>(currentNode);
            if(XMLString::equals(currentElement->getTagName(), mNames->mMapsTag))
            {
               FindMapsToLoad(currentElement, mapsToLoad);
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool XercesMapEncoder::LoadMapFromFile(const std::string& path)
   {

      const std::string absPath = osgDB::findDataFile(path);
      if(absPath == "")
      {
         LOG_ERROR("Map not found: " + path);
         return false;
      }

      XercesDOMParser parser;
      ParseErrorHandler handler(path);
      parser.setErrorHandler(&handler);

      std::string schema = osgDB::findDataFile("dtentitymap.xsd");

      if (schema.empty())
      {
         LOG_WARNING("Scheme file, " + schema + ", not found, should be in root of project assets.")
      }
      else // turn on schema checking
      {
         parser.setValidationScheme(XercesDOMParser::Val_Always);
         parser.setDoSchema(true);
         parser.setDoNamespaces(true);

         schema = osgDB::getRealPath(schema);
         XMLCh* SCHEMA = XMLString::transcode(schema.c_str());
         parser.setExternalNoNamespaceSchemaLocation(SCHEMA);
         XMLString::release(&SCHEMA);
      }

      try
      {

         parser.parse(absPath.c_str());

         // no need to free this pointer - owned by the parent parser object
         mDocument = parser.getDocument();

         if(mDocument == NULL)
         {
            LOG_ERROR("Could not open map. Path: " + absPath);
            return false;
         }


         DOMElement* elementRoot = mDocument->getDocumentElement();
         if(!elementRoot)
         {
            LOG_ERROR("Could not open map. Path: " + absPath);
            return false;
         }

         ParseMap(elementRoot, path);
      }
      catch( xercesc::XMLException& e )
      {
         char* message = xercesc::XMLString::transcode( e.getMessage() );
         std::ostringstream errBuf;
         errBuf << "Error parsing file: " << message << std::flush;
         XMLString::release( &message );
         return false;
      }
      return true;

   }

   ////////////////////////////////////////////////////////////////////////////////
   void WriteFile(DOMImplementation* impl, xercesc::DOMDocument* doc, const std::string& outputfile) 
   {
      DOMLSSerializer* writer;
      DOMLSOutput* xmlstream;
      XMLCh* OUTPUT;
      try
      {
         OUTPUT = XMLString::transcode( outputfile.c_str() );
         xmlstream = impl->createLSOutput();
         xmlstream->setSystemId( OUTPUT );
         writer = impl->createLSSerializer();
      }
      catch(...)
      {
         if( OUTPUT )
            XMLString::release( &OUTPUT );
         LOG_ERROR("Can not write file, "+ outputfile +", because creation of a writing tools failed.");
         return;
      }

      // turn on pretty print
      DOMConfiguration* dc = writer->getDomConfig();
      dc->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint,true);

      // write it!
      try
      {
         if( !writer->write(doc, xmlstream) )
         {
            LOG_ERROR("There was a problem writing file, " + outputfile)
         }

         writer->release();
         xmlstream->release();
      }
      catch (const OutOfMemoryException& e)
      {
         char* message = XMLString::transcode( e.getMessage() );
         std::string msg( message );
         XMLString::release( &message );
         LOG_ERROR("When writing file," +outputfile+ ", an OutOfMemoryException occurred." + msg);
         delete xmlstream;
      }
      catch (...)
      {
         LOG_ERROR("When writing file," +outputfile+ ", an exception occurred.");
         delete xmlstream;
      }
   
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool XercesMapEncoder::SaveMapToFile(const std::string& path, const std::string& p_dest)
   {

      //dtCore::RefPtr<dtUtil::XercesWriter> writer = new dtUtil::XercesWriter();

      DOMImplementation* domImplementation = 
         DOMImplementationRegistry::getDOMImplementation(XMLString::transcode("core"));


      mDocument = domImplementation->createDocument(0, XMLString::transcode("map"), 0);

      
      DOMElement* app = mDocument->getDocumentElement();

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
            SerializeSpawner(app, candidate);
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
            SerializeEntity(app, j->second);
         }
      }

      if(p_dest.empty()) 
      {
         const std::string foundPath = osgDB::findDataFile(path);
         WriteFile(domImplementation, mDocument, foundPath);
      }
      else 
      {
         WriteFile(domImplementation, mDocument, p_dest);
      }      
      
      mDocument->release();

      return true;

   }

   ////////////////////////////////////////////////////////////////////////////////
   bool XercesMapEncoder::LoadSceneFromFile(const std::string& path)
   {
      const std::string absPath = osgDB::findDataFile(path);
      if(absPath == "")
      {
         LOG_ERROR("Cannot load scene:" + path);
         return false;
      }

      XercesDOMParser parser;
      ParseErrorHandler handler(path);
      parser.setErrorHandler(&handler);

      std::string schema = osgDB::findDataFile("dtentityscene.xsd");

      if (schema.empty())
      {
         LOG_WARNING("Scheme file, " + schema + ", not found, should be in root of project assets.")
      }
      else // turn on schema checking
      {
         parser.setValidationScheme(XercesDOMParser::Val_Always);
         parser.setDoSchema(true);
         parser.setDoNamespaces(true);

         schema = osgDB::getRealPath(schema);
         XMLCh* SCHEMA = XMLString::transcode(schema.c_str());
         parser.setExternalNoNamespaceSchemaLocation(SCHEMA);
         XMLString::release(&SCHEMA);
      }

      std::list<std::string> mapsToLoad;

      try
      {
         parser.parse(absPath.c_str());

         // no need to free this pointer - owned by the parent parser object
         mDocument = parser.getDocument();

         if(mDocument == NULL)
         {
            LOG_ERROR("Could not open scene. Path: " + absPath);
            return false;
         }


         DOMElement* elementRoot = mDocument->getDocumentElement();
         if(!elementRoot)
         {
            LOG_ERROR("Could not open scene. Path: " + absPath);
            return false;
         }

         ParseScene(elementRoot, mapsToLoad);
      }
      catch( xercesc::XMLException& e )
      {
         char* message = xercesc::XMLString::transcode( e.getMessage() );
         std::ostringstream errBuf;
         errBuf << "Error parsing file: " << message << std::flush;
         XMLString::release( &message );
         return false;
      }

      for(std::list<std::string>::iterator i = mapsToLoad.begin(); i != mapsToLoad.end(); ++i)
      {
         mMapSystem->LoadMap(*i);
      }

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool XercesMapEncoder::SaveSceneToFile(const std::string& path)
   {

      DOMImplementation* domImplementation = 
         DOMImplementationRegistry::getDOMImplementation(XMLString::transcode("core"));


      mDocument = domImplementation->createDocument(0, XMLString::transcode("scene"), 0);

      DOMElement* scene = mDocument->getDocumentElement();

      std::vector<const EntitySystem*> es;
      mEntityManager->GetEntitySystems(es);
      std::vector<const EntitySystem*>::const_iterator i;
      for(i = es.begin(); i != es.end(); ++i)
      {
         if((*i)->StoreEntitySystemToMap())
         {
            SerializeEntitySystem(scene, *i);
         }
      }

      std::list<std::string> maps;
      mMapSystem->GetLoadedMaps(maps);
      if(!maps.empty())
      {
         DOMElement* mapselem = mDocument->createElement(mNames->mMapsTag);
         scene->appendChild(mapselem);
         std::list<std::string>::iterator j;
         for(j = maps.begin(); j != maps.end(); ++j)
         {
            SerializeMap(mapselem, *j);
         }
      }

      const std::string foundPath = osgDB::findDataFile(path);
      WriteFile(domImplementation, mDocument, foundPath);
      return true;
   }   
}
