#pragma once

#include "WavPlayer.h"

class UIContext
{
    public:
     UIContext() = default;
    virtual ~UIContext() = default;
    virtual void UpdateUI()=0;
 
    private:

};

class WavPlayerStateMachine
{
    public:
    WavPlayerStateMachine()  = default;
    ~WavPlayerStateMachine() = default;
    enum State
    {
        IDLE = 0,
        SELECTION,
        PERFORMANCE 
    };

    void Init();
    State GetState();
    void SetState(State newState);

    private:
    State currentState;
};