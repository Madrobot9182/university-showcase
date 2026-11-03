using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;

public class LevelHandler: MonoBehaviour
{
    public TextAsset txtLevelToLoad;//If this is null then use level generator
    public PelletHandler pelletHandler;

    public bool useExtraCreditGenerator;
    public MarkovLevelGenerator markovGenerator;
    public LevelGenerator extraCreditGenerator;


    void Start()
    {
        Level level = null;

        //If there's an existing level to load try that first
        if (txtLevelToLoad != null)
        {
            //Turn txt into a level
            level = TXTtoLevel.StringToLevel(txtLevelToLoad.text);
        }
        else
        {
            //Whether to use extra credit generator or not
            if (useExtraCreditGenerator)
            {
                level = extraCreditGenerator.GenerateLevel();
            }
            else
            {
                level = markovGenerator.GenerateLevel();
            }
        }

        //Set in case this level changed where the ghosts spawn
        AgentConstants.GHOST_START_POS = level.ghostStartPos;

        //Set in case this level changed where Pacman spawns
        PacmanInfo pacmanInfo = GameObject.FindObjectOfType<PacmanInfo>();
        if (pacmanInfo != null)
        {
            pacmanInfo.transform.position = level.pacmanStartPos;
        }

        //Initialize obstacles and grid based on level
        HW5NavigationHandler.Instance.Init(level);

        //Initialize pellets based on level
        foreach(ProtoPellet p in level.pellets)
        {
            pelletHandler.AddPellet(p.worldLocation, p.powerPellet);
        }

    }

    public void EvaluateTrain()
    {
        RoughlyEvaluateMarkovChain(markovGenerator.trainingLevels);
    }

    public void EvaluateTest()
    {
        RoughlyEvaluateMarkovChain(markovGenerator.testLevels);
    }

    public void RoughlyEvaluateMarkovChain(TextAsset[] levels)
    {
        //Approximate how likely it would be for a markovChain to generate each level

        float averageScore = 0f;//Average score across the levels

        foreach (TextAsset level in levels)
        {
            float numStates = 0f;
            string levelText = level.text;
            string[] levelRows = levelText.Split('\n');

            float thisLevelScore = 0f;

            for (int y = 0; y < levelRows.Length; y++)
            {
                for (int x = 0; x < levelRows[y].Length; x++)
                {

                    string markovState = markovGenerator.GetMarkovState(level.text, x, y);

                    //Check and see if we have seen this key before
                    if (markovGenerator.MarkovChain.ContainsKey(markovState))
                    {
                        numStates += 1.0f;
                        if (markovGenerator.MarkovChain[markovState].ContainsKey(levelRows[y][x]))
                        {
                            thisLevelScore += markovGenerator.MarkovChain[markovState][levelRows[y][x]];
                        }
                    }

                }
            }
            thisLevelScore /= numStates;//Average score per tile assuming 11x12 levels
            averageScore += thisLevelScore;
        }
        averageScore /= ((float)levels.Length);

        Debug.Log("Average Score: " + averageScore);
    }
}
