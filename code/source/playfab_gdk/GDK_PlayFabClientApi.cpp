#include <stdafx.h>
#include "async_provider.h"
#include "playfab_gdk/GDK_PlayFabClientApi.h"
#include "playfab/PlayFabClientApi.h"
#include "playfab/PlayFabBaseModel.h"

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

    auto provider = new ClientApiProvider<GetPlayerProfileRequest, GetPlayerProfileResult>(async, PlayFabClientAPI::GetPlayerProfile, std::move(request));
    return Provider::Run(std::unique_ptr<Provider>(provider));
}

HRESULT PlayFabGetPlayerProfileResult(
    _In_ XAsyncBlock* async,
    _Out_ PlayFabPlayerProfile* profile,
    _Out_ PlayFabResultHandle* resultHandle
) noexcept
{
    RETURN_HR_IF_FAILED(XAsyncGetResult(async, nullptr, sizeof(PlayFabResultHandle), resultHandle, nullptr));

    auto result = std::dynamic_pointer_cast<GetPlayerProfileResult>((*resultHandle)->result);
    profile->playerId = result->PlayerProfile->PlayerId.data();

    return S_OK;
}

void PlayFabReleaseResult(
    _In_ PlayFabResultHandle resultHandle
) noexcept
{
    if (resultHandle)
    {
        delete resultHandle;
    }
}