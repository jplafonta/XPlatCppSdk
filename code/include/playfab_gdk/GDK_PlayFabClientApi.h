#pragma once

#include "httpClient/pal.h"
#include "httpClient/async.h"

// Handle to PlayFabAuthenticationContext
typedef struct PlayFabAuthenticationContextHolder* PlayFabAuthenticationContextHandle;

// Handle to client model
typedef struct PlayFabResultHolder* PlayFabResultHandle;

struct PlayFabPlayerProfile
{
    const char* playerId;
    uint32_t titleId;
    // ...
};

// Sample GDK Conformant Playfab API
HRESULT PlayFabGetPlayerProfileAsync(
    _In_opt_ PlayFabAuthenticationContextHandle authContext,
    _In_ const char* playFabId,
    _In_ XAsyncBlock* async
) noexcept;

HRESULT PlayFabGetPlayerProfileResult(
    _In_ XAsyncBlock* async,
    _Out_ PlayFabPlayerProfile* profile,
    _Out_ PlayFabResultHandle* resultHandle
) noexcept;

void PlayFabReleaseResult(
    _In_ PlayFabResultHandle resultHandle
) noexcept;