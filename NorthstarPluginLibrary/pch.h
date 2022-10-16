#ifndef PCH_H
#define PCH_H

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include "spdlog/spdlog.h"

#include "lib/plugin_abi.h"

#include "lib/plugin.h"

#define DLLEXPORT __declspec(dllexport)
#define EXPORT extern "C" DLLEXPORT

#endif
