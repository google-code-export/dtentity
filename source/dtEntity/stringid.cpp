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

#include <dtEntity/stringid.h>
#include <OpenThreads/ReadWriteMutex>
#include <dtEntity/hash.h>
#include <dtEntity/dtentity_config.h>
#include <dtEntity/singleton.h>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

namespace dtEntity
{

   class StringIdManager : public Singleton<StringIdManager>
   {

      typedef std::map<unsigned int, std::string> ReverseLookupMap;
      ReverseLookupMap mReverseLookup;
      OpenThreads::ReadWriteMutex mMutex;

   public:

      ////////////////////////////////////////////////////////////////////////////////
      StringIdManager()
      {
         std::ifstream indbstr("sids.txt");
         if(!indbstr.fail()) 
         {
           while(indbstr.good() )
           {
              std::string line;
              std::getline(indbstr, line);
              
              if(line.empty())
              {
                 continue;
              }

              std::string::size_type offset = line.find_first_of(' ');           
              std::string hashstr = line.substr(0, offset);
              std::stringstream ss(hashstr);
              unsigned int hash; 
              ss >> hash;
              std::string text = line.substr(offset + 1, line.length() - 1);
              mReverseLookup[hash] = text;
           }
         }
         mReverseLookup[0] = "";
      }

      ////////////////////////////////////////////////////////////////////////////////
      static unsigned int Hash(const std::string& str)
      {
         const unsigned char* s = reinterpret_cast<const unsigned char*>(str.c_str());
         unsigned int hash;
         MurmurHash3_x86_32(s, static_cast<int>(str.size()), 0, &hash);
         return hash;
      }

      ////////////////////////////////////////////////////////////////////////////////
      void AddToReverseLookup(const std::string& str, unsigned int hash)
      {
         {
            OpenThreads::ScopedReadLock rlock(mMutex);
            if(mReverseLookup.find(hash) != mReverseLookup.end())
            {
               return;
            }
         }
         OpenThreads::ScopedWriteLock wlock(mMutex);
         mReverseLookup[hash] = str;
      }

      ////////////////////////////////////////////////////////////////////////////////
      // Warning: slow! Does a linear search over all strings!
      std::string ReverseLookup(unsigned int hash)
      {
         OpenThreads::ScopedReadLock lock(mMutex);
         ReverseLookupMap::const_iterator i = mReverseLookup.find(hash);         
         if(i != mReverseLookup.end())
            return i->second;
         return "<String not found>";
      }
   };


   ////////////////////////////////////////////////////////////////////////////////
   StringId SID(const std::string& str)
   {
      unsigned int hash = StringIdManager::Hash(str);
      StringIdManager::GetInstance().AddToReverseLookup(str, hash);
#if DTENTITY_USE_STRINGS_AS_STRINGIDS
      return str;
#else
      return hash;
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string GetStringFromSID(StringId id)
   {
#if DTENTITY_USE_STRINGS_AS_STRINGIDS
      return id;
#else
      return StringIdManager::GetInstance().ReverseLookup(id);
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   StringId SIDHash(const std::string& str)
   {
#if DTENTITY_USE_STRINGS_AS_STRINGIDS
      return str;
#else
      unsigned int hash = StringIdManager::Hash(str);
      return hash;
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   StringId SID(unsigned int hash)
   {
#if DTENTITY_USE_STRINGS_AS_STRINGIDS
      return StringIdManager::GetInstance().ReverseLookup(hash);
#else
      return hash;
#endif      
   }

   ////////////////////////////////////////////////////////////////////////////////
   unsigned int SIDToUInt(StringId v)
   {
#if DTENTITY_USE_STRINGS_AS_STRINGIDS
      return StringIdManager::Hash(v);
#else
      return v;
#endif
   }
}
