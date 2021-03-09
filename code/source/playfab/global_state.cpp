#include <stdafx.h>
#include <playfab/global_state.h>
#include <playfab/async_provider.h>
#include <httpClient/httpClient.h>

namespace PlayFab
{

GlobalState::GlobalState(const char* titleId) :
    m_settings{ MakeShared<PlayFabApiSettings>() }
{
    m_settings->titleId = titleId;
}

HRESULT GlobalState::Create(const char* titleId)
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(titleId);

    if (SingletonAccess())
    {
        // TODO define appropriate HRESULT in case GlobalState instance has already been created
        return E_UNEXPECTED;
    }

    // Initialize libHttpClient
    RETURN_HR_IF_FAILED(HCInitialize(nullptr));

    Allocator<GlobalState> a{};
    auto state = SharedPtr<GlobalState>(new (a.allocate(1)) GlobalState(titleId), Deleter<GlobalState>{}, a);

    // Have GlobalState hold a self reference to ensure it isn't cleaned up until CleanupAsync is called
    state->m_self = state;

    SingletonAccess(true, state);

    return S_OK;
}

HRESULT GlobalState::CleanupAsync(XAsyncBlock* async)
{
    struct CleanupProvider : public Provider
    {
        CleanupProvider(XAsyncBlock* async) : Provider(async) 
        {
        }

        HRESULT Begin(TaskQueue&& queue) override
        {
            auto state = SingletonAccess(true, nullptr).get(); // intentional raw ptr
            if (!state)
            {
                // TODO define appropriate HRESULT in case GlobalState hasn't been created
                return E_UNEXPECTED;
            }
            else if (state->m_self.use_count() > 1)
            {
                // Need to decide if it is client's responsibility to wait for any pending API calls to complete before
                // calling cleanup or if we should wait here
                return E_FAIL;
            }
            else
            {
                // m_self is the only remaining reference. GlobalState will be destroyed on this thread before
                // control returns to client
                state->m_self.reset();

                // Cleanup libHttpClient
                hcCleanupAsync.queue = queue.DeriveWorkerQueue().GetHandle();
                hcCleanupAsync.callback = HCCleanupComplete;
                hcCleanupAsync.context = this;

                RETURN_HR_IF_FAILED(HCCleanupAsync(&hcCleanupAsync));

                return E_PENDING;
            }
        }

        static void CALLBACK HCCleanupComplete(XAsyncBlock* async)
        {
            CleanupProvider* self = static_cast<CleanupProvider*>(async->context);
            assert(self);

            HRESULT hr = XAsyncGetStatus(async, false);
            assert(hr != E_PENDING);

            if (hr == E_HC_INTERNAL_STILLINUSE)
            {
                // If something else is still referencing libHttpClient, we do not care
                self->Complete(0);
                return;
            }
            else if (FAILED(hr))
            {
                // Pass along HCCleanup error
                self->Fail(hr);
                return;
            }

            self->Complete(0);
        }

        XAsyncBlock hcCleanupAsync{};
    };

    return Provider::Run(UniquePtr<Provider>{ MakeUnique<CleanupProvider>(async).release() });
}

SharedPtr<GlobalState> GlobalState::Get()
{
    return SingletonAccess();
}

SharedPtr<PlayFabApiSettings> GlobalState::Settings() const
{
    return m_settings;
}

SharedPtr<GlobalState> GlobalState::SingletonAccess(bool setValue, SharedPtr<GlobalState> newValue)
{
    static std::mutex s_mutex{};
    static SharedPtr<GlobalState> s_state{ nullptr };

    std::lock_guard<std::mutex> lock{ s_mutex };

    auto state = s_state;
    if (setValue)
    {
        s_state = newValue;
    }
    return state;
}

}