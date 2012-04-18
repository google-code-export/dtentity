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

#include <queue>
#include <OpenThreads/Mutex>
#include <OpenThreads/ScopedLock>

namespace dtEntity
{
   /**
    * A queue with thread safe push and pop methods
    */
   template<class T>
   class ThreadSafeQueue
   {
   public:

      void Push(T t);
      bool Empty();
      T    Pop();
      unsigned int GetSize();

   private:

      OpenThreads::Mutex mQueueMutex;
      std::queue<T> mQueue;
   };


   //////////////////////////////////////////


   template<class T>
   void ThreadSafeQueue<T>::Push(T t)
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mQueueMutex);
      mQueue.push(t);
   }

   template<class T>
   bool ThreadSafeQueue<T>::Empty()
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mQueueMutex);
      return mQueue.empty();
   }

   template<class T>
   T ThreadSafeQueue<T>::Pop()
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mQueueMutex);
      if(mQueue.empty()) return T();
      T t = mQueue.front();
      mQueue.pop();
      return t;
   }

   template<class T>
   unsigned int ThreadSafeQueue<T>::GetSize()
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mQueueMutex);
      return mQueue.size();
   }
}
