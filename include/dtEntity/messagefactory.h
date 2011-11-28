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
#include <dtEntity/objectfactory.h>
#include <dtEntity/message.h>

namespace dtEntity
{
   
   class DT_ENTITY_EXPORT MessageFactory
   {
   public:

      MessageFactory() {}

      /**
       * Register a message type with the message factory
       * The template parameter holds the message class to instantiate
       * Usage example:
       * mEntityManager->RegisterMessageType<TickMessage>(TickMessage::TYPE);
       * @TODO move somewhere else?
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
      * @TODO move somewhere else?
       */
      void GetRegisteredMessageTypes(std::vector<MessageType>& toFill);

      /**
       * Create a message of given type. A message class has to be registered with
       * RegisterMessageType before it can be instantiated with CreateMessage.
       * Please note that you can simply create messages with 'new', this is
       * just a factory for generic script- and tool-side access
      * * @TODO move somewhere else?
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
