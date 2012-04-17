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


#include <dtEntity/entity.h>
#include <dtEntity/entitysystem.h>
#include <dtEntity/dtentity_config.h>

#include <dtEntity/entitymanager.h>
#include <dtEntity/log.h>
#include <assert.h>

#if USE_BOOST_POOL
#include <boost/pool/object_pool.hpp>
#include <boost/pool/pool.hpp>
#endif

#if defined(_MSC_VER) && (_MSC_VER >=1500)
#   include <unordered_map>
#else
#   ifdef __APPLE__
#      include <ext/hash_map>
#   else
#      include <hash_map>
#   endif
#endif


namespace dtEntity
{

#if defined(_MSC_VER) && (_MSC_VER >=1500)

   template<class T>
   class ComponentStoreMap
      : public std::tr1::unordered_map<EntityId, T*>
   {
   };
#elif defined(__GNUG__)
   template<class T>
   class ComponentStoreMap
      : public __gnu_cxx::hash_map<EntityId, T*>
   {
   };
#else
	template<class T>
   class ComponentStoreMap
      : public std::map<EntityId, T*>
   {
   };
#endif

   template<class T>
   struct MemAllocPolicyNew
   {
      static T* Create()
      {
         return new T;
      }

      static void Destroy(T* t)
      {
         delete t;
      }

      static void DestroyAll(ComponentStoreMap<T>& components)
      {
         for(typename ComponentStoreMap<T>::iterator i = components.begin(); i != components.end(); ++i)
         {
            delete i->second;
         }
      }

   protected:
      ~MemAllocPolicyNew() {}
   };


#if USE_BOOST_POOL
   template<class T>
   struct MemAllocPolicyBoostPool
   {
      static T* Create()
      {
         return mComponentPool->construct();
      }

      static void Destroy(T* t)
      {
         mComponentPool->destroy(t);
      }

      static void DestroyAll(ComponentStoreMap<T>& components)
      {
         delete mComponentPool;
      }

   protected:
      ~MemAllocPolicyBoostPool() {}

   private:
      boost::object_pool<T>* mComponentPool;
   };
#endif

   /**
    * A simple base for an entity system that handles component allocation
    * and deletion.
    * Uses an unordered map for component storage
    */
   template<typename T, template<class> class MemAllocPolicy = MemAllocPolicyNew>
   class DefaultEntitySystem
      : public EntitySystem
      , public MemAllocPolicy<T>
   {
   public:

      typedef ComponentStoreMap<T> ComponentStore;

      ////////////////////////////////////////////////////////////////////////////////
      DefaultEntitySystem(EntityManager& em, ComponentType baseType = StringId())
         : EntitySystem(em, baseType)
         , mComponentType(T::TYPE)
      {
      }

      ////////////////////////////////////////////////////////////////////////////////
      ~DefaultEntitySystem()
      {
         DestroyAll(mComponents);
      }

      virtual ComponentType GetComponentType() const
      {
         return mComponentType;
      }

      ////////////////////////////////////////////////////////////////////////////////
      virtual bool HasComponent(EntityId eid) const
      {
         typename ComponentStore::const_iterator i = mComponents.find(eid);
         return(i != mComponents.end());
      }

      ////////////////////////////////////////////////////////////////////////////////
      T* GetComponent(EntityId eid)
      {
         typename ComponentStore::iterator i = mComponents.find(eid);
         if(i != mComponents.end())
         {
            return i->second;
         }
         return NULL;
      }

      ////////////////////////////////////////////////////////////////////////////////
      virtual bool GetComponent(EntityId eid, Component*& c)
      {
         typename ComponentStore::iterator i = mComponents.find(eid);
         if(i != mComponents.end())
         {
            c = i->second;
            return true;
         }
         return false;
      }

      ////////////////////////////////////////////////////////////////////////////////
      virtual bool GetComponent(EntityId eid, const Component*& c) const
      {
         typename ComponentStore::const_iterator i = mComponents.find(eid);
         if(i != mComponents.end())
         {
            c = i->second;
            return true;
         }
         return false;
      }

      ////////////////////////////////////////////////////////////////////////////////
      virtual bool CreateComponent(EntityId eid, Component*& component)
      {
         if(HasComponent(eid))
         {
            LOG_ERROR("Could not create component: already exists!");
            return false;
         }

         T* t = MemAllocPolicy<T>::Create();

         if(t == NULL)
         {
            LOG_ERROR("Out of memory!");
            return false;
         }
         component = t;
         mComponents[eid] = t;

         Entity* e;
         bool found = GetEntityManager().GetEntity(eid, e);
         if(!found)
         {
            LOG_ERROR("Can't add component to entity: entity does not exist!");
            return false;
         }
         component->OnAddedToEntity(*e);
         return true;
      }

      ////////////////////////////////////////////////////////////////////////////////
      virtual bool DeleteComponent(EntityId eid)
      {
         typename ComponentStore::iterator i = mComponents.find(eid);
         if(i == mComponents.end())
            return false;
         T* component = i->second;
         Entity* e;
         bool found = GetEntityManager().GetEntity(eid, e);
         assert(found);
         component->OnRemovedFromEntity(*e);
         mComponents.erase(i);
         MemAllocPolicy<T>::Destroy(component);
         return true;
      }

      ////////////////////////////////////////////////////////////////////////////////
      virtual void GetEntitiesInSystem(std::list<EntityId>& toFill) const
      {
         typename ComponentStore::const_iterator i = mComponents.begin();
         for(;i != mComponents.end(); ++i)
         {
            toFill.push_back(i->first);
         }
      }

      ////////////////////////////////////////////////////////////////////////////////
      unsigned int GetNumComponents() const 
      {
         return mComponents.size();
      }

      ////////////////////////////////////////////////////////////////////////////////
      virtual DynamicPropertyContainer GetComponentProperties() const
      {
         ConstPropertyMap m;
         T t;
         t.GetProperties(m);
         DynamicPropertyContainer c;
         c.SetProperties(m);
         return c;
      }

      ////////////////////////////////////////////////////////////////////////////////
      typename ComponentStore::iterator begin()
      {
         return mComponents.begin();
      }

      ////////////////////////////////////////////////////////////////////////////////
      typename ComponentStore::const_iterator begin() const
      {
         return mComponents.begin();
      }

      ////////////////////////////////////////////////////////////////////////////////
      typename ComponentStore::iterator end()
      {
         return mComponents.end();
      }

      ////////////////////////////////////////////////////////////////////////////////
      typename ComponentStore::const_iterator end() const
      {
         return mComponents.end();
      }

   protected:

      ComponentStore mComponents;
      ComponentType mComponentType;
   };
}

