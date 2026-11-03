using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class StalkState : State
{
    // Stalk Pacman from behind, attack if he approaches
    
    // When pacman eat's the power pellet, target him from the front and stay at double speed the whole time lol
    public StalkState(): base("Stalk") { }
    
    public override void EnterState(FSMAgent agent)
    {
        agent.SetTimer(3f);
    }

    public override void ExitState(FSMAgent agent)
    {
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

        //If timer complete, quickly burst at pacman
        if (agent.TimerComplete())
        {
            return new BurstState();
        }

        //If Pacman ate a power pellet, go to Taunt State
        if (PelletHandler.Instance.JustEatenPowerPellet)
        {
            return new TauntState(this);
        }
        
        // Else hover behind pacman
        agent.SetTarget(pacmanLocation - PacmanInfo.Instance.Facing * ObstacleHandler.Instance.GridSize * 4);

        //Stay in this state
        return this;
    }
}
