// Copyright(c) 2021 Vadim Slyusarev 

#include "optick_capi.h"

#if USE_OPTICK

#include "optick_core.h"

#if defined(__MACH__)
#include <stdlib.h>
#else 
#include <malloc.h>
#endif
#include <string.h>

OPTICK_API void OptickAPI_RegisterThread(const char* inThreadName, uint16_t inThreadNameLength)
{
        Optick::OptickString<256> threadName(inThreadName, inThreadNameLength);
        Optick::RegisterThread(threadName.data);
}

OPTICK_API uint64_t OptickAPI_CreateEventDescription(const char* inFunctionName, uint16_t inFunctionLength, const char* inFileName, uint16_t inFileNameLenght, uint32_t inFileLine)
{
        Optick::OptickString<128> name(inFunctionName, inFunctionLength);
        Optick::OptickString<256> file(inFileName, inFileNameLenght);
        uint8_t flags = Optick::EventDescription::COPY_NAME_STRING | Optick::EventDescription::COPY_FILENAME_STRING | Optick::EventDescription::IS_CUSTOM_NAME;
        return (uint64_t)::Optick::CreateDescription(name.data, file.data, inFileLine, nullptr, Optick::Category::None, flags);
}
OPTICK_API uint64_t OptickAPI_PushEvent(uint64_t inEventDescription)
{
        return (uint64_t)Optick::Event::Start(*((Optick::EventDescription*)inEventDescription));
}

OPTICK_API void OptickAPI_PopEvent(uint64_t inEventData)
{
        Optick::Event::Stop(*((Optick::EventData*)inEventData));
}

OPTICK_API void OptickAPI_NextFrame()
{
        Optick::Event::Pop();
        Optick::EndFrame();
        Optick::Update();
        Optick::BeginFrame();
        Optick::Event::Push(*Optick::GetFrameDescription());
}

OPTICK_API void OptickAPI_StartCapture()
{
        Optick::StartCapture();
}

OPTICK_API void OptickAPI_StopCapture(const char* inFileName, uint16_t inFileNameLength)
{
        Optick::OptickString<256> fileName(inFileName, inFileNameLength);
        Optick::StopCapture();
        Optick::SaveCapture(fileName.data);
}

OPTICK_API void OptickAPI_AttachTag_String(uint64_t inEventDescription, const char* inValue, uint16_t inValueLength)
{
        Optick::Tag::Attach(*(Optick::EventDescription*)inEventDescription, inValue, inValueLength);
}

OPTICK_API void OptickAPI_AttachTag_Int32(uint64_t inEventDescription, int32_t inValue)
{
        Optick::Tag::Attach(*(Optick::EventDescription*)inEventDescription, inValue);
}

OPTICK_API void OptickAPI_AttachTag_Float(uint64_t inEventDescription, float inValue)
{
        Optick::Tag::Attach(*(Optick::EventDescription*)inEventDescription, inValue);
}

OPTICK_API void OptickAPI_AttachTag_UInt32(uint64_t inEventDescription, uint32_t inValue)
{
        Optick::Tag::Attach(*(Optick::EventDescription*)inEventDescription, inValue);
}

OPTICK_API void OptickAPI_AttachTag_UInt64(uint64_t inEventDescription, uint64_t inValue)
{
        Optick::Tag::Attach(*(Optick::EventDescription*)inEventDescription, inValue);
}

OPTICK_API void OptickAPI_AttachTag_Point(uint64_t inEventDescription, float x, float y, float z)
{
        Optick::Tag::Attach(*(Optick::EventDescription*)inEventDescription, x, y, z);
}

#endif //USE_OPTICK
