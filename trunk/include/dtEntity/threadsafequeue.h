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
