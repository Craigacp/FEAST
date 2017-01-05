#include <jni.h>
#include <stdlib.h>
#include <FEAST/FSAlgorithms.h>
#include "craigacp_feast_FEAST.h"

jint throwOutOfMemoryError(JNIEnv *env, char *message)
{
    jclass exClass;
    char *className = "java/lang/OutOfMemoryError" ;

    exClass = (*env)->FindClass(env, className);

    return (*env)->ThrowNew(env, exClass, message);
}

/*
 * Class:     craigacp_feast_FEAST
 * Method:    BetaGamma
 * Signature: (I[[I[IDD)Lcraigacp/feast/ScoredFeatures;
 */
JNIEXPORT jobject JNICALL Java_craigacp_feast_FEAST_BetaGamma(JNIEnv * env, jclass clazz, jint numToSelect, jobjectArray data, jintArray labels, jdouble beta, jdouble gamma) {
    
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
    jsize numLabels, numFeatures;
    double *scores;
    int *indices;
    jdoubleArray javaScores;
    jintArray javaIndices;
    jclass cls;
    jmethodID midInit;
    jobject scoredFeatures;
    
    /* Extract a C array for the labels */
    labels = (*env)->GetIntArrayElements(env, javaLabels, NULL);
    if (labels == NULL) {
        throwOutOfMemoryError(env, "Could not allocate array to hold the labels");
    }
    numLabels = (*env)->GetArrayLength(env, javaLabels);

    /* Extract a C array for the data */


    /* Malloc space for indices and scores */
    scores = calloc(numToSelect, sizeof(double));
    indices = calloc(numToSelect, sizeof(int));

    switch (flag) {
        case 1: /* CMIM */
            /* uint* CMIM(uint k, uint noOfSamples, uint noOfFeatures, uint **featureMatrix, uint *classColumn, uint *outputFeatures, double *featureScores); */
            CMIM(numToSelect,numLabels,numFeatures,data,labels,indices,scores);
            break;
        case 2: /* CondMI */
            /* int* CondMI(uint k, uint noOfSamples, uint noOfFeatures, uint **featureMatrix, uint *classColumn, int *outputFeatures, double *featureScores) */
            CondMI(numToSelect,numLabels,numFeatures,data,labels,indices,scores);
            break;
        case 3: /* DISR */
            DISR(numToSelect,numLabels,numFeatures,data,labels,indices,scores);
            break;
        case 4: /* ICAP */
            ICAP(numToSelect,numLabels,numFeatures,data,labels,indices,scores);
            break;
        case 5: /* JMI */
            JMI(numToSelect,numLabels,numFeatures,data,labels,indices,scores);
            break;
        case 6: /* MIM */
            MIM(numToSelect,numLabels,numFeatures,data,labels,indices,scores);
            break;
        case 7: /* mRMR */
            mRMR_D(numToSelect,numLabels,numFeatures,data,labels,indices,scores);
            break;
        default:
            printf("Unknown flag %d\n", flag);
    }
    
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