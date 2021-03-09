#include <stdafx.h>
#include <playfab/async_provider.h>
#include <playfab/PlayFabClientApi.h>
#include <playfab/PlayFabBaseModel.h>

using namespace PlayFab;
using namespace PlayFab::ClientModels;

HRESULT PlayFabInitialize(
    _In_ const char* titleId
) noexcept
{
    return GlobalState::Create(titleId);
}

HRESULT PlayFabCleanupAsync(
    _In_ XAsyncBlock* async
) noexcept
{
    return GlobalState::CleanupAsync(async);
}

HRESULT PlayFabLoginWithCustomIDAsync(
    _In_ const PlayFabLoginWithCustomIDRequest* request,
    _In_ XAsyncBlock* async
) noexcept
{
    auto provider = MakeUnique<LoginApiProvider<PlayFabLoginWithCustomIDRequest, LoginResultWithUser>>(async, PlayFabClientInstanceAPI::LoginWithCustomID, *request);
    return Provider::Run(UniquePtr<Provider>(provider.release()));
}

HRESULT PlayFabLoginWithCustomIDResultGetUserHandle(
    _In_ PlayFabResultHandle resultHandle,
    _Out_ PlayFabUserHandle* userHandle
) noexcept
{
    auto result = std::dynamic_pointer_cast<LoginResultWithUser>(resultHandle->result);
    // TODO what part of this result is actually valuable? Can we just return the User
    *userHandle = new PlayFabUserHolder{ result->playFabUser };
    return S_OK;
}

HRESULT PlayFabLoginWithCustomIdResultGetPlayFabId(
    _In_ PlayFabResultHandle resultHandle,
    _Out_ const char** playFabId
) noexcept
{
    auto result = std::dynamic_pointer_cast<LoginResultWithUser>(resultHandle->result);
    *playFabId = result->PlayFabId.data();
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

HRESULT PlayFabGetResultHandle(
    _In_ XAsyncBlock* async,
    _Out_ PlayFabResultHandle* resultHandle
) noexcept
{
    RETURN_HR_IF_FAILED(XAsyncGetResult(async, nullptr, sizeof(PlayFabResultHandle), resultHandle, nullptr));
    return S_OK;
}

HRESULT PlayFabGetPlayerProfileResultGetProfile(
    _In_ PlayFabResultHandle resultHandle,
    _Out_ PlayFabPlayerProfileModel** profile
) noexcept
{
    auto result = std::dynamic_pointer_cast<GetPlayerProfileResult>(resultHandle->result);
    *profile = result->playerProfile.operator->();

    return S_OK;
}

HRESULT PlayFabResultDuplicateHandle(
    _In_ PlayFabResultHandle resultHandle,
    _Out_ PlayFabResultHandle* duplicatedHandle
) noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(resultHandle);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(duplicatedHandle);

    *duplicatedHandle = new PlayFabResultHolder{ resultHandle->result };
    return S_OK;
}

void PlayFabResultCloseHandle(
    _In_ PlayFabResultHandle resultHandle
) noexcept
{
    if (resultHandle)
    {
        delete resultHandle;
    }
}