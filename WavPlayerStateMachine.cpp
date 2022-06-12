#include "WavPlayerStateMachine.h"

void WavPlayerStateMachine::Init()
{
    SetState(SELECTION);
}

WavPlayerStateMachine::State WavPlayerStateMachine::GetState()
{
    return currentState;
}

void WavPlayerStateMachine::SetState(State newState)
{
    switch (newState)
    {
        case IDLE:
        {
            break;
        }
        case SELECTION:
        {
            break;
        }
        case PERFORMANCE:
        {
            break;
        }
        default:
        break;
    }
    currentState = newState;
}