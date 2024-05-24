public class LedController {

    private static native void led_native_init(String chipname, int line_num, String ledname);
    private static native int led_native_requestOutput();
    private static native int led_native_setOutput(byte value);

    static {
        System.loadLibrary("libservices.rpi.core");
    }

    public static void main(String[] args) {
        // Perform LED initialization
        led_native_init("gpiochip0", 20, "LED1");

        int requestResult = led_native_requestOutput();
        System.out.println("Request Output Result: " + requestResult);

        int setResult = led_native_setOutput((byte) 1);
        System.out.println("Set Output Result: " + setResult);
    }
}
