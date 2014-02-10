#pragma once

/* -*-c++-*-
* dtEntity Game and Simulation Engine
*
* Copyright (c) 2013 Martin Scheffler
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the "Software"), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies 
* or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*/

#include <list>
#include <dtEntity/entityid.h>
#include <osg/ref_ptr>
#include <dtEntity/dynamiclibrary.h>
#include <dtEntity/stringid.h>

namespace dtEntity
{
   class EntityManager;
   class EntitySystem;


   /**
    * PluginFactory is a pure virtual interface for the values
    * returned by an entity system plugin entry point.
    * The plugin factories are stored in the plugin manager and are used to list available
    * entity systems, start and stop them.
    */
   class ComponentPluginFactory
   {
   public:

      ComponentPluginFactory()
         : mLibrary(NULL)
      {
      }
      
      virtual ~ComponentPluginFactory()
      {
          delete mLibrary;
      }

      /** get the name of the entity system (the type string) */
      virtual std::string GetName() const = 0;

      /**
       * Implementations should construct their entity system now and assign it to es variable
       * Return true if success, else false
       */
      virtual bool Create(EntityManager* em, EntitySystem*& es) = 0;

      /** delete the entity system */
      virtual void Destroy() {}
      
      /**
       * return a textual description of the entity system functionality
       */
      virtual std::string GetDescription() const { return ""; }

      /** 
       * fill list with types of all entity systems this system depends on.
       * WARNING: circular dependencies are not handled and
       * will cause a crash!
      */
      virtual void GetDependencies(std::list<ComponentType>&) {}

      dtEntity::DynamicLibrary* GetLibrary() const { return mLibrary; }
      void SetLibrary(dtEntity::DynamicLibrary* l) 
      {
          if(mLibrary)
          {
              delete mLibrary;
          }
          mLibrary = l; 
      }

   protected:

      dtEntity::DynamicLibrary* mLibrary;
   };

   template <class ES>
   class ComponentPluginFactoryImpl : public ComponentPluginFactory
   {
   public:

      ComponentPluginFactoryImpl(const std::string& name, const std::string& desc = "")
         : mName(name)
         , mDesc(desc)
      {
      }

      virtual bool Create(EntityManager* em, EntitySystem*& es)
      {
         es = new ES(*em);
         return true;
      }

      /** get the name of the plugin */
      virtual std::string GetName() const
      {
         return mName;
      }

      /** get a description of the plugin */
      virtual std::string GetDescription() const
      {
         return mDesc;
      }
   private:
      std::string mName;
      std::string mDesc;
   };
}
