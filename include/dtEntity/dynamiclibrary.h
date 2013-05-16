#pragma once

#include <dtEntity/export.h>
#include <string>


namespace dtEntity {

/** DynamicLibrary - encapsulates the loading and unloading of dynamic libraries,
    typically used for loading ReaderWriter plug-ins.
*/
class DT_ENTITY_EXPORT DynamicLibrary
{
    public:

        typedef void*   HANDLE;
        typedef void*   PROC_ADDRESS;

        ~DynamicLibrary();

        /** returns a pointer to a DynamicLibrary object on successfully
          * opening of library returns NULL on failure.
          */
        static DynamicLibrary* loadLibrary(const std::string& libraryName);

        /** return name of library stripped of path.*/
        const std::string& getName() const     { return _name; }

        /** return name of library including full path to it.*/
        const std::string& getFullName() const { return _fullName; }

        /** return handle to .dso/.dll dynamic library itself.*/
        HANDLE             getHandle() const   { return _handle; }

        /** return address of function located in library.*/
        PROC_ADDRESS       getProcAddress(const std::string& procName);

    protected:

        /** get handle to library file */
        static HANDLE getLibraryHandle( const std::string& libraryName);

        /** disallow default constructor.*/
        DynamicLibrary() {}
        /** disallow copy constructor.*/
        DynamicLibrary(const DynamicLibrary&) {}
        /** disallow copy operator.*/
        DynamicLibrary& operator = (const DynamicLibrary&) { return *this; }

        /** Disallow public construction so that users have to go
          * through loadLibrary() above which returns NULL on
          * failure, a valid DynamicLibrary object on success.
          */
        DynamicLibrary(const std::string& name,HANDLE handle);
        
        HANDLE          _handle;
        std::string     _name;
        std::string     _fullName;

};

}
