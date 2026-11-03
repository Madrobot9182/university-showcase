using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Level
{
    //These two are the main datastructures for this class, and ones you'll have to interact with
    public List<ProtoPellet> pellets;//List of fake "proto" pellets, specifying where a pellet will be and whether it will be a power pellet
    public List<ProtoObstacle> obstacles;//List of fake "proto" obstacles, specifying where an obstacle will be, its shape, colour, and line width

    public Vector3 ghostStartPos = AgentConstants.GHOST_START_POS;//Where you want the ghosts to spawn. Note: You do not need to edit these.
    public Vector3 pacmanStartPos = AgentConstants.PACMAN_START_POS;//Where you want pacman to start. Note: You do not need to edit these.

    //Handles how transitions are allowed in this level
    public Dictionary<string, GraphNode> nodeDictionary;

    public float fitness;//Only used for the reduce function, if you use the optimizer, you can ignore this or use it for whatever you want
    

    /**
     *
     * All the difference constructors for ways to instantiate levels
     */

    public Level()
    {
        pellets = new List<ProtoPellet>();
        obstacles = new List<ProtoObstacle>();
        nodeDictionary = new Dictionary<string, GraphNode>();
    }

    public Level(List<ProtoPellet> _pellets, List<ProtoObstacle> _obstacles, Dictionary<string, GraphNode> _nodeDictionary)
    {
        pellets = _pellets;
        obstacles = _obstacles;
        nodeDictionary = _nodeDictionary;
    }

    public Level(List<ProtoPellet> _pellets, List<ProtoObstacle> _obstacles, Dictionary<string, GraphNode> _nodeDictionary, Vector3 _ghostStartPos)
    {
        pellets = _pellets;
        obstacles = _obstacles;
        nodeDictionary = _nodeDictionary;
        ghostStartPos = _ghostStartPos;
    }

    public Level(List<ProtoPellet> _pellets, List<ProtoObstacle> _obstacles, Dictionary<string, GraphNode> _nodeDictionary, Vector3 _ghostStartPos, Vector3 _pacmanStartPos)
    {
        pellets = _pellets;
        obstacles = _obstacles;
        nodeDictionary = _nodeDictionary;
        ghostStartPos = _ghostStartPos;
        pacmanStartPos = _pacmanStartPos;
    }

    //Clone function, useful for producing mutations
    public Level Clone()
    {
        List<ProtoPellet> copiedPellets = new List<ProtoPellet>();
        List<ProtoObstacle> copiedObstacles = new List<ProtoObstacle>();
        Dictionary<string, GraphNode> copiedNodeDictionary = new Dictionary<string, GraphNode>();

        foreach (ProtoPellet p in pellets)
        {
            copiedPellets.Add(p.Clone());
        }


        foreach (ProtoObstacle p in obstacles)
        {
            copiedObstacles.Add(p.Clone());
        }

        foreach (KeyValuePair< string, GraphNode > kvp in nodeDictionary){
            copiedNodeDictionary.Add(kvp.Key, kvp.Value);
        }

        return new Level(copiedPellets, copiedObstacles, copiedNodeDictionary, ghostStartPos, pacmanStartPos);
    }

    //Turns the obstacles into an array of obstacle points
    public Vector2[][] GetObstaclePoints()
    {
        List<Vector2[]> obstaclePoints = new List<Vector2[]>();
        foreach(ProtoObstacle p in obstacles)
        {
            List<Vector2> points = new List<Vector2>();

            for(int i = 0; i<p.shape.Length; i++)
            {
                points.Add(new Vector2(p.worldLocation.x + p.shape[i].x, p.worldLocation.y + p.shape[i].y));
            }

            obstaclePoints.Add(points.ToArray());
        }
        return obstaclePoints.ToArray();
    }

    
}
