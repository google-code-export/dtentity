#pragma once

#include <dtEntity/export.h>
#include <string>
#include <vector>

namespace dtEntity
{
    extern DT_ENTITY_EXPORT char GetNativePathSeparator();    
    extern DT_ENTITY_EXPORT std::string GetSimpleFileName(const std::string& fileName);
    extern DT_ENTITY_EXPORT std::string GetNameLessAllExtensions(const std::string& fileName);
    extern DT_ENTITY_EXPORT std::string GetFileExtension(const std::string& fileName);
    extern DT_ENTITY_EXPORT std::string GetFilePath(const std::string& filename);
}