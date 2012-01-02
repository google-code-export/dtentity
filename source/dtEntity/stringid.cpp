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
#include <dtEntity/crc32.h>
#include <map>

namespace dtEntity
{

   class DT_ENTITY_EXPORT StringIdManager
      : public Singleton<StringIdManager>
   {

      typedef std::map<unsigned int, std::string> ReverseLookupMap;
      ReverseLookupMap mReverseLookup;
      OpenThreads::ReadWriteMutex mMutex;

   public:

      ////////////////////////////////////////////////////////////////////////////////
      unsigned int Hash(const std::string& str)
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
      unsigned int hash = StringIdManager::GetInstance().Hash(str);
      StringIdManager::GetInstance().AddToReverseLookup(str, hash);
#if defined(DTENTITY_USE_STRINGS_AS_STRINGIDS)
      return str;
#else
      return hash;
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string GetStringFromSID(StringId id)
   {
#if defined(DTENTITY_USE_STRINGS_AS_STRINGIDS)
      return id;
#else
      return StringIdManager::GetInstance().ReverseLookup(id);
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   StringId SIDHash(const std::string& str)
   {
#if defined(DTENTITY_USE_STRINGS_AS_STRINGIDS)
      return str;
#else
      unsigned int hash = StringIdManager::GetInstance().Hash(str);
      return hash;
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   StringId SID(unsigned int hash)
   {
#if defined(DTENTITY_USE_STRINGS_AS_STRINGIDS)
      return StringIdManager::GetInstance().ReverseLookup(hash);
#else
      return hash;
#endif      
   }
}
