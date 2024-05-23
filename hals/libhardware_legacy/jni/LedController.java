//package rpi.gpio;

public class LedController {
    static {
        System.loadLibrary("LedController"); // Load the shared library
    }

    private long nativeHandle;

    public LedController(String chipname, int lineNum, String name) {
        init(chipname, lineNum, name);
    }

    private native void init(String chipname, int lineNum, String name);
    public native int requestOutput();
    public native int setOutput(byte val);
    public native int setBlinkOutput(byte fre);
    public native int stopBlink();

    public static void main(String[] args) {
        LedController led = new LedController("gpiochip0", 21, "led_1");
        if (led.requestOutput() == 1) {
            System.out.println("Output requested successfully");
        } else {
            System.out.println("Failed to request output");
        }

        if (led.setOutput((byte) 1) == 1) {
            System.out.println("LED set successfully");
        } else {
            System.out.println("Failed to set LED");
        }
    }
}
