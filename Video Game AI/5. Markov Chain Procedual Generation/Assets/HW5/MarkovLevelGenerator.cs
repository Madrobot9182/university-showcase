using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

public class MarkovLevelGenerator : LevelGenerator
{

    //Markov Chain represenation
    private Dictionary<string, Dictionary<char, float>> markovChain;
    public Dictionary<string, Dictionary<char, float>> MarkovChain { get { return markovChain; } }

    //null token for if we don't have required state info (probably don't change this)
    string nullToken = "0";

    //All possible level characters (ignorable)
    char[] levelChars = new char[] { 'G', 'S', '-', '.', 'o', 'X', '[', ']', '\'', '_', 'P', 'R', 'L', 'D', '=', 'T', 'C', 'Q' };

    //Clusters used for the visualization (ignorable)
    private Color32[] obstacleColours = new Color32[] { new Color32(250, 230, 190, 255), new Color32(160, 250, 130, 255), new Color32(240, 240, 50, 255), new Color32(255, 120, 120, 255), new Color32(170, 10, 60, 255), new Color32(20, 210, 220, 255), new Color32(250, 120, 250, 255), new Color32(0, 90, 200, 255), new Color32(130, 20, 160, 255) };


    /**
    * Function for training the Markov Chain, to be implemented by the student
    */
    private void TrainMarkovChain(TextAsset[] levels)//TODO; complete this function
    {
        //Initialize markov chain if we're about to train or retrain it
        markovChain = new Dictionary<string, Dictionary<char, float>>();


        //For tracking counts (optional to use, but might be helpful)
        Dictionary<string, float> keyCounts = new Dictionary<string, float>();

        //Iterate through each level to get counts
        foreach (TextAsset level in levels)
        {
            for (int y = 0; y < 12; y++)
            {
                for (int x = 0; x < 11; x++)
                {
                    string state = GetMarkovState(level.text, x, y);
                    char currentChar = level.text.Split('\n')[y][x];
                    
                    // Use markov chain as temporary counter for state action pair
                    if (!markovChain.ContainsKey(state))
                    {
                        markovChain.Add(state, new Dictionary<char, float>());
                    }

                    if (!markovChain[state].ContainsKey(currentChar))
                    {
                        markovChain[state].Add(currentChar, 0f);
                    }
                    markovChain[state][currentChar]++;    // Then increment count
                
                    
                    // Then track state count
                    if (!keyCounts.ContainsKey(state))
                    {
                        keyCounts.Add(state, 0f);
                    }
                    keyCounts[state]++;
                }
            }
        }
        
        
        // Calculate probabilities
        foreach (var state in markovChain.Keys.ToArray())
        {
            foreach (var c in markovChain[state].Keys.ToArray())
            {
                markovChain[state][c] = markovChain[state][c] / keyCounts[state];
            }
        }
    }

    /*
     * State representation for your Markov chain
     */
    public override Level GenerateLevel()
    {
        //Train MarkovChain if untrained
        if (markovChain==null)
        {
            TrainMarkovChain(trainingLevels);
        }

        //Start putting together the string representation of a level
        string newLevel = "";
        //go from top to bottom
        for (int y = 0; y < 12; y++)
        {
            string row = "";
            //go from left to right
            for (int x = 0; x < 11; x++)
            {
                //grab the state
                string state = GetMarkovState(newLevel+row, x, y);

                //If we have a markovChain query it
                if (markovChain!=null && markovChain.ContainsKey(state))
                {
                    //Sample a random value
                    float sampleVal = Random.Range(0f, 1f);
                    float sumVal = 0f;
                    char charToUse = '.';

                    //Select the sampled "action"
                    foreach(KeyValuePair<char, float> kvp in markovChain[state])
                    {
                        sumVal += kvp.Value;

                        if (sumVal >= sampleVal)
                        {
                            charToUse = kvp.Key;
                            break;
                        }
                    }

                    row += charToUse;
                }
                else
                {
                    //default token if not
                    row += ".";
                }
            }

            newLevel += row + "\n";
        }

        //Convert level from string representation to 
        Level outputLevel = TXTtoLevel.StringToLevel(newLevel);

        //Make the obstacles nice colours (ignorable)
        Color thisLevelColor = obstacleColours[Random.Range(0, obstacleColours.Length)];

        foreach (ProtoObstacle po in outputLevel.obstacles)
        {
            po.lineColor = thisLevelColor;
        }

        //Return output level
        return outputLevel;
    }

    

    /*
     * State representation for your Markov chain to be updated by the student
     */
    public string MarkovBasicState(string level, int x, int y){
        string[] levelRows = level.Split('\n');
    
        string markovState = "";
    
        //Left
        if (x - 1 >= 0 && y-1<levelRows.Length && x - 1 < levelRows[y].Length)
        {
            markovState += levelRows[y][x - 1];
        }
        else
        {
            markovState += nullToken;//something to represent offscreen token
        }
    
        //Up
        if (y - 1 >= 0 && y - 1 < levelRows.Length && x - 1 < levelRows[y].Length)
        {
            markovState += levelRows[y - 1][x];
        }
        else
        {
            markovState += nullToken;//something to represent offscreen token
        }
    
        //Left and Up
        if (y - 1 >= 0 && x - 1 >= 0 && y - 1 < levelRows.Length && x - 1 < levelRows[y].Length)
        {
            markovState += levelRows[y - 1][x - 1];
        }
        else
        {
            markovState += nullToken;//something to represent offscreen token
        }
    
        return markovState;
    }
    public string GetMarkovState(string level, int x, int y)
    {
        string[] levelRows = level.Split('\n');
        string markovState = MarkovBasicState(level, x, y);
        
        // Get bounds
        int height = HBound(levelRows) - 1;     // -1 somehow fixes it?
        int width = WBound(levelRows, y);
    
        // Check vertical symmetry (left-right)
        if (y < height && x < width && (width - x - 1) < width)
        {
            if (levelRows[y][x] == levelRows[y][width - x - 1])
            {
                markovState += levelRows[y][width - x - 1];
            }
        }
        
        // Check horizontal symmetry (top-bottom)
        if (y < height && x < width && (height - y - 1) < height)
        {
            // Access the vertically mirrored position safely
            if (levelRows[y][x] == levelRows[height - y - 1][x])
            {
                markovState += levelRows[height - y - 1][x];
            }
        }

        // Ghost/Player spawns in my txt
        // Neither G nor S appears within any level text
        if (level.Contains('S'))
        {
            markovState += '1';
        }
        else
        {
            markovState += '0';
        }
        if (level.Contains('G'))
        {
            markovState += '1';
        }
        else
        {
            markovState += '0';
        }

        // Append pellet count
        // Slight decrease in test accuracy
        // int pelletCount = level.Count(c => c.Equals('o'));
        // markovState += pelletCount.ToString();
        
        
        // Y-third and X-half
        // y decreases train, x increases train but makes level barren
        // markovState += (y / 4).ToString();
        markovState += (x / 5).ToString();
        
        // Tenth Unobstructed count
        // Test Accuracy goes up but levels become weird?
        // int tenth = level.Count(c => c.Equals('.') || c.Equals('-'));
        // markovState += (tenth / 10).ToString();
        
        // Add 1 left 2 up tiles, best result but makes levels barren
        // int left = 1;
        // int up = 2;
        // if (y < height && x < width && (x - left) >= 0 && (y - up) >= 0)
        // {
        //     markovState += levelRows[y - up][x - left];
        // }
        // else
        // {
        //     markovState += nullToken;   // No effect on train accuracy
        // }
        
        return markovState;
    }

    private int HBound(string[] levelRows)
    {
        return levelRows.Length;
    }

    private int WBound(string[] levelRows, int y)
    {
        if (y >= 0 && y < levelRows.Length)
        {
            return levelRows[y].Length;
        }
        return 0;
    }

}
