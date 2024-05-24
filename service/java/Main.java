import com.android.rpi.gpio.LedController;

public class Main {
    public static void main(String[] args) {
        // Khởi tạo một đối tượng LedController
        LedController ledController = new LedController("chipname", 1, "ledname");

        // Yêu cầu đầu ra cho đèn
        int requestResult = ledController.requestOutput();
        if (requestResult == 0) {
            System.out.println("Yêu cầu đầu ra thành công cho đèn");
        } else {
            System.out.println("Yêu cầu đầu ra cho đèn không thành công");
        }

        // Thiết lập giá trị đầu ra cho đèn
        byte outputValue = 1; // Điều này phụ thuộc vào cách điều khiển đèn của bạn
        int setResult = ledController.setOutput(outputValue);
        if (setResult == 0) {
            System.out.println("Thiết lập giá trị đầu ra cho đèn thành công");
        } else {
            System.out.println("Thiết lập giá trị đầu ra cho đèn không thành công");
        }
    }
}
