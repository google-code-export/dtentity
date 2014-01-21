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
#include <dtEntity/singleton.h>
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
         LVL_ALWAYS,
         LVL_ERROR,
         LVL_DEBUG,
         LVL_WARNING,
         LVL_INFO         
      };
   }

   ////////////////////////////////////////////////////////////////////////////////
   class LogListener
         : public osg::Referenced
   {
   public:
      virtual void LogMessage(LogLevel::e level, const std::string& filename, const std::string& methodname, int linenumber,
                      const std::string& msg) = 0;

   protected:
      virtual ~LogListener() { }
   };

   ////////////////////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT LogManager
         : public dtEntity::Singleton<LogManager>
   {
   public:

      typedef std::vector<osg::ref_ptr<LogListener> > Listeners;

      void LogMessage(LogLevel::e level, const std::string& filename, const std::string& methodname, int linenumber,
                      const std::string& msg) const;

      void AddListener(LogListener* l);
      void RemoveListener(LogListener* l);

      Listeners::size_type GetNumListeners() const;
      LogListener* GetListener(Listeners::size_type index) const;

   private:
      Listeners mListeners;
      mutable OpenThreads::Mutex mMutex;
   };

}
