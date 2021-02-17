package com.algine.android.module;

import android.annotation.SuppressLint;
import android.content.Context;
import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class Algine {
    static AlgineActivity algineActivity;
    static AlgineView algineView;
    static AlgineViewActionHandler algineViewActionHandler;
    static String applicationLibraryName;
    static boolean initialized;

    static {
        toDefaults();
    }

    @SuppressLint("ClickableViewAccessibility")
    public static void init(Context context) {
        if (isInitialized()) {
            throw new RuntimeException("Algine has been already initialized");
        }

        if (algineActivity == null) {
            throw new RuntimeException("Algine.init called, but instance of AlgineActivity has not created");
        }

        if (applicationLibraryName == null) {
            throw new RuntimeException("Algine.init called, but application library name has not set");
        }

        AlgineView view = new AlgineView(context); // will set Algine.algineView
        view.setOnTouchListener(new AlgineViewActionHandler()); // will set Algine.algineViewActionHandler

        view.setRenderer(new GLSurfaceView.Renderer() {
            @Override
            public void onSurfaceCreated(GL10 gl, EGLConfig config) {
                Bridge.init(Algine.getApplicationLibraryName());

                view.surfaceCreated();
            }

            @Override
            public void onSurfaceChanged(GL10 gl, int width, int height) {
                Bridge.surfaceResized(width, height);
            }

            @Override
            public void onDrawFrame(GL10 gl) {
                Bridge.renderFrame();
            }
        });

        algineActivity.setContentView(view);

        initialized = true;
    }

    public static void setApplicationLibraryName(String applicationLibraryName, boolean isFile) {
        if (isInitialized()) {
            throw new RuntimeException("You cannot set application library name after the initialization");
        }

        if (!isFile) {
            Algine.applicationLibraryName =
                    "lib" + applicationLibraryName + (BuildConfig.DEBUG ? "d" : "") + ".so";
        } else {
            Algine.applicationLibraryName = applicationLibraryName;
        }
    }

    public static void setApplicationLibraryName(String applicationLibraryName) {
        setApplicationLibraryName(applicationLibraryName, false);
    }

    public static AlgineActivity getActivity() {
        return algineActivity;
    }

    public static AlgineView getView() {
        return algineView;
    }

    public static String getApplicationLibraryName() {
        return applicationLibraryName;
    }

    public static boolean isInitialized() {
        return initialized;
    }

    static void toDefaults() {
        algineActivity = null;
        algineView = null;
        algineViewActionHandler = null;
        applicationLibraryName = null;
        initialized = false;
    }
}
