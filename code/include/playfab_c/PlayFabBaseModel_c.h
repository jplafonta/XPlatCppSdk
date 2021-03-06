#pragma once

/// <summary>
/// Opaque handle to PlayFab API results
/// </summary>
typedef struct PlayFabResultHolder* PlayFabResultHandle;

typedef struct PlayFabUserHolder* PlayFabUserHandle;

// TODO name
struct PlayFabDictionaryEntry
{
    const char* key;
    const char* value;
};