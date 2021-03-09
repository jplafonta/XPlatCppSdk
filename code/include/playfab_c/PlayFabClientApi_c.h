#pragma once

#include "httpClient/pal.h"
#include "httpClient/async.h"
#include "PlayFabClientDataModels_c.h"

/// <summary>
/// Initialize PlayFab global state
/// </summary>
HRESULT PlayFabInitialize(
    _In_ const char* titleId
) noexcept;

/// <summary>
/// Cleanup PlayFab global state. Shouldn't be called while async APIs are still running.
/// </summary>
HRESULT PlayFabCleanupAsync(
    _In_ XAsyncBlock* async
) noexcept;

/// <summary>
/// Get the result handle for an asynchronous API
/// </summary>
HRESULT PlayFabGetResultHandle(
    _In_ XAsyncBlock* async,
    _Out_ PlayFabResultHandle* resultHandle
) noexcept;

/// <summary>
/// Duplicate result.
/// </summary>
HRESULT PlayFabResultDuplicateHandle(
    _In_ PlayFabResultHandle resultHandle,
    _Out_ PlayFabResultHandle* duplicatedHandle
) noexcept;

/// <summary>
/// Close result. Invalidates all pointers associated with the result handle.
/// </summary>
void PlayFabResultCloseHandle(
    _In_ PlayFabResultHandle resultHandle
) noexcept;

/// <summary>
/// Login with custom ID
/// </summary>
HRESULT PlayFabLoginWithCustomIDAsync(
    _In_ const PlayFabLoginWithCustomIDRequest* request,
    _In_ XAsyncBlock* async
) noexcept;

/// <summary>
/// Retrieves PlayFabUserHandle from a login result. User handle is needed to make future API calls
/// </summary>
HRESULT PlayFabLoginWithCustomIDResultGetUserHandle(
    _In_ PlayFabResultHandle resultHandle,
    _Out_ PlayFabUserHandle* userHandle
) noexcept;

/// <summary>
/// Retrieves playFabId from a PlayFabLoginWithCustomIDAsync result.
/// </summary>
HRESULT PlayFabLoginWithCustomIdResultGetPlayFabId(
    _In_ PlayFabResultHandle resultHandle,
    _Out_ const char** playFabId
) noexcept;

/// <summary>
/// Get Player Profile
/// </summary>
HRESULT PlayFabGetPlayerProfileAsync(
    _In_ PlayFabUserHandle user,
    _In_ const PlayFabGetPlayerProfileRequest* request,
    _In_ XAsyncBlock* async
) noexcept;

/// <summary>
/// Retrieves the Player Profile model from a PlayFabGetPlayerProfileAsync result
/// </summary>
HRESULT PlayFabGetPlayerProfileResultGetProfile(
    _In_ PlayFabResultHandle resultHandle,
    _Out_ PlayFabPlayerProfileModel** profile
) noexcept;
