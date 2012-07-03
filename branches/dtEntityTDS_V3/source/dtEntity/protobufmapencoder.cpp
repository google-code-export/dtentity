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

#include <dtEntity/protobufmapencoder.h>

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
#include <data.pb.h>

namespace dtEntity
{
  
  
   ////////////////////////////////////////////////////////////////////////////////
   ProtoBufMapEncoder::ProtoBufMapEncoder(EntityManager& em)
      : mEntityManager(&em)
   {
      em.GetEntitySystem(MapComponent::TYPE, mMapSystem);
	}
   
   ////////////////////////////////////////////////////////////////////////////////
   ProtoBufMapEncoder::~ProtoBufMapEncoder()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool StartES(EntityManager& em, StringId componentType)
   {
      if(em.HasEntitySystem(componentType))
      {
         return true;
      }

      MapSystem* mapSystem;
      em.GetEntitySystem(MapComponent::TYPE, mapSystem);

      ComponentPluginManager& pluginManager = ComponentPluginManager::GetInstance();
      if(!pluginManager.FactoryExists(componentType))
      {
         return false;
      }
      return pluginManager.StartEntitySystem(em, componentType);
   }


   ////////////////////////////////////////////////////////////////////////////////
   void SerializeProperty(dtProtoBuf::Property& propertyobj, StringId propname, const Property& prop)
   {
      propertyobj.set_property_name(SIDToUInt(propname));
      
      switch(prop.GetDataType())
      {
      case DataType::ARRAY:      propertyobj.set_type(dtProtoBuf::ARRAY); break;
      case DataType::BOOL:       propertyobj.set_type(dtProtoBuf::BOOL); break;
      case DataType::DOUBLE:     propertyobj.set_type(dtProtoBuf::DOUBLE); break;
      case DataType::FLOAT:      propertyobj.set_type(dtProtoBuf::FLOAT); break;
      case DataType::GROUP:      propertyobj.set_type(dtProtoBuf::GROUP); break;
      case DataType::INT:        propertyobj.set_type(dtProtoBuf::INT); break;
      case DataType::MATRIX:     propertyobj.set_type(dtProtoBuf::MATRIX); break;
      case DataType::QUAT:       propertyobj.set_type(dtProtoBuf::QUAT); break;
      case DataType::STRING:     propertyobj.set_type(dtProtoBuf::STRING); break;
      case DataType::STRINGID:   propertyobj.set_type(dtProtoBuf::STRINGID); break;
      case DataType::UINT:       propertyobj.set_type(dtProtoBuf::UINT); break;
      case DataType::VEC2:       propertyobj.set_type(dtProtoBuf::VEC2); break;
      case DataType::VEC3:       propertyobj.set_type(dtProtoBuf::VEC3); break;
      case DataType::VEC4:       propertyobj.set_type(dtProtoBuf::VEC4); break;
      case DataType::VEC2D:      propertyobj.set_type(dtProtoBuf::VEC2D); break;
      case DataType::VEC3D:      propertyobj.set_type(dtProtoBuf::VEC3D); break;
      case DataType::VEC4D:      propertyobj.set_type(dtProtoBuf::VEC4D); break;
      default: assert(false);
      }

      switch(prop.GetDataType())
      {
      case DataType::ARRAY:     {
         PropertyArray g = prop.ArrayValue();

         unsigned int index = 0;
         for(PropertyArray::const_iterator i = g.begin(); i != g.end(); ++i)
         {
            std::ostringstream os;
            os << index++;
            SerializeProperty(*propertyobj.add_value_array(), SID(os.str()), **i);
         }
         break;
      }
      case DataType::BOOL:       propertyobj.set_value_bool(prop.BoolValue()); break;
      case DataType::DOUBLE:     propertyobj.set_value_double(prop.DoubleValue()); break;
      case DataType::FLOAT:      propertyobj.set_value_float(prop.FloatValue()); break;
      case DataType::GROUP:      {
         PropertyGroup g = prop.GroupValue();

         for(PropertyGroup::const_iterator i = g.begin(); i != g.end(); ++i)
         {
            SerializeProperty(*propertyobj.add_value_array(), i->first, *(i->second));
         }
         break;
      }
      case DataType::INT:        propertyobj.set_value_int(prop.IntValue()); break;
      case DataType::MATRIX:     {
         dtProtoBuf::Matrix& mat = *propertyobj.mutable_value_matrix();
         const osg::Matrix m = prop.MatrixValue();
         mat.set_value_0(m(0,0)); 
         mat.set_value_1(m(0,1));
         mat.set_value_2(m(0,2));
         mat.set_value_3(m(0,3));
         mat.set_value_4(m(1,0));
         mat.set_value_5(m(1,1));
         mat.set_value_6(m(1,2));
         mat.set_value_7(m(1,3));
         mat.set_value_8(m(2,0));
         mat.set_value_9(m(2,1));
         mat.set_value_10(m(2,2));
         mat.set_value_11(m(2,3));
         mat.set_value_12(m(3,0));
         mat.set_value_13(m(3,1));
         mat.set_value_14(m(3,2));
         mat.set_value_15(m(3,3));         
         break;
      }
      case DataType::QUAT:       {
         dtProtoBuf::Quat& q = *propertyobj.mutable_value_quat();
         const osg::Quat quat = prop.QuatValue();
         q.set_value_0(quat[0]);
         q.set_value_1(quat[1]);
         q.set_value_2(quat[2]);
         q.set_value_3(quat[3]);
         break;
      }
      case DataType::STRING:     {
         std::string str = prop.StringValue();
         if(str.size() != 0)
         {
            propertyobj.set_value_string(str); break;
         }
         break;
      }
      case DataType::STRINGID:   propertyobj.set_value_stringid(SIDToUInt(prop.StringIdValue())); break;
      case DataType::UINT:       propertyobj.set_value_uint(prop.UIntValue()); break;
      case DataType::VEC2:       {
         dtProtoBuf::Vec2& v = *propertyobj.mutable_value_vec2();
         const osg::Vec2 vec = prop.Vec2Value();
         v.set_value_0(vec[0]);
         v.set_value_1(vec[1]);
         break;
      }
      case DataType::VEC3:       {
         dtProtoBuf::Vec3& v = *propertyobj.mutable_value_vec3();
         const osg::Vec3 vec = prop.Vec3Value();
         v.set_value_0(vec[0]);
         v.set_value_1(vec[1]);
         v.set_value_2(vec[2]);
         break;
      }
      case DataType::VEC4:       {
         dtProtoBuf::Vec4& v = *propertyobj.mutable_value_vec4();
         const osg::Vec4 vec = prop.Vec4Value();
         v.set_value_0(vec[0]);
         v.set_value_1(vec[1]);
         v.set_value_2(vec[2]);
         v.set_value_3(vec[3]);
         break;
      }
      case DataType::VEC2D:      {
         dtProtoBuf::Vec2d& v = *propertyobj.mutable_value_vec2d();
         const osg::Vec2d vec = prop.Vec2dValue();
         v.set_value_0(vec[0]);
         v.set_value_1(vec[1]);
         break;
      }
      case DataType::VEC3D:      {
         dtProtoBuf::Vec3d& v = *propertyobj.mutable_value_vec3d();
         const osg::Vec3d vec = prop.Vec3dValue();
         v.set_value_0(vec[0]);
         v.set_value_1(vec[1]);
         v.set_value_2(vec[2]);
         break;
      }
      case DataType::VEC4D:      {
         dtProtoBuf::Vec4d& v = *propertyobj.mutable_value_vec4d();
         const osg::Vec4d vec = prop.Vec4dValue();
         v.set_value_0(vec[0]);
         v.set_value_1(vec[1]);
         v.set_value_2(vec[2]);
         v.set_value_3(vec[3]);
         break;
      }
      default: assert(false);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SerializeComponent(dtProtoBuf::Component& componentobj, ComponentType ctype, const GroupProperty& props, const GroupProperty& defaults)
   {
      componentobj.set_component_type(SIDToUInt(ctype));
      PropertyGroup::const_iterator i;
      for(i = props.Get().begin(); i != props.Get().end(); ++i)
      {
         const Property* deflt = defaults.Get(i->first);
         if(deflt == NULL || !((*deflt) == (*i->second)))
         {
            SerializeProperty(*componentobj.add_property(), i->first, *(i->second));
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SerializeSpawner(dtProtoBuf::Spawner& spawnerobj, Spawner* spawner)
   {
      spawnerobj.set_name(spawner->GetName());
      spawnerobj.set_guicategory(spawner->GetGUICategory());
      spawnerobj.set_addtospawnerstore(spawner->GetAddToSpawnerStore());
      spawnerobj.set_iconpath(spawner->GetIconPath());
      if(spawner->GetParent() != NULL)
      {
         spawnerobj.set_parent(spawner->GetParent()->GetName());
      }      

      Spawner::ComponentProperties spawnerprops;
      spawner->GetAllComponentProperties(spawnerprops);

      PropertyGroup defaults;
      Spawner::ComponentProperties::const_iterator i;
      for(i = spawnerprops.begin(); i != spawnerprops.end(); ++i)
      {
         // no need to save map component, is redundant
         if(i->first != MapComponent::TYPE)
         {            
            SerializeComponent(*spawnerobj.add_component(), i->first, i->second, defaults);
         }         
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SerializeEntity(dtProtoBuf::Entity& entityobj, EntityManager& em, EntityId eid, MapComponent* mapcomponent)
   {

      MapSystem* mapSystem;
      em.GetEntitySystem(MapComponent::TYPE, mapSystem);

      // get spawner of entity so that only property values are changed that are not identical
      // to the values from the spawner
      Spawner::ComponentProperties spawnerprops;

      std::string spawnername = mapcomponent->GetSpawnerName();
      if(spawnername != "")
      {
         Spawner* spawner;
         if(mapSystem->GetSpawner(spawnername, spawner))
         {
            spawner->GetAllComponentPropertiesRecursive(spawnerprops);
         }
         entityobj.set_spawner(spawnername);
      }

      std::vector<const Component*> comps;
      em.GetComponents(eid, comps);

      for(std::vector<const Component*>::const_iterator i = comps.begin(); i != comps.end(); ++i)
      {
         ComponentType componentType = (*i)->GetType();

         EntitySystem* es = em.GetEntitySystem(componentType);
         if(es->StoreComponentToMap(eid))
         {
            // Get default component values. If entity was created from a spawner
            // then overwrite component values with values from spawner
            GroupProperty defaultprops = es->GetComponentProperties();
            Spawner::ComponentProperties::iterator it = spawnerprops.find(componentType);
            if(it != spawnerprops.end())
            {
               defaultprops += (it->second);
            }

            SerializeComponent(*entityobj.add_component(), componentType, **i, defaultprops);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SerializeEntitySystem(dtProtoBuf::EntitySystem& esobj, const EntitySystem* es)
   {

      esobj.set_component_type(SIDToUInt(es->GetComponentType()));

      const PropertyGroup& p = es->Get();

      PropertyGroup::const_iterator i;
      for(i = p.begin(); i != p.end(); ++i)
      {
         SerializeProperty(*esobj.add_property(), i->first, *i->second);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   Property* ParseProperty(const dtProtoBuf::Property& propobj)
   {
      dtProtoBuf::PropertyType ptype = propobj.type();
      switch(ptype)
      {
      case dtProtoBuf::ARRAY: {
         ArrayProperty* p = new ArrayProperty();

         for(int i = 0; i < propobj.value_array_size(); ++i)
         {
            Property* property = ParseProperty(propobj.value_array(i));
            if(property != NULL)
            {
               p->Add(property);
            }
         }
         return p;
      }
      case dtProtoBuf::BOOL:     { return new BoolProperty(propobj.value_bool()); }
      case dtProtoBuf::DOUBLE:   { return new DoubleProperty(propobj.value_double()); }
      case dtProtoBuf::FLOAT:    { return new FloatProperty(propobj.value_float()); }
      case dtProtoBuf::GROUP:    {
         GroupProperty* p = new GroupProperty();

         for(int i = 0; i < propobj.value_array_size(); ++i)
         {
            Property* property = ParseProperty(propobj.value_array(i));
            if(property != NULL)
            {
               p->Add(SID(propobj.value_array(i).property_name()), property);
            }
         }
         return p;
      }
      case dtProtoBuf::INT:      { return new IntProperty(propobj.value_int()); }
      case dtProtoBuf::MATRIX:   {

         const dtProtoBuf::Matrix& v = propobj.value_matrix();
         osg::Matrix mat;
         mat(0,0) = v.value_0();
         mat(0,1) = v.value_1();
         mat(0,2) = v.value_2();
         mat(0,3) = v.value_3();
         mat(1,0) = v.value_4();
         mat(1,1) = v.value_5();
         mat(1,2) = v.value_6();
         mat(1,3) = v.value_7();
         mat(2,0) = v.value_8();
         mat(2,1) = v.value_9();
         mat(2,2) = v.value_10();
         mat(2,3) = v.value_11();
         mat(3,0) = v.value_12();
         mat(3,1) = v.value_13();
         mat(3,2) = v.value_14();
         mat(3,3) = v.value_15();
         return new MatrixProperty(mat);
      }


      case dtProtoBuf::QUAT:     {
         const dtProtoBuf::Quat& v = propobj.value_quat();
         return new QuatProperty(osg::Quat(v.value_0(), v.value_1(), v.value_2(), v.value_3()));
      }
      case dtProtoBuf::STRING:   {
         std::string str = "";
         if(propobj.has_value_string())
         {
            str = propobj.value_string();
         }
         return new StringProperty(str);
      }
      case dtProtoBuf::STRINGID: { return new StringIdProperty(SID(propobj.value_stringid())); }
      case dtProtoBuf::UINT:     { return new UIntProperty(propobj.value_uint()); }
      case dtProtoBuf::VEC2:     {
         const dtProtoBuf::Vec2& v = propobj.value_vec2();
         return new Vec2Property(osg::Vec2(v.value_0(), v.value_1()));
      }
      case dtProtoBuf::VEC3:     {
         const dtProtoBuf::Vec3& v = propobj.value_vec3();
         return new Vec3Property(osg::Vec3(v.value_0(), v.value_1(), v.value_2()));
      }
      case dtProtoBuf::VEC4:     {
         const dtProtoBuf::Vec4& v = propobj.value_vec4();
         return new Vec4Property(osg::Vec4(v.value_0(), v.value_1(), v.value_2(), v.value_3()));
      }
      case dtProtoBuf::VEC2D:    {
         const dtProtoBuf::Vec2d& v = propobj.value_vec2d();
         return new Vec2dProperty(osg::Vec2d(v.value_0(), v.value_1()));
      }
      case dtProtoBuf::VEC3D:    {
         const dtProtoBuf::Vec3d& v = propobj.value_vec3d();
         return new Vec3dProperty(osg::Vec3d(v.value_0(), v.value_1(), v.value_2()));
      }
      case dtProtoBuf::VEC4D:    {
         const dtProtoBuf::Vec4d& v = propobj.value_vec4d();
         return new Vec4dProperty(osg::Vec4d(v.value_0(), v.value_1(), v.value_2(), v.value_3()));
      }
      default:
         LOG_ERROR("Could not parse property, unknown type!");
         return NULL;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool SetPropertyFrom(Property* prop, const dtProtoBuf::Property& propobj)
   {
      dtProtoBuf::PropertyType ptype = propobj.type();
      switch(ptype)
      {
      case dtProtoBuf::ARRAY: {
         if(prop->GetDataType() != DataType::ARRAY)
         {
            return false;
         }

         PropertyArray pa;
         for(int i = 0; i < propobj.value_array_size(); ++i)
         {
            Property* property = ParseProperty(propobj.value_array(i));
            if(property != NULL)
            {
               pa.push_back(property);
            }
         }
         prop->SetArray(pa);
         return true;
      }
      case dtProtoBuf::BOOL:
      {
         if(prop->GetDataType() != DataType::BOOL)
         {
            return false;
         }
         prop->SetBool(propobj.value_bool());
         return true;
      }
      case dtProtoBuf::DOUBLE:
      {
         if(prop->GetDataType() != DataType::DOUBLE)
         {
            return false;
         }
         prop->SetDouble(propobj.value_double());
         return true;
      }
      case dtProtoBuf::FLOAT:
      {
         if(prop->GetDataType() != DataType::FLOAT)
         {
            return false;
         }
         prop->SetFloat(propobj.value_float());
         return true;
      }
      case dtProtoBuf::GROUP:    {
         if(prop->GetDataType() != DataType::GROUP)
         {
            return false;
         }
         PropertyGroup pg;

         for(int i = 0; i < propobj.value_array_size(); ++i)
         {
            Property* property = ParseProperty(propobj.value_array(i));
            if(property != NULL)
            {
               pg[SID(propobj.value_array(i).property_name())] = property;
            }
         }
         prop->SetGroup(pg);
         return true;
      }
      case dtProtoBuf::INT:
      {
         if(prop->GetDataType() != DataType::INT)
         {
            return false;
         }
         prop->SetInt(propobj.value_int());
         return true;
      }
      case dtProtoBuf::MATRIX:   {

         if(prop->GetDataType() != DataType::MATRIX)
         {
            return false;
         }
         const dtProtoBuf::Matrix& v = propobj.value_matrix();
         osg::Matrix mat;
         mat(0,0) = v.value_0();
         mat(0,1) = v.value_1();
         mat(0,2) = v.value_2();
         mat(0,3) = v.value_3();
         mat(1,0) = v.value_4();
         mat(1,1) = v.value_5();
         mat(1,2) = v.value_6();
         mat(1,3) = v.value_7();
         mat(2,0) = v.value_8();
         mat(2,1) = v.value_9();
         mat(2,2) = v.value_10();
         mat(2,3) = v.value_11();
         mat(3,0) = v.value_12();
         mat(3,1) = v.value_13();
         mat(3,2) = v.value_14();
         mat(3,3) = v.value_15();
         prop->SetMatrix(mat);
      }


      case dtProtoBuf::QUAT:     {
         if(prop->GetDataType() != DataType::QUAT)
         {
            return false;
         }
         const dtProtoBuf::Quat& v = propobj.value_quat();

         prop->SetQuat(osg::Quat(v.value_0(), v.value_1(), v.value_2(), v.value_3()));
         return true;

      }
      case dtProtoBuf::STRING:   {
         if(prop->GetDataType() != DataType::STRING)
         {
            return false;
         }

         std::string str = "";
         if(propobj.has_value_string())
         {
            str = propobj.value_string();
         }

         prop->SetString(str);
         return true;
      }
      case dtProtoBuf::STRINGID: {
         if(prop->GetDataType() != DataType::STRINGID)
         {
            return false;
         }
         prop->SetStringId(SID(propobj.value_stringid()));
         return true;
      }
      case dtProtoBuf::UINT:     {
         if(prop->GetDataType() != DataType::UINT)
         {
            return false;
         }
         prop->SetUInt(propobj.value_uint());
         return true;
      }
      case dtProtoBuf::VEC2:     {
         if(prop->GetDataType() != DataType::VEC2)
         {
            return false;
         }
         const dtProtoBuf::Vec2& v = propobj.value_vec2();

         prop->SetVec2(osg::Vec2(v.value_0(), v.value_1()));
         return true;
      }
      case dtProtoBuf::VEC3:     {
         if(prop->GetDataType() != DataType::VEC3)
         {
            return false;
         }
         const dtProtoBuf::Vec3& v = propobj.value_vec3();

         prop->SetVec3(osg::Vec3(v.value_0(), v.value_1(), v.value_2()));
         return true;
      }
      case dtProtoBuf::VEC4:     {
         if(prop->GetDataType() != DataType::VEC4)
         {
            return false;
         }
         const dtProtoBuf::Vec4& v = propobj.value_vec4();

         prop->SetVec4(osg::Vec4(v.value_0(), v.value_1(), v.value_2(), v.value_3()));
         return true;
      }
      case dtProtoBuf::VEC2D:    {
         if(prop->GetDataType() != DataType::VEC2D)
         {
            return false;
         }
         const dtProtoBuf::Vec2d& v = propobj.value_vec2d();

         prop->SetVec2D(osg::Vec2d(v.value_0(), v.value_1()));
         return true;
      }
      case dtProtoBuf::VEC3D:    {
         if(prop->GetDataType() != DataType::VEC3D)
         {
            return false;
         }
         const dtProtoBuf::Vec3d& v = propobj.value_vec3d();

         prop->SetVec3D(osg::Vec3d(v.value_0(), v.value_1(), v.value_2()));
         return true;
      }
      case dtProtoBuf::VEC4D:    {
         if(prop->GetDataType() != DataType::VEC4D)
         {
            return false;
         }
         const dtProtoBuf::Vec4d& v = propobj.value_vec4d();

         prop->SetVec4D(osg::Vec4d(v.value_0(), v.value_1(), v.value_2(), v.value_3()));
         return true;
      }
      default:
         LOG_ERROR("Could not parse property, unknown type!");
         return false;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AddComponentToSpawner(const dtProtoBuf::Component& compobj, Spawner& spawner)
   {

      GroupProperty props;
      for(int i = 0; i < compobj.property_size(); ++i)
      {
         const dtProtoBuf::Property& propobj = compobj.property(i);
         Property* property = ParseProperty(propobj);
         if(property != NULL)
         {
            props.Add(SID(propobj.type()), property);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ParseSpawner(EntityManager& em, const dtProtoBuf::Spawner& spawnerobj, const std::string& mapName)
   {
      MapSystem* mapSystem;
      em.GetEntitySystem(MapComponent::TYPE, mapSystem);

      Spawner* spawner;
      if(spawnerobj.has_parent())
      {
         spawner = new Spawner(spawnerobj.name(), mapName);
      }
      else
      {
         Spawner* parentspawner;
         bool found = mapSystem->GetSpawner(spawnerobj.parent(), parentspawner);
         if(!found)
         {
            LOG_ERROR("Cannot initialize spawner: Parent spawner not found. Name: " + spawnerobj.parent());
            return;
         }
         spawner = new Spawner(spawnerobj.name(), mapName, parentspawner);
      }

      if(spawnerobj.has_guicategory())
      {
         spawner->SetGUICategory(spawnerobj.guicategory());
      }

      if(spawnerobj.has_iconpath())
      {
         spawner->SetIconPath(spawnerobj.iconpath());
      }

      if(!spawnerobj.has_addtospawnerstore())
      {
         spawner->SetAddToSpawnerStore(spawnerobj.addtospawnerstore());
      }

      for(int i = 0; i < spawnerobj.component_size(); ++i)
      {
         AddComponentToSpawner(spawnerobj.component(i), *spawner);
      }


      // add meta data component to entity
      GroupProperty mapComponentProps;
      if(spawner->HasComponent(MapComponent::TYPE))
      {
         mapComponentProps = spawner->GetComponentValues(MapComponent::TYPE);
      }

      mapComponentProps.Add(MapComponent::SpawnerNameId, new StringIdProperty(SID(spawnerobj.name())));
      mapComponentProps.Add(MapComponent::MapNameId, new StringProperty(mapName));
      spawner->AddComponent(MapComponent::TYPE, mapComponentProps);
      mapSystem->AddSpawner(*spawner);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CreateComponent(EntityManager& em, const dtProtoBuf::Component& componentobj, EntityId entityId, const std::string& mapname)
   {

      Component* component;
      ComponentType ctype = SID(componentobj.component_type());
      bool found = em.GetComponent(entityId, ctype, component);
      if(!found)
      {
         bool success = StartES(em, ctype);
         if(!success)
         {
            LOG_WARNING("In Map " << mapname << ": Cannot add component, no entity system of this type registered: " << GetStringFromSID(ctype));
            return;
         }
         em.CreateComponent(entityId, ctype, component);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SetupComponent(EntityManager& em, const dtProtoBuf::Component& componentobj, EntityId entityId, const std::string& mapname)
   {
      StringId componentType = SID(componentobj.component_type());

      Component* component;
      bool found = em.GetComponent(entityId, componentType, component);
      if(!found)
      {
         return;
      }

      for(int i = 0; i < componentobj.property_size(); ++i)
      {
         const dtProtoBuf::Property& prop = componentobj.property(i);
         Property* toset = component->Get(SID(prop.property_name()));
         if(toset == NULL)
         {
            LOG_WARNING("In Map " << mapname << ": Property " << GetStringFromSID(SID(prop.property_name()))
           << " does not exist in component "
           << GetStringFromSID(componentType));
         }
         else
         {
            bool success = SetPropertyFrom(toset, prop);
            if(!success)
            {
               LOG_ERROR("Property type mismatch!");
            }
            component->OnPropertyChanged(SID(prop.property_name()), *toset);
         }
      }

      component->Finished();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ParseEntity(EntityManager& em, const dtProtoBuf::Entity& entityobj, const std::string& mapName)
   {

      dtEntity::Entity* newentity;
      bool success = em.CreateEntity(newentity);
      assert(success);

      if(entityobj.has_spawner() && !entityobj.spawner().empty())
      {
         Spawner* spawner;
         MapSystem* mapSystem;
         em.GetEntitySystem(MapComponent::TYPE, mapSystem);
         bool found = mapSystem->GetSpawner(entityobj.spawner(), spawner);
         if(!found)
         {
            LOG_ERROR("Spawner not found: " + entityobj.spawner());
            return;
         }

         success = spawner->Spawn(*newentity);
         assert(success);
      }

      for(int i = 0; i < entityobj.component_size(); ++i)
      {
         CreateComponent(em, entityobj.component(i), newentity->GetId(), mapName);
      }

      for(int i = 0; i < entityobj.component_size(); ++i)
      {
         SetupComponent(em, entityobj.component(i), newentity->GetId(), mapName);
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
   void SetupEntitySystem(EntityManager& em, const dtProtoBuf::EntitySystem& esobj, const std::string& filename)
   {
      StringId componentType = SID(esobj.component_type());

      if(!em.HasEntitySystem(componentType))
      {
         bool success = StartES(em, componentType);
         if(!success)
         {
            LOG_WARNING("In scene " << filename << ": Cannot setup entity system. Component type not found: " + GetStringFromSID(componentType));
            return;
         }
      }
      EntitySystem* es = em.GetEntitySystem(componentType);
      assert(es != NULL);

      for(int i = 0; i < esobj.property_size(); ++i)
      {
         const dtProtoBuf::Property& prop = esobj.property(i);
         Property* toset = es->Get(SID(prop.property_name()));
         if(toset == NULL)
         {
            LOG_WARNING("In System " << filename << ": Property " << GetStringFromSID(SID(prop.property_name()))
           << " does not exist in component "
           << GetStringFromSID(componentType));
         }
         else
         {
            bool success = SetPropertyFrom(toset, prop);
            if(!success)
            {
               LOG_ERROR("Property type mismatch!");
            }
            es->OnPropertyChanged(SID(prop.property_name()), *toset);
         }
      }

      es->Finished();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool ProtoBufMapEncoder::LoadMapFromFile(const std::string& path)
   {
      GOOGLE_PROTOBUF_VERIFY_VERSION;

      const std::string absPath = osgDB::findDataFile(path);
      if(absPath == "")
      {
         LOG_ERROR("Map not found: " + path);
         return false;
      }

      dtProtoBuf::Map mapobj;
      std::fstream input(absPath.c_str(), std::ios::in | std::ios::binary);

      bool success = mapobj.ParseFromIstream(&input);
      input.close();

      if(!success)
      {
        LOG_ERROR("Failed to parse map file.");
        return false;
      }

      for(int i = 0; i < mapobj.spawner_size(); ++i)
      {
         const dtProtoBuf::Spawner& spawnerobj = mapobj.spawner(i);
         ParseSpawner(*mEntityManager, spawnerobj, path);
      }

      for(int i = 0; i < mapobj.entity_size(); ++i)
      {
         const dtProtoBuf::Entity& entityobj = mapobj.entity(i);
         ParseEntity(*mEntityManager, entityobj, path);
      }

      return true;

	}

   ////////////////////////////////////////////////////////////////////////////////
   bool ProtoBufMapEncoder::LoadSceneFromFile(const std::string& path)
   {
      GOOGLE_PROTOBUF_VERIFY_VERSION;

      const std::string absPath = osgDB::findDataFile(path);
      if(absPath == "")
      {
         LOG_ERROR("Scene not found: " + path);
         return false;
      }

      dtProtoBuf::Scene sceneobj;
      std::fstream input(absPath.c_str(), std::ios::in | std::ios::binary);

      bool success = sceneobj.ParseFromIstream(&input);

      input.close();

      if(!success)
      {
        LOG_ERROR("Failed to parse scene file.");
        return false;
      }


      ComponentPluginManager& pluginManager = ComponentPluginManager::GetInstance();

      // first load all libraries
      for(int i = 0; i < sceneobj.libraries_size(); ++i)
      {
         std::string fullLibName = osgDB::Registry::instance()->createLibraryNameForNodeKit(sceneobj.libraries(i));
         pluginManager.AddPlugin(fullLibName, true);
      }

      for(int i = 0; i < sceneobj.entitysystem_size(); ++i)
      {
         SetupEntitySystem(*mEntityManager, sceneobj.entitysystem(i), path);
      }

      for(int i = 0; i < sceneobj.map_size(); ++i)
      {
         mMapSystem->LoadMap(sceneobj.map(i));
      }

      return true;
   }


   ////////////////////////////////////////////////////////////////////////////////
   bool ProtoBufMapEncoder::SaveMapToFile(const std::string& path, const std::string& p_dest)
   {
      // Verify that the version of the library that we linked against is
      // compatible with the version of the headers we compiled against.
      GOOGLE_PROTOBUF_VERIFY_VERSION;


      dtProtoBuf::Map mapobj;

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
         SerializeSpawner(*mapobj.add_spawner(), candidate);
      }

      std::vector<EntityId> eids;
      mMapSystem->GetEntitiesInMap(path, eids);
      for(std::vector<EntityId>::const_iterator i = eids.begin(); i != eids.end(); ++i)
      {
         MapComponent* mapcomp;
         if(mEntityManager->GetComponent(*i, mapcomp) && mapcomp->GetSaveWithMap())
         {
            SerializeEntity(*mapobj.add_entity(), *mEntityManager, *i, mapcomp);
         }
      }
   
      std::fstream output(p_dest.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
      bool success = mapobj.SerializeToOstream(&output);
      output.close();

      if(!success)
      {
         LOG_ERROR("Could not write map.");
      }

      return success;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool ProtoBufMapEncoder::SaveSceneToFile(const std::string& path)
   {

      // Verify that the version of the library that we linked against is
      // compatible with the version of the headers we compiled against.
      GOOGLE_PROTOBUF_VERIFY_VERSION;


      dtProtoBuf::Scene sceneobj;

      MapSystem* mapSystem;
      mEntityManager->GetES(mapSystem);
      ComponentPluginManager& pluginManager = ComponentPluginManager::GetInstance();

      const std::map<std::string, bool>& pluginList = pluginManager.GetLoadedPlugins();
      std::map<std::string, bool>::const_iterator itr;
      for(itr = pluginList.begin(); itr != pluginList.end(); ++itr)
      {
         if (itr->second == true)
         {
            // this plugin must be saved to file: add it
            sceneobj.add_libraries(itr->first);
         }
      }

      std::vector<const EntitySystem*> es;
      mEntityManager->GetEntitySystems(es);
      std::vector<const EntitySystem*>::const_iterator i;
      for(i = es.begin(); i != es.end(); ++i)
      {
         if((*i)->StorePropertiesToScene())
         {
            SerializeEntitySystem(*sceneobj.add_entitysystem(), *i);
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

         for(Ordered::iterator k = ordered.begin(); k != ordered.end(); ++k)
         {
            sceneobj.add_map(k->second);
         }
      }

      std::fstream output(path.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
      bool success = sceneobj.SerializeToOstream(&output);
      output.close();

      if(!success)
      {
         LOG_ERROR("Could not write scene.");
      }

      return success;
   }   

   ////////////////////////////////////////////////////////////////////////////////
   bool ProtoBufMapEncoder::EncodeMessage(const Message& m, std::iostream& stream)
   {
      dtProtoBuf::Message messageobj;
      messageobj.set_message_type(SIDToUInt(m.GetType()));
      const PropertyGroup& props = m.Get();
      for(PropertyGroup::const_iterator i = props.begin(); i != props.end(); ++i)
      {
         SerializeProperty(*messageobj.add_property(), i->first, *i->second);
      }
      return messageobj.SerializeToOstream(&stream);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Message* ProtoBufMapEncoder::DecodeMessage(std::istream& stream)
   {
      dtProtoBuf::Message messageobj;
      if(!messageobj.ParseFromIstream(&stream))
      {
         return NULL;
      }

      Message* msg;
      bool success = MessageFactory::GetInstance().CreateMessage(SID(messageobj.message_type()), msg);
      if(!success)
      {
         LOG_ERROR("Message type not found!");
         return NULL;
      }
      for(int i = 0; i < messageobj.property_size(); ++i)
      {
         const dtProtoBuf::Property& prop = messageobj.property(i);
         Property* toset = msg->Get(SID(prop.property_name()));
         if(toset == NULL)
         {
            LOG_WARNING("Error decoding message : Property " << GetStringFromSID(SID(prop.property_name()))
           << " does not exist in message "
           << GetStringFromSID(SID(messageobj.message_type())) );
         }
         else
         {
            bool success = SetPropertyFrom(toset, prop);
            if(!success)
            {
               LOG_ERROR("Message Parsing: " << GetStringFromSID(SID(messageobj.message_type())) << " Property type mismatch!");
            }
         }
      }
      return msg;
   }
}
