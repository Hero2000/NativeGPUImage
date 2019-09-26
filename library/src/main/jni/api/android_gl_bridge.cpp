//
// Created by ben622 on 2019/9/25.
//

#include "android_gl_bridge.hpp"
#include "../util/jni_util.hpp"
#include "../util/filter_type.hpp"
#include "../gpu_image_render.hpp"

using namespace ben::util;

ben::ngp::NGPNativeBridge::NGPNativeBridge(JNIEnv *env) : JavaClass(env) {
    initialize(env);
}

void ben::ngp::NGPNativeBridge::initialize(JNIEnv *env) {
    setClass(env);
    addNativeMethod("setFilter", (void *) &setFilter,
                    kTypeVoid,
                    "Lcom/ben/android/library/filter/NativeFilter;", NULL);

    addNativeMethod("setBitmap", (void *) &setBitmap,
                    kTypeVoid,
                    "Landroid/graphics/Bitmap;", NULL);

    addNativeMethod("capture", (void *) &capture,
                    kTypeVoid,
                    "Landroid/graphics/Bitmap;", NULL);

    addNativeMethod("requestRender", (void *) &requestRender,
                    kTypeVoid, NULL);
    registerNativeMethods(env);
}

void ben::ngp::NGPNativeBridge::mapFields() {

}

const char *ben::ngp::NGPNativeBridge::getCanonicalName() const {
    return "com/ben/android/library/NGPNativeBridge";
}

void ben::ngp::NGPNativeBridge::capture(JNIEnv *env, jclass javaThis,jobject object) {
    GPUImageRender *render = getNativeClassPtr<GPUImageRender>(
            GPU_IMAGE_RENDER_CLASS);
    render->getFilter()->setIsFBO(true);
    if (dynamic_cast<GPUImageFilterGroup*>(render->getFilter())!=NULL) {
        GPUImageFilterGroup *filterGroup = dynamic_cast<GPUImageFilterGroup *>(render->getFilter());
        for (ben::ngp::GPUImageFilter *filter : filterGroup->getFilters()) {
            filter->setIsFBO(true);
        }
        for (ben::ngp::GPUImageFilter *filter : filterGroup->getMergedFilters()) {
            filter->setIsFBO(true);
        }
    }
    render->reqeustRender();
    unsigned char *srcByteBuffer;
    int result = 0;
    int i, j;
    AndroidBitmapInfo srcInfo;

    result = AndroidBitmap_getInfo(env, object, &srcInfo);
    if (result != ANDROID_BITMAP_RESULT_SUCCESS) {
        return;
    }

    result = AndroidBitmap_lockPixels(env, object, (void **) &srcByteBuffer);
    if (result != ANDROID_BITMAP_RESULT_SUCCESS) {
        return;
    }

    int width = srcInfo.width;
    int height = srcInfo.height;
    glReadPixels(0, 0, srcInfo.width, srcInfo.height, GL_RGBA, GL_UNSIGNED_BYTE,
                 srcByteBuffer);

    int *pIntBuffer = (int *) srcByteBuffer;

    for (i = 0; i < height / 2; i++) {
        for (j = 0; j < width; j++) {
            int temp = pIntBuffer[(height - i - 1) * width + j];
            pIntBuffer[(height - i - 1) * width + j] = pIntBuffer[i * width + j];
            pIntBuffer[i * width + j] = temp;
        }
    }
    AndroidBitmap_unlockPixels(env, object);
    LOGI("%s","read pixels successful!");

    render->getFilter()->setIsFBO(false);
    if (dynamic_cast<GPUImageFilterGroup*>(render->getFilter())!=NULL) {
        GPUImageFilterGroup *filterGroup = dynamic_cast<GPUImageFilterGroup *>(render->getFilter());
        for (ben::ngp::GPUImageFilter *filter : filterGroup->getFilters()) {
            filter->setIsFBO(false);
        }
        for (ben::ngp::GPUImageFilter *filter : filterGroup->getMergedFilters()) {
            filter->setIsFBO(false);
        }
    }
}

void ben::ngp::NGPNativeBridge::requestRender(JNIEnv *env, jclass javaThis) {
    GPUImageRender *render = getNativeClassPtr<GPUImageRender>(
            GPU_IMAGE_RENDER_CLASS);
    render->reqeustRender();
}

void ben::ngp::NGPNativeBridge::setBitmap(JNIEnv *env, jclass javaThis, jobject object) {
    GPUImageRender *render = getNativeClassPtr<GPUImageRender>(
            GPU_IMAGE_RENDER_CLASS);
    //set rotation
    render->setRotation(ROTATION_180);
    render->renderBitmap(env, object);
}

void ben::ngp::NGPNativeBridge::setFilter(JNIEnv *env, jclass javaThis, jobject object) {
    //convert java object to native object.
    NativeFilter *nativeFilterPtr = getNativeClassPtr<NativeFilter>(JAVA_NATIVE_FILTER);
    LOGI("native filter type:%d", nativeFilterPtr->getFilterType());
    nativeFilterPtr->setJavaObject(env, object);
    LOGI("java filter type:%d", nativeFilterPtr->getFilterType());
    GPUImageRender *render = getNativeClassPtr<GPUImageRender>(
            GPU_IMAGE_RENDER_CLASS);
    switch (nativeFilterPtr->getFilterType()) {
        case CONTRAST: {
            break;
        }
        case GRAYSCALE: {
            break;
        }
        case SHARPEN: {
            break;
        }
        case SEPIA: {
            break;
        }
        case SOBEL_EDGE_DETECTION: {
            break;
        }
        case THRESHOLD_EDGE_DETECTION: {
            break;
        }
        case THREE_X_THREE_CONVOLUTION: {
            break;
        }
        case FILTER_GROUP: {
            break;
        }
        case EMBOSS: {
            break;
        }
        case POSTERIZE: {
            break;
        }
        case GAMMA: {
            break;
        }
        case BRIGHTNESS: {
            break;
        }
        case INVERT: {
            break;
        }
        case HUE: {
            break;
        }
        case PIXELATION: {
            GPUImagePixelationFilter *filterPtr = getNativeClassPtr<GPUImagePixelationFilter>(
                    JAVA_PIXELATION_FILTER);
            LOGE("pixel %f",filterPtr->getPixel());
            filterPtr->setJavaObject(env, object);
            LOGE("pixel %f",filterPtr->getPixel());
            render->resetFilter(filterPtr);
            break;
        }
        case SATURATION: {
            break;
        }
        case EXPOSURE: {
            break;
        }
        case HIGHLIGHT_SHADOW: {
            break;
        }
        case MONOCHROME: {
            break;
        }
        case OPACITY: {
            break;
        }
        case RGB: {
            break;
        }
        case WHITE_BALANCE: {
            GPUImageWhiteBalanceFilter *filterPtr = getNativeClassPtr<GPUImageWhiteBalanceFilter>(
                    JAVA_WHITE_BALANCE_FILTER);
            filterPtr->setJavaObject(env, object);
            render->resetFilter(filterPtr);
            break;
        }
        case VIGNETTE: {
            break;
        }
        case TONE_CURVE: {
            break;
        }
        case LUMINANCE: {
            break;
        }
        case LUMINANCE_THRESHSOLD: {
            break;
        }
        case BLEND_COLOR_BURN: {
            break;
        }
        case BLEND_COLOR_DODGE: {
            break;
        }
        case BLEND_DARKEN: {
            break;
        }
        case BLEND_DIFFERENCE: {
            break;
        }
        case BLEND_DISSOLVE: {
            break;
        }
        case BLEND_EXCLUSION: {
            break;
        }
        case BLEND_SOURCE_OVER: {
            break;
        }
        case BLEND_HARD_LIGHT: {
            break;
        }
        case BLEND_LIGHTEN: {
            break;
        }
        case BLEND_ADD: {
            break;
        }
        case BLEND_DIVIDE: {
            break;
        }
        case BLEND_MULTIPLY: {
            break;
        }
        case BLEND_OVERLAY: {
            break;
        }
        case BLEND_SCREEN: {
            break;
        }
        case BLEND_ALPHA: {
            break;
        }
        case BLEND_COLOR: {
            break;
        }
        case BLEND_HUE: {
            break;
        }
        case BLEND_SATURATION: {
            break;
        }
        case BLEND_LUMINOSITY: {
            break;
        }
        case BLEND_LINEAR_BURN: {
            break;
        }
        case BLEND_SOFT_LIGHT: {
            break;
        }
        case BLEND_SUBTRACT: {
            break;
        }
        case BLEND_CHROMA_KEY: {
            break;
        }
        case BLEND_NORMAL: {
            break;
        }
        case LOOKUP_AMATORKA: {
            break;
        }
        case GAUSSIAN_BLUR: {
            //高斯滤波
            GPUImageGaussianBlurFilter *filterPtr = getNativeClassPtr<GPUImageGaussianBlurFilter>(
                    JAVA_GAUSSIAN_BLUR_FILTER);
            LOGE("blursize %f",filterPtr->getBlurSize());
            filterPtr->setJavaObject(env, object);
            LOGE("blursize %f",filterPtr->getBlurSize());
            break;
        }
        case CROSSHATCH: {
            break;
        }
        case BOX_BLUR: {
            break;
        }
        case CGA_COLORSPACE: {
            break;
        }
        case DILATION: {
            GPUImageDilationFilter *filterPtr = getNativeClassPtr<GPUImageDilationFilter>(
                    JAVA_DILATION_FILTER);
            LOGE("radius %f",filterPtr->getRadius());
            filterPtr->setJavaObject(env, object);
            LOGE("radius %f",filterPtr->getRadius());
            render->resetFilter(filterPtr);
            render->setRotation(ROTATION_180);
            break;
        }
        case KUWAHARA: {
            break;
        }
        case RGB_DILATION: {
            break;
        }
        case SKETCH: {
            break;
        }
        case TOON: {
            break;
        }
        case SMOOTH_TOON: {
            break;
        }
        case BULGE_DISTORTION: {
            break;
        }
        case GLASS_SPHERE: {
            break;
        }
        case HAZE: {
            break;
        }
        case LAPLACIAN: {
            break;
        }
        case NON_MAXIMUM_SUPPRESSION: {
            break;
        }
        case SPHERE_REFRACTION: {
            break;
        }
        case SWIRL: {
            break;
        }
        case WEAK_PIXEL_INCLUSION: {
            break;
        }
        case FALSE_COLOR: {
            break;
        }
        case COLOR_BALANCE: {
            break;
        }
        case LEVELS_FILTER_MIN: {
            break;
        }
        case BILATERAL_BLUR: {
            break;
        }
        case ZOOM_BLUR: {
            GPUImageZoomBlurFilter *filterPtr = getNativeClassPtr<GPUImageZoomBlurFilter>(
                    JAVA_ZOOM_BLUR_FILTER);
            filterPtr->setJavaObject(env, object);
            render->resetFilter(filterPtr);
            break;
        }
        case HALFTONE: {
            break;
        }
        case TRANSFORM2D: {
            break;
        }
        case SOLARIZE: {
            break;
        }
        case VIBRANCE: {
            break;
        }
        default: {
            LOGE("filter of type %d is not supported!", nativeFilterPtr->getFilterType());
            break;
        }
    }


}
