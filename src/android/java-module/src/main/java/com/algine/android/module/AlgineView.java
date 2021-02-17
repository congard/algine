package com.algine.android.module;

import android.annotation.SuppressLint;
import android.content.Context;
import android.opengl.GLSurfaceView;

public class AlgineView extends GLSurfaceView {
    private boolean isSurfaceCreated = false;

    public AlgineView(Context context) {
        super(context);

        if (Algine.algineView != null) {
            throw new IllegalStateException("Instance of AlgineView has been already created");
        }

        Algine.algineView = this;

        // to call onSurfaceCreated only once
        setPreserveEGLContextOnPause(true);

        // Pick an EGLConfig with RGB8 color, 16-bit depth, no stencil,
        // supporting OpenGL ES 2.0 or later backwards-compatible versions.
        setEGLConfigChooser(8, 8, 8, 0, 16, 0);
        setEGLContextClientVersion(3);
    }

    void surfaceCreated() {
        isSurfaceCreated = true;
    }

    public boolean isSurfaceCreated() {
        return isSurfaceCreated;
    }

    @SuppressLint("ClickableViewAccessibility")
    @Override
    public void setOnTouchListener(OnTouchListener l) {
        if (!(l instanceof AlgineViewActionHandler)) {
            Algine.algineViewActionHandler = null;
        } else {
            Algine.algineViewActionHandler = (AlgineViewActionHandler) l;
        }

        super.setOnTouchListener(l);
    }
}
