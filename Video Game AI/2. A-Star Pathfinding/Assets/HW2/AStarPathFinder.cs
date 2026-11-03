using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AStarPathFinder : GreedyPathFinder
{
    public static int nodesOpened = 0;

    //ASSIGNMENT 2: EDIT BELOW THIS LINE, IMPLEMENT A*
    public override Vector3[] CalculatePath(GraphNode startNode, GraphNode goalNode)
    {
        nodesOpened = 0;

        AStarNode start = new AStarNode(null, startNode, Heuristic(startNode, goalNode));
        float gScore = 0;

        PriorityQueue<AStarNode> openSet = new PriorityQueue<AStarNode>();
        List<GraphNode> closeSet = new List<GraphNode>();   // To track repeats
        
        openSet.Enqueue(start);
        //Dictionary<string, float> gScores = new Dictionary<string, float>();
        Dictionary<Vector3, AStarNode> nodeCache = new Dictionary<Vector3, AStarNode>();
            
        int attempts = 0;
        
        while (openSet.Count() > 0 && attempts<10000)
        {
            attempts += 1;
            AStarNode currNode = openSet.Dequeue();

            //Did we find the goal?
            if (currNode.Location == goalNode.Location)
            {
                Debug.Log("CHECKED " + nodesOpened + " NODES");
                //Reconstruct the path?
                return ReconstructPath(start, currNode);
            }

            closeSet.Add(currNode.GraphNode);
                
            // Check each neighbor
            foreach (GraphNode neighbour in currNode.GraphNode.Neighbors)
            {
                if (closeSet.Contains(neighbour))   // Already visited
                {
                    Debug.Log("Close skip " + neighbour.Location);
                    continue;
                }

                gScore = currNode.GScore + 1; 
                
                // If there is a cached node, check if the neighbor gScore is higher and skip it (unnecessary)
                if (nodeCache.ContainsKey(neighbour.Location) && gScore >= nodeCache[neighbour.Location].GScore)
                {
                    continue;
                }
                
                AStarNode aStarNeighbour = new AStarNode(currNode, neighbour, Heuristic(neighbour, goalNode));

                // Check if the neighbor is already in the open set
                if (!openSet.Data.Contains(aStarNeighbour))
                {
                    openSet.Enqueue(aStarNeighbour);
                    nodeCache[neighbour.Location] = aStarNeighbour;  // Store the current best Node Value in cache
                } 
                else if (gScore < nodeCache[neighbour.Location].GScore)     // We found a better path to this neighbour
                {
                    // Swap the old node with new node
                    openSet.Remove(nodeCache[neighbour.Location]);      // Using the cache to save on AStarNode initialization
                    openSet.Enqueue(aStarNeighbour);
                    nodeCache[neighbour.Location] = aStarNeighbour;    // Update the best value in cache
                }
            }
        }
        Debug.Log("CHECKED "+ nodesOpened + " NODES");
        return null;
    }
    //ASSIGNMENT 2: EDIT ABOVE THIS LINE, IMPLEMENT A*

    //EXTRA CREDIT ASSIGNMENT 2 EDIT BELOW THIS LINE
    public float Heuristic(GraphNode currNode, GraphNode goalNode)
    {
        float normalize = 1 / ObstacleHandler.Instance.GridSize;
        float mathatten = Mathf.Abs(currNode.Location.x-goalNode.Location.x) + Mathf.Abs(currNode.Location.y - goalNode.Location.y);
        mathatten *= normalize; // GScore increments by one, so should heuristic
        if (ObstacleHandler.Instance.AnyIntersect(currNode.Location, goalNode.Location))
        {
            // Punish paths crossing obstacles
            mathatten *= 2f;
        }

        return mathatten;
    }
    //EXTRA CREDIT ASSIGNMENT 2 EDIT ABOVE THIS LINE

    //Code for reconstructing the path, don't edit this.
    private Vector3[] ReconstructPath(AStarNode startNode, AStarNode currNode)
    {
        List<Vector3> backwardsPath = new List<Vector3>();

        while (currNode != startNode)
        {
            backwardsPath.Add(currNode.Location);
            currNode = currNode.Parent;
        }
        backwardsPath.Reverse();

        return backwardsPath.ToArray();
    }
}



