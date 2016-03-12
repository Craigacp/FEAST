/*******************************************************************************
** MIM implements the Mutual Information Maximisation method, which selects
** features with the largest univariate mutual informations.
**
** Initial Version - 22/02/2014
** Updated - 22/02/2014 - Patched calloc.
** Updated - 12/03/2016 - Changed initial value of maxMI to -1.0 to prevent segfaults when I(X;Y) = 0.0 for all X.
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

double* MIM(int k, int noOfSamples, int noOfFeatures, double *featureMatrix, double *classColumn, double *outputFeatures)
{
    double **feature2D = (double **) checkedCalloc(noOfFeatures,sizeof(double *));
    
    /*holds the class MI values*/
    double *classMI = (double *)checkedCalloc(noOfFeatures,sizeof(double));
    char *selectedFeatures = (char *)checkedCalloc(noOfFeatures,sizeof(char));
    /*Changed to ensure it always picks a feature*/
    double maxMI = -1.0;
    int maxMICounter = -1;
    int i,j;

    /**********************************************************
    ** this pulls out a pointer to the first sample of
    ** each feature and stores it as a multidimensional array
    ** so it can be indexed nicely
    ***********************************************************/
    for(j = 0; j < noOfFeatures; j++)
    {
        feature2D[j] = featureMatrix + (int)j*noOfSamples;
    }

    /***********************************************************
    ** SETUP COMPLETE
    ** Algorithm starts here
    ***********************************************************/
    
    for (i = 0; i < noOfFeatures; i++)
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
    ** MI feature as the first output feature.
    *************/
    
    /**
     * Ideally this should use a quick sort, but it's still quicker than
     * calling BetaGamma with beta=0 and gamma=0
     */
    for (i = 1; i < k; i++)
    {
        maxMI = -1.0;
        for (j = 0; j < noOfFeatures; j++) {
            if (!selectedFeatures[j]) {
                if (maxMI < classMI[j]) {
                    maxMI = classMI[j];
                    maxMICounter = j;
                }
            }
        }
        selectedFeatures[maxMICounter] = 1;
        outputFeatures[i] = maxMICounter;
    }/*for the number of features to select*/
    
  FREE_FUNC(classMI);
  FREE_FUNC(feature2D);
  FREE_FUNC(selectedFeatures);
  
  classMI = NULL;
  feature2D = NULL;
  selectedFeatures = NULL;
  
  return outputFeatures;
}/*MIM(int,int,int,double[][],double[],double[])*/

