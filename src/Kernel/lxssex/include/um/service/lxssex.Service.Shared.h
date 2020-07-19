#pragma once

typedef struct _CHANNEL
{
	unsigned __int32 Id;
	unsigned __int64 SectionSize;
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
	Unknown			= 0b00000000,
	WindowsSide		= 0b00000001,
	LinuxSide		= 0b00000010,
	ChannelSync		= 0b00000100,
	ChannelOpened	= 0b00001000,
	ChannelClosed	= 0b00010000,
} CHANNEL_EVENT_TYPE, * PCHANNEL_EVENT_TYPE;

enum LXSSEX_SERVICE_IOCTL
{
	OpenChannel = 0x0,
	SetEvent = 0x1,
	GetEvent = 0x2,
};