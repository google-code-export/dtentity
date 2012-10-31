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

#include <dtEntityOSG/groupcomponent.h>
#include <osg/Group>

namespace dtEntityOSG
{


   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId GroupComponent::TYPE(dtEntity::SID("Group"));
   const dtEntity::StringId GroupComponent::ChildrenId(dtEntity::SID("Children"));
   
   ////////////////////////////////////////////////////////////////////////////
   GroupComponent::GroupComponent()
      : BaseClass(new osg::Group())
   {
      // keep this constructor empty to prevent redundancy
      Init();
   }

   ////////////////////////////////////////////////////////////////////////////
   GroupComponent::GroupComponent(osg::Group* group)
      : BaseClass(group)
   {
      // keep this constructor empty to prevent redundancy
       Init();
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroupComponent::Init()
   {
      mChildren = dtEntity::DynamicArrayProperty(
           dtEntity::DynamicArrayProperty::SetValueCB(this, &GroupComponent::SetChildren),
           dtEntity::DynamicArrayProperty::GetValueCB(this, &GroupComponent::GetChildren)
        );
   
      GetNode()->setName("GroupComponent");
      Register(ChildrenId, &mChildren);
   }

   ////////////////////////////////////////////////////////////////////////////
   GroupComponent::~GroupComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Group* GroupComponent::GetGroup() const
   {
      return static_cast<osg::Group*>(BaseClass::GetNode());
   }

   ////////////////////////////////////////////////////////////////////////////
   bool GroupComponent::AddChildComponent(dtEntity::ComponentType ct)
   {
      dtEntity::Component* component;
      if(!mEntity->GetComponent(ct, component))
      {
         return false;
      }
      NodeComponent* nc = static_cast<NodeComponent*>(component);
      GetAttachmentGroup()->addChild(nc->GetNode());
      nc->SetParentComponent(this->GetType());
      mChildrenVal.Add(new dtEntity::StringIdProperty(ct));
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool GroupComponent::RemoveChildComponent(dtEntity::ComponentType ct)
   {
      dtEntity::Component* component;
      if(!mEntity->GetComponent(ct, component))
      {
         return false;
      }
      NodeComponent* nc = static_cast<NodeComponent*>(component);
      GetAttachmentGroup()->removeChild(nc->GetNode());
      nc->SetParentComponent(dtEntity::StringId());

      for(dtEntity::PropertyArray::const_iterator i = mChildrenVal.Get().begin(); i != mChildrenVal.Get().end(); ++i)
      {
         if((*i)->StringIdValue() == ct)
         {
            mChildrenVal.Remove(*i);
            return true;
         }
      }
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroupComponent::SetChildren(const dtEntity::PropertyArray& arr)
   {
      mChildrenVal.Set(arr);
      
      assert(mEntity != NULL && "Please add group component to entity before adding children!");

      GetAttachmentGroup()->removeChild(0, GetAttachmentGroup()->getNumChildren());

      dtEntity::PropertyArray::const_iterator it;
      for(it = arr.begin(); it != arr.end(); ++it)
      {
         Property* prop = *it;
         dtEntity::StringId componentType = prop->StringIdValue();

         Component* c;
         bool found = mEntity->GetComponent(componentType, c);
         if(!found)
         {
            LOG_WARNING("Could not attach component to group, not found: "
               + dtEntity::GetStringFromSID(componentType));
         }
         else
         {
            NodeComponent* nc = dynamic_cast<NodeComponent*>(c);

            if(nc)
            {
               GetAttachmentGroup()->addChild(nc->GetNode());
               nc->SetParentComponent(this->GetType());
            }
            else
            {
               LOG_ERROR("Cannot attach as child: Not a node component!");
            }
         }
      }
      GetNode()->dirtyBound();
   }

}
