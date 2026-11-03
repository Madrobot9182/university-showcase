using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.UI;
using Random = UnityEngine.Random;

public class KClusterer
{
    const int K = 7;//TODO; set K to the optimal value that you've found via experimentation
    const int MAX_ATTEMPTS = 1000;//Maximum number of clustering attempts, you may want to use this
    const float threshold = 0.005f;//Threshold for cluster similarity, you may want to use this and alter it if so
    static List<float> normalizedFactors = new List<float>(16);
        
    //TODO; fix this function
    public Dictionary<Datapoint, List<Datapoint>> Cluster(Datapoint[] datapoints)
    {
        Dictionary<Datapoint, List<Datapoint>> clustersByCenters = new Dictionary<Datapoint, List<Datapoint>>();
        Dictionary<Datapoint, List<Datapoint>> newClusters = new Dictionary<Datapoint, List<Datapoint>>();
        FindNormalFactors(datapoints);      // Get normal factors

        //Select K random centers to start
        List<Datapoint> centers = new List<Datapoint>();

        while (centers.Count < K)
        {
            // Generate a random index less than the size of the array.  
            int randomIndex = Random.Range(0, datapoints.Length);
            Datapoint randomCenter = datapoints[randomIndex];

            if (!centers.Contains(randomCenter))
            {
                centers.Add(randomCenter);
            }
        }

        //Instantiate clusters by these centers
        foreach (Datapoint center in centers)
        {
            clustersByCenters.Add(center, new List<Datapoint>());
        }

        //Map each datapoint to the closest center
        foreach (Datapoint pnt in datapoints)
        {
            Datapoint closestCenter = null;
            float minDistance = float.PositiveInfinity;

            foreach (Datapoint center in centers)
            {
                float thisDistance = Distance(pnt, center);
                if (thisDistance < minDistance)
                {
                    closestCenter = center;
                    minDistance = thisDistance;
                }
            }

            clustersByCenters[closestCenter].Add(pnt);
        }

        List<Datapoint> oldCentroids = new List<Datapoint>();//clustersByCenters.Keys.ToList();
        //Dictionary<Datapoint, List<Datapoint>> tempClusters;
        int attempts = 0;
        // Mine, repeatedly improve upon older centroids up to a threshold 
        while ((DifferenceBetweenCenters(centers.ToArray(), oldCentroids.ToArray()) > threshold) &&
               (attempts < MAX_ATTEMPTS))
        {
            attempts++;
            newClusters.Clear();
  
                oldCentroids = clustersByCenters.Keys.ToList(); 
                //List<Datapoint> points = clustersByCenters.Values.ToList();

                //Map each datapoint to the closest center
                foreach (Datapoint pnt in datapoints)
                {
                    Datapoint closestCenter = null;
                    float minDistance = float.PositiveInfinity;

                    foreach (Datapoint center in oldCentroids)
                    {
                        float thisDistance = Distance(pnt, center);
                        if (thisDistance < minDistance)
                        {
                            closestCenter = center;
                            minDistance = thisDistance;
                        }
                    }
                    // To check if that entry exist yet
                    if (!newClusters.TryGetValue(closestCenter, out List<Datapoint> empty))
                    {
                        empty = new List<Datapoint>();
                        newClusters.Add(closestCenter, empty);
                    }
                    newClusters[closestCenter].Add(pnt);
                }

                // Calculate new centers
                clustersByCenters.Clear();
                foreach (List<Datapoint> cluster in newClusters.Values)
                {
                    Datapoint avgCenter = GetAverage(cluster.ToArray());
                    clustersByCenters[avgCenter] = cluster;
                }
                
            centers = new List<Datapoint>(clustersByCenters.Keys);
        }
        Debug.Log("We ran " + attempts + " Attempts with a difference of " + DifferenceBetweenCenters(centers.ToArray(), oldCentroids.ToArray()));
        return clustersByCenters;
    }

    //Calculate the difference between sets of centers
    private float DifferenceBetweenCenters(Datapoint[] centers1, Datapoint[] centers2)
    {
        List<Datapoint> mappedPoints = new List<Datapoint>();
        float totalDistance = 0;
        foreach(Datapoint c1 in centers1)
        {
            Datapoint minPoint = null;
            float minDistance = float.PositiveInfinity;

            foreach(Datapoint c2 in centers2)
            {
                if (!mappedPoints.Contains(c2))
                {
                    float thisDistance = Distance(c1, c2);

                    if (thisDistance < minDistance)
                    {
                        minDistance = thisDistance;
                        minPoint = c2;
                    }
                }
            }
            mappedPoints.Add(minPoint);
            totalDistance += minDistance;
        }
        
        return totalDistance;
    }

    //Calculate and returns the geometric median of the given datapoints
    public Datapoint GetMedian(Datapoint[] datapoints)
    {
        Datapoint medianPnt = null;
        float totalDistance = float.PositiveInfinity;

        for(int i = 0; i<datapoints.Length; i++)
        {
            float thisDistance = 0;
            for(int j=0; j<datapoints.Length; j++)
            {
                if (i != j)
                {
                    thisDistance += Distance(datapoints[i], datapoints[j]);
                }
            }

            if (thisDistance < totalDistance)
            {
                totalDistance = thisDistance;
                medianPnt = datapoints[i];
            }
        }

        return medianPnt;
    }

    //Calculate and returns the average of the given datapoints
    public Datapoint GetAverage(Datapoint[] datapoints)
    {
        Datapoint sumDatapoint = new Datapoint("", 0, 0, 0, 0, 0, 0, 0, 0);
        int churnedVal = 0;

        foreach(Datapoint d in datapoints)
        {
            sumDatapoint = new Datapoint("", sumDatapoint.HoursPlayed + d.HoursPlayed, sumDatapoint.Level + d.Level, sumDatapoint.PelletsEaten + d.PelletsEaten, sumDatapoint.FruitEaten + d.FruitEaten, sumDatapoint.GhostsEaten + d.GhostsEaten, sumDatapoint.AvgScore + d.AvgScore, sumDatapoint.MaxScore + d.MaxScore, sumDatapoint.TotalScore + d.TotalScore);

            if (d.Churned)
            {
                churnedVal += 1;
            }
            else
            {
                churnedVal -= 1;
            }
        }
        //Calculate averages
        int hoursPlayed = (int)(((float)sumDatapoint.HoursPlayed) / ((float)datapoints.Length));
        int level = (int)(((float)sumDatapoint.Level) / ((float)datapoints.Length));
        int pelletsEaten = (int)(((float)sumDatapoint.PelletsEaten) / ((float)datapoints.Length));
        int fruitEaten = (int)(((float)sumDatapoint.FruitEaten) / ((float)datapoints.Length));
        int ghostsEaten = (int)(((float)sumDatapoint.GhostsEaten) / ((float)datapoints.Length));
        float avgScore = (((float)sumDatapoint.AvgScore) / ((float)datapoints.Length));
        int maxScore = (int)(((float)sumDatapoint.MaxScore) / ((float)datapoints.Length));
        int totalScore = (int)(((float)sumDatapoint.TotalScore) / ((float)datapoints.Length));

        bool churned = false;
        if (churnedVal > 0)
        {
            churned = true;
        }

        return new Datapoint("", hoursPlayed, level, pelletsEaten, fruitEaten, ghostsEaten, avgScore, maxScore, totalScore, churned);
    }

    //TODO; alter this distance function
    //WARNING: DO NOT USE CHURNED AS PART OF THIS FUNCTION
    
    // public static float Distance(Datapoint a, Datapoint b)
    // {
    //     // This is just a straight eyeball guess that could definitely be done in a smarter way
    //     float dist = 0;     // Aim for around the thousands (2500) range
    //     dist += (Mathf.Abs(a.HoursPlayed - b.HoursPlayed));         // Around 2500
    //     dist += (Mathf.Abs(a.Level - b.Level)) * 5f;                     // Alternates between <100 and around 730
    //     dist += (Mathf.Abs(a.PelletsEaten - b.PelletsEaten)) / 100f;       // in the 100,000 range (up to ~400,000) 
    //     dist += (Mathf.Abs(a.FruitEaten - b.FruitEaten)) * 3f;           // 300-1000 
    //     dist += (Mathf.Abs(a.GhostsEaten - b.GhostsEaten));         // Around <4000
    //     dist += (Mathf.Abs(a.AvgScore - b.AvgScore)) * 2f;               // 400-2000
    //     dist += (Mathf.Abs(a.MaxScore - b.MaxScore));               // 2000-4000
    //     dist += (Mathf.Abs(a.TotalScore - b.TotalScore)) / 60f;           // Around 150,000
    //     return dist / 2500f;
    // }

    public static float Distance(Datapoint a, Datapoint b)
    {
        // min-max normalization
        // x` = x - min(x) / max(x) - min(x)
        // normalize two points, then take euclidean distance
        // I'm so sorry for the shear amount of lines lol
        float dist = 0, normA = 0, normB = 0;
        
        normA = (a.HoursPlayed - normalizedFactors[0]) / (normalizedFactors[1] - normalizedFactors[0]);
        normB = (b.HoursPlayed - normalizedFactors[0]) / (normalizedFactors[1] - normalizedFactors[0]);
        dist += Mathf.Pow(normA - normB, 2);
        normA = (a.Level - normalizedFactors[2]) / (normalizedFactors[3] - normalizedFactors[2]);
        normB = (b.Level - normalizedFactors[2]) / (normalizedFactors[3] - normalizedFactors[2]);
        dist += Mathf.Pow(normA - normB, 2);
        normA = (a.PelletsEaten - normalizedFactors[4]) / (normalizedFactors[5] - normalizedFactors[4]);
        normB = (b.PelletsEaten - normalizedFactors[4]) / (normalizedFactors[5] - normalizedFactors[4]);
        dist += Mathf.Pow(normA - normB, 2);
        normA = (a.FruitEaten - normalizedFactors[6]) / (normalizedFactors[7] - normalizedFactors[6]);
        normB = (b.FruitEaten - normalizedFactors[6]) / (normalizedFactors[7] - normalizedFactors[6]);
        dist += Mathf.Pow(normA - normB, 2);
        normA = (a.GhostsEaten - normalizedFactors[8]) / (normalizedFactors[9] - normalizedFactors[8]);
        normB = (b.GhostsEaten - normalizedFactors[8]) / (normalizedFactors[9] - normalizedFactors[8]);
        dist += Mathf.Pow(normA - normB, 2);
        normA = (a.AvgScore - normalizedFactors[10]) / (normalizedFactors[11] - normalizedFactors[10]);
        normB = (b.AvgScore - normalizedFactors[10]) / (normalizedFactors[11] - normalizedFactors[10]);
        dist += Mathf.Pow(normA - normB, 2);
        normA = (a.MaxScore - normalizedFactors[12]) / (normalizedFactors[13] - normalizedFactors[12]);
        normB = (b.MaxScore - normalizedFactors[12]) / (normalizedFactors[13] - normalizedFactors[12]);
        dist += Mathf.Pow(normA - normB, 2);
        normA = (a.TotalScore - normalizedFactors[14]) / (normalizedFactors[15] - normalizedFactors[14]);
        normB = (b.TotalScore - normalizedFactors[14]) / (normalizedFactors[15] - normalizedFactors[14]);
        dist += Mathf.Pow(normA - normB, 2);

        return Mathf.Sqrt(dist);
    }
    
    public static void FindNormalFactors(Datapoint[] datapoints)
    {
        if (normalizedFactors.Count != 0) { return; }

        // Alternate between min and max
        for (int i = 0; i < 16; i++)
        {
            if (i % 2 == 0) // Even
            {
                normalizedFactors.Add(float.PositiveInfinity);
            }
            else
            {
                normalizedFactors.Add(float.NegativeInfinity);
            }
        }
        
        foreach (Datapoint d in datapoints)
        {
            normalizedFactors[0] = Mathf.Min(normalizedFactors[0], d.HoursPlayed);
            normalizedFactors[1] = Mathf.Max(normalizedFactors[1], d.HoursPlayed);
            normalizedFactors[2] = Mathf.Min(normalizedFactors[2], d.Level);
            normalizedFactors[3] = Mathf.Max(normalizedFactors[3], d.Level);
            normalizedFactors[4] = Mathf.Min(normalizedFactors[4], d.PelletsEaten);
            normalizedFactors[5] = Mathf.Max(normalizedFactors[5], d.PelletsEaten);
            normalizedFactors[6] = Mathf.Min(normalizedFactors[6], d.FruitEaten);
            normalizedFactors[7] = Mathf.Max(normalizedFactors[7], d.FruitEaten);
            normalizedFactors[8] = Mathf.Min(normalizedFactors[8], d.GhostsEaten);
            normalizedFactors[9] = Mathf.Max(normalizedFactors[9], d.GhostsEaten);
            normalizedFactors[10] = Mathf.Min(normalizedFactors[10], d.AvgScore);
            normalizedFactors[11] = Mathf.Max(normalizedFactors[11], d.AvgScore);
            normalizedFactors[12] = Mathf.Min(normalizedFactors[12], d.MaxScore);
            normalizedFactors[13] = Mathf.Max(normalizedFactors[13], d.MaxScore);
            normalizedFactors[14] = Mathf.Min(normalizedFactors[14], d.TotalScore);
            normalizedFactors[15] = Mathf.Max(normalizedFactors[15], d.TotalScore);
        }
    }
}

