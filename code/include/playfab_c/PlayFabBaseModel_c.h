#pragma once

/// <summary>
/// Handle to global state created by PlayFabInitialize. A PlayFabStateHandle will be needed to call login methods.
/// Cleaned up with PlayFabCleanupAsync.
/// </summary>
typedef struct PlayFabGlobalState* PlayFabStateHandle;

/// <summary>
/// User handle returned from successful login calls. Contains auth context needed to make PlayFab requests.
/// </summary>
typedef struct PlayFabUser* PlayFabUserHandle;

HRESULT PlayFabUserDuplicateHandle(
    _In_ PlayFabUserHandle userHandle,
    _Out_ PlayFabUserHandle* duplicatedHandle
) noexcept;

void PlayFabUserCloseHandle(
    _In_ PlayFabUserHandle userHandle
) noexcept;

/// <summary>
/// Opaque handle to PlayFab API results. Lifetime of result objects will be tied to a PlayFabResultHandle.
/// </summary>
typedef struct PlayFabResultHolder* PlayFabResultHandle;

HRESULT PlayFabResultDuplicateHandle(
    _In_ PlayFabResultHandle resultHandle,
    _Out_ PlayFabResultHandle* duplicatedHandle
) noexcept;

void PlayFabResultCloseHandle(
    _In_ PlayFabResultHandle resultHandle
) noexcept;

struct PlayFabDictionaryEntry
{
    const char* key;
    const char* value;
};