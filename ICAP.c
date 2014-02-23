/*******************************************************************************
** ICAP.c implements the Interaction Capping criterion from 
** 
** "Machine Learning Based on Attribute Interactions"
** A. Jakulin, PhD Thesis (2005)
**
** Initial Version - 19/08/2010
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

double* ICAP(int k, int noOfSamples, int noOfFeatures, double *featureMatrix, double *classColumn, double *outputFeatures)
{
    /*holds the class MI values*/
    double *classMI = (double *)checkedCalloc(noOfFeatures,sizeof(double));
    char *selectedFeatures = (char *)checkedCalloc(noOfFeatures,sizeof(char));
    
    /*separates out the features*/
    double **feature2D = (double **) checkedCalloc(noOfFeatures,sizeof(double *));
    
    /*holds the intra feature MI values*/
    int sizeOfMatrix = k*noOfFeatures;
    double *featureMIMatrix = (double *)checkedCalloc(sizeOfMatrix,sizeof(double));
    double *featureCMIMatrix = (double *)checkedCalloc(sizeOfMatrix,sizeof(double));
    
    double maxMI = 0.0;
    int maxMICounter = -1;
    
    double score, currentScore, totalFeatureInteraction, interactionInfo;
    int currentHighestFeature, arrayPosition;
    
    int i, j, m;

    for (j = 0; j < noOfFeatures; j++)
        feature2D[j] = featureMatrix + (int) j * noOfSamples;
    
    for (i = 0; i < sizeOfMatrix; i++)
    {
        featureMIMatrix[i] = -1;
        featureCMIMatrix[i] = -1;
    }/*for featureMIMatrix and featureCMIMatrix - blank to -1*/
    
    /*SETUP COMPLETE*/
    /*Algorithm starts here*/
    
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
   *************/
    
    for (i = 1; i < k; i++)
    {
        /**********************************************************************
        ** to ensure it selects some features
        **if this is zero then it will not pick features where the redundancy is greater than the
        **relevance
        **********************************************************************/
        score = -DBL_MAX;
        currentHighestFeature = 0;
        currentScore = 0.0;
        
        for (j = 0; j < noOfFeatures; j++)
        {
            /*if we haven't selected j*/
            if (!selectedFeatures[j])
            {
                currentScore = classMI[j];
                totalFeatureInteraction = 0.0;
                
                for (m = 0; m < i; m++)
                {
                    arrayPosition = m*noOfFeatures + j;
                    
                    if (featureMIMatrix[arrayPosition] == -1)
                    {
                        /*work out interaction*/
                        
                        /*double calculateMutualInformation(double *firstVector, double *secondVector, int vectorLength);*/
                        featureMIMatrix[arrayPosition] = calculateMutualInformation(feature2D[(int) outputFeatures[m]], feature2D[j], noOfSamples);
                        /*double calculateConditionalMutualInformation(double *firstVector, double *targetVector, double* conditionVector, int vectorLength);*/
                        featureCMIMatrix[arrayPosition] = calculateConditionalMutualInformation(feature2D[(int) outputFeatures[m]], feature2D[j], classColumn, noOfSamples);
                    }/*if not already known*/
                    
                    interactionInfo = featureCMIMatrix[arrayPosition] - featureMIMatrix[arrayPosition];
                    
                    if (interactionInfo < 0)
                    {
                      totalFeatureInteraction += interactionInfo;
                    }
                }/*for the number of already selected features*/
                
                currentScore += totalFeatureInteraction;

                
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
  FREE_FUNC(featureCMIMatrix);
  FREE_FUNC(selectedFeatures);
  
  classMI = NULL;
  feature2D = NULL;
  featureMIMatrix = NULL;
  featureCMIMatrix = NULL;
  selectedFeatures = NULL;

  return outputFeatures;
}/*ICAP(int,int,int,double[][],double[],double[])*/

