#include <jni.h>

#ifndef __FEASTJNIUtil_h
#define __FEASTJNIUtil_h
#ifdef __cplusplus
extern "C" {
#endif

jint throwOutOfMemoryError(JNIEnv *env, char *message);

#ifdef __cplusplus
}
#endif
#endif

