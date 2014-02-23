/*******************************************************************************
** mRMR_D.c implements the minimum Relevance Maximum Redundancy criterion
** using the difference variant, from
**
** "Feature Selection Based on Mutual Information: Criteria of Max-Dependency, Max-Relevance, and Min-Redundancy"
** H. Peng et al. IEEE PAMI (2005)
**
** Initial Version - 13/06/2008
** Updated - 12/02/2013 - patched the use of DBL_MAX
** Updated - 22/02/2014 - Moved feature index increment to mex code.
** Updated - 22/02/2014 - Patched calloc.
**
** Author - Adam Pocock
** 
** Part of the FEAture Selection Toolbox (FEAST), please reference
** "Conditional Likelihood Maximisation: A Unifying Framework for Information
** Theoretic Feature Selection"
** G. Brown, A. Pocock, M.-J. Zhao, M. Lujan
** Journal of Machine Learning Research (JMLR), 2012
**
** Please check www.cs.manchester.ac.uk/~gbrown/fstoolbox for updates.
** 
** Copyright (c) 2010-2014, A. Pocock, G. Brown, The University of Manchester
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
** 
**   - Redistributions of source code must retain the above copyright notice, this 
**     list of conditions and the following disclaimer.
**   - Redistributions in binary form must reproduce the above copyright notice, 
**     this list of conditions and the following disclaimer in the documentation 
**     and/or other materials provided with the distribution.
**   - Neither the name of The University of Manchester nor the names of its 
**     contributors may be used to endorse or promote products derived from this 
**     software without specific prior written permission.
** 
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
** ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
** ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
*******************************************************************************/

#include "FSAlgorithms.h"
#include "FSToolbox.h"

/* MIToolbox includes */
#include "ArrayOperations.h"
#include "MutualInformation.h"

double* mRMR_D(int k, int noOfSamples, int noOfFeatures, double *featureMatrix, double *classColumn, double *outputFeatures)
{
  double **feature2D = (double**) checkedCalloc(noOfFeatures,sizeof(double*));
  /*holds the class MI values*/
  double *classMI = (double *)checkedCalloc(noOfFeatures,sizeof(double));
  int *selectedFeatures = (int *)checkedCalloc(noOfFeatures,sizeof(int));
  /*holds the intra feature MI values*/
  int sizeOfMatrix = k*noOfFeatures;
  double *featureMIMatrix = (double *)checkedCalloc(sizeOfMatrix,sizeof(double));
  
  double maxMI = 0.0;
  int maxMICounter = -1;
  
  /*init variables*/
  
  double score, currentScore, totalFeatureMI;
  int currentHighestFeature;
  
  int arrayPosition, i, j, x;
  
  for(j = 0; j < noOfFeatures; j++)
  {
    feature2D[j] = featureMatrix + (int)j*noOfSamples;
  }
  
  for (i = 0; i < sizeOfMatrix;i++)
  {
    featureMIMatrix[i] = -1;
  }/*for featureMIMatrix - blank to -1*/
  

  for (i = 0; i < noOfFeatures;i++)
  {
    classMI[i] = calculateMutualInformation(feature2D[i], classColumn, noOfSamples);
    if (classMI[i] > maxMI)
    {
      maxMI = classMI[i];
      maxMICounter = i;
    }/*if bigger than current maximum*/
  }/*for noOfFeatures - filling classMI*/
  
  selectedFeatures[maxMICounter] = 1;
  outputFeatures[0] = maxMICounter;
  
  /*************
  ** Now we have populated the classMI array, and selected the highest
  ** MI feature as the first output feature
  ** Now we move into the mRMR-D algorithm
  *************/
  
  for (i = 1; i < k; i++)
  {
    /****************************************************
    ** to ensure it selects some features
    **if this is zero then it will not pick features where the redundancy is greater than the 
    **relevance
    ****************************************************/
    score = -DBL_MAX;
    currentHighestFeature = 0;
    currentScore = 0.0;
    totalFeatureMI = 0.0;
    
    for (j = 0; j < noOfFeatures; j++)
    {
      /*if we haven't selected j*/
      if (selectedFeatures[j] == 0)
      {
        currentScore = classMI[j];
        totalFeatureMI = 0.0;
        
        for (x = 0; x < i; x++)
        {
          arrayPosition = x*noOfFeatures + j;
          if (featureMIMatrix[arrayPosition] == -1)
          {
            /*work out intra MI*/
            
            /*double calculateMutualInformation(double *firstVector, double *secondVector, int vectorLength);*/
            featureMIMatrix[arrayPosition] = calculateMutualInformation(feature2D[(int) outputFeatures[x]], feature2D[j], noOfSamples);
          }
          
          totalFeatureMI += featureMIMatrix[arrayPosition];
        }/*for the number of already selected features*/
        
        currentScore -= (totalFeatureMI/i);
        if (currentScore > score)
		{
		  score = currentScore;
		  currentHighestFeature = j;
		}
	  }/*if j is unselected*/
	}/*for number of features*/
  
    selectedFeatures[currentHighestFeature] = 1;
    outputFeatures[i] = currentHighestFeature;
  
  }/*for the number of features to select*/
  
  FREE_FUNC(classMI);
  FREE_FUNC(feature2D);
  FREE_FUNC(featureMIMatrix);
  FREE_FUNC(selectedFeatures);
  
  classMI = NULL;
  feature2D = NULL;
  featureMIMatrix = NULL;
  selectedFeatures = NULL;
  
  return outputFeatures;
}/*mRMR(int,int,int,double[][],double[],double[])*/

