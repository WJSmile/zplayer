#include <jni.h>
#include <string>

#include "Player.h"
#include "XLog.h"

extern "C" {
#include <android/native_window_jni.h>
#include <libavcodec/jni.h>
}

std::mutex mux;
extern "C"
JNIEXPORT
jint JNI_OnLoad(JavaVM *vm, void *res) {
    av_jni_set_java_vm(vm, nullptr);
    return JNI_VERSION_1_6;
}

Player *getDistinguishFromObj(JNIEnv *env, jobject obj) {
    auto objClazz = (jclass) env->GetObjectClass(obj);
    if (objClazz == nullptr) {
        XLOGE("env->GetObjectClass return null.");
        return nullptr;
    }
    jfieldID fid = env->GetFieldID(objClazz, "playerId", "J");
    if (fid == nullptr) {
        XLOGE("env->GetFieldID error.");
        return nullptr;
    }
    auto p = (jlong) env->GetLongField(obj, fid);
    if (p == -1) {
        return nullptr;
    }
    return (Player *) p;
}

void setDistinguishFromObj(JNIEnv *env, jobject obj, jlong jlong1) {
    auto objClazz = (jclass) env->GetObjectClass(obj);
    if (objClazz == nullptr) {
        XLOGE("env->GetObjectClass return null.");
        return;
    }
    jfieldID fid = env->GetFieldID(objClazz, "playerId", "J");
    if (fid == nullptr) {
        XLOGE("env->GetFieldID error.");
        return;
    }
    env->SetLongField(obj, fid, jlong1);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_zwj_lib_zplayer_ZPlayer_open(JNIEnv *env, jobject thiz, jstring url, jobject holder,
                                      jboolean is_use_gl) {
    mux.lock();
    Player *player = getDistinguishFromObj(env, thiz);
    if (player == nullptr) {
        player = new Player();
        setDistinguishFromObj(env, thiz, reinterpret_cast<jlong>(player));
    }
    player->open(env->GetStringUTFChars(url, nullptr), holder,
                 ANativeWindow_fromSurface(env, holder), is_use_gl);
    mux.unlock();
}



extern "C"
JNIEXPORT void JNICALL
Java_com_zwj_lib_zplayer_ZPlayer_paused(JNIEnv *env, jobject thiz, jboolean boolean) {
    mux.lock();
    Player *player = getDistinguishFromObj(env, thiz);
    if (player != nullptr) {
        player->paused(boolean);
    }
    mux.unlock();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zwj_lib_zplayer_ZPlayer_stop(JNIEnv *env, jobject thiz) {
    mux.lock();
    Player *player = getDistinguishFromObj(env, thiz);
    if (player != nullptr) {
        player->stop();
    }
    mux.unlock();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zwj_lib_zplayer_ZPlayer_setOnProgressListen(JNIEnv *env, jobject thiz,
                                                     jobject on_progress_listen) {


    mux.lock();
    Player *player = getDistinguishFromObj(env, thiz);
    if (player != nullptr) {
        player->setOnProgressListen(env,on_progress_listen);
    }
    mux.unlock();

}