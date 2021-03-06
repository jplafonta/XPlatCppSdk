#pragma once

#include "httpClient/pal.h"
#include "httpClient/async.h"
#include "PlayFabClientDataModels_c.h"

HRESULT PlayFabGetResultHandle(
    _In_ XAsyncBlock* async,
    _Out_ PlayFabResultHandle* resultHandle
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

// APIs
HRESULT PlayFabLoginWithCustomIDAsync(
    _In_ const PlayFabLoginWithCustomIDRequest* request,
    _In_ XAsyncBlock* async
) noexcept;

HRESULT PlayFabLoginWithCustomIDResultGetUserHandle(
    _In_ PlayFabResultHandle resultHandle,
    _Out_ PlayFabUserHandle* userHandle
) noexcept;

HRESULT PlayFabLoginWithCustomIdResultGetPlayFabId(
    _In_ PlayFabResultHandle resultHandle,
    _Out_ const char** playFabId
) noexcept;

HRESULT PlayFabGetPlayerProfileAsync(
    _In_ PlayFabUserHandle user,
    _In_ const PlayFabGetPlayerProfileRequest* request,
    _In_ XAsyncBlock* async
) noexcept;

HRESULT PlayFabGetPlayerProfileResultGetProfile(
    _In_ PlayFabResultHandle resultHandle,
    _Out_ PlayFabPlayerProfileModel** profile
) noexcept;
