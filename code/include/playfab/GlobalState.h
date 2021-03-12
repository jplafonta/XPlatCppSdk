#pragma once

#include <playfab_c/PlayFabBaseModel_c.h>
#include <playfab/ClientLoginApi.h>

namespace PlayFab
{

// Any global state will go here
struct GlobalState 
{
    GlobalState() = default;
    virtual ~GlobalState() = default;

    ClientLoginApi clientLoginApi;
};

}

struct PlayFabGlobalState
{
    PlayFab::SharedPtr<PlayFab::GlobalState> state;

    static HRESULT Create(_Outptr_ PlayFabStateHandle* stateHandle);
    HRESULT CleanupAsync(XAsyncBlock* async);

private:
    PlayFabGlobalState();
};
