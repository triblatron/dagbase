#pragma once

#if defined(_MSC_VER)
    #pragma warning( disable : 4244 )
    #pragma warning( disable : 4251 )
    #pragma warning( disable : 4267 )
    #pragma warning( disable : 4275 )
    #pragma warning( disable : 4290 )
    #pragma warning( disable : 4786 )
    #pragma warning( disable : 4305 )
    #pragma warning( disable : 4996 )
#endif

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
    #  if defined( DAGBASE_LIBRARY_STATIC )
    #    define DAGBASE_API
    #  elif defined( DAGBASE_LIBRARY )
    #    define DAGBASE_API   __declspec(dllexport)
    #  else
    #    define DAGBASE_API   __declspec(dllimport)
    #  endif
#  if defined( SHELL_LIBRARY_STATIC )
#    define SHELL_API
#  elif defined( SHELL_LIBRARY )
#    define SHELL_API   __declspec(dllexport)
#  else
#    define SHELL_API   __declspec(dllimport)
#  endif
#else
    #  define DAGBASE_API
    #  define SHELL_API
#endif
