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

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <CEGUI/CEGUIExceptions.h>
#include <dtEntity/core.h>
#include <dtEntity/systeminterface.h>
#include <dtEntityCEGUI/resourceprovider.h>
#include <osgDB/FileNameUtils>
#include <dtEntity/log.h>


namespace dtEntityCEGUI
{

   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   ResourceProvider::ResourceProvider()
      : CEGUI::DefaultResourceProvider()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   /** This will load the file with the supplied filename using the search paths
    *  find the files.
    *
    * @see SetDataFilePathList()
    */
   void ResourceProvider::loadRawDataContainer(const CEGUI::String& filename, CEGUI::RawDataContainer& output, const CEGUI::String& resourceGroup)
   {
      std::string strFilename(filename.c_str());
      std::string strResGroup(resourceGroup.c_str());

      ResourceGroupMap::const_iterator iter;
      if(!d_resourceGroups.empty())
      {      
         iter = d_resourceGroups.find(resourceGroup);      
      }
      else
      {
      // Look up resource group directory
         iter = d_resourceGroups.end();
      }

      // If there was no entry for this group, then find the full path name
      bool found = false;
      if(iter != d_resourceGroups.end())
      {
         try
         {
            //pass to base class for reading
            CEGUI::DefaultResourceProvider::loadRawDataContainer(filename, output, resourceGroup );
            found = true;
         }
         catch(CEGUI::Exception& e)
         {
            LOG_INFO(e.getMessage().c_str());
         }
      }

      if( ! found)
      {
         typedef std::pair<StrStrMultiMap::iterator, StrStrMultiMap::iterator> StrStrIterPair;
         StrStrIterPair range = mResGroupSearchSuffixMap.equal_range(strResGroup);

         std::string foundFilename(strFilename);
         StrStrMultiMap::iterator curIter = range.first;
         for( ; curIter != range.second; ++curIter)
         {
            const std::string& path = curIter->second;
            const std::string combinedPath = osgDB::concatPaths(path, strFilename);
            foundFilename = dtEntity::GetSystemInterface()->FindDataFile(combinedPath);
            if( ! foundFilename.empty())
            {
               break;
            }
         }

         if(foundFilename.empty())
         {
            std::string errorStr = 
               "dtEntity::ResourceProvider can't find file '" +
               std::string(filename.c_str()) + "'.";

            throw CEGUI::FileIOException(errorStr);
         }
         else
         {
            //pass to base class for reading
            CEGUI::String foundFilenameString(foundFilename);
            CEGUI::DefaultResourceProvider::loadRawDataContainer(foundFilenameString, output, resourceGroup );
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool ResourceProvider::AddSearchSuffix(const std::string& resourceGroup,
                                          const std::string& searchSuffix)
   {
      size_t numElements = mResGroupSearchSuffixMap.size();
      mResGroupSearchSuffixMap.insert(std::make_pair(resourceGroup, searchSuffix));
      return numElements < mResGroupSearchSuffixMap.size();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool ResourceProvider::RemoveSearchSuffix(const std::string& resourceGroup,
                                             const std::string& searchSuffix)
   {
      bool success = false;

      StrStrMultiMap::iterator foundIter = mResGroupSearchSuffixMap.lower_bound(searchSuffix);

      if(foundIter != mResGroupSearchSuffixMap.end())
      {
         mResGroupSearchSuffixMap.erase(foundIter);
         success = true;
      }

      return success;
   }

   ////////////////////////////////////////////////////////////////////////////////
   unsigned ResourceProvider::RemoveSearchSuffixes(const std::string& resourceGroup)
   {
      size_t numElements = mResGroupSearchSuffixMap.size();

      typedef std::pair<StrStrMultiMap::iterator, StrStrMultiMap::iterator> StrStrIterPair;
      StrStrIterPair range = mResGroupSearchSuffixMap.equal_range(resourceGroup);
      mResGroupSearchSuffixMap.erase(range.first, range.second);

      return unsigned(numElements - mResGroupSearchSuffixMap.size());
   }

   ////////////////////////////////////////////////////////////////////////////////
   unsigned ResourceProvider::ClearSearchSuffixes()
   {
      unsigned numElements = unsigned(mResGroupSearchSuffixMap.size());
      mResGroupSearchSuffixMap.clear();
      return numElements;
   }
}
