package com.android.rpi.gpio;

import android.util.Log;
import com.android.rpi.RpiService;


public class LedService extends RpiService {

    static final String TAG = "LedController";

    private static native void led_native_init(String chipname, int line_num, String ledname);
    private static native int led_native_requestOutput();
    private static native int led_native_setOutput(byte value);

    public LedService(String chipname, int line_num, String ledname) {
        Log.d(TAG, "Init " + chipname);
        led_native_init(chipname, line_num, ledname);
    }

    public int requestOutput() {
        return led_native_requestOutput();
    }

    public int setOutput(byte value) {
        return led_native_setOutput(value);
    }

}
