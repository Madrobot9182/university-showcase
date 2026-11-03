using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ClydeState : State
{

    public ClydeState() : base("Clyde") { }

    public override void EnterState(FSMAgent agent)
    {
        agent.SetTimer(20f);
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

        //If timer complete, go to Scatter State
        if (agent.TimerComplete())
        {
            return new ScatterState(new Vector3(ObstacleHandler.Instance.XBound * -1, ObstacleHandler.Instance.YBound * -1), this);
        }

        //If Pacman ate a power pellet, go to Frightened State
        if (PelletHandler.Instance.JustEatenPowerPellet)
        {
            return new FrightenedState(this);
        }
        
        //If pacman is within 8 straight cells, scatter
        //if (pacmanLocation == agent.GetPosition())
        if (Mathf.Sqrt(Mathf.Pow(pacmanLocation.x - agent.GetPosition().x, 2) + Mathf.Pow(pacmanLocation.y - agent.GetPosition().y, 2))
            <= ObstacleHandler.Instance.GridSize * 8)
        {
            return new ScatterState(new Vector3(ObstacleHandler.Instance.XBound * -1, ObstacleHandler.Instance.YBound * -1), this);
        }
        
        // Else just target pacman
        agent.SetTarget(pacmanLocation);

        //Stay in this state
        return this;
    }
}
