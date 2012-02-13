#pragma once


#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#  ifdef OSGLIBROCKET_LIBRARY
#    define OSGLIBROCKET_EXPORT __declspec(dllexport)
#  else
#     define OSGLIBROCKET_EXPORT __declspec(dllimport)
#   endif
#else
#   ifdef OSGLIBROCKET_LIBRARY
#      define OSGLIBROCKET_EXPORT __attribute__ ((visibility("default")))
#   else
#      define OSGLIBROCKET_EXPORT
#   endif
#endif

#ifdef _WIN32
#   pragma warning (disable: 4251)
#   pragma warning(disable : 4355) // 'this' used in initializer list
#endif
