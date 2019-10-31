const char PROJECT_VERSION_STRING[] = "1.0-android";

// Android build system fix
// TODO: find a way to keep android native BC API's from stripping
#include <jni.h>
#include <android/asset_manager_jni.h>
#include <android/native_window_jni.h>
void Java_info_djukic_bcgl_BCGLLib_nativeInitFileSystem(JNIEnv *env, jclass type, jobject manager, jstring local_path, jstring external_path);
void Java_info_djukic_bcgl_BCGLLib_nativeSurfaceCreated(JNIEnv *env, jclass type, jint id, jobject surface);
void Java_info_djukic_bcgl_BCGLLib_nativeSurfaceChanged(JNIEnv *env, jclass type, jint id, jobject surface, jint format, jint width, jint height);
void Java_info_djukic_bcgl_BCGLLib_nativeSurfaceDestroyed(JNIEnv *env, jclass type, jint id, jobject surface);
void Java_info_djukic_bcgl_BCGLLib_nativeAppChangeState(JNIEnv *env, jclass type, jint state);
void Java_info_djukic_bcgl_BCGLLib_nativeTouchEvent(JNIEnv *env, jclass type, jint event, jint id, jfloat x, jfloat y);
void Java_info_djukic_bcgl_BCGLLib_nativeKeyEvent(JNIEnv *env, jclass type, jint event, jint key, jint code);
void __keep_symbols_from_stripping()
{
    Java_info_djukic_bcgl_BCGLLib_nativeInitFileSystem(0, 0, 0, 0, 0);
    Java_info_djukic_bcgl_BCGLLib_nativeSurfaceCreated(0, 0, 0, 0);
    Java_info_djukic_bcgl_BCGLLib_nativeSurfaceChanged(0, 0, 0, 0, 0, 0, 0);
    Java_info_djukic_bcgl_BCGLLib_nativeSurfaceDestroyed(0, 0, 0, 0);
    Java_info_djukic_bcgl_BCGLLib_nativeAppChangeState(0, 0, 0);
    Java_info_djukic_bcgl_BCGLLib_nativeTouchEvent(0, 0, 0, 0, 0, 0);
    Java_info_djukic_bcgl_BCGLLib_nativeKeyEvent(0, 0, 0, 0, 0);
}
