using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Linq;

//Assignment 1 Script
public class GridHandler : NodeHandler
{
    private float gridSize;

    //Holds all of the nodes
    private Dictionary<string, GraphNode> nodeDictionary;
    public override void CreateNodes()
    {
        nodeDictionary = new Dictionary<string, GraphNode>();

        //ASSIGNMENT 1 EDIT BELOW THIS LINE

        gridSize = ObstacleHandler.Instance.GridSize;//0.2f is the distance from the centre of one grid cell to an adjacent one

        bool validGrid = true;     // use to determine if the grid at the point is not intersecting an object
        for (float x = ObstacleHandler.Instance.XBound * -1; x <= ObstacleHandler.Instance.XBound + gridSize; x += gridSize) {
            for (float y = ObstacleHandler.Instance.YBound * -1; y <= ObstacleHandler.Instance.YBound + gridSize; y += gridSize) {
                Vector3 loc = new Vector3(x, y);
                
                //Is this grid cell (center) fully in an obstacle? Check the center point
                if (ObstacleHandler.Instance.PointInObstacles(loc)) {
                    validGrid = false;
                }
                
                //Does an obstacles lines intersect with the grid lines?
                Vector3[] gridCorners = new Vector3[4]
                    {
                        new Vector3(loc.x - gridSize/2, loc.y - gridSize/2),
                        new Vector3(loc.x + gridSize/2, loc.y - gridSize/2),
                        new Vector3(loc.x - gridSize/2, loc.y + gridSize/2),
                        new Vector3(loc.x + gridSize/2, loc.y + gridSize/2),
                    };
                
                // Check each corner and edge for intersection
                for (int i = 0; i < 4 && validGrid; i++)
                {
                    int next = (i+1) % 4;

                    if (ObstacleHandler.Instance.PointInObstacles(gridCorners[i]))
                    {
                        validGrid = false;
                    }
                    if (ObstacleHandler.Instance.AnyIntersect(gridCorners[i], gridCorners[next]))
                    {
                        validGrid = false;
                    }
                }
                
                //Is there an obstacle fully inside the grid? Sample points within the cell, check for obstacles within
                for (float dx = -(gridSize / 2); dx < (gridSize / 2) && validGrid; dx+=gridSize/8)
                {
                    for (float dy = -(gridSize / 2); dy < (gridSize / 2) && validGrid; dy+=gridSize/8)
                    {
                        if (ObstacleHandler.Instance.PointInObstacles(new Vector2(x + dx, y + dy)))
                        {
                            validGrid = false;
                        }
                    }
                }
                
                // Add to dictionary if grid cell is valid
                if(validGrid){ nodeDictionary.Add(loc.ToString(), new GraphNode(loc));}
                validGrid = true;   // Reset for each point
            }
        }
        
        //ASSIGNMENT 1 EDIT ABOVE THIS LINE

        //Create Neighbors
        foreach (KeyValuePair<string, GraphNode> kvp in nodeDictionary)
        {
            //Left
            if (nodeDictionary.ContainsKey((kvp.Value.Location + (Vector3.left * gridSize)).ToString()))
            {
                kvp.Value.AddNeighbor(nodeDictionary[(kvp.Value.Location + (Vector3.left * gridSize)).ToString()]);
            }
            //Right
            if (nodeDictionary.ContainsKey((kvp.Value.Location + (Vector3.right * gridSize)).ToString()))
            {
                kvp.Value.AddNeighbor(nodeDictionary[(kvp.Value.Location + (Vector3.right * gridSize)).ToString()]);
            }
            //Up
            if (nodeDictionary.ContainsKey((kvp.Value.Location + (Vector3.up * gridSize)).ToString()))
            {
                kvp.Value.AddNeighbor(nodeDictionary[(kvp.Value.Location + (Vector3.up * gridSize)).ToString()]);
            }
            //Down
            if (nodeDictionary.ContainsKey((kvp.Value.Location + (Vector3.down * gridSize)).ToString()))
            {
                kvp.Value.AddNeighbor(nodeDictionary[(kvp.Value.Location + (Vector3.down * gridSize)).ToString()]);
            }
        }
    }

    public override void VisualizeNodes()
    {
        //Visualize grid points
        foreach (KeyValuePair<string, GraphNode> kvp in nodeDictionary)
        {
            //Draw left line
            Debug.DrawLine(kvp.Value.Location + Vector3.left * gridSize / 2f + Vector3.up * gridSize / 2f, kvp.Value.Location + Vector3.left * gridSize / 2f + Vector3.down * gridSize / 2f, Color.white);
            //Draw right line
            Debug.DrawLine(kvp.Value.Location + Vector3.right * gridSize / 2f + Vector3.up * gridSize / 2f, kvp.Value.Location + Vector3.right * gridSize / 2f + Vector3.down * gridSize / 2f, Color.white);
            //Draw top line
            Debug.DrawLine(kvp.Value.Location + Vector3.up * gridSize / 2f + Vector3.left * gridSize / 2f, kvp.Value.Location + Vector3.up * gridSize / 2f + Vector3.right * gridSize / 2f, Color.white);
            //Draw bottom line
            Debug.DrawLine(kvp.Value.Location + Vector3.down * gridSize / 2f + Vector3.left * gridSize / 2f, kvp.Value.Location + Vector3.down * gridSize / 2f + Vector3.right * gridSize / 2f, Color.white);
        }
    }

    //Find closest node (used for pathing)
    public override GraphNode ClosestNode(Vector3 position)
    {
        float minDist = 1000;
        GraphNode closest = null;
        foreach (KeyValuePair<string, GraphNode> kvp in nodeDictionary)
        {
            float dist = (kvp.Value.Location - position).sqrMagnitude;
            if (dist < minDist)
            {
                minDist = dist;
                closest = kvp.Value;
            }
        }
        return closest;
    }
}
