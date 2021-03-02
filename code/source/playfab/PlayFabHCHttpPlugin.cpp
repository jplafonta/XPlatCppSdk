#include <stdafx.h>
#include <playfab/PlayFabHCHttpPlugin.h>
#include <playfab/PlayFabSettings.h>

namespace PlayFab
{
    // RAII wrapper around HCCallHandle
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
        void CompleteRequest();

        static void CALLBACK HCPerformComplete(XAsyncBlock* async);

        HCCallHandle m_callHandle{ nullptr };
        std::unique_ptr<CallRequestContainer> m_requestContainer;
        XAsyncBlock m_asyncBlock{};
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
            requestContainer.release();
            HCHttpCall::Perform(std::unique_ptr<CallRequestContainer>{ container });
        }
    }

    size_t PlayFabHCHttpPlugin::Update()
    {
        throw PlayFabException(PlayFabExceptionCode::ThreadMisuse, "You should not call Update() when using XAsync");
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
        HRESULT hr = call->Initialize(std::move(requestContainer));
        if (FAILED(hr))
        {
            call->CompleteRequest();
            return;
        }

        call->m_asyncBlock.callback = HCPerformComplete;
        call->m_asyncBlock.context = call.get();
        call->m_asyncBlock.queue = call->m_requestContainer->m_queue.GetHandle();

        hr = HCHttpCallPerformAsync(call->m_callHandle, &call->m_asyncBlock);
        if (SUCCEEDED(hr))
        {
            call.release();
        }
        else
        {
            call->CompleteRequest();
        }
    }

    HRESULT HCHttpCall::Initialize(std::unique_ptr<CallRequestContainer> requestContainer)
    {
        RETURN_HR_IF_FAILED(HCHttpCallCreate(&m_callHandle));
        RETURN_HR_IF_FAILED(HCHttpCallRequestSetUrl(m_callHandle, "POST", requestContainer->GetFullUrl().data()));

        // Default PlayFab headers
        RETURN_HR_IF_FAILED(HCHttpCallRequestSetHeader(m_callHandle, "Accept", "application/json", true));
        RETURN_HR_IF_FAILED(HCHttpCallRequestSetHeader(m_callHandle, "Content-Type", "application/json; charset=utf-8", true));
        RETURN_HR_IF_FAILED(HCHttpCallRequestSetHeader(m_callHandle, "X-PlayFabSDK", PlayFabSettings::versionString.data(), true));
        RETURN_HR_IF_FAILED(HCHttpCallRequestSetHeader(m_callHandle, "X-ReportErrorAsSuccess", "true", true));

        for (const auto& pair : requestContainer->GetRequestHeaders())
        {
            if (!pair.first.empty() && !pair.second.empty())
            {
                RETURN_HR_IF_FAILED(HCHttpCallRequestSetHeader(m_callHandle, pair.first.data(), pair.second.data(), true));
            }
        }

        RETURN_HR_IF_FAILED(HCHttpCallRequestSetRequestBodyString(m_callHandle, requestContainer->GetRequestBody().data()));
        m_requestContainer = std::move(requestContainer);

        return S_OK;
    }

    void HCHttpCall::CompleteRequest()
    {
        m_requestContainer->finished = true;

        // TODO fix leak potential here
        auto& queue{ m_requestContainer->m_queue };
        queue.RunCompletion([requestContainer{ m_requestContainer.release() }]()
        {
            CallRequestContainerCallback callback = requestContainer->GetCallback();
            if (callback)
            {
                callback(
                    requestContainer->responseJson.get("code", Json::Value::null).asInt(),
                    requestContainer->responseString,
                    std::unique_ptr<CallRequestContainerBase>(static_cast<CallRequestContainerBase*>(requestContainer))
                );
            }
        });
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

        call->CompleteRequest();
    }
}