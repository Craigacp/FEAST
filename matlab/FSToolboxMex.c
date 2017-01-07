/*******************************************************************************
** FSToolboxMex.c is the entry point for the feature selection toolbox.
** It provides a MATLAB interface to the various selection algorithms.
**
** Initial Version - 27/06/2011
** Updated         - 22/02/2014 - Moved increment of feature numbers here.
**                   17/12/2016 - Added feature scores.
**
** Author - Adam Pocock
** 
** Part of the FEAture Selection Toolbox (FEAST), please reference
** "Conditional Likelihood Maximisation: A Unifying Framework for Information
** Theoretic Feature Selection"
** G. Brown, A. Pocock, M.-J. Zhao, M. Lujan
** Journal of Machine Learning Research (JMLR), 2012
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
#include "FEAST/FSAlgorithms.h"
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
     ** this function takes 4-6 arguments:
     ** flag = which algorithm to use,
     ** k = number of features to select,
     ** featureMatrix[][] = matrix of features,
     ** classColumn[] = targets,
     ** optionalParam1 = (path angle or beta value),
     ** optionalParam2 = (gamma value),
     ** the arguments should all be discrete integers.
     ** and has one output:
     ** selectedFeatures[] of size k
     *************************************************************/

    int flag, k;
    double optionalParam1, optionalParam2;
    int numberOfFeatures, numberOfSamples, numberOfTargets;
    double *featureMatrix, *targets, *output, *outputFeatures, *scores;
    double **feature2D;

    double entropyTest;
    int i, j;

    /************************************************************
     ** number to function map
     ** 1 = MIFS
     ** 2 = mRMR
     ** 3 = CMIM
     ** 4 = JMI
     ** 5 = DISR
     ** 6 = CIFE
     ** 7 = ICAP
     ** 8 = CondRed
     ** 9 = BetaGamma
     ** 10 = CMI
     *************************************************************/
    if ((nlhs != 1) && (nlhs != 2)) {
        printf("Incorrect number of output arguments\n");
        return;
    }/*if not 1 output*/
    if ((nrhs < 4) || (nrhs > 6)) {
        printf("Incorrect number of input arguments\n");
        return;
    }/*if not 4-6 inputs*/

    /*get the flag for which algorithm*/
    flag = (int) mxGetScalar(prhs[0]);

    /*get the number of features to select, cast out as it is a double*/
    k = (int) mxGetScalar(prhs[1]);

    numberOfFeatures = mxGetN(prhs[2]);
    numberOfSamples = mxGetM(prhs[2]);

    numberOfTargets = mxGetM(prhs[3]);

    if (nrhs == 6) {
        optionalParam1 = (double) mxGetScalar(prhs[4]);
        optionalParam2 = (double) mxGetScalar(prhs[5]);
    } else if (nrhs == 5) {
        optionalParam1 = (double) mxGetScalar(prhs[4]);
        optionalParam2 = 0.0;
    }

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
        featureMatrix = mxGetPr(prhs[2]);
        targets = mxGetPr(prhs[3]);

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
                case 1: /* MIFS */
                {
                    plhs[0] = mxCreateDoubleMatrix(k, 1, mxREAL);
                    output = (double *) mxGetPr(plhs[0]);
                    if (nrhs == 4) {
                        /* MIFS is Beta = 1, Gamma = 0 */
                        optionalParam1 = 1.0;
                        optionalParam2 = 0.0;
                    }

                    /*double* BetaGamma(uint k, uint noOfSamples, uint noOfFeatures,double *feature2D, double *classColumn, double *outputFeatures, double *featureScores, double beta, double gamma)*/
                    discBetaGamma(k, numberOfSamples, numberOfFeatures, feature2D, targets, output, scores, optionalParam1, optionalParam2);

                    incrementVector(output, k);
                    break;
                }
                case 2: /* mRMR */
                {
                    plhs[0] = mxCreateDoubleMatrix(k, 1, mxREAL);
                    output = (double *) mxGetPr(plhs[0]);

                    /*double* disc_mRMR_D(uint k, uint noOfSamples, uint noOfFeatures,double *feature2D, double *classColumn, double *outputFeatures, double *featureScores)*/
                    disc_mRMR_D(k, numberOfSamples, numberOfFeatures, feature2D, targets, output, scores);

                    incrementVector(output, k);
                    break;
                }
                case 3: /* CMIM */
                {
                    plhs[0] = mxCreateDoubleMatrix(k, 1, mxREAL);
                    output = (double *) mxGetPr(plhs[0]);

                    /*double* discCMIM(uint k, uint noOfSamples, uint noOfFeatures,double *feature2D, double *classColumn, double *outputFeatures, double *featureScores)*/
                    discCMIM(k, numberOfSamples, numberOfFeatures, feature2D, targets, output, scores);

                    incrementVector(output, k);
                    break;
                }
                case 4: /* JMI */
                {
                    plhs[0] = mxCreateDoubleMatrix(k, 1, mxREAL);
                    output = (double *) mxGetPr(plhs[0]);

                    /*double* JMI(uint k, uint noOfSamples, uint noOfFeatures,double *feature2D, double *classColumn, double *outputFeatures, double *featureScores)*/
                    discJMI(k, numberOfSamples, numberOfFeatures, feature2D, targets, output, scores);

                    incrementVector(output, k);
                    break;
                }
                case 5: /* DISR */
                {
                    plhs[0] = mxCreateDoubleMatrix(k, 1, mxREAL);
                    output = (double *) mxGetPr(plhs[0]);

                    /*double* discDISR(uint k, uint noOfSamples, uint noOfFeatures,double *feature2D, double *classColumn, double *outputFeatures, double *featureScores)*/
                    discDISR(k, numberOfSamples, numberOfFeatures, feature2D, targets, output, scores);

                    incrementVector(output, k);
                    break;
                }
                case 6: /* CIFE */
                {
                    plhs[0] = mxCreateDoubleMatrix(k, 1, mxREAL);
                    output = (double *) mxGetPr(plhs[0]);

                    /* CIFE is Beta = 1, Gamma = 1 */
                    optionalParam1 = 1.0;
                    optionalParam2 = 1.0;

                    /*double* discBetaGamma(uint k, uint noOfSamples, uint noOfFeatures,double *feature2D, double *classColumn, double *outputFeatures, double *featureScores, double beta, double gamma)*/
                    discBetaGamma(k, numberOfSamples, numberOfFeatures, feature2D, targets, output, scores, optionalParam1, optionalParam2);

                    incrementVector(output, k);
                    break;
                }
                case 7: /* ICAP */
                {
                    plhs[0] = mxCreateDoubleMatrix(k, 1, mxREAL);
                    output = (double *) mxGetPr(plhs[0]);

                    /*double* discICAP(uint k,uint numberOfSamples,uint numberOfFeatures,double *feature2D, double *targets, double *output, double *featureScores);*/
                    discICAP(k, numberOfSamples, numberOfFeatures, feature2D, targets, output, scores);

                    incrementVector(output, k);
                    break;
                }
                case 8: /* CondRed */
                {
                    plhs[0] = mxCreateDoubleMatrix(k, 1, mxREAL);
                    output = (double *) mxGetPr(plhs[0]);

                    /* CondRed is Beta = 0, Gamma = 1 */
                    optionalParam1 = 0.0;
                    optionalParam2 = 1.0;

                    /*double* discBetaGamma(uint k, uint noOfSamples, uint noOfFeatures,double *featureMatrix, double *classColumn, double *outputFeatures, double *featureScores, double beta, double gamma)*/
                    discBetaGamma(k, numberOfSamples, numberOfFeatures, feature2D, targets, output, scores, optionalParam1, optionalParam2);

                    incrementVector(output, k);
                    break;
                }
                case 9: /* BetaGamma */
                {
                    if (nrhs != 6) {
                        printf("Insufficient arguments specified for Beta Gamma FS\n");
                        plhs[0] = mxCreateDoubleMatrix(0, 0, mxREAL);
                        if (nlhs == 2) {
                            plhs[1] = mxCreateDoubleMatrix(0, 0, mxREAL);
                        }
                        mxFree(feature2D);
                        return;
                    } else {
                        plhs[0] = mxCreateDoubleMatrix(k, 1, mxREAL);
                        output = (double *) mxGetPr(plhs[0]);

                        /*double* discBetaGamma(uint k, uint noOfSamples, uint noOfFeatures,double *featureMatrix, double *classColumn, double *outputFeatures, double *featureScores, double beta, double gamma)*/
                        discBetaGamma(k, numberOfSamples, numberOfFeatures, feature2D, targets, output, scores, optionalParam1, optionalParam2);

                        incrementVector(output, k);
                    }
                    break;
                }
                case 10: /* CMI */
                {
                    output = (double *) mxCalloc(k, sizeof(double));

                    /*double* discCondMI(uint k, uint noOfSamples, uint noOfFeatures,double *featureMatrix, double *classColumn, double *outputFeatures, double *featureScores)*/
                    discCondMI(k, numberOfSamples, numberOfFeatures, feature2D, targets, output, scores);

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
            }/*switch on flag*/
            mxFree(feature2D);
            if (nlhs == 1) {
                mxFree(scores);
            }
            return;
        }
    }
}/*mex function entry*/
