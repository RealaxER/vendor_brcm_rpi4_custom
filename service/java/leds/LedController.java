package com.android.rpi.gpio;

public class LedController {

    private static native void led_native_init(String chipname, int line_num, String ledname);
    private static native int led_native_requestOutput();
    private static native int led_native_setOutput(byte value);

    // Khởi tạo LedController
    public LedController(String chipname, int line_num, String ledname) {
        led_native_init(chipname, line_num, ledname);
    }

    // Yêu cầu đầu ra cho đèn
    public int requestOutput() {
        return led_native_requestOutput();
    }

    // Thiết lập giá trị đầu ra cho đèn
    public int setOutput(byte value) {
        return led_native_setOutput(value);
    }

    static {
        System.loadLibrary("libservices.rpi.core");
    }
}
