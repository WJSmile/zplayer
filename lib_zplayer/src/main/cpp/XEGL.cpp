//
// Created by zwj on 2022/12/4.
//


#include "XEGL.h"
#include "XLog.h"
#include <EGL/egl.h>

class CEGL : public XEGL {
public:
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext eglContext = EGL_NO_CONTEXT;

    bool Init(void *window) override {
        auto *nativeWindow = (ANativeWindow *) window;

        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (display == EGL_NO_DISPLAY) {
            XLOGE("egl 初始化失败");
        }
        if (EGL_TRUE != eglInitialize(display, nullptr, nullptr)) {
            XLOGE("eglInitialize 初始化失败");
            return false;
        }
        EGLint configSpec[] = {
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_NONE
        };
        EGLConfig config = nullptr;
        EGLint num = 0;
        if (EGL_TRUE != eglChooseConfig(display, configSpec, &config, 1, &num)) {
            XLOGE("configSpec 初始化失败");
            return false;
        }
        surface = eglCreateWindowSurface(display, config, nativeWindow, nullptr);

        const EGLint ctxAttr[] = {
                EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE
        };
        eglContext = eglCreateContext(display, config, EGL_NO_CONTEXT, ctxAttr);
        if (eglContext == EGL_NO_CONTEXT) {
            XLOGE("eglContext 初始化失败");
            return false;
        }
        if (EGL_TRUE != eglMakeCurrent(display, surface, surface, eglContext)) {
            XLOGE("eglMakeCurrent 初始化失败");
            return false;
        }
        return true;
    }

    void Draw() override {
        if (display == EGL_NO_DISPLAY || surface == EGL_NO_SURFACE) {
            return;
        }
        eglSwapBuffers(display, surface);
    }
};

XEGL *XEGL::Get() {
    static CEGL cegl;
    return &cegl;
}
