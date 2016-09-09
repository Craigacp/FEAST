/*******************************************************************************
** WeightedCondMI.c, implements the CMI criterion using a greedy forward search
** with the weighted mutual information
**
** Initial Version - 19/08/2010
** Updated - 08/08/2011
**
** Author - Adam Pocock
** 
** Part of the Feature Selection Toolbox, please reference
** "Information Theoretic Feature Selection for Cost-Sensitive Problems"
** A. Pocock, N. Edakunni, M.-J. Zhao, M. Lujan, G. Brown
** ArXiv, 2016
**
** Please check www.cs.manchester.ac.uk/~gbrown/fstoolbox for updates.
** 
** Copyright (c) 2010-2016, A. Pocock, G. Brown, The University of Manchester
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

#include "FEAST/WeightedFSAlgorithms.h"
#include "FEAST/FSToolbox.h"

/* for memcpy */
#include <string.h>

/* MIToolbox includes */
#include "MIToolbox/WeightedMutualInformation.h"
#include "MIToolbox/ArrayOperations.h"

double* WeightedCondMI(int k, int noOfSamples, int noOfFeatures, double *featureMatrix, double *classColumn, double *weightVector, double *outputFeatures)
{
  /*holds the class MI values*/
  double *classMI = (double *) checkedCalloc(noOfFeatures,sizeof(double));
  
  char *selectedFeatures = (char *) checkedCalloc(noOfFeatures,sizeof(char));
  
  /*Changed to ensure it always picks a feature*/
  double maxMI = -1.0;
  int maxMICounter = -1;
  
  double **feature2D = (double**) checkedCalloc(noOfFeatures,sizeof(double*));
  
  double score, currentScore;
  int currentHighestFeature;
  
  uint *conditionVector = (uint *) checkedCalloc(noOfSamples,sizeof(uint));
  uint *labelColumn = (uint *) checkedCalloc(noOfSamples,sizeof(uint));
  uint *normalisedVector = (uint *) checkedCalloc(noOfSamples,sizeof(uint));
  
  int i,j;
  
  normaliseArray(classColumn,labelColumn,noOfSamples);

  for (j = 0; j < k; j++) 
  {
    outputFeatures[j] = -1;
  }

  for(j = 0; j < noOfFeatures; j++)
  {
    feature2D[j] = featureMatrix + (int)j*noOfSamples;
  }
  
  for (i = 0; i < noOfFeatures;i++)
  {    
    /*discAndCalc mutual info
    **double discAndCalcWeightedMutualInformation(double *firstVector, double *secondVector, double *weightVector, int vectorLength);
    */
    classMI[i] = discAndCalcWeightedMutualInformation(feature2D[i], classColumn, weightVector, noOfSamples);
    
    if (classMI[i] > maxMI)
    {
      maxMI = classMI[i];
      maxMICounter = i;
    }/*if bigger than current maximum*/
  }/*for noOfFeatures - filling classMI*/
  
  selectedFeatures[maxMICounter] = 1;
  outputFeatures[0] = maxMICounter;
  
  memcpy(conditionVector,feature2D[maxMICounter],sizeof(int)*noOfSamples);
      
  /*****************************************************************************
  ** We have populated the classMI array, and selected the highest
  ** MI feature as the first output feature
  ** Now we move into the CondMI algorithm
  *****************************************************************************/
  
  for (i = 1; i < k; i++)
  {
    score = 0.0;
    currentHighestFeature = -1;
    currentScore = 0.0;
    
    for (j = 0; j < noOfFeatures; j++)
    {
      /*if we haven't selected j*/
      if (selectedFeatures[j] == 0)
      {
        currentScore = 0.0;
        
        /*double discAndCalcWeightedConditionalMutualInformation(double *firstVector, double *targetVector, double *conditionVector, double *weightVector, int vectorLength);*/
        normaliseArray(feature2D[j],normalisedVector,noOfSamples);
        currentScore = calcWeightedConditionalMutualInformation(normalisedVector,labelColumn,conditionVector,weightVector,noOfSamples);
        
        if (currentScore > score)
        {
          score = currentScore;
          currentHighestFeature = j;
        }
      }/*if j is unselected*/
    }/*for number of features*/
  
    outputFeatures[i] = currentHighestFeature;
    
    if (currentHighestFeature != -1)
    {
      selectedFeatures[currentHighestFeature] = 1;
      normaliseArray(feature2D[currentHighestFeature],normalisedVector,noOfSamples);
      mergeArrays(normalisedVector,conditionVector,conditionVector,noOfSamples);
    }
  
  }/*for the number of features to select*/
  
  FREE_FUNC(classMI);
  FREE_FUNC(conditionVector);
  FREE_FUNC(normalisedVector);
  FREE_FUNC(labelColumn);
  FREE_FUNC(feature2D);
  FREE_FUNC(selectedFeatures);
  
  classMI = NULL;
  conditionVector = NULL;
  normalisedVector = NULL;
  labelColumn = NULL;
  feature2D = NULL;
  selectedFeatures = NULL;

  return outputFeatures;
}/*WeightedCondMI(int,int,int,double[][],double[],double[],double[])*/

