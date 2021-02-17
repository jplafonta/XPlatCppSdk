#include <stdafx.h>
#include <playfab/PlayFabHCHttpPlugin.h>
#include <playfab/PlayFabSettings.h>
#include "../code/source/playfab_gdk/task_queue.h"
#include "../code/source/playfab_gdk/async_provider.h"

namespace PlayFab
{
    // Wrapper around HCCallHandle
    class HCHttpCall
    {
    public:
        static void Perform(std::unique_ptr<CallRequestContainer> requestContainer);

        virtual ~HCHttpCall() noexcept;

    private:
        HCHttpCall() = default;
        HCHttpCall(const HCHttpCall& other) = delete;
        HCHttpCall& operator=(HCHttpCall other) = delete;

        HRESULT Initialize(std::unique_ptr<CallRequestContainer> requestContainer);

        static void CALLBACK HCPerformComplete(XAsyncBlock* async);

        HCCallHandle m_callHandle{ nullptr };
        std::unique_ptr<CallRequestContainer> m_requestContainer;
        XAsyncBlock m_asyncBlock{};
        TaskQueue m_queue;
    };

    PlayFabHCHttpPlugin::PlayFabHCHttpPlugin()
    {
        HRESULT hr = HCInitialize(nullptr);
        assert(SUCCEEDED(hr));
    }

    PlayFabHCHttpPlugin::~PlayFabHCHttpPlugin()
    {
        HCCleanup();
    }

    void PlayFabHCHttpPlugin::MakePostRequest(std::unique_ptr<CallRequestContainerBase> requestContainer)
    {
        CallRequestContainer* container = dynamic_cast<CallRequestContainer*>(requestContainer.get());
        if (container != nullptr)
        {
            container->ThrowIfSettingsInvalid();
            {
                std::unique_lock<std::mutex> lock{ m_mutex };
                ++m_activeRequestCount;
            }
            requestContainer.release();
            HCHttpCall::Perform(std::unique_ptr<CallRequestContainer>{ container });
        }
    }

    size_t PlayFabHCHttpPlugin::Update()
    {
        if (PlayFabSettings::threadedCallbacks)
        {
            throw PlayFabException(PlayFabExceptionCode::ThreadMisuse, "You should not call Update() when PlayFabSettings::threadedCallbacks == true");
        }

        std::unique_ptr<CallRequestContainer> requestContainer = nullptr;
        {
            std::unique_lock<std::mutex> lock{ m_mutex };
            if (m_completedRequests.empty())
            {
                return m_activeRequestCount;
            }

            requestContainer = std::move(this->m_completedRequests[0]);
            this->m_completedRequests.pop_front();
            m_activeRequestCount--;
        }

        InvokeCallback(std::move(requestContainer));

        // activeRequestCount can be altered by HandleResults, so we have to re-lock and return an updated value
        {
            std::unique_lock<std::mutex> lock{ m_mutex };
            return m_activeRequestCount;
        }
    }

    void PlayFabHCHttpPlugin::CompleteRequest(std::unique_ptr<CallRequestContainer> requestContainer)
    {
        requestContainer->finished = true;
        if (PlayFabSettings::threadedCallbacks)
        {
            // Invoke the callback on XTaskQueue completion port
            auto& asyncProvider = *static_cast<Provider*>(requestContainer->GetCustomData());
            // TODO fix leak potential here
            asyncProvider.TaskQueue().RunCompletion([ requestContainer{ requestContainer.release() } ]() mutable
            {
                PlayFabHCHttpPlugin::InvokeCallback(std::unique_ptr<CallRequestContainer>(requestContainer));
            });
        }
        else
        {
            // Stash the result and invoke callback when client calls Update
            std::unique_lock<std::mutex> lock{ m_mutex };
            m_completedRequests.push_back(std::move(requestContainer));
        }
    }

    void PlayFabHCHttpPlugin::InvokeCallback(std::unique_ptr<CallRequestContainer> requestContainer)
    {
        CallRequestContainer& reqContainer = *requestContainer;
        CallRequestContainerCallback callback = reqContainer.GetCallback();
        if (callback)
        {
            callback(
                reqContainer.responseJson.get("code", Json::Value::null).asInt(),
                reqContainer.responseString,
                std::unique_ptr<CallRequestContainerBase>(static_cast<CallRequestContainerBase*>(requestContainer.release()))
            );
        }
    }

    HCHttpCall::~HCHttpCall() noexcept
    {
        if (m_callHandle)
        {
            HCHttpCallCloseHandle(m_callHandle);
        }
    }

    void HCHttpCall::Perform(std::unique_ptr<CallRequestContainer> requestContainer)
    {
        std::unique_ptr<HCHttpCall> call{ new HCHttpCall };
        call->Initialize(std::move(requestContainer));

        auto asyncProvider = static_cast<Provider*>(call->m_requestContainer->GetCustomData());
        call->m_asyncBlock.callback = HCPerformComplete;
        call->m_asyncBlock.context = call.get();
        if (asyncProvider)
        {
            call->m_queue = asyncProvider->TaskQueue();
        }
        call->m_asyncBlock.queue = call->m_queue.GetHandle();

        HRESULT hr = HCHttpCallPerformAsync(call->m_callHandle, &call->m_asyncBlock);
        if (SUCCEEDED(hr))
        {
            call.release();
        }
    }

    HRESULT HCHttpCall::Initialize(std::unique_ptr<CallRequestContainer> requestContainer)
    {
        RETURN_HR_IF_FAILED(HCHttpCallCreate(&m_callHandle));
        RETURN_HR_IF_FAILED(HCHttpCallRequestSetUrl(m_callHandle, "POST", requestContainer->GetFullUrl().data()));

        // Default PlayFab headers
        HCHttpCallRequestSetHeader(m_callHandle, "Accept", "application/json", true);
        HCHttpCallRequestSetHeader(m_callHandle, "Content-Type", "application/json; charset=utf-8", true);
        HCHttpCallRequestSetHeader(m_callHandle, "X-PlayFabSDK", PlayFabSettings::versionString.data(), true);
        HCHttpCallRequestSetHeader(m_callHandle, "X-ReportErrorAsSuccess", "true", true);

        for (const auto& pair : requestContainer->GetRequestHeaders())
        {
            if (!pair.first.empty() && !pair.second.empty())
            {
                HCHttpCallRequestSetHeader(m_callHandle, pair.first.data(), pair.second.data(), true);
            }
        }

        HCHttpCallRequestSetRequestBodyString(m_callHandle, requestContainer->GetRequestBody().data());

        m_requestContainer = std::move(requestContainer);

        return S_OK;
    }

    void HCHttpCall::HCPerformComplete(XAsyncBlock* async)
    {
        // Re-Take ownership of HCHttpCall released in Perform
        std::unique_ptr<HCHttpCall> call{ static_cast<HCHttpCall*>(async->context) };
        CallRequestContainer& reqContainer = *call->m_requestContainer;

        // Get Http code and response string
        uint32_t httpCode{ 0 };
        HCHttpCallResponseGetStatusCode(call->m_callHandle, &httpCode);

        const char* responseString{ nullptr };
        HCHttpCallResponseGetResponseString(call->m_callHandle, &responseString);
        reqContainer.responseString = responseString;

        Json::CharReaderBuilder jsonReaderFactory;
        std::unique_ptr<Json::CharReader> jsonReader(jsonReaderFactory.newCharReader());
        JSONCPP_STRING jsonParseErrors;
        const bool parsedSuccessfully = jsonReader->parse(reqContainer.responseString.c_str(), reqContainer.responseString.c_str() + reqContainer.responseString.length(), &reqContainer.responseJson, &jsonParseErrors);
        if (parsedSuccessfully)
        {
            // fully successful response
            reqContainer.errorWrapper.HttpCode = reqContainer.responseJson.get("code", Json::Value::null).asInt();
            reqContainer.errorWrapper.HttpStatus = reqContainer.responseJson.get("status", Json::Value::null).asString();
            reqContainer.errorWrapper.Data = reqContainer.responseJson.get("data", Json::Value::null);
            reqContainer.errorWrapper.ErrorName = reqContainer.responseJson.get("error", Json::Value::null).asString();
            reqContainer.errorWrapper.ErrorCode = static_cast<PlayFabErrorCode>(reqContainer.responseJson.get("errorCode", Json::Value::null).asInt());
            reqContainer.errorWrapper.ErrorMessage = reqContainer.responseJson.get("errorMessage", Json::Value::null).asString();
            reqContainer.errorWrapper.ErrorDetails = reqContainer.responseJson.get("errorDetails", Json::Value::null);
        }
        else
        {
            reqContainer.errorWrapper.HttpCode = httpCode;
            reqContainer.errorWrapper.HttpStatus = reqContainer.responseString;
            reqContainer.errorWrapper.ErrorCode = PlayFabErrorCode::PlayFabErrorPartialFailure;
            reqContainer.errorWrapper.ErrorName = "Failed to parse PlayFab response";
            reqContainer.errorWrapper.ErrorMessage = jsonParseErrors;
        }

        //reqContainer.errorWrapper.RequestId = std::move(requestId);

        // TODO consider refactoring this somehow
        auto hcHttpPlugin = PlayFabPluginManager::GetPlugin<PlayFabHCHttpPlugin>(PlayFabPluginContract::PlayFab_Transport);
        hcHttpPlugin->CompleteRequest(std::move(call->m_requestContainer));
    }
}