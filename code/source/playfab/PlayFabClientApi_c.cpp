#include <stdafx.h>
#include <playfab/AsyncProvider.h>
#include <playfab/PlayFabClientApi.h>
#include <playfab/PlayFabBaseModel.h>

using namespace PlayFab;
using namespace PlayFab::ClientModels;

HRESULT PlayFabInitialize(
    _Outptr_ PlayFabStateHandle* stateHandle
) noexcept
{
    return PlayFabGlobalState::Create(stateHandle);
}

HRESULT PlayFabCleanupAsync(
    _In_ XAsyncBlock* async,
    _In_ PlayFabStateHandle stateHandle
) noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(stateHandle);
    return stateHandle->CleanupAsync(async);
}

HRESULT PlayFabLoginWithCustomIDAsync(
    _In_ PlayFabStateHandle stateHandle,
    _In_ const PlayFabLoginWithCustomIDRequest* request,
    _In_ XAsyncBlock* async
) noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(stateHandle);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(request);

    auto provider = MakeUnique<ClientLoginApiProvider<PlayFabLoginWithCustomIDRequest>>(async, stateHandle->state, &ClientLoginApi::LoginWithCustomID, *request);
    return Provider::Run(UniquePtr<Provider>(provider.release()));
}

HRESULT PlayFabLoginWithCustomIDGetResult(
    _In_ XAsyncBlock* async,
    _Out_ PlayFabResultHandle* handle,
    _Out_ PlayFabLoginResult** result
) noexcept
{
    RETURN_HR_IF_FAILED(XAsyncGetResult(async, nullptr, sizeof(PlayFabResultHandle), handle, nullptr));
    *result = (PlayFabLoginResult*)(std::dynamic_pointer_cast<LoginResult>((*handle)->result).get());
    return S_OK;
}

HRESULT PlayFabGetPlayerProfileAsync(
    _In_opt_ PlayFabUserHandle user,
    _In_ const PlayFabGetPlayerProfileRequest* request,
    _In_ XAsyncBlock* async
) noexcept
{
    auto provider = MakeUnique<ClientApiProvider<PlayFabGetPlayerProfileRequest, GetPlayerProfileResult>>(async, user->user, &PlayFabClientInstanceAPI::GetPlayerProfile, *request);
    return Provider::Run(UniquePtr<Provider>(provider.release()));
}

HRESULT PlayFabGetPlayerProfileGetResult(
    _In_ XAsyncBlock* async,
    _Out_ PlayFabResultHandle* handle,
    _Out_ PlayFabPlayerProfileResult** result
) noexcept
{
    RETURN_HR_IF_FAILED(XAsyncGetResult(async, nullptr, sizeof(PlayFabResultHandle), handle, nullptr));
    *result = (PlayFabPlayerProfileResult*)(std::dynamic_pointer_cast<GetPlayerProfileResult>((*handle)->result).get());
    return S_OK;
}

HRESULT PlayFabCreateSharedGroupAsync(
    _In_ PlayFabUserHandle user,
    _In_ const PlayFabCreateSharedGroupRequest* request,
    _In_ XAsyncBlock* async
) noexcept
{
    auto provider = MakeUnique<ClientApiWithSerializableResultProvider<PlayFabCreateSharedGroupRequest, CreateSharedGroupResult>>(async, user->user, &PlayFabClientInstanceAPI::CreateSharedGroup, *request);
    return Provider::Run(UniquePtr<Provider>(provider.release()));
}

HRESULT PlayFabCreateSharedGroupGetResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* bufferSize
) noexcept
{
    return XAsyncGetResultSize(async, bufferSize);
}

HRESULT PlayFabCreateSharedGroupGetResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ PlayFabCreateSharedGroupResult** result,
    _Out_opt_ size_t* bufferUsed
) noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(result);

    size_t sizeUsed{ 0 };
    HRESULT hr = XAsyncGetResult(async, nullptr, bufferSize, buffer, &sizeUsed);
    assert(FAILED(hr) || sizeUsed > 0);

    *result = static_cast<PlayFabCreateSharedGroupResult*>(buffer);
    if (bufferUsed)
    {
        *bufferUsed = sizeUsed;
    }
    return hr;
}

HRESULT PlayFabGetTimeAsync(
    _In_ PlayFabUserHandle user,
    _In_ XAsyncBlock* async
) noexcept
{
    GetTimeRequest request;
    auto provider = MakeUnique<ClientApiWithSerializableResultProvider<GetTimeRequest, GetTimeResult>>(async, user->user, &PlayFabClientInstanceAPI::GetTime, request);
    return Provider::Run(UniquePtr<Provider>(provider.release()));
}

HRESULT PlayFabGetTimeGetResult(
    _In_ XAsyncBlock* async,
    _Out_ PlayFabGetTimeResult* result
) noexcept
{
    return XAsyncGetResult(async, nullptr, sizeof(PlayFabGetTimeResult), result, nullptr);
}

HRESULT PlayFabResultDuplicateHandle(
    _In_ PlayFabResultHandle resultHandle,
    _Out_ PlayFabResultHandle* duplicatedHandle
) noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(resultHandle);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(duplicatedHandle);

    *duplicatedHandle = MakeUnique<PlayFabResultHolder>(*resultHandle).release();
    return S_OK;
}

void PlayFabResultCloseHandle(
    _In_ PlayFabResultHandle resultHandle
) noexcept
{
    UniquePtr<PlayFabResultHolder>{ resultHandle };
}

HRESULT PlayFabUserDuplicateHandle(
    _In_ PlayFabUserHandle userHandle,
    _Out_ PlayFabUserHandle* duplicatedHandle
) noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(userHandle);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(duplicatedHandle);

    *duplicatedHandle = MakeUnique<PlayFabUser>(*userHandle).release();
    return S_OK;
}

void PlayFabUserCloseHandle(
    _In_ PlayFabUserHandle userHandle
) noexcept
{
    UniquePtr<PlayFabUser>{ userHandle };
}