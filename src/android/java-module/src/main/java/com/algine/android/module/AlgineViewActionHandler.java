package com.algine.android.module;

import android.view.MotionEvent;
import android.view.View;

public class AlgineViewActionHandler implements View.OnTouchListener {
    private final static double maxClickDistance = 16.0;
    private final static long maxClickDeltaTime = 250;

    final PointerInfo[] pointers = new PointerInfo[10]; // max 10 fingers

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        int maskedAction = event.getActionMasked();

        switch (maskedAction) {
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_POINTER_DOWN: {
                PointerBaseActionInfo info = getPointerBaseActionInfo(event);

                pointers[info.pointerId] = new PointerInfo(System.currentTimeMillis(), info.x, info.y);

                Algine.getView().queueEvent(() ->
                        Bridge.pointerDown(info.x, info.y, info.pointerId));
                break;
            }
            case MotionEvent.ACTION_MOVE: {
                for (int i = 0; i < event.getPointerCount(); i++) {
                    int currentPointerId = event.getPointerId(i);

                    float currentX = event.getX(i);
                    float currentY = event.getY(i);

                    PointerInfo info = pointers[currentPointerId];

                    if (info.x == currentX && info.y == currentY) // no move detected for pointer index i
                        continue;

                    double delta = Math.sqrt(Math.pow(info.pressX - currentX, 2) + Math.pow(info.pressY - currentY, 2));

                    if (delta > info.maxDelta) {
                        info.maxDelta = delta;
                    }

                    Algine.getView().queueEvent(() ->
                            Bridge.pointerMove(currentX, currentY, currentPointerId));

                    info.update(currentX, currentY);
                }
                break;
            }
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_POINTER_UP: {
                PointerBaseActionInfo actionInfo = getPointerBaseActionInfo(event);

                Algine.getView().queueEvent(() ->
                        Bridge.pointerUp(actionInfo.x, actionInfo.y, actionInfo.pointerId));

                PointerInfo info = pointers[actionInfo.pointerId];

                long releaseTime = System.currentTimeMillis();
                long deltaTime = releaseTime - info.pressTime;

                if (info.maxDelta <= maxClickDistance && deltaTime <= maxClickDeltaTime) {
                    Algine.getView().queueEvent(() ->
                            Bridge.pointerClick(actionInfo.x, actionInfo.y, actionInfo.pointerId));

                    v.performClick();
                }

                pointers[actionInfo.pointerId] = null;

                break;
            }
        }

        return true;
    }

    private static PointerBaseActionInfo getPointerBaseActionInfo(MotionEvent event) {
        return new PointerBaseActionInfo(event);
    }

    // due to the fact that ACTION_MOVE called more
    // frequently than ACTION_UP and ACTION_DOWN,
    // we don't need to store these values on every event.
    // Just use these values when we really need them.
    private static class PointerBaseActionInfo {
        int pointerIndex, pointerId;
        float x, y;

        PointerBaseActionInfo(MotionEvent event) {
            pointerIndex = event.getActionIndex();
            pointerId = event.getPointerId(pointerIndex);
            x = event.getX(pointerIndex);
            y = event.getY(pointerIndex);
        }
    }

    static class PointerInfo {
        long pressTime;
        double maxDelta;
        float pressX, pressY;
        float x, y;

        PointerInfo(long pressTime, float pressX, float pressY) {
            this.pressTime = pressTime;
            this.pressX = pressX;
            this.pressY = pressY;

            update(pressX, pressY);
        }

        void update(float x, float y) {
            this.x = x;
            this.y = y;
        }
    }
}
