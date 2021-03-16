#pragma once

#include <httpClient/pal.h>

/// <summary>
/// Opaque handle to PlayFab API results. Lifetime of result objects will be tied to a PlayFabResultHandle.
/// </summary>
typedef struct PlayFabResultHolder* PlayFabResultHandle;

/// <summary>
/// Duplicate a result handle.
/// </summary>
/// <param name="userHandle">PlayFabResultHandle to duplicate.</param>
/// <param name="duplicatedHandle">Pointer to duplicated handle.</param>
/// <returns>Result code for this API operation.</returns>
HRESULT PlayFabResultDuplicateHandle(
    _In_ PlayFabResultHandle resultHandle,
    _Out_ PlayFabResultHandle* duplicatedHandle
) noexcept;

/// <summary>
/// Close a result handle. If it is the last handle, the underlying result object will be destroyed.
/// </summary>
/// <param name="resultHandle">PlayFabResultHandle to close.</param>
/// <returns>Result code for this API operation.</returns>
void PlayFabResultCloseHandle(
    _In_ PlayFabResultHandle resultHandle
) noexcept;

struct PlayFabDictionaryEntry
{
    const char* key;
    const char* value;
};
