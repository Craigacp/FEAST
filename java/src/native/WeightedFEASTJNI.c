#include <jni.h>
#include <stdlib.h>
#include <FEAST/WeightedFSAlgorithms.h>
#include "FEASTJNIUtil.h"
#include "craigacp_feast_WeightedFEAST.h"

/*
 * Class:     craigacp_feast_WeightedFEAST
 * Method:    weightedFeast
 * Signature: (II[[I[I[D)Lcraigacp/feast/ScoredFeatures;
 * 
 * Flag mapping:
 * 
 * 1 = CMIM
 * 2 = CondMI
 * 3 = DISR
 * 4 = JMI
 * 5 = MIM
 * 
 */
JNIEXPORT jobject JNICALL Java_craigacp_feast_WeightedFEAST_weightedFeast(JNIEnv * env, jclass clazz, jint flag, jint numToSelect, jobjectArray javaData, jintArray javaLabels, jdoubleArray javaWeights) {
    jint *labels;
    jint **data;
    jdouble *weights;
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
    
    /* Extract a C array for the weights */
    weights = (*env)->GetDoubleArrayElements(env, javaWeights, NULL);
    if (weights == NULL) {
        throwOutOfMemoryError(env, "Could not allocate array to hold the weights");
    }
    
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
            weightedCMIM(numToSelect,numLabels,numFeatures,udata,ulabels,weights,uindices,scores);
            break;
        case 2: /* CondMI */
            /* int* CondMI(uint k, uint noOfSamples, uint noOfFeatures, uint **featureMatrix, uint *classColumn, int *outputFeatures, double *featureScores) */
            weightedCondMI(numToSelect,numLabels,numFeatures,udata,ulabels,weights,indices,scores);
            break;
        case 3: /* DISR */
            weightedDISR(numToSelect,numLabels,numFeatures,udata,ulabels,weights,uindices,scores);
            break;
        case 4: /* JMI */
            weightedJMI(numToSelect,numLabels,numFeatures,udata,ulabels,weights,uindices,scores);
            break;
        case 5: /* MIM */
            weightedMIM(numToSelect,numLabels,numFeatures,udata,ulabels,weights,uindices,scores);
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