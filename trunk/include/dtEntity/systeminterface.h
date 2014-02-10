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

#include <string>
#include <vector>

namespace dtEntity
{     
   #if defined(_MSC_VER)
       typedef __int64 Timer_t;
   #else
       typedef unsigned long long Timer_t;
   #endif

   class SystemInterface
   {
   public:

      virtual ~SystemInterface() {}

      /**
       * Send tick messages to main message pump, also emit
       * all queued messages
       */
      virtual void EmitTickMessagesAndQueuedMessages() = 0;

      /**
       * Send PostUpdateMessage to main message pump
       */
      virtual void EmitPostUpdateMessage() = 0;

      /**
       * Get simulation time delta since last tick in seconds
       */
      virtual float GetDeltaSimTime() const = 0;

      /**
       * Get real time delta since last tick in seconds
       */
      virtual float GetDeltaRealTime() const = 0;

      /**
       * Get scaling value from real time to simulation time
       */
      virtual float GetTimeScale() const = 0;

      /**
       * Set scaling value from real time to simulation time.
       * A value of 2 means simulation runs twice as fast as real time
       * Causes a TimeChangedMessage to be sent
       */
      virtual void SetTimeScale(float) = 0;

      /**
       * Get number of seconds since start of simulation
       */
      virtual double GetSimulationTime() const = 0;

      /**
       * Set number of seconds since start of simulation
       * Causes a TimeChangedMessage to be sent
       */
      virtual void SetSimulationTime(double) = 0;

      /**
       * Number of microseconds since 1.1.1970
       * Causes a TimeChangedMessage to be sent
       */
      virtual void SetSimulationClockTime(Timer_t) = 0;

      /**
       * Get number of microseconds since 1/1/1970
       */
      virtual Timer_t GetSimulationClockTime() const = 0;

      /**
       * Get current system time in number of microseconds since 1/1/1970
       */
      virtual Timer_t GetRealClockTime() const = 0;

      /**
       * Add data file path 
       */
      virtual void AddDataFilePath(const std::string& path) = 0;

      /**
       * Input is an absolute path. Return is a data file path list entry containing
       * that path or "" if it is not contained.
       */
      virtual std::string GetDataFilePathFromFilePath(const std::string& path) const = 0;

      /**
       * Get absolute path from a relative file path.
       * Returns empty string if file is not found
       */
      virtual std::string FindDataFile(const std::string& filename) const = 0;

      /**
       * Get absolute path from a library name.
       * Returns empty string if file is not found
       */
      virtual std::string FindLibraryFile(const std::string& filename) const = 0;

      /**
        * return true if a file with given absolute or relative path exists
        */
      virtual bool FileExists(const std::string& filename) const = 0;

      /**
       * Return a list of files in given system directory path.
       */
      typedef std::vector<std::string> DirectoryContents;
      virtual DirectoryContents GetDirectoryContents(const std::string& dirName) const = 0;

      /**
       * Add an entry to log. level is a value of dtEntity::LogLevel
       */
      virtual void LogMessage(unsigned int level, const std::string& filename, 
          const std::string& methodname, int linenumber, const std::string& msg) const = 0;

      /**
       * Return number of command line args used to start dtEntity 
      */
      virtual int GetArgC() = 0;

      /**
       * Return command line arg with given number (use std::string instead of raw c pointers
       */
      virtual std::vector<std::string> GetArgV() = 0;

   };
  
}
