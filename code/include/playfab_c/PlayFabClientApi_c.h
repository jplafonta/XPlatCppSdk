#pragma once

#include "httpClient/pal.h"
#include "httpClient/async.h"
#include "PlayFabClientDataModels_c.h"

/// <summary>
/// Create PlayFab global state
/// </summary>
HRESULT PlayFabInitialize(
    _Outptr_ PlayFabStateHandle* stateHandle
) noexcept;

/// <summary>
/// Cleanup PlayFab global state.
/// </summary>
HRESULT PlayFabCleanupAsync(
    _In_ XAsyncBlock* async,
    _In_ PlayFabStateHandle stateHandle
) noexcept;

/// <summary>
/// Async API pattern 1: Complex nested result returned via opaque PlayFabResultHandle + strongly typed result pointer.
/// The result pointer is valid until the result handle is closed.
/// </summary>
HRESULT PlayFabLoginWithCustomIDAsync(
    _In_ PlayFabStateHandle stateHandle,
    _In_ const PlayFabLoginWithCustomIDRequest* request,
    _In_ XAsyncBlock* async
) noexcept;

HRESULT PlayFabLoginWithCustomIDGetResult(
    _In_ XAsyncBlock* async,
    _Out_ PlayFabResultHandle* handle,
    _Out_ PlayFabLoginResult** result
) noexcept;

/// <summary>
/// Async API pattern 1 (see above)
/// </summary>
HRESULT PlayFabGetPlayerProfileAsync(
    _In_ PlayFabUserHandle user,
    _In_ const PlayFabGetPlayerProfileRequest* request,
    _In_ XAsyncBlock* async
) noexcept;

HRESULT PlayFabGetPlayerProfileGetResult(
    _In_ XAsyncBlock* async,
    _Out_ PlayFabResultHandle* handle,
    _Out_ PlayFabPlayerProfileResult** result
) noexcept;

/// <summary>
/// Async API pattern 2: result is trivially serializable into a buffer, but is not fixed size.
/// Result retrieved by requesting needed buffer size, allocating a byte buffer for the result, and
/// and finally calling the get result method which will serialize the result into the provided buffer.
/// The result is valid until buffer is cleaned up.
/// </summary>
HRESULT PlayFabCreateSharedGroupAsync(
    _In_ PlayFabUserHandle user,
    _In_ const PlayFabCreateSharedGroupRequest* request,
    _In_ XAsyncBlock* async
) noexcept;

HRESULT PlayFabCreateSharedGroupGetResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* bufferSize
) noexcept;

HRESULT PlayFabCreateSharedGroupGetResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ PlayFabCreateSharedGroupResult** result,
    _Out_opt_ size_t* bufferUsed
) noexcept;

/// <summary>
/// Async API pattern 3: result is fixed size. Result is obtained by calling get result method with a pointer
/// to the result object to populate. Result is valid until title cleans up result object.
/// </summary>
HRESULT PlayFabGetTimeAsync(
    _In_ PlayFabUserHandle user,
    _In_ XAsyncBlock* async
) noexcept;

HRESULT PlayFabGetTimeGetResult(
    _In_ XAsyncBlock* async,
    _Out_ PlayFabGetTimeResult* result
) noexcept;


