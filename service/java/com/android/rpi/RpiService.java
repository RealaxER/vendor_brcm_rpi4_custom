package com.android.rpi;


public class RpiService {
    static {
        System.loadLibrary("libservices.rpi.core");
    }
}
