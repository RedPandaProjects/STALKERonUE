// Copyright(c) 2021 Vadim Slyusarev 

#pragma once
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Config
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "optick.config.h"
#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTS 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(OPTICK_EXPORTS) && defined(_MSC_VER)
#define OPTICK_API __declspec(dllexport)
#else
#define OPTICK_API 
#endif


#ifdef __cplusplus
extern "C" {
#endif

#if USE_OPTICK
        OPTICK_API void OptickAPI_RegisterThread(const char* inThreadName, uint16_t inThreadNameLength);

        OPTICK_API uint64_t OptickAPI_CreateEventDescription(const char* inFunctionName, uint16_t inFunctionLength, const char* inFileName, uint16_t inFileNameLenght, uint32_t inFileLine);
        OPTICK_API uint64_t OptickAPI_PushEvent(uint64_t inEventDescription);
        OPTICK_API void OptickAPI_PopEvent(uint64_t inEventData);
        
        OPTICK_API void OptickAPI_NextFrame();

        OPTICK_API void OptickAPI_StartCapture();
        OPTICK_API void OptickAPI_StopCapture(const char* inFileName, uint16_t inFileNameLength);

        OPTICK_API void OptickAPI_AttachTag_String(uint64_t inEventDescription, const char* inValue, uint16_t intValueLength);
        OPTICK_API void OptickAPI_AttachTag_Int32(uint64_t inEventDescription, int inValue);
        OPTICK_API void OptickAPI_AttachTag_Float(uint64_t inEventDescription, float inValue);
        OPTICK_API void OptickAPI_AttachTag_UInt32(uint64_t inEventDescription, uint32_t inValue);
        OPTICK_API void OptickAPI_AttachTag_UInt64(uint64_t inEventDescription, uint64_t inValue);
        OPTICK_API void OptickAPI_AttachTag_Point(uint64_t inEventDescription, float x, float y, float z);
#else
        inline void OptickAPI_RegisterThread(const char* inThreadName, uint16_t inThreadNameLength) {}
        inline uint64_t OptickAPI_CreateEventDescription(const char* inFunctionName, uint16_t inFunctionLength, const char* inFileName, uint16_t inFileNameLenght, uint32_t inFileLine) { return 0; }
        inline uint64_t OptickAPI_PushEvent(uint64_t inEventDescription) { return 0; }
    inline void OptickAPI_PopEvent(uint64_t inEventData) {}
        inline void OptickAPI_NextFrame() {}
        inline void OptickAPI_StartCapture() {}
        inline void OptickAPI_StopCapture(const char* inFileName, uint16_t inFileNameLength) {}
        inline void OptickAPI_AttachTag_String(uint64_t inEventDescription, const char* inValue, uint16_t intValueLength) {}
        inline void OptickAPI_AttachTag_Int(uint64_t inEventDescription, int inValue) {}
        inline void OptickAPI_AttachTag_Float(uint64_t inEventDescription, float inValue) {}
        inline void OptickAPI_AttachTag_Int32(uint64_t inEventDescription, uint32_t inValue) {}
        inline void OptickAPI_AttachTag_UInt64(uint64_t inEventDescription, uint64_t inValue) {}
        inline void OptickAPI_AttachTag_Point(uint64_t inEventDescription, float x, float y, float z) {}
#endif

#ifdef __cplusplus
} // extern "C"
#endif
