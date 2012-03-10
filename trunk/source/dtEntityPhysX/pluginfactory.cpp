/* -*-c++-*-
* testPhysX - testPhysX(.h & .cpp) - Using 'The MIT License'
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
* Martin Scheffler
*/

#include <dtEntity/componentpluginmanager.h>
#include <dtEntityPhysX/physxcomponent.h>
#include <dtEntityPhysX/physxpagedterraincomponent.h>
#include <dtEntityPhysX/physxgroundclampingcomponent.h>

class PhysXFactory : public dtEntity::ComponentPluginFactory
{
public:

   virtual bool Create(dtEntity::EntityManager* em, dtEntity::EntitySystem*& es)
   {
      es = new dtEntityPhysX::PhysXSystem(*em);
      return true;
   }

   /** delete the entity system */
   virtual void Destroy() 
   {
   }

   /** get the name of the plugin */
   virtual std::string GetName() const 
   {
      return "PhysX";
   }

   virtual dtEntity::ComponentType GetType() const 
   {
      return dtEntityPhysX::PhysXComponent::TYPE;
   }

   /** get a description of the plugin */
   virtual std::string GetDescription() const 
   {
      return "DtEntity component for nVidia PhysX";
   }

   virtual void GetDependencies(std::list<dtEntity::ComponentType>& l) 
   {
      l.push_back(dtEntity::SID("MatrixTransform"));
      l.push_back(dtEntity::SID("StaticMesh"));
   };
};

////////////////////////////////////////////////////////////////////////////////

class PagedTerrainFactory : public dtEntity::ComponentPluginFactory
{
public:

   virtual bool Create(dtEntity::EntityManager* em, dtEntity::EntitySystem*& es)
   {
      es = new dtEntityPhysX::PhysXPagedTerrainSystem(*em);
      return true;
   }

   /** delete the entity system */
   virtual void Destroy() 
   {
   }

   /** get the name of the plugin */
   virtual std::string GetName() const 
   {
      return "PhysX Paged Terrain";
   }

   virtual dtEntity::ComponentType GetType() const 
   {
      return dtEntityPhysX::PhysXPagedTerrainComponent::TYPE;
   }

   /** get a description of the plugin */
   virtual std::string GetDescription() const 
   {
      return "Paged Terrain component for nVidia PhysX";
   }

   virtual void GetDependencies(std::list<dtEntity::ComponentType>& l) 
   {
      l.push_back(dtEntityPhysX::PhysXComponent::TYPE);
   };
};

////////////////////////////////////////////////////////////////////////////////

class GroundClampingFactory : public dtEntity::ComponentPluginFactory
{
public:

   virtual bool Create(dtEntity::EntityManager* em, dtEntity::EntitySystem*& es)
   {
      es = new dtEntityPhysX::PhysXGroundClampingSystem(*em);
      return true;
   }

   /** delete the entity system */
   virtual void Destroy() 
   {
   }

   /** get the name of the plugin */
   virtual std::string GetName() const 
   {
      return "PhysX Ground Clamping";
   }

   virtual dtEntity::ComponentType GetType() const 
   {
      return dtEntityPhysX::PhysXGroundClampingComponent::TYPE;
   }

   /** get a description of the plugin */
   virtual std::string GetDescription() const 
   {
      return "PhysX Ground Clamping";
   }

   virtual void GetDependencies(std::list<dtEntity::ComponentType>& l) 
   {
      l.push_back(dtEntityPhysX::PhysXComponent::TYPE);
      l.push_back(dtEntityPhysX::PhysXPagedTerrainComponent::TYPE);
   };
};

////////////////////////////////////////////////////////////////////////////////
extern "C" DTE_EXPORT_MACRO void dtEntityMessages_dtEntityPhysX(dtEntity::MessageFactory& mf)
{
}

REGISTER_DTENTITYPLUGIN(dtEntityPhysX, 3,
   new PhysXFactory(),
   new PagedTerrainFactory(),
   new GroundClampingFactory()
)
