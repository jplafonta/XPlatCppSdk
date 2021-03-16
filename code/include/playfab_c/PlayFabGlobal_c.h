#pragma once

#include <httpClient/pal.h>
#include <httpClient/async.h>

/// <summary>
/// Handle to global state created by PlayFabInitialize. A PlayFabStateHandle will be needed to call login methods.
/// Cleaned up with PlayFabCleanupAsync.
/// </summary>
typedef struct PlayFabGlobalState* PlayFabStateHandle;

/// <summary>
/// Create PlayFab global state.
/// </summary>
/// <param name="stateHandle">Pointer to PlayFabStateHandle.</param>
/// <returns>Result code for this API operation.</returns>
HRESULT PlayFabInitialize(
    _Outptr_ PlayFabStateHandle* stateHandle
) noexcept;

/// <summary>
/// Cleanup PlayFab global state.
/// </summary>
/// <param name="async">XAsyncBlock for the async operation.</param>
/// <param name="stateHandle">Handle to the PlayFab state to cleanup.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>Asynchronous result returned via XAsyncGetStatus.</remarks>
HRESULT PlayFabCleanupAsync(
    _In_ XAsyncBlock* async,
    _In_ PlayFabStateHandle stateHandle
) noexcept;