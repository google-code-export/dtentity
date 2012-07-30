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

#include <dtEntityOSG/export.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/dynamicproperty.h>
#include <dtEntityOSG/nodecomponent.h>
#include <dtEntity/property.h>
#include <dtEntity/stringid.h>

namespace osg
{
   class Group;
}

namespace dtEntityOSG
{
   /**
    * Holds a single OSG group.
    */
   class DTENTITY_OSG_EXPORT GroupComponent : public NodeComponent
   {
   public:

      typedef NodeComponent BaseClass;

      static const dtEntity::ComponentType TYPE;
      static const dtEntity::StringId ChildrenId;
   
      /**
       * CTor, creates a new group component
       */
      GroupComponent();

      /**
       * CTor, uses passed group component
       */
      GroupComponent(osg::Group* group);

      virtual ~GroupComponent();

      virtual dtEntity::ComponentType GetType() const
      { 
         return TYPE; 
      }

      virtual osg::Group* GetGroup() const;
      
      // return group to attach children to
      virtual osg::Group* GetAttachmentGroup() const { return GetGroup(); }

      bool AddChildComponent(dtEntity::ComponentType c);
      bool RemoveChildComponent(dtEntity::ComponentType c);

      /**
       * Expects an array of stringid properties containing component types
       * of children. These will be attached.
       */
      void SetChildren(const dtEntity::PropertyArray& arr);
      dtEntity::PropertyArray GetChildren() const { return mChildrenVal.Get(); }

   private:

      // to prevent redundant code in constructors
      void Init();

      dtEntity::DynamicArrayProperty mChildren;
      dtEntity::ArrayProperty mChildrenVal;
   };


   ///////////////////////////////////////////////////////////////////////////
   // storage only
   class DTENTITY_OSG_EXPORT GroupSystem
      : public dtEntity::DefaultEntitySystem<GroupComponent>
   {
   public:
      GroupSystem(dtEntity::EntityManager& em)
         : dtEntity::DefaultEntitySystem<GroupComponent>(em, NodeComponent::TYPE)
      {

      }
   };
   

}
