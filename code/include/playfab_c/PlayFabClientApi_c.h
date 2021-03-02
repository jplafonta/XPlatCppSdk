#pragma once

#include "httpClient/pal.h"
#include "httpClient/async.h"
#include "PlayFabClientDataModels_c.h"

HRESULT PlayFabGetPlayerProfileAsync(
    _In_opt_ PlayFabAuthenticationContextHandle authContext,
    _In_ const char* playFabId,
    _In_ XAsyncBlock* async
) noexcept;

HRESULT PlayFabGetResultHandle(
    _In_ XAsyncBlock* async,
    _Out_ PlayFabResultHandle* resultHandle
) noexcept;

HRESULT PlayFabGetPlayerProfileResultGetProfile(
    _In_ PlayFabResultHandle resultHandle,
    _Out_ PlayFabPlayerProfileModel** profile
) noexcept;

// Duplicates a result
HRESULT PlayFabResultDuplicateHandle(
    _In_ PlayFabResultHandle resultHandle,
    _Out_ PlayFabResultHandle* duplicatedHandle
) noexcept;

// Releases a result
void PlayFabResultCloseHandle(
    _In_ PlayFabResultHandle resultHandle
) noexcept;