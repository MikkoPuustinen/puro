#pragma once

#if defined (_MSC_VER)
    #define PURO_MSVC 1
#elif defined(__GNUC__)
    #define PURO_GCC 1
#endif

#ifndef PURO_DEBUG
    #ifdef _DEBUG
        #define PURO_DEBUG 1
    #endif
#endif

#if PURO_MSVC
    #define NOMINMAX
#endif

// TODO errorif works with Windows only for now

#if PURO_MSVC
    #include <Windows.h>
    #include <intrin.h> // MSVS breakpoint
    #define breakpoint __debugbreak()

#elif PURO_GCC 
    #define breakpoint raise(SIGABRT)
#endif

#if PURO_DEBUG == 1
    #define errorif(condition, msg) if ((condition)) { std::cout << msg << std::endl; breakpoint; }
#else
    #define errorif(condition, msg) ((void)0)
#endif


#ifndef PURO_BUFFER_WRAP_VECTOR_RESIZING
    #define PURO_BUFFER_WRAP_VECTOR_RESIZING 1
#endif


 
