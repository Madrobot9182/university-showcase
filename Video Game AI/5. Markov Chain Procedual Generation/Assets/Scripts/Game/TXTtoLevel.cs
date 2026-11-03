using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TXTtoLevel
{
    const float gridSize = 0.2f;//A grid cell's width

    private static Vector2[] squareObstacle = new Vector2[] { new Vector2(-1 * gridSize * 1.01f / 2f, -1 * gridSize * 1.01f / 2f), new Vector2(gridSize * 1.01f / 2f, -1 * gridSize * 1.01f / 2f), new Vector2(gridSize * 1.01f / 2f, gridSize * 1.01f / 2f), new Vector2(-1 * gridSize * 1.01f / 2f, gridSize * 1.01f / 2f) };
    private static Vector2[] leftWallObstacle = new Vector2[] { new Vector2(-1 * gridSize * 1.01f / 2f, -1 * gridSize * 1.01f / 2f), new Vector2(-1*gridSize * 1.01f / 2f, gridSize * 1.01f / 2f) };
    private static Vector2[] rightWallObstacle = new Vector2[] { new Vector2(gridSize * 1.01f / 2f, -1 * gridSize * 1.01f / 2f), new Vector2(gridSize * 1.01f / 2f, gridSize * 1.01f / 2f) };
    private static Vector2[] upWallObstacle = new Vector2[] { new Vector2(-1*gridSize * 1.01f / 2f, gridSize * 1.01f / 2f), new Vector2(gridSize * 1.01f / 2f, gridSize * 1.01f / 2f) };
    private static Vector2[] downWallObstacle = new Vector2[] { new Vector2(-1 * gridSize * 1.01f / 2f, -1 * gridSize * 1.01f / 2f), new Vector2(gridSize * 1.01f / 2f, -1*gridSize * 1.01f / 2f) };
    
    public static Level StringToLevel(string txtLevel)
    {

        //Initialize all the bits
        List<Vector3> ghostSpawnOptions = new List<Vector3>();
        List<Vector3> pacmanSpawnOptions = new List<Vector3>();

        List<ProtoPellet> pellets = new List<ProtoPellet>();
        List<ProtoObstacle> obstacles = new List<ProtoObstacle>();
        Dictionary<string, GraphNode> nodeDictionary = new Dictionary<string, GraphNode>();

        //Parse lines to collect information
        string[] lines = txtLevel.Split('\n');
        for(int y = 0; y<lines.Length; y++){
            for(int x = 0; x<lines[y].Length; x++){
                //Vector3 calculation
                float xPosition = ((float)(x - 5))*gridSize;
                float yPosition = ((float)(6 - y)) * gridSize;
                Vector3 position = new Vector3(xPosition, yPosition);

                //Add various objects
                if (lines[y][x] == 'G')
                {
                    ghostSpawnOptions.Add(position);
                }
                else if (lines[y][x] == 'S')
                {
                    pacmanSpawnOptions.Add(position);
                }
                else if (lines[y][x] == '-')
                {

                }
                else if (lines[y][x] == '.')
                {
                    pellets.Add(new ProtoPellet(position));
                }
                else if (lines[y][x] == 'o')
                {
                    pellets.Add(new ProtoPellet(position, true));
                }
                else if (lines[y][x] == 'X')
                {
                    obstacles.Add(new ProtoObstacle(position, squareObstacle));
                }
                else if (lines[y][x] == '[')
                {
                    obstacles.Add(new ProtoObstacle(position, leftWallObstacle));
                    pellets.Add(new ProtoPellet(position));
                }
                else if (lines[y][x] == ']')
                {
                    obstacles.Add(new ProtoObstacle(position, rightWallObstacle));
                    pellets.Add(new ProtoPellet(position));
                }
                else if (lines[y][x] == '\'')
                {
                    obstacles.Add(new ProtoObstacle(position, upWallObstacle));
                    pellets.Add(new ProtoPellet(position));
                }
                else if (lines[y][x] == '_')
                {
                    obstacles.Add(new ProtoObstacle(position, downWallObstacle));
                    pellets.Add(new ProtoPellet(position));
                }
                else if (lines[y][x] == 'P')
                {
                    obstacles.Add(new ProtoObstacle(position, leftWallObstacle));
                    obstacles.Add(new ProtoObstacle(position, upWallObstacle));
                    pellets.Add(new ProtoPellet(position));
                }
                else if (lines[y][x] == 'R')
                {
                    obstacles.Add(new ProtoObstacle(position, rightWallObstacle));
                    obstacles.Add(new ProtoObstacle(position, upWallObstacle));
                    pellets.Add(new ProtoPellet(position));
                }
                else if (lines[y][x] == 'L')
                {
                    obstacles.Add(new ProtoObstacle(position, leftWallObstacle));
                    obstacles.Add(new ProtoObstacle(position, downWallObstacle));
                    pellets.Add(new ProtoPellet(position));
                }
                else if (lines[y][x] == 'D')
                {
                    obstacles.Add(new ProtoObstacle(position, rightWallObstacle));
                    obstacles.Add(new ProtoObstacle(position, downWallObstacle));
                    pellets.Add(new ProtoPellet(position));
                }
                else if (lines[y][x] == '=')
                {
                    obstacles.Add(new ProtoObstacle(position, upWallObstacle));
                    obstacles.Add(new ProtoObstacle(position, downWallObstacle));
                    pellets.Add(new ProtoPellet(position));
                }
                else if (lines[y][x] == 'T')
                {
                    obstacles.Add(new ProtoObstacle(position, leftWallObstacle));
                    obstacles.Add(new ProtoObstacle(position, rightWallObstacle));
                    pellets.Add(new ProtoPellet(position));
                }
                else if (lines[y][x] == 'C')
                {
                    obstacles.Add(new ProtoObstacle(position, leftWallObstacle));
                    obstacles.Add(new ProtoObstacle(position, upWallObstacle));
                    obstacles.Add(new ProtoObstacle(position, downWallObstacle));
                    pellets.Add(new ProtoPellet(position));
                }
                else if (lines[y][x] == 'Q')
                {
                    obstacles.Add(new ProtoObstacle(position, rightWallObstacle));
                    obstacles.Add(new ProtoObstacle(position, upWallObstacle));
                    obstacles.Add(new ProtoObstacle(position, downWallObstacle));
                    pellets.Add(new ProtoPellet(position));
                }

                //Create nodes
                if(lines[y][x] != 'X')
                {
                    nodeDictionary.Add(position.ToString(), new GraphNode(position));

                    //Check for left to right
                    if (x - 1 >= 0)
                    {
                        Vector3 leftPosition = new Vector3(((float)(x-1 - 5)) * gridSize, yPosition);
                        char leftChar = lines[y][x - 1];
                        //Right
                        if (lines[y][x]=='G' || lines[y][x] == 'S' || lines[y][x] == '-' || lines[y][x] == '.' || lines[y][x] == 'o' || lines[y][x] == ']' || lines[y][x] == '\'' || lines[y][x] == '_' || lines[y][x] == 'R' || lines[y][x] == 'D' || lines[y][x] == '=' || lines[y][x] == 'Q')
                        {
                            //Left
                            if (leftChar == 'G' || leftChar == 'S' || leftChar == '-' || leftChar == '.' || leftChar == 'o' || leftChar == '[' || leftChar == '\'' || leftChar == '_' || leftChar == 'P' || leftChar == 'L' || leftChar == '=' || leftChar == 'C')
                            {
                                nodeDictionary[leftPosition.ToString()].AddNeighbor(nodeDictionary[position.ToString()]);
                                nodeDictionary[position.ToString()].AddNeighbor(nodeDictionary[leftPosition.ToString()]);
                            }
                        }
                    }

                    //Check for up to down
                    if (y - 1 >= 0)
                    {
                        Vector3 upPosition = new Vector3(xPosition, ((float)(6 - (y-1)) * gridSize));
                        char upChar = lines[y-1][x];
                        //Below
                        if (lines[y][x] == 'G' || lines[y][x] == 'S' || lines[y][x] == '-' || lines[y][x] == '.' || lines[y][x] == 'o' || lines[y][x] == '[' || lines[y][x] == ']' || lines[y][x] == '_' || lines[y][x] == 'L' || lines[y][x] == 'D' || lines[y][x] == 'T')
                        {
                            //Above
                            if (upChar == 'G' || upChar == 'S' || upChar == '-' || upChar == '.' || upChar == 'o' || upChar == '[' || upChar == ']' || upChar == '\'' || upChar == 'P' || upChar == 'R' || upChar == 'T')
                            {
                                nodeDictionary[upPosition.ToString()].AddNeighbor(nodeDictionary[position.ToString()]);
                                nodeDictionary[position.ToString()].AddNeighbor(nodeDictionary[upPosition.ToString()]);
                            }
                        }
                    }

                }
                
            }
        }
        //Pick spawn positions
        Vector3 ghostSpawn = AgentConstants.GHOST_START_POS;
        Vector3 pacmanSpawn = AgentConstants.PACMAN_START_POS;

        if (ghostSpawnOptions.Count>0)
        {
            int ghostIndex = Random.Range(0, ghostSpawnOptions.Count);
            ghostSpawn = ghostSpawnOptions[ghostIndex];
        }
        if (pacmanSpawnOptions.Count > 0)
        {
            int pacmanIndex = Random.Range(0, pacmanSpawnOptions.Count);
            pacmanSpawn = pacmanSpawnOptions[pacmanIndex];
        }

        return new Level(pellets, obstacles, nodeDictionary, ghostSpawn, pacmanSpawn);
    }
}
