/*
* Delta3D Open Source Game and Simulation Engine
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

#include <dtEntity/entitymanagerbase.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/profile.h>

#include <dtEntity/basemessages.h>
#include <dtEntity/log.h>
#include <dtCore/system.h>

namespace dtEntity
{
 
   ///////////////////////////////////////////////////////////////////////////////////////////////////////  
   IMPLEMENT_MANAGEMENT_LAYER(EntityManagerBase);

   ///////////////////////////////////////////////////////////////////////////////////////////////////////  
   EntityManagerBase::EntityManagerBase()
      : mEntityManager(new EntityManager())
   {
      AddSender(&dtCore::System::GetInstance());
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////  
   EntityManagerBase::~EntityManagerBase()
   {
      
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////  
   EntityManager& EntityManagerBase::GetEntityManager() const
   {
      return *mEntityManager;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PopulateTickMessage(Message& msg, float dsimtime, float drealtime)
   {
      msg.SetFloat(TickMessage::DeltaSimTimeId, dsimtime);
      msg.SetFloat(TickMessage::DeltaRealTimeId, drealtime);

      double simulationTime = dtCore::System::GetInstance().GetSimulationTime();
      msg.SetDouble(TickMessage::SimulationTimeId, simulationTime);
      msg.SetFloat(TickMessage::SimTimeScaleId, dtCore::System::GetInstance().GetTimeScale());
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////  
   void EntityManagerBase::LockTraversalMutex()
   {
      mTraversalMutex.lock();
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////  
   void EntityManagerBase::UnlockTraversalMutex()
   {
      mTraversalMutex.unlock();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EntityManagerBase::OnMessage(MessageData* data)
   {
      mTraversalMutex.lock();

     /* try
      {

         if(data->message == dtCore::System::MESSAGE_POST_EVENT_TRAVERSAL)
         {
            double* timeChange = (double*)data->userData;
            PostEventTraversalMessage msg;
            PopulateTickMessage(msg, timeChange[0], timeChange[1]);
            mEntityManager->EmitMessage(msg);
         }
         else if (data->message == dtCore::System::MESSAGE_PRE_FRAME)
         {
            double* timeChange = (double*)data->userData;
            TickMessage msg;
            PopulateTickMessage(msg, timeChange[0], timeChange[1]);

            // first send queued messages, may come from network
            mEntityManager->EmitQueuedMessages(msg.GetSimulationTime());

            // then update the systems
            mEntityManager->EmitMessage(msg);


            EndOfFrameMessage eofmsg;
            PopulateTickMessage(eofmsg, timeChange[0], timeChange[1]);
            mEntityManager->EmitMessage(eofmsg);
         }
         else if (data->message == dtCore::System::MESSAGE_FRAME_SYNCH)
         {
            double* timeChange = (double*)data->userData;
            FrameSynchMessage msg;
            PopulateTickMessage(msg, timeChange[0], timeChange[1]);
            mEntityManager->EmitMessage(msg);

         }
         else if (data->message == dtCore::System::MESSAGE_POST_FRAME)
         {
            double* timeChange = (double*)data->userData;
            PostFrameMessage msg;
            PopulateTickMessage(msg, timeChange[0], timeChange[1]);
            mEntityManager->EmitMessage(msg);
         }
      }
      catch(const std::exception& e)
      {
         LOG_ERROR("Exception caught: " + std::string(e.what()));
      }
      catch(...)
      {
         LOG_ERROR("Unknown exception caught!");
      }*/

      mTraversalMutex.unlock();

      /*else if (data->message == dtCore::System::MESSAGE_PAUSE_START)
      {
         SendMessage(*GetMessageFactory().CreateMessage(MessageType::INFO_PAUSED));
      }
      else if (data->message == dtCore::System::MESSAGE_PAUSE_END)
      {
         SendMessage(*GetMessageFactory().CreateMessage(MessageType::INFO_RESUMED));
      }
      else if (data->message == dtCore::System::MESSAGE_PAUSE)
      {
         double* timeChange = (double*)data->userData;
         PreFrame(0.0, *timeChange);
      }
*/
   }
}

