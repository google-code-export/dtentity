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

#include <dtEntity/logmanager.h>


namespace dtEntity
{
   ////////////////////////////////////////////////////////////////////////////////
   void LogManager::LogMessage(LogLevel::e level, const std::string& filename, const std::string& methodname, int linenumber,
                   const std::string& msg) const
   {
      Listeners listeners;
      {
         OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mMutex);
         listeners = mListeners;
      }
      for(Listeners::iterator i = listeners.begin(); i != listeners.end(); ++i)
      {
         (*i)->LogMessage(level, filename, methodname, linenumber, msg);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LogManager::AddListener(LogListener* l)
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mMutex);
      mListeners.push_back(l);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LogManager::RemoveListener(LogListener* l)
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mMutex);

      for(Listeners::iterator i = mListeners.begin(); i != mListeners.end(); ++i)
      {
         if(*i == l)
         {
            mListeners.erase(i);
            return;
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   LogManager::Listeners::size_type LogManager::GetNumListeners() const
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mMutex);
      return mListeners.size();
   }

   ////////////////////////////////////////////////////////////////////////////////
   LogListener* LogManager::GetListener(LogManager::Listeners::size_type index) const
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mMutex);
      return mListeners[index];
   }
}
