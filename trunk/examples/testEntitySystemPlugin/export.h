#pragma once

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#  ifdef TESTENTTITYSYSTEM_LIBRARY
#    define TESTENTTITYSYSTEM_EXPORT __declspec(dllexport)
#  else
#     define TESTENTTITYSYSTEM_EXPORT __declspec(dllimport)
#   endif
#else
#   ifdef TESTENTTITYSYSTEM_LIBRARY
#      define TESTENTTITYSYSTEM_EXPORT __attribute__ ((visibility("default")))
#   else
#      define TESTENTTITYSYSTEM_EXPORT
#   endif
#endif


