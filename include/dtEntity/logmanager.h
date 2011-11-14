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

#include <dtEntity/singleton.h>
#include <osg/Referenced>
#include <osg/ref_ptr>
#include <string>
#include <vector>
#include <OpenThreads/Mutex>

namespace dtEntity
{
   ////////////////////////////////////////////////////////////////////////////////
   namespace LogLevel
   {
      enum e
      {
         DEBUG,
         INFO,
         WARNING,
         ERROR,
         ALWAYS
      };
   }

   ////////////////////////////////////////////////////////////////////////////////
   class LogListener
         : public osg::Referenced
   {
   public:
      virtual void LogMessage(LogLevel::e level, const std::string& filename, const std::string& methodname, int linenumber,
                      const std::string& msg) const = 0;

   protected:
      virtual ~LogListener() { }
   };

   ////////////////////////////////////////////////////////////////////////////////
   class LogManager
         : public dtEntity::Singleton<LogManager>
   {
   public:
      void LogMessage(LogLevel::e level, const std::string& filename, const std::string& methodname, int linenumber,
                      const std::string& msg) const;

      void AddListener(LogListener* l);
      void RemoveListener(LogListener* l);

      int GetNumListeners() const;
      LogListener* GetListener(int index) const;

   private:
      std::vector<osg::ref_ptr<LogListener> > mListeners;
      mutable OpenThreads::Mutex mMutex;
   };

}
