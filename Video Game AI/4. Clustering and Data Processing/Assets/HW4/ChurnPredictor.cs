using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ChurnPredictor
{
    //TODO; FIX THIS FUNCTION FOR EXTRA CREDIT, TEST WITH "Check Accuracy" BUTTON
    public Datapoint[] AssignPredictedChurn(Datapoint[] verificationDatapoints, Dictionary<Datapoint, List<Datapoint>> clustersByCenters)
    {
        foreach(Datapoint d in verificationDatapoints)
        {
            // Get the closestCenter
            Datapoint closestCenter = null;
            float minDistance = float.PositiveInfinity;
        
            foreach (Datapoint center in clustersByCenters.Keys)
            {
                float dist = KClusterer.Distance(d, center);
                if (dist < minDistance)
                {
                    minDistance = dist;
                    closestCenter = center;
                }
            }
        
            // Get churn rate for this cluster
            List<Datapoint> train = clustersByCenters[closestCenter];
            float churnCount = 0;
            foreach (Datapoint point in train)
            {
                if (point.Churned)
                {
                    churnCount++;
                }
            }

            float churnRate = churnCount / (float)train.Count;
            d.SetPredictedChurn(churnRate < 0.5f);  // This seems to work well 
        }
        
        return verificationDatapoints;         
    }
}
