#include <dtEntity/fileutils.h>

namespace dtEntity
{
    const char UNIX_PATH_SEPARATOR = '/';
    const char WINDOWS_PATH_SEPARATOR = '\\';
    static const char * const PATH_SEPARATORS = "/\\";

    ////////////////////////////////////////////////////////////////////////////////
    char GetNativePathSeparator()
    {
    #if defined(WIN32) && !defined(__CYGWIN__)
        return WINDOWS_PATH_SEPARATOR;
    #else
        return UNIX_PATH_SEPARATOR;
    #endif
    }

    ////////////////////////////////////////////////////////////////////////////////
    std::string GetSimpleFileName(const std::string& fileName)
    {
        std::string::size_type slash = fileName.find_last_of(PATH_SEPARATORS);
        if (slash==std::string::npos) return fileName;
        else return std::string(fileName.begin()+slash+1,fileName.end());
    }

    ////////////////////////////////////////////////////////////////////////////////
    // strip all extensions from the filename.
    std::string GetNameLessAllExtensions(const std::string& fileName)
    {
        // Finds start serach position: from last slash, or the begining of the string if none found
        std::string::size_type startPos = fileName.find_last_of(PATH_SEPARATORS);            // Finds forward slash *or* back slash
        if (startPos == std::string::npos) startPos = 0;
        std::string::size_type dot = fileName.find_first_of('.', startPos);        // Finds *FIRST* dot from start pos
        if (dot==std::string::npos) return fileName;
        return std::string(fileName.begin(),fileName.begin()+dot);
    }

    ////////////////////////////////////////////////////////////////////////////////
    std::string GetFileExtension(const std::string& fileName)
    {
        std::string::size_type dot = fileName.find_last_of('.');
        std::string::size_type slash = fileName.find_last_of(PATH_SEPARATORS);
        if (dot==std::string::npos || (slash!=std::string::npos && dot<slash)) return std::string("");
        return std::string(fileName.begin()+dot+1,fileName.end());
    }

    ////////////////////////////////////////////////////////////////////////////////
    std::string GetFilePath(const std::string& fileName)
    {
        std::string::size_type slash = fileName.find_last_of(PATH_SEPARATORS);
        if (slash==std::string::npos) return std::string();
        else return std::string(fileName, 0, slash);
    }
}
