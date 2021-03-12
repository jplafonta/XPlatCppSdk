#include <stdafx.h>

#include <playfab/ClientLoginApi.h>
#include <playfab/PlayFabPluginManager.h>
#include <playfab/PlayFabSettings.h>
#include <playfab/PlayFabUser.h>

namespace PlayFab
{
    using namespace ClientModels;

    void ClientLoginApi::LoginWithCustomID(
        const PlayFabLoginWithCustomIDRequest& request,
        const ProcessApiCallback<LoginResult> callback,
        const TaskQueue& queue,
        const ErrorCallback errorCallback,
        void* customData
    )
    {
        std::shared_ptr<PlayFabAuthenticationContext> context = MakeShared<PlayFabAuthenticationContext>();
        std::shared_ptr<PlayFabApiSettings> settings = MakeShared<PlayFabApiSettings>();
        settings->titleId = request.titleId;

        IPlayFabHttpPlugin& http = *PlayFabPluginManager::GetPlugin<IPlayFabHttpPlugin>(PlayFabPluginContract::PlayFab_Transport);
        Json::Value requestJson = JsonUtils::ToJsonObject(request);
        if (!request.titleId)
        {
            requestJson["TitleId"] = Json::Value(settings->titleId);
        }

        std::string jsonAsString = requestJson.toStyledString();

        std::unordered_map<std::string, std::string> headers;

        auto reqContainer = std::unique_ptr<CallRequestContainer>(new CallRequestContainer(
            "/Client/LoginWithCustomID",
            headers,
            jsonAsString,
            OnLoginWithCustomIDResult,
            settings,
            context,
            customData));

        reqContainer->successCallback = std::shared_ptr<void>((callback == nullptr) ? nullptr : new ProcessApiCallback<LoginResult>(callback));
        reqContainer->errorCallback = errorCallback;
        reqContainer->m_queue = queue;

        http.MakePostRequest(std::unique_ptr<CallRequestContainerBase>(static_cast<CallRequestContainerBase*>(reqContainer.release())));
    }

    void ClientLoginApi::OnLoginWithCustomIDResult(int /*httpCode*/, const std::string& /*result*/, const std::shared_ptr<CallRequestContainerBase>& reqContainer)
    {
        CallRequestContainer& container = static_cast<CallRequestContainer&>(*reqContainer);
        std::shared_ptr<PlayFabAuthenticationContext> context = container.GetContext();

        LoginResult outResult;
        if (ValidateResult(outResult, container))
        {
            context->HandlePlayFabLogin(outResult.PlayFabId, outResult.SessionTicket, outResult.EntityToken->Entity->Id, outResult.EntityToken->Entity->Type, outResult.EntityToken->EntityToken);
            outResult.authenticationContext = context;

            auto user = MakeShared<User>(container.GetApiSettings(), context, outResult.SettingsForUser);
            outResult.userHandle = MakeUnique<PlayFabUser>(user).release();

            std::shared_ptr<void> internalPtr = container.successCallback;
            if (internalPtr.get() != nullptr)
            {
                const auto& callback = *static_cast<ProcessApiCallback<LoginResult> *>(internalPtr.get());
                callback(outResult, container.GetCustomData());
            }
        }
    }

    bool ClientLoginApi::ValidateResult(PlayFabResultCommon& resultCommon, const CallRequestContainer& container)
    {
        if (container.errorWrapper.HttpCode == 200)
        {
            resultCommon.FromJson(container.errorWrapper.Data);
            resultCommon.Request = container.errorWrapper.Request;
            return true;
        }
        else // Process the error case
        {
            if (PlayFabSettings::globalErrorHandler != nullptr)
            {
                PlayFabSettings::globalErrorHandler(container.errorWrapper, container.GetCustomData());
            }

            if (container.errorCallback != nullptr)
            {
                container.errorCallback(container.errorWrapper, container.GetCustomData());
            }
            return false;
        }
    }

    namespace ClientModels
    {
        LoginResult::LoginResult(const LoginResult& src) :
            PlayFabLoginResultCommon(),
            EntityToken(src.EntityToken),
            InfoResultPayload(src.InfoResultPayload),
            LastLoginTime(src.LastLoginTime),
            NewlyCreated(src.NewlyCreated),
            PlayFabId(src.PlayFabId),
            SessionTicket(src.SessionTicket),
            SettingsForUser(src.SettingsForUser),
            pfTreatmentAssignment(src.pfTreatmentAssignment)
        {
            userHandle = MakeUnique<PlayFabUser>(*src.userHandle).release();
            lastLoginTime = LastLoginTime.isNull() ? nullptr : LastLoginTime.operator->();
            newlyCreated = NewlyCreated;
        }
    }
}
