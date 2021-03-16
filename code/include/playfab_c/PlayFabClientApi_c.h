#pragma once

#include <httpClient/pal.h>
#include <httpClient/async.h>
#include "PlayFabClientDataModels_c.h"
#include "PlayFabLogin_c.h"

/// <summary>
/// Retrieves a player's profile.
/// </summary>
/// <param name="user">PlayFabUserHandle used to authenticate.</param>
/// <param name="request">Populated request object.</param>
/// <param name="async">XAsyncBlock for the async operation.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// If successful, call <see cref="PlayFabGetPlayerProfileGetResult"/> to get the result.
/// </remarks>
HRESULT PlayFabGetPlayerProfileAsync(
    _In_ PlayFabUserHandle user,
    _In_ const PlayFabGetPlayerProfileRequest* request,
    _In_ XAsyncBlock* async
) noexcept;

/// <summary>
/// Get the async result of a successful PlayFabGetPlayerProfileAsync call.
/// </summary>
/// <param name="async">XAsyncBlock for the async operation.</param>
/// <param name="handle">Opaque result handle.</param>
/// <param name="result">Pointer to the result object.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// The lifetime of the result object is tied to the result handle. When the result is no longer needed, call
/// PlayFabResultCloseHandle to release the result object.
/// </remarks>
HRESULT PlayFabGetPlayerProfileGetResult(
    _In_ XAsyncBlock* async,
    _Out_ PlayFabResultHandle* handle,
    _Out_ PlayFabPlayerProfileResult** result
) noexcept;

/// <summary>
/// Requests the creation of a shared group object, containing key/value pairs which may be updated by all members of the group. 
/// Upon creation, the current user will be the only member of the group. Shared Groups are designed for sharing data between a very 
/// small number of players, please see our guide: https://docs.microsoft.com/gaming/playfab/features/social/groups/using-shared-group-data
/// </summary>
/// <param name="user">PlayFabUserHandle used to authenticate.</param>
/// <param name="request">Populated request object.</param>
/// <param name="async">XAsyncBlock for the async operation.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// If successful, call <see cref="PlayFabCreateSharedGroupGetResult"/> to get the result.
/// </remarks>
HRESULT PlayFabCreateSharedGroupAsync(
    _In_ PlayFabUserHandle user,
    _In_ const PlayFabCreateSharedGroupRequest* request,
    _In_ XAsyncBlock* async
) noexcept;

/// <summary>
/// Gets the size in bytes of the buffer needed for a PlayFabCreateSharedGroupResult.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="bufferSize">The size in bytes for the result buffer.</param>
/// <returns>Result code for this API operation.</returns>
HRESULT PlayFabCreateSharedGroupGetResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* bufferSize
) noexcept;

/// <summary>
/// Gets the async result of a successful PlayFabCreateSharedGroupAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="bufferSize">The size of the buffer for the result object.</param>
/// <param name="buffer">Byte buffer used for result value and its fields.</param>
/// <param name="result">Pointer to the result object.</param>
/// <param name="bufferUsed">The number of bytes in the provided buffer that were used.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// result is a pointer within buffer and does not need to be freed separately.
/// </remarks>
HRESULT PlayFabCreateSharedGroupGetResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ PlayFabCreateSharedGroupResult** result,
    _Out_opt_ size_t* bufferUsed
) noexcept;

/// <summary>
/// Retrieves the current server time.
/// </summary>
/// <param name="user">PlayFabUserHandle used to authenticate.</param>
/// <param name="async">XAsyncBlock for the async operation.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// If successful, call <see cref="PlayFabGetTimeGetResult"/> to get the result.
/// </remarks>
HRESULT PlayFabGetTimeAsync(
    _In_ PlayFabUserHandle user,
    _In_ XAsyncBlock* async
) noexcept;

/// <summary>
/// Gets the async result of a successful PlayFabGetTimeAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="result">Pointer to a result object to populate.</param>
/// <returns>Result code for this API operation.</returns>
HRESULT PlayFabGetTimeGetResult(
    _In_ XAsyncBlock* async,
    _Out_ PlayFabGetTimeResult* result
) noexcept;
