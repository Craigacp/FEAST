/*******************************************************************************
** CMIM.c, implements a discrete version of the 
** Conditional Mutual Information Maximisation criterion, using the fast
** exact implementation from
**
** "Fast Binary Feature Selection using Conditional Mutual Information Maximisation"
** F. Fleuret, JMLR (2004)
**
** Initial Version - 13/06/2008
** Updated - 23/06/2011 - Patched first feature selection error.
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
  
double* CMIM(int k, int noOfSamples, int noOfFeatures, double *featureMatrix, double *classColumn, double *outputFeatures)
{
  /*holds the class MI values
  **the class MI doubles as the partial score from the CMIM paper
  */
  double *classMI = (double *)checkedCalloc(noOfFeatures,sizeof(double));
  /*in the CMIM paper, m = lastUsedFeature*/
  int *lastUsedFeature = (int *)checkedCalloc(noOfFeatures,sizeof(int));
  
  double score, conditionalInfo;
  int iMinus, currentFeature;
  
  double maxMI = 0.0;
  int maxMICounter = -1;
  
  int j,i;

  double **feature2D = (double**) checkedCalloc(noOfFeatures,sizeof(double*));

  for(j = 0; j < noOfFeatures; j++)
  {
    feature2D[j] = featureMatrix + (int)j*noOfSamples;
  }
  
  for (i = 0; i < noOfFeatures;i++)
  {
    classMI[i] = calculateMutualInformation(feature2D[i], classColumn, noOfSamples);
    
    if (classMI[i] > maxMI)
    {
      maxMI = classMI[i];
      maxMICounter = i;
    }/*if bigger than current maximum*/
  }/*for noOfFeatures - filling classMI*/
  
  outputFeatures[0] = maxMICounter;
  
  /*****************************************************************************
  ** We have populated the classMI array, and selected the highest
  ** MI feature as the first output feature
  ** Now we move into the CMIM algorithm
  *****************************************************************************/
  
  for (i = 1; i < k; i++)
  {
    score = 0.0;
    iMinus = i-1;
    
    for (j = 0; j < noOfFeatures; j++)
    {
      while ((classMI[j] > score) && (lastUsedFeature[j] < i))
      {
        /*double calculateConditionalMutualInformation(double *firstVector, double *targetVector, double *conditionVector, int vectorLength);*/
        currentFeature = (int) outputFeatures[lastUsedFeature[j]];
        conditionalInfo = calculateConditionalMutualInformation(feature2D[j],classColumn,feature2D[currentFeature],noOfSamples);
        if (classMI[j] > conditionalInfo)
        {
          classMI[j] = conditionalInfo;
        }/*reset classMI*/
        /*moved due to C indexing from 0 rather than 1*/
        lastUsedFeature[j] += 1;
      }/*while partial score greater than score & not reached last feature*/
      if (classMI[j] > score)
      {
        score = classMI[j];
        outputFeatures[i] = j;
      }/*if partial score still greater than score*/
	}/*for number of features*/
  }/*for the number of features to select*/
  
  FREE_FUNC(classMI);
  FREE_FUNC(lastUsedFeature);
  FREE_FUNC(feature2D);

  classMI = NULL;
  lastUsedFeature = NULL;
  feature2D = NULL;

  return outputFeatures;
}/*CMIM(int,int,int,double[][],double[],double[])*/

