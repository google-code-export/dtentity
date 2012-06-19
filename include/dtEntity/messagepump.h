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

#include <dtEntity/entityid.h>
#include <dtEntity/export.h>
#include <dtEntity/message.h>
#include <dtEntity/threadsafequeue.h>
#include <map>
#include <list>

namespace dtEntity
{

   
   
   ////////////////////////////////////////////////////////////////////////////////


   namespace FilterOptions
   {
       /* These bits define the way a functor is registered to a message.
       * When multiple functors are registered to a message type then they are called
       * in the order of their PRIORITY (highest priority first).
       * The UNREGISTERED bit has to be set to false. It is used internally to mark
       * deregistered functors for removal.
       *
       * If SINGLE_SHOT is set then the functor is unregistered after it is called the first time.
       * Registrants can register for local messages, network messages, or both.
       * Message senders can send a message to local or network or both.
       */
      enum e
      {
         PRIORITY_LOWEST                 = 0,
         PRIORITY_DEFAULT                = 1,
         PRIORITY_HIGHER                 = 2,
         PRIORITY_HIGHEST                = 3,
         ORDER_LATE                      = 0,
         ORDER_DEFAULT                   = 1,
         ORDER_EARLY                     = 2,
         ORDER_EARLIEST                  = 3,
         UNREGISTERED                    = (1<<3),    
         SINGLE_SHOT                     = (1<<4),
         
         DEFAULT  = ORDER_DEFAULT
      };
   }


   ////////////////////////////////////////////////////////////////////////////////


   /*
    * Internal data structure for message registration
    */
   struct MsgRegistryEntry
   {
      unsigned int mOptions;
      MessageFunctor mFunctor;    
      StringId mFuncName;
   };

   struct FutureMessageEntry
   {
      double mTimeToPost;
      Message* mMessage;
   };


   ////////////////////////////////////////////////////////////////////////////////


   class DT_ENTITY_EXPORT MessagePump
   {
   public:

      // thread safe message queue. The int stores the sending options.
      typedef ThreadSafeQueue<const Message*> MessageQueue;
      typedef ThreadSafeQueue<FutureMessageEntry> FutureMessageQueue;

      /**
       * CTor
       */
      MessagePump();

      /**
       * DTor
       */
      virtual ~MessagePump();

      /**
      * Register a functor to be called when a message of given type is emitted.
      * @param msgtype Type of message to register to
      * @param ftr This functor is called when a message of msgtype is emitted
      * @param options Option bits for registration (see FilterOptions comments)
      * @param funcname name for debugging and profiling purposes
      *
      */
      virtual void RegisterForMessages(MessageType msgtype, MessageFunctor ftr, unsigned int options = FilterOptions::DEFAULT, const std::string& funcname = "");

      /**
      * Unregister functor from message
      * @param msgtype Type of message to unregister from
      * @param ftr Unregister this message
      * @return true if success
      */
      virtual bool UnregisterForMessages(MessageType msgtype, MessageFunctor& ftr);

      /**
      * returns true if functor is registered to message
      */
      virtual bool IsRegistered(MessageType msgtype, const MessageFunctor& ftr);

      /**
      * Immediately send a message to all registered functors
      * Warning: beware of circular dependencies!
      * @param msg The message to send
      */
      void EmitMessage(const Message& msg);

      /**
      * Enqueue message and emit it on next PreFrame event
      * @param when Future simulation time that event should be emitted at 
      * ( 0 means emit immediately) 
      */
      void EnqueueMessage(const Message& msg);
      void EnqueueMessage(const Message& msg, double when);

      /**
      * Emit all messages in message queue
      */
      void EmitQueuedMessages(double simtime);

      /**
       * Discard all queued messages
       */
      void ClearQueue();

      /**
       * Unregister all functors
       */
      void UnregisterAll();

   protected:

      // Registry for message functors
      typedef std::multimap<MessageType, MsgRegistryEntry> MessageFunctorRegistry;
      MessageFunctorRegistry mMessageFunctors;


      // stores messages til next tick
      MessageQueue mMessageQueue;

      FutureMessageQueue mFutureMessageQueue;
      // stores messages until their time to post has come
      std::list<FutureMessageEntry> mFutureMessages;

   };
}
