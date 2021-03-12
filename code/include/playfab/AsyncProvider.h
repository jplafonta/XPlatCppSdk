// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include <XAsyncProvider.h>
#include <playfab/TaskQueue.h>
#include <playfab/PlayFabError.h>
#include <playfab/PlayFabUser.h>

namespace PlayFab
{

// Provider is a base class that makes implementing XAsync API providers easier. It is designed to reduce the 
// amount of boilerplate and edge cases each provider needs to implement.
// 
// Each XAsync API should define a derived provider class and override the relevant operations: Begin, DoWork,
// and GetResult.
class Provider
{
public:
    Provider(const Provider&) = delete;
    Provider& operator=(const Provider&) = delete;
    virtual ~Provider() noexcept = default;

    // Runs an XAsync Provider. After calling Run, the provider operations Begin, DoWork, and GetResult will 
    // be called  by the XAsync framework. The lifetime of the of provider class will be managed by the Provider
    // base class; it will be destroyed automatically after the operation is complete and the client has gotten
    // the result.
    static HRESULT Run(_In_ UniquePtr<Provider>&& provider) noexcept;

protected:
    // Create a Provider from a client provided XAsyncBlock
    Provider(_In_ XAsyncBlock* async) noexcept;

    // Provider Operations to be overriden.

    // The Begin operation should start the asynchronous task, either by calling Schedule(), or by calling
    // another asynchronous API. Begin will be invoked synchronously by Run(), so this operation should never block.
    //
    // Default implementation will call Schedule with no delay.
    virtual HRESULT Begin(TaskQueue&& queue);

    // Perform any long running work. This method is invoked  is guaranteed always be invoked on the thread dictated by the Provider's
    // XAsync task queue.
    //
    // Default implementation will return S_OK.
    virtual HRESULT DoWork(TaskQueue&& queue);

    // The GetResult operation should copy the result payload into a client provided buffer. GetResult will be called
    // synchronously when the client calls the appropriate get result API.
    //
    // Providers implementing XAsync APIs with a result payload MUST override this method.
    // Default implementation will assert and return E_UNEXPECTED (it will never be invoked for XAsync APIs with
    // no result payload).
    virtual HRESULT GetResult(void* buffer, size_t bufferSize);

    // Schedule DoWork operation to the TaskQueue. Can be called multiple times.
    virtual HRESULT Schedule(uint32_t delay) const;

protected:
    // Methods to be called by derived classes to mark XAsyncOp as complete

    // Mark the async operation as sucessfully completed. Provide the needed buffer size to hold the result object.
    void Complete(size_t resultSize);

    // Marks the operation as complete with a failure code. By design, the client won't get a result payload
    // when the operation fails.  The Provider's GetResult method will never be invoked in this case.
    void Fail(HRESULT hr);

private:
    static HRESULT CALLBACK XAsyncProvider(_In_ XAsyncOp op, _Inout_ const XAsyncProviderData* data) noexcept;
    XAsyncBlock* m_async{ nullptr };
};

// Class used to implement XAsync Providers for PlayFab Client APIs.
template<typename RequestT, typename ResultT>
class ClientApiProvider : public Provider
{
public:
    // Wrapped PlayFab client API must have a signature matching ClientApiT
    using ClientApiT = void (PlayFabClientInstanceAPI::*)(const RequestT&, ProcessApiCallback<ResultT>, const TaskQueue&, const ErrorCallback, void*);

    ClientApiProvider(XAsyncBlock* async, SharedPtr<User> user, ClientApiT clientApi, const RequestT& request)
        : Provider{ async },
        m_user{ std::move(user) },
        m_clientApi{ clientApi },
        m_request{ std::move(request) }
    {
    }

protected:
    HRESULT Begin(TaskQueue&& queue) override
    {
        (m_user->ClientApi.*m_clientApi)(std::move(m_request), ProcessApiCallback, queue, OnError, this);
        return S_OK;
    }

    HRESULT GetResult(void* buffer, size_t bufferSize) override
    {
        assert(bufferSize == sizeof(PlayFabResultHolder*));
        auto resultHolder = static_cast<PlayFabResultHolder**>(buffer);
        *resultHolder = new PlayFabResultHolder{ m_result };
        return S_OK;
    }

private:
    static void ProcessApiCallback(const ResultT& result, void* context)
    {
        auto pThis = static_cast<ClientApiProvider*>(context);
        // Hold result until client calls GetResult API
        pThis->m_result = std::make_shared<ResultT>(result);
        pThis->Complete(sizeof(PlayFabResultHolder*));
    }

    // Default error handler for PlayFab Client APIs
    static void OnError(const PlayFabError& /*error*/, void* context) noexcept
    {
        auto pThis = static_cast<ClientApiProvider*>(context);

        // TODO translate PlayFabError to HRESULT
        pThis->Fail(E_FAIL);
    }

    SharedPtr<User> m_user;
    ClientApiT m_clientApi;
    const RequestT& m_request;
    SharedPtr<ResultT> m_result;
};

// Class used to implement XAsync Providers for PlayFab Client APIs.
template<typename RequestT, typename ResultT>
class ClientApiWithSerializableResultProvider : public Provider
{
public:
    static_assert(std::is_base_of_v<SerializableResult, ResultT>, "ResultT must be a PlayFab::SerializableResult");

    // Wrapped PlayFab client API must have a signature matching ClientApiT
    using ClientApiT = void (PlayFabClientInstanceAPI::*)(const RequestT&, ProcessApiCallback<ResultT>, const TaskQueue&, const ErrorCallback, void*);

    ClientApiWithSerializableResultProvider(XAsyncBlock* async, SharedPtr<User> user, ClientApiT clientApi, const RequestT& request)
        : Provider{ async },
        m_user{ std::move(user) },
        m_clientApi{ clientApi },
        m_request{ std::move(request) }
    {
    }

protected:
    HRESULT Begin(TaskQueue&& queue) override
    {
        (m_user->ClientApi.*m_clientApi)(std::move(m_request), ProcessApiCallback, queue, OnError, this);
        return S_OK;
    }

    HRESULT GetResult(void* buffer, size_t bufferSize) override
    {
        m_result.Serialize(buffer, bufferSize);
        return S_OK;
    }

private:
    static void ProcessApiCallback(const ResultT& result, void* context)
    {
        auto pThis = static_cast<ClientApiWithSerializableResultProvider*>(context);
        pThis->m_result = result;
        pThis->Complete(pThis->m_result.RequiredBufferSize());
    }

    static void OnError(const PlayFabError& /*error*/, void* context) noexcept
    {
        auto pThis = static_cast<ClientApiWithSerializableResultProvider*>(context);
        pThis->Fail(E_FAIL);
    }

    SharedPtr<User> m_user;
    ClientApiT m_clientApi;
    const RequestT& m_request;
    ResultT m_result;
};

// Class used to implement XAsync Providers for PlayFab Login APIs.
template<typename RequestT>
class ClientLoginApiProvider : public Provider
{
public:
    using LoginApiT = void (ClientLoginApi::*)(const RequestT&, ProcessApiCallback<ClientModels::LoginResult>, const TaskQueue&, const ErrorCallback, void*);

    ClientLoginApiProvider(XAsyncBlock* async, SharedPtr<GlobalState> state, LoginApiT loginApi, const RequestT& request)
        : Provider{ async },
        m_api{ loginApi },
        m_request{ std::move(request) }
    {
    }

protected:
    HRESULT Begin(TaskQueue&& queue) override
    {
        (m_state->clientLoginApi.*m_api)(m_request, ProcessApiCallback, queue, OnError, this);
        return S_OK;
    }

    HRESULT GetResult(void* buffer, size_t bufferSize) override
    {
        assert(bufferSize == sizeof(PlayFabResultHolder*));
        auto resultHolder = static_cast<PlayFabResultHolder**>(buffer);
        *resultHolder = new PlayFabResultHolder{ m_result };
        return S_OK;
    }

private:
    static void ProcessApiCallback(const ClientModels::LoginResult& result, void* context)
    {
        auto pThis = static_cast<ClientLoginApiProvider*>(context);
        // Hold result until client calls GetResult API
        pThis->m_result = MakeShared<ClientModels::LoginResult>(result);
        pThis->Complete(sizeof(PlayFabResultHolder*));
    }

    // Default error handler for PlayFab Client APIs
    static void OnError(const PlayFabError& /*error*/, void* context) noexcept
    {
        auto pThis = static_cast<ClientLoginApiProvider*>(context);

        // TODO translate PlayFabError to HRESULT
        pThis->Fail(E_FAIL);
    }

    SharedPtr<GlobalState> m_state;
    LoginApiT m_api;
    const RequestT& m_request;
    SharedPtr<ClientModels::LoginResult> m_result;
};

} // namespace PlayFab
