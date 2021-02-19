package com.algine.android.module;

import android.app.Activity;
import android.os.Bundle;
import androidx.annotation.Nullable;

public class AlgineActivity extends Activity {

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (Algine.algineActivity != null) {
            throw new IllegalStateException("Instance of AlgineActivity has been already created");
        }

        Algine.algineActivity = this;
    }

    @Override
    protected void onPause() {
        super.onPause();

        if (Algine.isInitialized()) {
            Algine.getView().onPause();
            Algine.getView().queueEvent(Bridge::onPause);
        }
    }

    @Override
    protected void onResume() {
        super.onResume();

        if (Algine.isInitialized()) {
            Algine.getView().onResume();

            if (!Algine.getView().isSurfaceCreated()) {
                new Thread(() -> {
                    // wait until the surface is created
                    while (!Algine.getView().isSurfaceCreated());

                    Algine.getView().queueEvent(Bridge::onResume);
                }).start();
            } else {
                Algine.getView().queueEvent(Bridge::onResume);
            }
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        if (Algine.isInitialized()) {
            Algine.getView().queueEvent(Bridge::onDestroy);
        }

        Algine.toDefaults();
    }
    
    // TODO: implement onBackPressed action
}
