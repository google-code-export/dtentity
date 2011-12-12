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

//#define USE_XERCES

#include <dtEntityQtWidgets/delegatefactory.h>
#include <dtEntityQtWidgets/propertydelegate.h>

#include <dtEntity/log.h>
#include <dtEntity/rapidxmlmapencoder.h>

#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgDB/ReadFile>

#include <rapidxml.hpp>


#include <iostream>
#include <sstream>

namespace dtEntityQtWidgets
{
   // lifted from http://www.codeguru.com/forum/showthread.php?t=231054
   template <class T>
   bool fromString(T& t, const std::string& s, std::ios_base& (*f)(std::ios_base&) = std::dec)
   {
     std::istringstream iss(s);
     return !(iss >> f >> t).fail();
   }

   std::string ToString(unsigned int i)
   {
      std::ostringstream os; os << i; return os.str(); 
   }

#ifdef USE_XERCES
   using namespace xercesc;
#else
   using namespace rapidxml;
#endif

   ////////////////////////////////////////////////////////////////////////////////
   DelegateFactory::DelegateFactory()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   DelegateFactory::~DelegateFactory()
   {
      qDeleteAll(mChildFactories);
   }

   ////////////////////////////////////////////////////////////////////////////////
   DelegateFactory* DelegateFactory::GetFactoryForChildren(const QString& propname)
   {
      if(mChildFactories.contains(propname))
      {
         return mChildFactories[propname];
      }
      else if(mChildFactories.contains("*"))
      {
         return mChildFactories["*"];
      }
      return this;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DelegateFactory::SetFactoryForChildren(const QString& propname, DelegateFactory* factory)
   {
      if(mChildFactories.contains(propname))
      {
         delete mChildFactories[propname];
      }
      mChildFactories[propname] = factory;
   }
   
   /////////////////////////////////////////////////////////////////////////////////
   PropertySubDelegate* DelegateFactory::CreateDefault(dtEntity::DataType::e dtype) const
   {
      switch(dtype)
      {
      case dtEntity::DataType::CHAR:      return new CharPropertyDelegate();
      case dtEntity::DataType::FLOAT:     return new FloatPropertyDelegate();
      case dtEntity::DataType::STRING:    return new StringPropertyDelegate();      
      case dtEntity::DataType::ARRAY:     return new ArrayPropertyDelegate(NULL);      
      case dtEntity::DataType::BOOL:      return new BoolPropertyDelegate();      
      case dtEntity::DataType::DOUBLE:    return new DoublePropertyDelegate();
      case dtEntity::DataType::GROUP:     return new GroupPropertyDelegate();      
      case dtEntity::DataType::INT:       return new IntPropertyDelegate();      
      case dtEntity::DataType::MATRIX:    return new MatrixPropertyDelegate();      
      case dtEntity::DataType::QUAT:      return new QuatPropertyDelegate();
      case dtEntity::DataType::STRINGID:  return new StringIdPropertyDelegate();
      case dtEntity::DataType::UCHAR:     return new CharPropertyDelegate();
      case dtEntity::DataType::UINT:      return new IntPropertyDelegate();      
      case dtEntity::DataType::VEC2:
      case dtEntity::DataType::VEC2D:     return new Vec2PropertyDelegate();
      case dtEntity::DataType::VEC3:
      case dtEntity::DataType::VEC3D:     return new Vec3PropertyDelegate();
      case dtEntity::DataType::VEC4:
      case dtEntity::DataType::VEC4D:     return new Vec4PropertyDelegate();
      default:  LOG_ERROR("No delegate for this data type found!"); return NULL;
      }

   }

   /////////////////////////////////////////////////////////////////////////////////
   PropertySubDelegate* DelegateFactory::Create(TreeItem* parent,
      const QString& propname,
      const dtEntity::Property* prop) const
   {
      if(mChildFactories.contains(propname))
      {
         return mChildFactories[propname]->Create(parent, propname, prop);
      }
      return CreateDefault(prop->GetType());
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   DateTimeDelegateFactory::DateTimeDelegateFactory()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   PropertySubDelegate* DateTimeDelegateFactory::Create(TreeItem* parent,
      const QString& propname,
      const dtEntity::Property* prop) const
   {
      return new DateTimePropertyDelegate();
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   SwitchDelegateFactory::SwitchDelegateFactory(const QMap<QString, dtEntity::Property*>& groups)
      : mGroups(groups)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   PropertySubDelegate* SwitchDelegateFactory::Create(TreeItem* parent,
      const QString& propname,
      const dtEntity::Property* prop) const
   {
      return new SwitchPropertyDelegate(mGroups);
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   FileSelectorDelegateFactory::FileSelectorDelegateFactory(const QString& filters)
      : mFilters(filters)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   PropertySubDelegate* FileSelectorDelegateFactory::Create(TreeItem* parent,
      const QString& propname,
      const dtEntity::Property* prop) const
   {
      return new FilePathPropertyDelegate(mFilters);
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   ColorSelectorDelegateFactory::ColorSelectorDelegateFactory()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   PropertySubDelegate* ColorSelectorDelegateFactory::Create(TreeItem* parent,
      const QString& propname,
      const dtEntity::Property* prop) const
   {
      return new ColorPropertyDelegate();
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   ComponentDelegateFactory::ComponentDelegateFactory()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   PropertySubDelegate* ComponentDelegateFactory::Create(TreeItem* parent,
      const QString& propname,
      const dtEntity::Property* prop) const
   {
      return new ComponentPropertyDelegate();
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   EnumDelegateFactory::EnumDelegateFactory(const QString& values)
      : mValues(values)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   PropertySubDelegate* EnumDelegateFactory::Create(TreeItem* parent,
      const QString& propname,
      const dtEntity::Property* prop) const
   {
      return new EnumPropertyDelegate(mValues);
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   ArrayDelegateFactory::ArrayDelegateFactory()
      : mDataPrototype(NULL)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   ArrayDelegateFactory::~ArrayDelegateFactory()
   {
      delete mDataPrototype;    
   }

   ////////////////////////////////////////////////////////////////////////////////
   PropertySubDelegate* ArrayDelegateFactory::Create(TreeItem* parent,
      const QString& propname,
      const dtEntity::Property* prop) const
   {
      if(mChildFactories.contains(propname))
      {
         return mChildFactories[propname]->Create(parent, propname, prop);
      }
      if(prop->GetType() == dtEntity::DataType::ARRAY)
      {
         return new ArrayPropertyDelegate(mDataPrototype);
      }
      if(prop->GetType() == mDataPrototype->GetType() && !mChildFactories.empty())
      {
         return (*mChildFactories.begin())->Create(parent, propname, prop);
      }
      return CreateDefault(prop->GetType());
   }

   ////////////////////////////////////////////////////////////////////////////////
   DelegateFactory* ArrayDelegateFactory::GetFactoryForChildren(const QString& propname)
   {
      return DelegateFactory::GetFactoryForChildren(propname);
   }

   ////////////////////////////////////////////////////////////////////////////////
   RootDelegateFactory::RootDelegateFactory()
   {
      mChildFactories["*"] = this;
   }

   ////////////////////////////////////////////////////////////////////////////////
   PropertySubDelegate* RootDelegateFactory::Create(TreeItem* parent,
      const QString& propname,
      const dtEntity::Property* prop) const
   {
      return CreateDefault(prop->GetType());
   }

   ////////////////////////////////////////////////////////////////////////////////
   struct DelegateFactoryParser
   {
	  
      ///////////////////////////////////////////////////////////////////////////////////////////////////////
      void ParseFileInput(xml_node<>* element, DelegateFactory* delegateFactory)
      {
         std::string propertyName;
         std::string filters = "*";
         for(xml_attribute<>* attr = element->first_attribute();
              attr; attr = attr->next_attribute())
         {
            if(strcmp(attr->name(), "propertyname") == 0)
            {
               propertyName = attr->value();
            }
            else if(strcmp(attr->name(), "filters") == 0)
            {
               filters = attr->value();
            }
         }

         DelegateFactory* factory = new FileSelectorDelegateFactory(filters.c_str());
         delegateFactory->SetFactoryForChildren(propertyName.c_str(), factory);
      }

      ///////////////////////////////////////////////////////////////////////////////////////////////////////
      void ParseColorInput(xml_node<>* element, DelegateFactory* delegateFactory)
      {
         std::string propertyName;
         for(xml_attribute<>* attr = element->first_attribute();
              attr; attr = attr->next_attribute())
         {
            if(strcmp(attr->name(), "propertyname") == 0)
            {
               propertyName = attr->value();
            }
         }

         DelegateFactory* factory = new ColorSelectorDelegateFactory();
         delegateFactory->SetFactoryForChildren(propertyName.c_str(), factory);
      }

      ///////////////////////////////////////////////////////////////////////////////////////////////////////
      void ParseComponentInput(xml_node<>* element, DelegateFactory* delegateFactory)
      {
         std::string propertyName;
         for(xml_attribute<>* attr = element->first_attribute();
              attr; attr = attr->next_attribute())
         {
            if(strcmp(attr->name(), "propertyname") == 0)
            {
               propertyName = attr->value();
            }
         }
         DelegateFactory* factory = new ComponentDelegateFactory();
         delegateFactory->SetFactoryForChildren(propertyName.c_str(), factory);
      }

      ///////////////////////////////////////////////////////////////////////////////////////////////////////
      void ParseEnumInput(xml_node<>* element, DelegateFactory* delegateFactory)
      {
         std::string propertyName;
         std::string values;
         for(xml_attribute<>* attr = element->first_attribute();
              attr; attr = attr->next_attribute())
         {
            if(strcmp(attr->name(), "propertyname") == 0)
            {
               propertyName = attr->value();
            }
            else if(strcmp(attr->name(), "values") == 0)
            {
               values = attr->value();
            }
         }
         DelegateFactory* factory = new EnumDelegateFactory(values.c_str());
         delegateFactory->SetFactoryForChildren(propertyName.c_str(), factory);
      }

      ///////////////////////////////////////////////////////////////////////////////////////////////////////
      void ParseArrayInput(xml_node<>* element, DelegateFactory* delegateFactory)
      {
         std::string propertyName;
         for(xml_attribute<>* attr = element->first_attribute();
              attr; attr = attr->next_attribute())
         {
            if(strcmp(attr->name(), "propertyname") == 0)
            {
               propertyName = attr->value();
            }
         }

         ArrayDelegateFactory* factory = new ArrayDelegateFactory();
         delegateFactory->SetFactoryForChildren(propertyName.c_str(), factory);

         for(xml_node<>* currentNode(element->first_node());
             currentNode != NULL; currentNode = currentNode->next_sibling())
         {
            if(currentNode->type() != node_element)
            {
               continue;
            }

            if(strcmp(currentNode->name(), "prototype") == 0)
            {
               for(xml_node<>* iNode(currentNode->first_node());
                   iNode != NULL; iNode = iNode->next_sibling())
               {
                  if(iNode->type() != node_element)
                  {
                     continue;
                  }

                  factory->mDataPrototype = dtEntity::RapidXMLMapEncoder::ParseProperty(iNode);
                  if(factory->mDataPrototype != NULL)
                  {
                     break;
                  }
               }
            }            
            else if(strcmp(currentNode->name(), "inputs") == 0)
            {
               Translator* translator = NULL;
               ParseDelegateFactory(currentNode, factory, translator);
               assert(translator == NULL);
            }
         }
      }

      ///////////////////////////////////////////////////////////////////////////////////////////////////////
      bool ParseTranslation(xml_node<>* element, Translator*& translator)
      {
        
         QString componentname = "NoName";
         QString language;

         for(xml_attribute<>* attr = element->first_attribute();
              attr; attr = attr->next_attribute())
         {
            if(strcmp(attr->name(), "language") == 0)
            {
               language = attr->value();
            }
         }

         if(language != QLocale::system().name())
         {
            return false;
         }

         QMap<QString, QString> propertynames;
         QMap<QString, QString> descriptions;

         for(xml_node<>* currentNode(element->first_node());
             currentNode != NULL; currentNode = currentNode->next_sibling())
         {
            if(currentNode->type() != node_element)
            {
               continue;
            }

            if(strcmp(currentNode->name(), "componentname") == 0)
            {
               componentname = currentNode->value();
            }
            else if(strcmp(currentNode->name(), "property") == 0)
            {
               QString propname;

               for(xml_attribute<>* attr = element->first_attribute();
                    attr; attr = attr->next_attribute())
               {
                  if(strcmp(attr->name(), "name") == 0)
                  {
                     propname = attr->value();
                  }
               }

               QString trans = currentNode->value();
               propertynames[propname] = trans;
            }
            else if(strcmp(currentNode->name(), "description") == 0)
            {
               QString propname;

               for(xml_attribute<>* attr = element->first_attribute();
                    attr; attr = attr->next_attribute())
               {
                  if(strcmp(attr->name(), "name") == 0)
                  {
                     propname = attr->value();
                  }
               }

               QString trans = currentNode->value();
               descriptions[propname] = trans;
            }

         }

         translator = new Translator();
         translator->mComponentName = componentname;
         translator->mPropertyNames = propertynames;
         translator->mDescriptions = descriptions;
         return true;
      }

      ///////////////////////////////////////////////////////////////////////////////////////////////////////
      bool ParseTranslations(xml_node<>* element, Translator*& translator)
      {
         for(xml_node<>* currentNode(element->first_node());
             currentNode != NULL; currentNode = currentNode->next_sibling())
         {
            if(currentNode->type() == node_element && strcmp(currentNode->name(), "translation") == 0)
            {
               return ParseTranslation(currentNode, translator);
            }
         }
         return false;
      }

      ///////////////////////////////////////////////////////////////////////////////////////////////////////
      void ParseDateTimeInput(xml_node<>* element, DelegateFactory* delegateFactory)
      {
         std::string propertyName;
         for(xml_attribute<>* attr = element->first_attribute();
              attr; attr = attr->next_attribute())
         {
            if(strcmp(attr->name(), "name") == 0)
            {
               propertyName = attr->value();
            }
         }

         DelegateFactory* factory = new DateTimeDelegateFactory();
         delegateFactory->SetFactoryForChildren(propertyName.c_str(), factory);
      }

      ///////////////////////////////////////////////////////////////////////////////////////////////////////
      void ParseDelegateFactory(xml_node<>* element, DelegateFactory* delegateFactory, Translator*& translator)
      {
         for(xml_node<>* currentNode(element->first_node());
             currentNode != NULL; currentNode = currentNode->next_sibling())
         {
            if(currentNode->type() == node_element)
            {

               const char* tagname = currentNode->name();
               if(strcmp("fileinput", tagname) == 0)
               {
                  ParseFileInput(currentNode, delegateFactory);
               }
               else if(strcmp("colorinput", tagname) == 0)
               {
                  ParseColorInput(currentNode, delegateFactory);
               }
               else if(strcmp("componentinput", tagname) == 0)
               {
                  ParseComponentInput(currentNode, delegateFactory);
               }
               else if(strcmp("enuminput", tagname) == 0)
               {
                  ParseEnumInput(currentNode, delegateFactory);
               }
               else if(strcmp("arrayinput", tagname) == 0)
               {
                  ParseArrayInput(currentNode, delegateFactory);
               }
               else if(strcmp("datetimeinput", tagname) == 0)
               {
                  ParseDateTimeInput(currentNode, delegateFactory);
               }
               else if(strcmp("switchinput", tagname) == 0)
               {
                  ParseSwitchInput(currentNode, delegateFactory, translator);
               }
               else if(strcmp("groupinput", tagname) == 0)
               {
                  ParseDelegateFactory(currentNode, delegateFactory, translator);
               }               
               else if(strcmp("translations", tagname) == 0)
               {
                  ParseTranslations(currentNode, translator);
               }
            }
         }
      }

      ///////////////////////////////////////////////////////////////////////////////////////////////////////
      void ParseSwitchInput(xml_node<>* element, DelegateFactory* delegateFactory, Translator*& translator)
      {
         std::string propertyName;
         for(xml_attribute<>* attr = element->first_attribute();
              attr; attr = attr->next_attribute())
         {
            if(strcmp(attr->name(), "name") == 0)
            {
               propertyName = attr->value();
            }
         }

         QMap<QString, dtEntity::Property*> switchgroups;

         for(xml_node<>* currentNode(element->first_node());
             currentNode != NULL; currentNode = currentNode->next_sibling())
         {
            if(currentNode->type() == node_element && strcmp("switchgroup", currentNode->name()) == 0)
            {
               std::string switchgroupname;
               for(xml_attribute<>* attr = currentNode->first_attribute();
                    attr; attr = attr->next_attribute())
               {
                  if(strcmp(attr->name(), "name") == 0)
                  {
                     switchgroupname = attr->value();
                  }
               }

               dtEntity::GroupProperty* gp = new dtEntity::GroupProperty();
               for(xml_node<>* iNode(currentNode->first_node());
                   iNode != NULL; iNode = iNode->next_sibling())
               {
                  if(iNode->type() != node_element)
                  {
                     continue;
                  }

                  std::string propname;
                  for(xml_attribute<>* attr = iNode->first_attribute();
                       attr; attr = attr->next_attribute())
                  {
                     if(strcmp(attr->name(), "name") == 0)
                     {
                        propname = attr->value();
                     }
                  }
                  gp->Add(dtEntity::SID(propname), dtEntity::RapidXMLMapEncoder::ParseProperty(iNode));
                  ParseDelegateFactory(currentNode, delegateFactory, translator);
               }
               switchgroups[switchgroupname.c_str()] = gp;
            }
         }

         DelegateFactory* factory = new SwitchDelegateFactory(switchgroups);
         delegateFactory->SetFactoryForChildren(propertyName.c_str(), factory);
      }
        
      ///////////////////////////////////////////////////////////////////////////////////////////////////////
      bool LoadFactoryFromFile(const QString& path, DelegateFactory*& factory, Translator*& translator)
      {
         const std::string foundPath = osgDB::findDataFile(path.toStdString());
         if(!osgDB::fileExists(foundPath))
         {
            LOG_DEBUG("Cannot load delegate factory, file not found: " + foundPath);
            return NULL;
         }

         FILE* fptr = fopen(foundPath.c_str(), "rt");

         if(!fptr)
         {
            return false;
         }



         std::ostringstream os;
         char buffer[1024];
         while (fgets(buffer, 1024, fptr))
         {
            os << buffer;
         }
         std::string str = os.str();

         char* data = const_cast<char*>(str.c_str());

         fclose(fptr);

         try
         {
            xml_document<> doc;    // character type defaults to char
            doc.parse<0>(data);

            xml_node<>* delegatenode = doc.first_node("delegates");

            factory = new DelegateFactory();

            for(xml_attribute<>* attr = delegatenode->first_attribute();
                 attr; attr = attr->next_attribute())
            {
               if(strcmp(attr->name(), "allowcreatefromgui") == 0 && strcmp(attr->value(), "false"))
               {
                  factory->SetAllowCreateFromGUI(false);
               }
            }

            ParseDelegateFactory(delegatenode, factory, translator);
            return true;

         }
         catch(const std::exception& ex)
         {
            LOG_ERROR("XML Parsing error: "+ std::string(ex.what()));
            return false;
         }
      }
   };  



   ////////////////////////////////////////////////////////////////////////////////
   bool CreateDelegateFactory(const QString& path, DelegateFactory*& target, Translator*& trans)
   {
      DelegateFactoryParser parser;
      return parser.LoadFactoryFromFile(path, target, trans);               
   }
}
