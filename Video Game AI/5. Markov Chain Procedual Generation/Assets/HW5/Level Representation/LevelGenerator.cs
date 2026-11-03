using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LevelGenerator : MonoBehaviour
{
	public TextAsset[] trainingLevels, testLevels;

	public virtual Level GenerateLevel()
	{
		return null;
	}
}
