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
#include <dtEntity/objectfactory.h>
#include <dtEntity/message.h>
#include <dtEntity/singleton.h>

namespace dtEntity
{
   
   class DT_ENTITY_EXPORT MessageFactory
      : public Singleton<MessageFactory>
   {
   public:

      MessageFactory() {}

      /**
       * Register a message type with the message factory
       * The template parameter holds the message class to instantiate
       * Usage example:
       * mMessageFactory->RegisterMessageType<TickMessage>(TickMessage::TYPE);
       * @param mtype MessageType id of message
       */
      ////////////////////////////////////////////////////////////////////////////////
      template <typename T>
      void RegisterMessageType(MessageType mtype)
      {
         mMessageFactory.template RegisterType<T>(mtype);
      }

      /**
       * fill up a vector with the types of all registered messages
       */
      void GetRegisteredMessageTypes(std::vector<MessageType>& toFill);

      /**
       * Create a message of given type. A message class has to be registered with
       * RegisterMessageType before it can be instantiated with CreateMessage.
       * Please note that you can simply create messages with 'new', this is
       * just a factory for generic script- and tool-side access
       * @param msgType type id of message class
       * @param msg Receives newly created message. Ownership of the message object
       *        is transfered to caller - post your message or delete it!
       * @return true if success - false if message type is not registered
       */
      bool CreateMessage(MessageType msgType, Message*& msg) const;

      // not implemented
      MessageFactory(const MessageFactory& other);

   private:
      // factory for message objects
      ObjectFactory<MessageType, Message> mMessageFactory;

   };
}
