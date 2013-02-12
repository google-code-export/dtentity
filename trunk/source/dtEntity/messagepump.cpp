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

#include <dtEntity/messagepump.h>

#include <dtEntity/dtentity_config.h>
#include <dtEntity/log.h>

#if DTENTITY_PROFILING_ENABLED
#include <dtEntity/profile.h>
#endif

namespace dtEntity
{

   MessagePump::MessagePump() 
   {     
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   MessagePump::~MessagePump() 
   {
      ClearQueue();
   }
  
   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   void MessagePump::RegisterForMessages(MessageType msgtype, MessageFunctor ftr, unsigned int options, const std::string& funcname)
   {
      MsgRegistryEntry e;
      e.mOptions = options;
      e.mFunctor = ftr;
      e.mFuncName = funcname.empty() ? msgtype : dtEntity::SID(funcname);
     
      unsigned int priority = options & 3;
     
      std::pair<MessageFunctorRegistry::iterator, MessageFunctorRegistry::iterator> keyRange;
      keyRange = mMessageFunctors.equal_range(msgtype);

      MessageFunctorRegistry::iterator it = keyRange.first;
      while(it != keyRange.second)
      {
         unsigned int otherpriority = it->second.mOptions & 3;

         if(it->second.mFunctor == ftr && ((it->second.mOptions & FilterOptions::UNREGISTERED) == 0))
         {
           LOG_ERROR("Trying to register a functor twice for same message: " << GetStringFromSID(msgtype));
         }

         if(otherpriority <= priority)
         {
            break;
         }

         ++it;
      }
      mMessageFunctors.insert(it, std::make_pair(msgtype, e));
      assert(IsRegistered(msgtype, ftr));
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   bool MessagePump::UnregisterForMessages(MessageType msgtype, MessageFunctor& ftr)
   {
      std::pair<MessageFunctorRegistry::iterator, MessageFunctorRegistry::iterator> keyRange;
      keyRange = mMessageFunctors.equal_range(msgtype);
      MessageFunctorRegistry::iterator it = keyRange.first;
      for(; it != keyRange.second; ++it)
      {
         if(it->second.mFunctor == ftr)
         {
            it->second.mOptions |= FilterOptions::UNREGISTERED;
            return true;
         }
      }
      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   bool MessagePump::IsRegistered(MessageType msgtype, const MessageFunctor& ftr)
   {
      std::pair<MessageFunctorRegistry::iterator, MessageFunctorRegistry::iterator> keyRange;
      keyRange = mMessageFunctors.equal_range(msgtype);
      MessageFunctorRegistry::iterator it = keyRange.first;
      for(; it != keyRange.second; ++it)
      {
         if(it->second.mFunctor == ftr)
         {
            return true;
         }
      }
      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   void MessagePump::EmitMessage(const Message& msg)
   {
      dtEntity::MessageType messageType = msg.GetType();
      if(messageType == MessageType())
      {
         LOG_ERROR("Trying to send a message with an empty type string!");
         return;
      }
      std::pair<MessageFunctorRegistry::iterator, MessageFunctorRegistry::iterator> keyRange;
      keyRange = mMessageFunctors.equal_range(messageType);
      
      // cannot call functors directly in loop because functors may call the
      // RegisterForMessage function and invalidate the iterator.
      typedef std::list<std::pair<MessageFunctor, StringId> > FunctorsToCall;
      FunctorsToCall functorsToCall;
      

      MessageFunctorRegistry::iterator it = keyRange.first;
      while(it != keyRange.second)
      {
         MsgRegistryEntry entry = it->second;
         unsigned int regoptions = entry.mOptions;

         if((regoptions & FilterOptions::UNREGISTERED) != 0)
         {
            mMessageFunctors.erase(it++);
            continue;         
         }

         assert(messageType == it->first && "Something went wrong with message registration!");
         
         functorsToCall.push_back(std::make_pair(entry.mFunctor, entry.mFuncName));

         if((regoptions & FilterOptions::SINGLE_SHOT) != 0)
         {
            mMessageFunctors.erase(it++);
            continue;    
         }
         ++it;
      }

      FunctorsToCall::iterator j;
      for(j = functorsToCall.begin(); j != functorsToCall.end(); ++j)
      {
#if DTENTITY_PROFILING_ENABLED
         CProfileManager::Start_Profile(j->second);
#endif
         (j->first)(msg);
#if DTENTITY_PROFILING_ENABLED
         CProfileManager::Stop_Profile();
#endif
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MessagePump::EnqueueMessage(const Message& msg)
   {
      mMessageQueue.Push(msg.Clone());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MessagePump::EnqueueMessage(const Message& msg, double when)
   {
      if(when <= 0.001)
      {
         mMessageQueue.Push(msg.Clone());
      }
      else
      {
         FutureMessageEntry entry;
         entry.mMessage = msg.Clone();
         entry.mTimeToPost = when;
         mFutureMessageQueue.Push(entry);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MessagePump::EmitQueuedMessages(double now)
   {
      
      while(!mMessageQueue.Empty())
      {
         const Message* entry = mMessageQueue.Pop();
         EmitMessage(*entry);
         delete entry;
      }

      while(!mFutureMessageQueue.Empty())
      {
         FutureMessageEntry entry = mFutureMessageQueue.Pop();
         mFutureMessages.push_back(entry);
      }

      if(!mFutureMessages.empty())
      {
         std::list<FutureMessageEntry>::iterator i = mFutureMessages.begin();
         while(i != mFutureMessages.end())
         {         
            FutureMessageEntry entry = *i;
            
            if(entry.mTimeToPost <= now)
            {
               EmitMessage(*entry.mMessage);
               delete entry.mMessage;
               i = mFutureMessages.erase(i);               
            }
            else
            {
               ++i;
            }
         }
      }  
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MessagePump::ClearQueue()
   {
      while(!mMessageQueue.Empty())
      {
         delete mMessageQueue.Pop();
      }
      while(!mFutureMessageQueue.Empty())
      {
         delete mFutureMessageQueue.Pop().mMessage;
      }
      for(std::list<FutureMessageEntry>::iterator i = mFutureMessages.begin();
          i != mFutureMessages.end(); ++i)
      {
         delete i->mMessage;
      }
      mFutureMessages.clear();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MessagePump::UnregisterAll()
   {
      mMessageFunctors.clear();
   }
}

