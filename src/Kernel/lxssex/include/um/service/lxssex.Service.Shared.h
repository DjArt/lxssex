#pragma once

#ifdef WINNT
#include <ntdef.h>
#else
#include <stdint.h>
#define UINT32 uint32_t
#define UINT64 uint64_t
#endif

typedef struct _CHANNEL
{
	UINT32 Id;
	UINT64 SectionSize;
} CHANNEL, * PCHANNEL;

typedef struct _CHANNEL_REGISTRATION
{
	union
	{
		CHANNEL Channel;
		void* ChannelHandle;
	};
} CHANNEL_REGISTRATION, * PCHANNEL_REGISTRATION;

typedef enum _CHANNEL_EVENT_TYPE
{
	ChannelSync = 0b00000000,
	WindowsSide = 0b00000001,
	LinuxSide = 0b00000010,
	ChannelOpened = 0b00000100,
	ChannelClosed = 0b00001000,
} CHANNEL_EVENT_TYPE, * PCHANNEL_EVENT_TYPE;

#ifdef WINNT
DEFINE_ENUM_FLAG_OPERATORS(CHANNEL_EVENT_TYPE)
#endif