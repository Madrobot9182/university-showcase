using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TauntState : State
{
    // When pacman eat's the power pellet, target him from the front and stay at double speed the whole time lol
    private State _returnState;
    private Vector3 lastPosition;
    public TauntState(State returnState) : base("Taunt")
    {
        _returnState = returnState;
    }
    
    public override State Update(FSMAgent agent)
    {
        //Check and see if we've been eaten, if so become eyes
        Vector3 pacmanLocation = PacmanInfo.Instance.transform.position;
        if (agent.CloseEnough(pacmanLocation))
        {
            return new EyesState(_returnState);
        }

        //Check and see if our timer completed, if so return to returnState
        if (agent.TimerComplete())
        {
            return _returnState;
        }
        
        // Target in front of pacman
        float dist = (agent.GetPosition() - pacmanLocation).sqrMagnitude;
        Vector3 target = pacmanLocation + PacmanInfo.Instance.Facing * ObstacleHandler.Instance.GridSize * 4;
        
        // Turn around if we're too close to pacman and they're facing away
        if (dist <= ObstacleHandler.Instance.GridSize * 3 && 
            PacmanInfo.Instance.Facing != (agent.GetPosition() - lastPosition))
        {
            target = (-target);
        }
        agent.SetTarget(target);
        
        //Stay in this state
        return this;
    }

    //Upon entering state, start a timer, set speed modifier to double, and set up animations
    public override void EnterState(FSMAgent agent)
    {
        agent.SetAnimationStateFrightened();
        agent.SetSpeedModifierDouble();
        agent.SetTimer(12);

        lastPosition = agent.GetPosition();
    }

    //Upon exiting state, set speed and animation state to normal
    public override void ExitState(FSMAgent agent)
    {
        agent.SetAnimationStateNormal();
        agent.SetSpeedModifierNormal();
    }
}
