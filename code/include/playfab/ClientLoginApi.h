#pragma once

#include <playfab/PlayFabClientDataModels.h>
#include <playfab/PlayFabError.h>
#include <playfab/TaskQueue.h>

namespace PlayFab
{
    class CallRequestContainerBase;
    class CallRequestContainer;
    class PlayFabApiSettings;

    class ClientLoginApi
    {
    public:
        void LoginWithCustomID(const PlayFabLoginWithCustomIDRequest& request, const ProcessApiCallback<ClientModels::LoginResult> callback, const TaskQueue& taskQueue, const ErrorCallback errorCallback = nullptr, void* customData = nullptr);

    private:
        static void OnLoginWithCustomIDResult(int httpCode, const std::string& result, const std::shared_ptr<CallRequestContainerBase>& reqContainer);
        static bool ValidateResult(PlayFabResultCommon& resultCommon, const CallRequestContainer& container);
    };
}
