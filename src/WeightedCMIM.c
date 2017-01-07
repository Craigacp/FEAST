/*******************************************************************************
** WeightedCMIM.c, implements a discrete version of the 
** Conditional Mutual Information Maximisation criterion, using the fast
** exact implementation from
**
** "Fast Binary Feature Selection using Conditional Mutual Information Maximisation"
** F. Fleuret, JMLR (2004)
**
** Uses the weighted mutual information.
**
** Initial Version - 13/06/2008
** Updated - 08/08/2011
**           17/12/2016 - Added feature scores.
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

uint* weightedCMIM(uint k, uint noOfSamples, uint noOfFeatures, uint **featureMatrix, uint *classColumn, double *weightVector, uint *outputFeatures, double *featureScores) {
    /*holds the class MI values
     **the class MI doubles as the partial score from the CMIM paper
     */
    double *classMI = (double *) checkedCalloc(noOfFeatures,sizeof(double));
    /*in the CMIM paper, m = lastUsedFeature*/
    uint *lastUsedFeature = (uint *) checkedCalloc(noOfFeatures,sizeof(uint));

    double score, conditionalInfo;
    int currentFeature;

    double maxMI = 0.0;
    int maxMICounter = -1;

    int j, i;

    for (i = 0; i < noOfFeatures; i++) {
        classMI[i] = calcWeightedMutualInformation(featureMatrix[i],classColumn,weightVector,noOfSamples);

        if (classMI[i] > maxMI) {
            maxMI = classMI[i];
            maxMICounter = i;
        }/*if bigger than current maximum*/
    }/*for noOfFeatures - filling classMI*/

    outputFeatures[0] = maxMICounter;
    featureScores[0] = maxMI;

    /*****************************************************************************
     ** We have populated the classMI array, and selected the highest
     ** MI feature as the first output feature
     ** Now we move into the CMIM algorithm
     *****************************************************************************/

    for (i = 1; i < k; i++) {
        score = 0.0;

        for (j = 0; j < noOfFeatures; j++) {
            while ((classMI[j] > score) && (lastUsedFeature[j] < i)) {
                /*double calcWeightedConditionalMutualInformation(uint *firstVector, uint *targetVector, uint *conditionVector, double *weightVector, int vectorLength);*/
                currentFeature = outputFeatures[lastUsedFeature[j]];
                conditionalInfo = calcWeightedConditionalMutualInformation(featureMatrix[j], classColumn, featureMatrix[currentFeature], weightVector, noOfSamples);
                if (classMI[j] > conditionalInfo) {
                    classMI[j] = conditionalInfo;
                }/*reset classMI*/
                /*moved due to C indexing from 0 rather than 1*/
                lastUsedFeature[j] += 1;
            }/*while partial score greater than score & not reached last feature*/
            if (classMI[j] > score) {
                score = classMI[j];
                featureScores[i] = score;
                outputFeatures[i] = j;
            }/*if partial score still greater than score*/
        }/*for number of features*/
    }/*for the number of features to select*/

    FREE_FUNC(classMI);
    FREE_FUNC(lastUsedFeature);

    classMI = NULL;
    lastUsedFeature = NULL;

    return outputFeatures;
}/*weightedCMIM(uint,uint,uint,uint[][],uint[],double[],uint[],double[])*/

double* discWeightedCMIM(uint k, uint noOfSamples, uint noOfFeatures, double **featureMatrix, double *classColumn, double *weightVector, double *outputFeatures, double *featureScores) {
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

    weightedCMIM(k, noOfSamples, noOfFeatures, intFeature2D, intClass, weightVector, intOutputs, featureScores);

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
}/*discWeightedCMIM(int,int,int,double[][],double[],double[],double[],double[])*/
