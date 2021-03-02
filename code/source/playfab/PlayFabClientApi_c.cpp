#include <stdafx.h>
#include <playfab/async_provider.h>
#include <playfab/PlayFabClientApi.h>
#include <playfab/PlayFabBaseModel.h>

using namespace PlayFab;
using namespace PlayFab::ClientModels;

HRESULT PlayFabGetPlayerProfileAsync(
    _In_opt_ PlayFabAuthenticationContextHandle authContext,
    _In_ const char* playFabId,
    _In_ XAsyncBlock* async
) noexcept
{
    UNREFERENCED_PARAMETER(authContext);

    GetPlayerProfileRequest request{};
    request.PlayFabId = playFabId;

    auto provider = MakeUnique<ClientApiProvider<GetPlayerProfileRequest, GetPlayerProfileResult>>(async, PlayFabClientAPI::GetPlayerProfile, std::move(request));
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