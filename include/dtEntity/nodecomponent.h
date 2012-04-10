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

#include <osg/ref_ptr>
#include <dtEntity/export.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/component.h>
#include <dtEntity/property.h>
#include <dtEntity/stringid.h>
#include <osg/Node>

namespace dtEntity
{

   ///////////////////////////////////////////////////////////////////////////
   /**
    * Holds a single OSG node. Subclass this if you want to wrap an object
    * derived from osg::Node. NodeComponent instances can be attached to
    * the scene by the Layer system.
    */
   class DT_ENTITY_EXPORT NodeComponent : public Component
   {

   public:
      
      static const ComponentType TYPE;
      
      NodeComponent();
      NodeComponent(osg::Node* node);
      virtual ~NodeComponent();

      virtual ComponentType GetType() const { return TYPE; }

      virtual void OnAddedToEntity(Entity& entity);
      virtual void OnRemovedFromEntity(Entity& entity);

      /**
       * returns encapsulated node
       */
      virtual osg::Node* GetNode() const;

      /**
       * Sets encapsulated node. Also, if the currently held node
       * is attached as a child to another component then it is
       * removed from that parent and the passed node is added as a child
       */
      virtual void SetNode(osg::Node* node);
   
      /**
       * type of compponent that this node component is attached to.
       * Is 0 if not attached to a parent
       */
      ComponentType GetParentComponent() const { return mParentComponent; }
      void SetParentComponent(ComponentType c) { mParentComponent = c; }

      void SetNodeMask(unsigned int nodemask, bool recursive = false);
      unsigned int GetNodeMask() const;

   protected:

      Entity* mEntity;

   private:

      osg::ref_ptr<osg::Node> mNode;
      ComponentType mParentComponent;
      
   };

  
   ///////////////////////////////////////////////////////////////////////////
   // storage only
   class DT_ENTITY_EXPORT NodeSystem
      : public DefaultEntitySystem<NodeComponent>
   {
   public:
      NodeSystem(EntityManager& em)
         : DefaultEntitySystem<NodeComponent>(em)
      {

      }
   };}
