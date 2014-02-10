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
