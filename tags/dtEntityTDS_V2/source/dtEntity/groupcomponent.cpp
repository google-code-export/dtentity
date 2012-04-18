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

#include <dtEntity/groupcomponent.h>


namespace dtEntity
{


   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   const StringId GroupComponent::TYPE(SID("Group"));   
   const StringId GroupComponent::ChildrenId(SID("Children"));
   
   ////////////////////////////////////////////////////////////////////////////
   GroupComponent::GroupComponent()
      : BaseClass(new osg::Group())
   {
      GetNode()->setName("GroupComponent");
      Register(ChildrenId, &mChildren);
   }

   ////////////////////////////////////////////////////////////////////////////
   GroupComponent::GroupComponent(osg::Group* group)
      : BaseClass(group)
   {
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
   bool GroupComponent::AddChildComponent(ComponentType ct)
   {
      Component* component;
      if(!mEntity->GetComponent(ct, component))
      {
         return false;
      }
      NodeComponent* nc = static_cast<NodeComponent*>(component);
      GetAttachmentGroup()->addChild(nc->GetNode());
      nc->SetParentComponent(this->GetType());
      mChildren.Add(new StringIdProperty(ct));
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool GroupComponent::RemoveChildComponent(ComponentType ct)
   {
      Component* component;
      if(!mEntity->GetComponent(ct, component))
      {
         return false;
      }
      NodeComponent* nc = static_cast<NodeComponent*>(component);
      GetAttachmentGroup()->removeChild(nc->GetNode());
      nc->SetParentComponent(StringId());

      PropertyArray children = mChildren.Get();
      for(PropertyArray::iterator i = children.begin(); i != children.end(); ++i)
      {
         if((*i)->StringIdValue() == ct)
         {
            mChildren.Remove(*i);
            return true;
         }
      }
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroupComponent::SetChildren(const PropertyArray& arr)
   {
      mChildren.Set(arr);
      this->OnPropertyChanged(ChildrenId, mChildren);
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroupComponent::Finished()
   {
      BaseClass::Finished();
      assert(mEntity != NULL && "Please add group component to entity before adding children!");

      GetAttachmentGroup()->removeChild(0, GetAttachmentGroup()->getNumChildren());

      PropertyArray arr = mChildren.Get();
      PropertyArray::const_iterator it;
      for(it = arr.begin(); it != arr.end(); ++it)
      {
         Property* prop = *it;
         StringId componentType = prop->StringIdValue();

         Component* c;
         bool found = mEntity->GetComponent(componentType, c);
         if(!found)
         {
            LOG_WARNING("Could not attach component to group, not found: "
               + GetStringFromSID(componentType));
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
