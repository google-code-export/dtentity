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
        
      }

      ////////////////////////////////////////////////////////////////////////////////
      static unsigned int Hash(const std::string& str)
      {
         const unsigned char* s = reinterpret_cast<const unsigned char*>(str.c_str());
         unsigned int hash;
         MurmurHash3_x86_32(s, str.size(), 0, &hash);
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
