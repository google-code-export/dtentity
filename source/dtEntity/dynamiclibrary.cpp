//The dlopen calls were not adding to OS X until 10.3
#ifdef __APPLE__
#include <AvailabilityMacros.h>
#if !defined(MAC_OS_X_VERSION_10_3) || (MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_3)
#define APPLE_PRE_10_3
#endif
#endif

#if defined(WIN32) && !defined(__CYGWIN__)
#include <io.h>
#include <windows.h>
#include <winbase.h>
#elif defined(__APPLE__) && defined(APPLE_PRE_10_3)
#include <mach-o/dyld.h>
#else // all other unix
#include <unistd.h>
#ifdef __hpux
// Although HP-UX has dlopen() it is broken! We therefore need to stick
// to shl_load()/shl_unload()/shl_findsym()
#include <dl.h>
#include <errno.h>
#else
#include <dlfcn.h>
#endif
#endif

#include <dtEntity/core.h>
#include <dtEntity/log.h>
#include <dtEntity/systeminterface.h>
#include <dtEntity/dynamiclibrary.h>

namespace dtEntity
{
    static const char * const PATH_SEPARATORS = "/\\";
    std::string getSimpleFileName(const std::string& fileName)
    {
        std::string::size_type slash = fileName.find_last_of(PATH_SEPARATORS);
        if (slash==std::string::npos) return fileName;
        else return std::string(fileName.begin()+slash+1,fileName.end());
    }
    
    template<typename T, typename R>
    T convertPointerType(R src)
    {
        T dest;
        memcpy(&dest, &src, sizeof(src));
        return dest;
    }

    DynamicLibrary::DynamicLibrary(const std::string& name, HANDLE handle)
    {
        _name = name;
        _handle = handle;
        LOG_INFO("Opened DynamicLibrary "<<_name);
    }

    DynamicLibrary::~DynamicLibrary()
    {
        if (_handle)
        {
            LOG_INFO("Closing DynamicLibrary "<<_name);
    #if defined(WIN32) && !defined(__CYGWIN__)
            FreeLibrary((HMODULE)_handle);
    #elif defined(__APPLE__) && defined(APPLE_PRE_10_3)
            NSUnLinkModule(static_cast<NSModule>(_handle), FALSE);
    #elif defined(__hpux)
            // fortunately, shl_t is a pointer
            shl_unload (static_cast<shl_t>(_handle));
    #else // other unix
            dlclose(_handle);
    #endif
        }
    }

    DynamicLibrary* DynamicLibrary::loadLibrary(const std::string& libraryName)
    {

        HANDLE handle = NULL;

        std::string fullLibraryName = dtEntity::GetSystemInterface()->FindLibraryFile(libraryName);
        if (!fullLibraryName.empty()) handle = getLibraryHandle( fullLibraryName ); // try the lib we have found
        else handle = getLibraryHandle( libraryName ); // havn't found a lib ourselves, see if the OS can find it simply from the library name.

        if (handle) return new DynamicLibrary(libraryName,handle);

        // else no lib found so report errors.
        LOG_INFO("DynamicLibrary::failed loading \""<<libraryName<<"\"");

        return NULL;
    }

    DynamicLibrary::HANDLE DynamicLibrary::getLibraryHandle( const std::string& libraryName)
    {
        HANDLE handle = NULL;

    #if defined(WIN32) && !defined(__CYGWIN__)
    
     handle = LoadLibrary( libraryName.c_str() );
    
    #elif defined(__APPLE__) && defined(APPLE_PRE_10_3)
        NSObjectFileImage image;
        // NSModule os_handle = NULL;
        if (NSCreateObjectFileImageFromFile(libraryName.c_str(), &image) == NSObjectFileImageSuccess) {
            // os_handle = NSLinkModule(image, libraryName.c_str(), TRUE);
            handle = NSLinkModule(image, libraryName.c_str(), TRUE);
            NSDestroyObjectFileImage(image);
        }
    #elif defined(__hpux)
        // BIND_FIRST is neccessary for some reason
        handle = shl_load ( libraryName.c_str(), BIND_DEFERRED|BIND_FIRST|BIND_VERBOSE, 0);
        return handle;
    #else // other unix

        // dlopen will not work with files in the current directory unless
        // they are prefaced with './'  (DB - Nov 5, 2003).
        std::string localLibraryName;
        if( libraryName == getSimpleFileName( libraryName ) )
            localLibraryName = "./" + libraryName;
        else
            localLibraryName = libraryName;

        handle = dlopen( localLibraryName.c_str(), RTLD_LAZY | RTLD_GLOBAL);
        if( handle == NULL )
        {
            if (fileExists(localLibraryName))
            {
                LOG_WARN("Warning: dynamic library '" << libraryName << "' exists, but an error occurred while trying to open it:");
                LOG_WARN(dlerror());
            }
            else
            {
                LOG_INFO("Warning: dynamic library '" << libraryName << "' does not exist (or isn't readable):");
                LOG_INFO(dlerror());
            }
        }
    #endif
        return handle;
    }

    DynamicLibrary::PROC_ADDRESS DynamicLibrary::getProcAddress(const std::string& procName)
    {
        if (_handle==NULL) return NULL;
    #if defined(WIN32) && !defined(__CYGWIN__)
        return convertPointerType<DynamicLibrary::PROC_ADDRESS, FARPROC>( GetProcAddress( (HMODULE)_handle, procName.c_str() ) );
    #elif defined(__APPLE__) && defined(APPLE_PRE_10_3)
        std::string temp("_");
        NSSymbol symbol;
        temp += procName;   // Mac OS X prepends an underscore on function names
        symbol = NSLookupSymbolInModule(static_cast<NSModule>(_handle), temp.c_str());
        return NSAddressOfSymbol(symbol);
    #elif defined(__hpux)
        void* result = NULL;
        if (shl_findsym (reinterpret_cast<shl_t*>(&_handle), procName.c_str(), TYPE_PROCEDURE, result) == 0)
        {
            return result;
        }
        else
        {
            LOG_WARN("DynamicLibrary::failed looking up " << procName);
            LOG_WARN("DynamicLibrary::error " << strerror(errno));
            return NULL;
        }
    #else // other unix
        void* sym = dlsym( _handle,  procName.c_str() );
        if (!sym) {
            LOG_WARN("DynamicLibrary::failed looking up " << procName);
            LOG_WARN("DynamicLibrary::error " << dlerror());
        }
        return sym;
    #endif
    }

}