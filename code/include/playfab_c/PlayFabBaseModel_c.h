#pragma once

/// <summary>
/// Opaque handle to PlayFab API results. Passed to  various "Result" APIs to retrieve strongly typed results.
/// The lifetime of those strongly typed results will be tied to this result handle.
/// </summary>
typedef struct PlayFabResultHolder* PlayFabResultHandle;

/// <summary>
/// User handle returned from successful login calls. Contains auth context needed to make PlayFab requests.
/// </summary>
typedef struct PlayFabUserHolder* PlayFabUserHandle;

struct PlayFabDictionaryEntry
{
    const char* key;
    const char* value;
};