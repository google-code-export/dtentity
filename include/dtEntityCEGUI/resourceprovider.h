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


#include <dtEntityCEGUI/export.h>
#include <CEGUI/CEGUIDefaultResourceProvider.h>

namespace dtEntityCEGUI
{

   ///A simple CEGUI ResourceProvider based on the CEGUI::DefaultResourceProvider

   /** Add the functionality of using search paths to find the data files
     * used by CEGUI.
     *
     * @see SetDataFilePathList()
     */
   class DTENTITY_CEGUI_EXPORT ResourceProvider :   public CEGUI::DefaultResourceProvider
   {
   public:
      ResourceProvider();
      ~ResourceProvider(){};

      ///Load the data of the supplied filename.
      void loadRawDataContainer(const CEGUI::String& filename, CEGUI::RawDataContainer& output, const CEGUI::String& resourceGroup);

      /**
       * Add an alternate sub-directory (relative to any project context)
       * to search when trying to find a file in a specified resource group.
       * @param resourceGroup The resource group for the alternate search suffix.
       * @param searchSuffix The sub-directory to append to existing project search
       *        paths. This suffix will be used in case a file is not found for the
       *        specified resource group.
       * @return TRUE if the suffix was successfully added.
       */
      bool AddSearchSuffix(const std::string& resourceGroup, const std::string& searchSuffix);

      /**
       * Remove a previously added search suffix for a specified resource group.
       * @param resourceGroup The resource group for the alternate search suffix.
       * @param searchSuffix The sub-directory search suffix that was previously added
       *        for the specified resource group. This should match exactly as it was added.
       * @return TRUE if the suffix was successfully found and removed.
       */
      bool RemoveSearchSuffix(const std::string& resourceGroup, const std::string& searchSuffix);

      /**
       * Remove all previously added search suffixes for a specified resource group.
       * @param resourceGroup The resource group for the alternate search suffixes.
       * @return The number of suffixes that were successfully removed.
       */
      unsigned RemoveSearchSuffixes(const std::string& resourceGroup);

      /**
       * Remove all previously added search suffixes for all resource groups.
       * @return The number of suffixes that were successfully removed.
       */
      unsigned ClearSearchSuffixes();

   private:
      typedef std::multimap<std::string, std::string> StrStrMultiMap;
      StrStrMultiMap mResGroupSearchSuffixMap;
   };
}

