// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdafx.h>
#include <assert.h>
#include "async_provider.h"
#include "playfab_gdk/GDK_PlayFabClientApi.h"

namespace PlayFab
{

Provider::Provider(_In_ XAsyncBlock* async) noexcept
    : m_async{ async }
{
}

HRESULT Provider::Run(_In_ std::unique_ptr<Provider>&& provider) noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(provider->m_async);
    RETURN_HR_IF_FAILED(XAsyncBegin(provider->m_async, provider.get(), nullptr, nullptr, XAsyncProvider));
    provider.release();
    return S_OK;
}

TaskQueue Provider::TaskQueue() const noexcept
{
    return m_async->queue;
}

HRESULT Provider::Begin()
{
    return Schedule(0);
}

HRESULT Provider::DoWork()
{
    return S_OK;
}

HRESULT Provider::GetResult(void*, size_t)
{
    assert(false);
    return E_UNEXPECTED;
}

HRESULT Provider::Schedule(uint32_t delay) const
{
    return XAsyncSchedule(m_async, delay);
}

void Provider::Complete(size_t resultSize)
{
    XAsyncComplete(m_async, S_OK, resultSize);
}

void Provider::Fail(HRESULT hr)
{
    XAsyncComplete(m_async, hr, 0);
}
HRESULT CALLBACK Provider::XAsyncProvider(_In_ XAsyncOp op, _Inout_ const XAsyncProviderData* data) noexcept
{
    auto provider{ static_cast<Provider*>(data->context) };
    switch (op)
    {
    case XAsyncOp::Begin:
        try
        {
            return provider->Begin();
        }
        catch (...)
        {
            return E_UNEXPECTED;
        }
    case XAsyncOp::DoWork:
        try
        {
            return provider->DoWork();
        }
        catch (...)
        {
            return E_UNEXPECTED;
        }
    case XAsyncOp::GetResult:
        try
        {
            return provider->GetResult(data->buffer, data->bufferSize);
        }
        catch (...)
        {
            return E_UNEXPECTED;
        }
    case XAsyncOp::Cancel:
    {
        // TODO allow providers to implement cancellation
        return S_OK;
    }
    case XAsyncOp::Cleanup:
    {
        // Cleanup should only fail in catostrophic cases. Can't pass result to client 
        // at this point so die with exception.

        // Re-take ownership of provider, it will be destroyed here
        std::unique_ptr<Provider>{ provider };
        return S_OK;
    }
    default:
    {
        assert(false);
        return S_OK;
    }
    }
}

} // PlayFab