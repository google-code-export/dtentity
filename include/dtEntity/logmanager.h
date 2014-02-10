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
