/*******************************************************************************
** WeightedMIM implements a weighted variant of the Mutual Information 
** Maximisation method, which selects features with the largest univariate 
** mutual informations.
**
** Initial Version - 07/01/2017
**
** Author - Adam Pocock
** 
** Part of the Feature Selection Toolbox, please reference
** "Information Theoretic Feature Selection for Cost-Sensitive Problems"
** A. Pocock, N. Edakunni, M.-J. Zhao, M. Lujan, G. Brown
** ArXiv, 2017
**
** Please check www.github.com/Craigacp/FEAST for updates.
** 
** Copyright (c) 2010-2017, A. Pocock, G. Brown, The University of Manchester
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

/* MIToolbox includes */
#include "MIToolbox/ArrayOperations.h"
#include "MIToolbox/WeightedMutualInformation.h"

uint* weightedMIM(uint k, uint noOfSamples, uint noOfFeatures, uint **featureMatrix, uint *classColumn, double *weightVector, uint *outputFeatures, double *featureScores) {
    char *selectedFeatures = (char *) checkedCalloc(noOfFeatures,sizeof(char));

    /*holds the class MI values*/
    double *classMI = (double *) checkedCalloc(noOfFeatures,sizeof(double));

    /*Changed to ensure it always picks a feature*/
    double maxMI = -1.0;
    int maxMICounter = -1;
    int i, j;

    /***********************************************************
     ** SETUP COMPLETE
     ** Algorithm starts here
     ***********************************************************/

    for (i = 0; i < noOfFeatures; i++) {
        /*calculate mutual info
         **double calcWeightedMutualInformation(uint *firstVector, uint *secondVector, double *weightVector int vectorLength);
         */
        classMI[i] = calcWeightedMutualInformation(featureMatrix[i], classColumn, weightVector, noOfSamples);

        if (classMI[i] > maxMI) {
            maxMI = classMI[i];
            maxMICounter = i;
        }/*if bigger than current maximum*/
    }/*for noOfFeatures - filling classMI*/

    selectedFeatures[maxMICounter] = 1;
    outputFeatures[0] = maxMICounter;
    featureScores[0] = maxMI;

    /**
     * Ideally this should use a quick sort.
     */
    for (i = 1; i < k; i++) {
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
        featureScores[i] = maxMI;
    }/*for the number of features to select*/

    FREE_FUNC(classMI);
    FREE_FUNC(selectedFeatures);

    classMI = NULL;
    selectedFeatures = NULL;

    return outputFeatures;
}/*WeightedMIM(uint,uint,uint,uint[][],uint[],double[],uint[],double[])*/

double* discWeightedMIM(uint k, uint noOfSamples, uint noOfFeatures, double **featureMatrix, double *classColumn, double *weightVector, double *outputFeatures, double *featureScores) {
    uint *intFeatures = (uint *) checkedCalloc(noOfSamples*noOfFeatures,sizeof(uint));
    uint *intClass = (uint *) checkedCalloc(noOfSamples,sizeof(uint));
    uint *intOutputs = (uint *) checkedCalloc(k,sizeof(uint));

    uint **intFeature2D = (uint**) checkedCalloc(noOfFeatures,sizeof(uint*));

    int i;

    for (i = 0; i < noOfFeatures; i++) {
        intFeature2D[i] = intFeatures + i*noOfSamples;
        normaliseArray(featureMatrix[i],intFeature2D[i],noOfSamples);
    }

    normaliseArray(classColumn,intClass,noOfSamples);

    weightedMIM(k, noOfSamples, noOfFeatures, intFeature2D, intClass, weightVector, intOutputs, featureScores);

    for (i = 0; i < k; i++) {
        outputFeatures[i] = intOutputs[i];
    }

    FREE_FUNC(intFeatures);
    FREE_FUNC(intClass);
    FREE_FUNC(intOutputs);
    FREE_FUNC(intFeature2D);

    intFeatures = NULL;
    intClass = NULL;
    intOutputs = NULL;
    intFeature2D = NULL;

    return outputFeatures;
}/*discWeightedMIM(int,int,int,double[][],double[],double[],double[],double[])*/
