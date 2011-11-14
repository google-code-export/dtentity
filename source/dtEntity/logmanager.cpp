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

#include <dtEntity/logmanager.h>


namespace dtEntity
{
   ////////////////////////////////////////////////////////////////////////////////
   void LogManager::LogMessage(LogLevel::e level, const std::string& filename, const std::string& methodname, int linenumber,
                   const std::string& msg) const
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mMutex);

      for(int i = 0; i < mListeners.size(); ++i)
      {
         mListeners[i]->LogMessage(level, filename, methodname, linenumber, msg);
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
      std::vector<osg::ref_ptr<LogListener> >::iterator i;
      for(i = mListeners.begin(); i != mListeners.end(); ++i)
      {
         if(*i == l)
         {
            mListeners.erase(i);
            return;
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   int LogManager::GetNumListeners() const
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mMutex);
      return mListeners.size();
   }

   ////////////////////////////////////////////////////////////////////////////////
   LogListener* LogManager::GetListener(int index) const
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mMutex);
      return mListeners[index];
   }
}
