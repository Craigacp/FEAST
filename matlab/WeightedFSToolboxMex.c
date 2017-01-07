/*******************************************************************************
** WeightedFSToolboxMex.c is the entry point for the feature selection toolbox.
** It provides a MATLAB interface to the various selection algorithms.
**
** Initial Version - 27/06/2011
**
** Author - Adam Pocock
** 
** Part of the Feature Selection Toolbox, please reference
** "Conditional Likelihood Maximisation: A Unifying Framework for Mutual
** Information Feature Selection"
** G. Brown, A. Pocock, M.-J. Zhao, M. Lujan
** Journal of Machine Learning Research (JMLR), 2011
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

#include "FEAST/FSToolbox.h"
#include "FEAST/WeightedFSAlgorithms.h"
#include "MIToolbox/ArrayOperations.h"
#include "MIToolbox/Entropy.h"

/******************************************************************************
 ** entry point for the mex call
 ** nlhs - number of outputs
 ** plhs - pointer to array of outputs
 ** nrhs - number of inputs
 ** prhs - pointer to array of inputs
 ******************************************************************************/
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    /*************************************************************
     ** this function takes 5 arguments:
     ** flag = which algorithm to use,
     ** k = number of features to select,
     ** weightVector[] = vector of example weights
     ** featureMatrix[][] = matrix of features,
     ** classColumn[] = targets,
     ** the arguments apart from weightVector should 
     ** be discrete integers, with the weightVector being positive
     ** real numbers,
     ** and has one output:
     ** selectedFeatures[] of size k
     *************************************************************/

    int flag, k;
    double optionalParam1, optionalParam2;
    int numberOfFeatures, numberOfSamples, numberOfWeights, numberOfTargets;
    double *featureMatrix, *targets, *weightVector, *output, *outputFeatures, *scores;
    double **feature2D;

    double entropyTest;
    int i, j;

    /************************************************************
     ** number to function map
     ** 1 = Not implemented, was MIFS
     ** 2 = Not implemented, was mRMR
     ** 3 = CMIM
     ** 4 = JMI
     ** 5 = DISR
     ** 6 = Not implemented, was CIFE
     ** 7 = Not implemented, was ICAP
     ** 8 = Not implemented, was CondRed
     ** 9 = Not implemented, was BetaGamma
     ** 10 = CMI
     *************************************************************/
    if ((nlhs != 1) && (nlhs != 2)) {
        printf("Incorrect number of output arguments\n");
        return;
    }/*if not 1 output*/
    if (nrhs != 5) {
        printf("Incorrect number of input arguments\n");
        return;
    }/*if not 5 inputs*/

    /*get the flag for which algorithm*/
    flag = (int) mxGetScalar(prhs[0]);

    /*get the number of features to select, cast out as it is a double*/
    k = (int) mxGetScalar(prhs[1]);

    numberOfWeights = mxGetN(prhs[2]);

    numberOfFeatures = mxGetN(prhs[3]);
    numberOfSamples = mxGetM(prhs[3]);

    numberOfTargets = mxGetM(prhs[4]);

    if (numberOfTargets != numberOfSamples) {
        printf("Number of targets must match number of samples\n");
        printf("Number of targets = %d, Number of Samples = %d, Number of Features = %d\n", numberOfTargets, numberOfSamples, numberOfFeatures);

        plhs[0] = mxCreateDoubleMatrix(0, 0, mxREAL);
        if (nlhs == 2) {
            plhs[1] = mxCreateDoubleMatrix(0, 0, mxREAL);
        }
        return;
    }/*if size mismatch*/
    else if ((k < 1) || (k > numberOfFeatures)) {
        printf("You have requested k = %d features, which is not possible\n", k);
        plhs[0] = mxCreateDoubleMatrix(0, 0, mxREAL);
        if (nlhs == 2) {
            plhs[1] = mxCreateDoubleMatrix(0, 0, mxREAL);
        }
        return;
    } else {
        weightVector = mxGetPr(prhs[2]);
        featureMatrix = mxGetPr(prhs[3]);
        targets = mxGetPr(prhs[4]);

        /*double calculateEntropy(double *dataVector, int vectorLength)*/
        entropyTest = discAndCalcEntropy(targets, numberOfSamples);
        if (entropyTest < 0.0000001) {
            printf("The class label Y has entropy of 0, therefore all mutual informations containing Y will be 0. No feature selection is performed\n");
            plhs[0] = mxCreateDoubleMatrix(0, 0, mxREAL);
            if (nlhs == 2) {
                plhs[1] = mxCreateDoubleMatrix(0, 0, mxREAL);
            }
            return;
        } else {
            if (nlhs == 2) {
                plhs[1] = mxCreateDoubleMatrix(k, 1, mxREAL);
                scores = mxGetPr(plhs[1]);
            } else {
                scores = (double *) mxCalloc(k, sizeof(double));
            }
            feature2D = generateDoubleIndices(featureMatrix,numberOfSamples,numberOfFeatures);
            /*printf("Flag = %d, k = %d, numFeatures = %d, numSamples = %d\n",flag,k,numberOfFeatures,numberOfSamples);*/
            switch (flag) {
                case 3: /* CMIM */
                {
                    plhs[0] = mxCreateDoubleMatrix(k, 1, mxREAL);
                    output = (double *) mxGetPr(plhs[0]);

                    discWeightedCMIM(k, numberOfSamples, numberOfFeatures, feature2D, targets, weightVector, output, scores);

                    incrementVector(output, k);
                    break;
                }
                case 4: /* JMI */
                {
                    plhs[0] = mxCreateDoubleMatrix(k, 1, mxREAL);
                    output = (double *) mxGetPr(plhs[0]);

                    discWeightedJMI(k, numberOfSamples, numberOfFeatures, feature2D, targets, weightVector, output, scores);

                    incrementVector(output, k);
                    break;
                }
                case 5: /* DISR */
                {
                    plhs[0] = mxCreateDoubleMatrix(k, 1, mxREAL);
                    output = (double *) mxGetPr(plhs[0]);

                    discWeightedDISR(k, numberOfSamples, numberOfFeatures, feature2D, targets, weightVector, output, scores);

                    incrementVector(output, k);
                    break;
                }
                case 10: /* CMI */
                {
                    output = (double *) mxCalloc(k, sizeof(double));

                    discWeightedCondMI(k, numberOfSamples, numberOfFeatures, feature2D, targets, weightVector, output, scores);

                    i = 0;

                    while ((output[i] != -1) && (i < k)) {
                        i++;
                    }

                    plhs[0] = mxCreateDoubleMatrix(i, 1, mxREAL);
                    outputFeatures = (double *) mxGetPr(plhs[0]);

                    for (j = 0; j < i; j++) {
                        outputFeatures[j] = output[j] + 1; /*C indexes from 0 not 1*/
                    }/*for number of selected features*/

                    mxFree(output);
                    output = NULL;
                    break;
                }
                default:
                {
                    printf("Unsupported operation. %d is an unknown flag.\n", flag);
                    break;
                }
            }/*switch on flag*/
            mxFree(feature2D);
            if (nlhs == 1) {
                mxFree(scores);
            }
            return;
        }
    }
}/*mex function entry*/
