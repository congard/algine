package com.algine.android.module;

import android.opengl.GLSurfaceView;

import static com.algine.android.module.Algine.algineViewActionHandler;

public class Bridge {

    static {
        System.loadLibrary(BuildConfig.ALGINE_LIBRARY);
    }

    // output calls

    public static native void init(String applicationLibrary);

    public static native void pointerDown(float x, float y, int pointerId);
    public static native void pointerMove(float x, float y, int pointerId);
    public static native void pointerUp(float x, float y, int pointerId);
    public static native void pointerClick(float x, float y, int pointerId);

    public static native void surfaceResized(int width, int height);

    public static native void renderFrame();

    public static native void onResume();
    public static native void onPause();
    public static native void onDestroy();

    // input calls

    private static void setAlgineViewRenderMode(int renderMode) {
        AlgineView view = Algine.getView();
        view.post(() -> view.setRenderMode(renderMode));
    }

    public static void renderLoop() {
        setAlgineViewRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
    }

    public static void stopRenderLoop() {
        setAlgineViewRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }

    public static boolean isRenderLoopRunning() {
        return Algine.getView().getRenderMode() == GLSurfaceView.RENDERMODE_CONTINUOUSLY;
    }

    public static boolean isActionHandlerAvailable() {
        return algineViewActionHandler != null;
    }

    public static boolean isPointerAvailable(int pointerId) {
        return isActionHandlerAvailable() && algineViewActionHandler.pointers[pointerId] != null;
    }

    public static float getPointerX(int pointerId) {
        if (!isPointerAvailable(pointerId))
            return -1;

        return algineViewActionHandler.pointers[pointerId].x;
    }

    public static float getPointerY(int pointerId) {
        if (!isPointerAvailable(pointerId))
            return -1;

        return algineViewActionHandler.pointers[pointerId].y;
    }

    public static float[] getPointerPos(int pointerId) {
        return new float[] {getPointerX(pointerId), getPointerY(pointerId)};
    }

    public static int[] getViewDimensions() {
        return new int[] {Algine.getView().getWidth(), Algine.getView().getHeight()};
    }
}
