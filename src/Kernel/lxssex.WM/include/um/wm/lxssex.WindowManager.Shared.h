#pragma once

#include <um/service/lxssex.Service.Shared.h>

typedef enum _WM_CHANNEL_PAIR_FUNCTION
{
	WM = 0x0,
	EGL = 0x1,
	GLES = 0x2,
} WM_CHANNEL_PAIR_FUNCTION, * PWM_CHANNEL_PAIR_FUNCTION;

typedef struct _WM_CHANNEL_PAIR
{
	PCHANNEL InputChannel;
	PCHANNEL OutputChannel;
	WM_CHANNEL_PAIR_FUNCTION Function;
} WM_CHANNEL_PAIR, * PWM_CHANNEL_PAIR;

typedef struct _WM_CHANNEL_PAIR_REGISTRATION
{
	PVOID RegistrationContext;
	PVOID ChannelContext;
	WM_CHANNEL_PAIR WindowManagerChannel;
} WM_CHANNEL_PAIR_REGISTRATION, * PWM_CHANNEL_PAIR_REGISTRATION;

typedef struct _WM_CHANNEL_PAIR_QUERY
{
	PVOID RegistrationContext;
	UINT32 Count;
	PWM_CHANNEL_PAIR Buffer;
} WM_CHANNEL_PAIR_QUERY, * PWM_CHANNEL_PAIR_QUERY;

enum LXSSEX_WM_IOCTL
{
	WMOpenChannelPair = 0x0,
	WMQueryUnhandledPairs = 0x1
};