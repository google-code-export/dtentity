#include <dtEntity/fileutils.h>

#if defined(WIN32) && !defined(__CYGWIN__)
#include <windows.h>
#include <io.h>
#include <direct.h>
#include <winbase.h> 
#else
#include <dirent.h>
#endif

#define OSGDB_STRING_TO_FILENAME(s) s
#define OSGDB_FILENAME_TO_STRING(s) s
#define OSGDB_FILENAME_TEXT(x) x
#define OSGDB_WINDOWS_FUNCT(x) x ## A
#define OSGDB_WINDOWS_FUNCT_STRING(x) #x "A"
typedef char filenamechar;
typedef std::string filenamestring;

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
#if defined(WIN32) && !defined(__CYGWIN__)


    DirectoryContents GetDirectoryContents(const std::string& dirName)
    {
        DirectoryContents contents;

        OSGDB_WINDOWS_FUNCT(WIN32_FIND_DATA) data;
        HANDLE handle = OSGDB_WINDOWS_FUNCT(FindFirstFile)((OSGDB_STRING_TO_FILENAME(dirName) + OSGDB_FILENAME_TEXT("\\*")).c_str(), &data);
        if (handle != INVALID_HANDLE_VALUE)
        {
            do
            {
                contents.push_back(OSGDB_FILENAME_TO_STRING(data.cFileName));
            }
            while (OSGDB_WINDOWS_FUNCT(FindNextFile)(handle, &data) != 0);

            FindClose(handle);
        }
        return contents;
    }

#else

    DirectoryContents GetDirectoryContents(const std::string& dirName)
    {
        DirectoryContents contents;

        DIR *handle = opendir(dirName.c_str());
        if (handle)
        {
            dirent *rc;
            while((rc = readdir(handle))!=NULL)
            {
                contents.push_back(rc->d_name);
            }
            closedir(handle);
        }

        return contents;
    }

#endif // unix getDirectoryContexts

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
