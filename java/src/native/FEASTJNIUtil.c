#include <jni.h>
#include "FEASTJNIUtil.h"

jint throwOutOfMemoryError(JNIEnv *env, char *message)
{
    jclass exClass;
    char *className = "java/lang/OutOfMemoryError" ;

    exClass = (*env)->FindClass(env, className);

    return (*env)->ThrowNew(env, exClass, message);
}

