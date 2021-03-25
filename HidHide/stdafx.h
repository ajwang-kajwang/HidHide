// (c) Eric Korff de Gidts
// SPDX-License-Identifier: MIT
// stdafx.h
#pragma once
#include <ntddk.h>
#include <initguid.h>
#define NTSTRSAFE_NO_CB_FUNCTIONS
#include <Ntstrsafe.h>
#include <wdf.h>
#include <wdmsec.h>
#include <Devpkey.h>

// Define _countof
#define _countof(array) (sizeof(array) / sizeof(array[0]))

// Define standard text conversions
#define __L(s) L#s
#define _L(s) __L(s)

// The typical process PID for system
#define SYSTEM_PID 4

// API call-back prototype missing so define it here
typedef
_Function_class_(CREATE_PROCESS_NOTIFY_ROUTINE)
_IRQL_requires_same_
_IRQL_requires_max_(PASSIVE_LEVEL)
VOID
CREATE_PROCESS_NOTIFY_ROUTINE(
    _In_ HANDLE ParentId,
    _In_ HANDLE ProcessId,
    _In_ BOOLEAN Create
); 

// API call-back prototype missing so define it here
typedef
_Function_class_(LOAD_IMAGE_NOTIFY_ROUTINE)
_IRQL_requires_same_
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
LOAD_IMAGE_NOTIFY_ROUTINE(
    _In_opt_ PUNICODE_STRING FullImageName,
    _In_ HANDLE ProcessId,
    _In_ PIMAGE_INFO ImageInfo
);

// Include the message file generated by the message compiler from the ETW manifest
#pragma warning(push)
#pragma warning(disable: 26451) // Warning(s) in code generated by the message compiler
#include "HidHideETW.h"
#pragma warning(pop)
