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

#include <dtEntity/export.h>
#include <dtEntity/propertycontainer.h>
#include <assert.h>
#include <dtEntity/FastDelegate.h>
#include <dtEntity/FastDelegateBind.h>

namespace dtEntity
{

   /**
    * Pure virtual Message interface class. Holds a number of properties
    */
   class DT_ENTITY_EXPORT Message
      : public PropertyContainer
   {
      
   public:
    
      /**
       * Default Constructor.
       */
      Message()
         : mType(MessageType())
      {
      }

      /**
       * Constructor.
       * @param type The unique class ID of the message class
       */
      Message(MessageType type)
         : mType(type)
      {
         assert(mType != dtEntity::StringId() && "No message type set!");
      }

      // DTor
      virtual ~Message() 
      {
      }

      /**
       * @return class type id of message
       */
      MessageType GetType() const { return mType; }

      /**
       * Create a copy of the message on the heap.
       * Example generic implementation:
       * virtual Message* Clone() const { return CloneContainer<MyMessageClass>(); }
       */
      virtual Message* Clone() const {
         assert(mType != StringId() && "Cannot clone a message without type!");
         return CloneContainer<Message>();
      }

   private:

      // no copy CTor
      Message(const Message& other);

      MessageType mType;
   };

   // Functors of this type can be registered to receive messages of a specific type.
   typedef fastdelegate::FastDelegate1< const Message&, void> MessageFunctor;

}
