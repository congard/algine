package com.algine.android.module;

import android.app.Activity;
import android.opengl.GLSurfaceView;
import android.os.Handler;
import android.os.Looper;
import android.widget.Toast;

import static com.algine.android.module.Algine.algineViewActionHandler;

public class Bridge {

    static {
        System.loadLibrary(BuildConfig.ALGINE_LIBRARY);
    }

    // output calls

    public static native void init(String applicationLibrary, Activity activity);

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
        return isActionHandlerAvailable() && algineViewActionHandler.getPointer(pointerId) != null;
    }

    public static float getPointerX(int pointerId) {
        if (!isPointerAvailable(pointerId))
            return -1;

        return algineViewActionHandler.getPointer(pointerId).getX();
    }

    public static float getPointerY(int pointerId) {
        if (!isPointerAvailable(pointerId))
            return -1;

        return algineViewActionHandler.getPointer(pointerId).getY();
    }

    public static float[] getPointerPos(int pointerId) {
        return new float[] {getPointerX(pointerId), getPointerY(pointerId)};
    }

    public static int[] getViewDimensions() {
        return new int[] {Algine.getView().getWidth(), Algine.getView().getHeight()};
    }

    public static void showToast(String text, int length) {
        new Handler(Looper.getMainLooper()).post(() ->
                Toast.makeText(Algine.getActivity(), text, length).show());
    }
}
