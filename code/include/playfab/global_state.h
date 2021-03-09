#pragma once

namespace PlayFab
{

class GlobalState
{
public:
    static HRESULT Create(const char* titleId);
    static HRESULT CleanupAsync(XAsyncBlock* async);
    static SharedPtr<GlobalState> Get();

    SharedPtr<PlayFabApiSettings> Settings() const;

private:
    GlobalState(const char* titleId);

    SharedPtr<GlobalState> m_self;
    SharedPtr<PlayFabApiSettings> m_settings;

    static SharedPtr<GlobalState> SingletonAccess(bool setValue = false, SharedPtr<GlobalState> newValue = nullptr);
};

}