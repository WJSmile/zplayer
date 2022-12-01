#include <jni.h>
#include <string>
#include <android/native_window_jni.h>
#include "Player.h"
#include "XLog.h"

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
Java_com_zwj_lib_zplayer_ZPlayer_open(JNIEnv *env, jobject thiz, jstring url, jobject holder) {
    Player *player = getDistinguishFromObj(env, thiz);
    if (player == nullptr) {
        player = new Player();
        setDistinguishFromObj(env, thiz, reinterpret_cast<jlong>(player));
    }
    player->open(env->GetStringUTFChars(url, nullptr), ANativeWindow_fromSurface(env, holder));
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zwj_lib_zplayer_ZPlayer_paused(JNIEnv *env, jobject thiz, jboolean boolean) {
    Player *player = getDistinguishFromObj(env, thiz);
    if (player != nullptr) {
        player->paused(boolean);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zwj_lib_zplayer_ZPlayer_stop(JNIEnv *env, jobject thiz) {
    Player *player = getDistinguishFromObj(env, thiz);
    if (player != nullptr) {
        player->stop();
    }
}