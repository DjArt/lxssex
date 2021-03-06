﻿using static EGL.Constants;

namespace EGL.Enums
{
    public enum QueryStringType : uint
    {
        ClientAPIs = EGL_CLIENT_APIS,
        Vendor = EGL_VENDOR,
        Version = EGL_VERSION,
        Extensions = EGL_EXTENSIONS,
    }
}
