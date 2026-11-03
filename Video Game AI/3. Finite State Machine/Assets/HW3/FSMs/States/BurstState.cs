using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BurstState : State
{
    // Quicky charge at pacman for a short amount of time
    public BurstState(): base("Burst") { }
    
    public override void EnterState(FSMAgent agent)
    {
        agent.SetTimer(0.8f);
        agent.SetSpeedModifierDouble();
    }

    public override void ExitState(FSMAgent agent)
    {
        agent.SetSpeedModifierNormal();
        base.ExitState(agent);
    }
    
    public override State Update(FSMAgent agent)
    {
        //Handle Following Pacman
        Vector3 pacmanLocation = PacmanInfo.Instance.transform.position;
        
        if (agent.CloseEnough(pacmanLocation))
        {
            ScoreHandler.Instance.KillPacman();
        }

        //If timer complete, Return to Stalk State
        if (agent.TimerComplete())
        {
            return new StalkState();
        }

        //If Pacman ate a power pellet, go to Taunt State
        if (PelletHandler.Instance.JustEatenPowerPellet)
        {
            return new TauntState(this);
        }
        
        // Else just charge at pacman
        agent.SetTarget(pacmanLocation);

        //Stay in this state
        return this;
    }
}
