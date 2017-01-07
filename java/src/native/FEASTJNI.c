/*******************************************************************************
** FEASTJNI.c - a JNI wrapper for FEAST
**
** Initial Version - 07/01/2017
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
#include <jni.h>
#include <stdlib.h>
#include <FEAST/FSAlgorithms.h>
#include "FEASTJNIUtil.h"
#include "craigacp_feast_FEAST.h"

/*
 * Class:     craigacp_feast_FEAST
 * Method:    BetaGamma
 * Signature: (I[[I[IDD)Lcraigacp/feast/ScoredFeatures;
 */
JNIEXPORT jobject JNICALL Java_craigacp_feast_FEAST_BetaGamma(JNIEnv * env, jclass clazz, jint numToSelect, jobjectArray javaData, jintArray javaLabels, jdouble beta, jdouble gamma) {
    jint *labels;
    jint **data;
    jsize numLabels, numFeatures;
    double *scores;
    int *indices;
    jdoubleArray javaScores;
    jintArray javaIndices, feature;
    jclass cls;
    jmethodID midInit;
    jobject scoredFeatures;
    int i;
    
    /* Extract a C array for the labels */
    labels = (*env)->GetIntArrayElements(env, javaLabels, NULL);
    if (labels == NULL) {
        throwOutOfMemoryError(env, "Could not allocate array to hold the labels");
    }
    numLabels = (*env)->GetArrayLength(env, javaLabels);

    /* Extract a C array for the data */
    numFeatures = (*env)->GetArrayLength(env, javaData);
    data = calloc(numFeatures, sizeof(int*));
    for (i = 0; i < numFeatures; i++) {
        feature = (jintArray)(*env)->GetObjectArrayElement(env, javaData, i);
        data[i] = (*env)->GetIntArrayElements(env, feature, NULL);
    }

    /* Malloc space for indices and scores */
    scores = calloc(numToSelect, sizeof(double));
    indices = calloc(numToSelect, sizeof(int));

    /* uint* BetaGamma(uint k, uint noOfSamples, uint noOfFeatures, uint **featureMatrix, uint *classColumn, uint *outputFeatures, double *featureScores, double beta, double gamma); */
    /* The cast from int** to uint** could cause problems on different architectures.
       The Java code above this should verify all integers are non-negative otherwise 
       things will crash. */
    BetaGamma(numToSelect,numLabels,numFeatures,(uint**)data,(uint*)labels,(uint*)indices,scores,beta,gamma);
    free(data);
    
    /* Copy out the scores array */
    javaScores = (*env)->NewDoubleArray(env, numToSelect);
    if (javaScores == NULL) {
        throwOutOfMemoryError(env, "Could not allocate array to hold the scores");
    }
    (*env)->SetDoubleArrayRegion(env, javaScores, 0 , numToSelect, scores);
    free(scores);

    /* Copy out the indices array */
    javaIndices = (*env)->NewIntArray(env, numToSelect);
    if (javaIndices == NULL) {
        throwOutOfMemoryError(env, "Could not allocate array to hold the indices");
    }
    (*env)->SetIntArrayRegion(env, javaIndices, 0 , numToSelect, indices);
    free(indices);

    /* Create the ScoredFeatures object and return it */
    cls = (*env)->FindClass(env, "craigacp/feast/ScoredFeatures");
    midInit = (*env)->GetMethodID(env, cls, "<init>", "([I[D)V");
    scoredFeatures = (*env)->NewObject(env, cls, midInit, javaIndices, javaScores);
    
    return scoredFeatures;
}

/*
 * Class:     craigacp_feast_FEAST
 * Method:    feast
 * Signature: (II[[I[I)Lcraigacp/feast/ScoredFeatures;
 * 
 * Flag mapping:
 * 1 = CMIM
 * 2 = CondMI
 * 3 = DISR
 * 4 = ICAP
 * 5 = JMI
 * 6 = MIM
 * 7 = mRMR
 * 
 */
JNIEXPORT jobject JNICALL Java_craigacp_feast_FEAST_feast(JNIEnv * env, jclass clazz, jint flag, jint numToSelect, jobjectArray javaData, jintArray javaLabels) {
    jint *labels;
    jint **data;
    jsize numLabels, numFeatures;
    double *scores;
    int *indices;
    jdoubleArray javaScores;
    jintArray javaIndices, feature;
    jclass cls;
    jmethodID midInit;
    jobject scoredFeatures;
    int i,j;
    uint **udata;
    uint *ulabels, *uindices;
    
    /* Extract a C array for the labels */
    labels = (*env)->GetIntArrayElements(env, javaLabels, NULL);
    if (labels == NULL) {
        throwOutOfMemoryError(env, "Could not allocate array to hold the labels");
    }
    numLabels = (*env)->GetArrayLength(env, javaLabels);

    /* Extract a C array for the data */
    numFeatures = (*env)->GetArrayLength(env, javaData);
    data = calloc(numFeatures, sizeof(int*));
    for (i = 0; i < numFeatures; i++) {
        feature = (jintArray)(*env)->GetObjectArrayElement(env, javaData, i);
        data[i] = (*env)->GetIntArrayElements(env, feature, NULL);
        for (j = 0; j < numLabels; j++) {
            if (data[i][j] < 0) {
                printf("Found negative value %d at location data[%d][%d]\n",data[i][j],i,j);
            }
        }
    }

    /* Malloc space for indices and scores */
    scores = calloc(numToSelect, sizeof(double));
    indices = calloc(numToSelect, sizeof(int));

    udata = (uint **) data;
    ulabels = (uint *) labels;
    uindices = (uint *) indices;
    /* The cast from int** to uint** could cause problems on different architectures.
       The Java code above this should verify all integers are non-negative otherwise 
       things will crash. */
    switch (flag) {
        case 1: /* CMIM */
            /* uint* CMIM(uint k, uint noOfSamples, uint noOfFeatures, uint **featureMatrix, uint *classColumn, uint *outputFeatures, double *featureScores); */
            CMIM(numToSelect,numLabels,numFeatures,udata,ulabels,uindices,scores);
            break;
        case 2: /* CondMI */
            /* int* CondMI(uint k, uint noOfSamples, uint noOfFeatures, uint **featureMatrix, uint *classColumn, int *outputFeatures, double *featureScores) */
            CondMI(numToSelect,numLabels,numFeatures,udata,ulabels,indices,scores);
            break;
        case 3: /* DISR */
            DISR(numToSelect,numLabels,numFeatures,udata,ulabels,uindices,scores);
            break;
        case 4: /* ICAP */
            ICAP(numToSelect,numLabels,numFeatures,udata,ulabels,uindices,scores);
            break;
        case 5: /* JMI */
            JMI(numToSelect,numLabels,numFeatures,udata,ulabels,uindices,scores);
            break;
        case 6: /* MIM */
            MIM(numToSelect,numLabels,numFeatures,udata,ulabels,uindices,scores);
            break;
        case 7: /* mRMR */
            mRMR_D(numToSelect,numLabels,numFeatures,udata,ulabels,uindices,scores);
            break;
        default:
            printf("Unknown flag %d\n", flag);
    }
    free(data);
    
    /* Copy out the scores array */
    javaScores = (*env)->NewDoubleArray(env, numToSelect);
    if (javaScores == NULL) {
        throwOutOfMemoryError(env, "Could not allocate array to hold the scores");
    }
    (*env)->SetDoubleArrayRegion(env, javaScores, 0 , numToSelect, scores);
    free(scores);

    /* Copy out the indices array */
    javaIndices = (*env)->NewIntArray(env, numToSelect);
    if (javaIndices == NULL) {
        throwOutOfMemoryError(env, "Could not allocate array to hold the indices");
    }
    (*env)->SetIntArrayRegion(env, javaIndices, 0 , numToSelect, indices);
    free(indices);

    /* Create the ScoredFeatures object and return it */
    cls = (*env)->FindClass(env, "craigacp/feast/ScoredFeatures");
    midInit = (*env)->GetMethodID(env, cls, "<init>", "([I[D)V");
    scoredFeatures = (*env)->NewObject(env, cls, midInit, javaIndices, javaScores);
    
    return scoredFeatures;
}
