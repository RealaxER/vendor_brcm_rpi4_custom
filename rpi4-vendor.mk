# Automatically generated file. DO NOT MODIFY
#
# This file is generated by device/brcm/rpi4/setup-makefiles.sh

PRODUCT_SOONG_NAMESPACES += \
    vendor/brcm/rpi4

PRODUCT_COPY_FILES += \
    vendor/brcm/rpi4/proprietary/vendor/firmware/brcm/BCM4345C0.hcd:$(TARGET_COPY_OUT_VENDOR)/firmware/brcm/BCM4345C0.hcd \
    vendor/brcm/rpi4/proprietary/vendor/firmware/brcm/BCM4345C5.hcd:$(TARGET_COPY_OUT_VENDOR)/firmware/brcm/BCM4345C5.hcd

# HAL LED conventional 
PRODUCT_PACKAGES += \
    exam_led_conventional \
    libgpiod \
    led_conventional.default

# Binder C 
PRODUCT_PACKAGES += \
    service_manager_my \
    test_client \
    test_server 

# Binder C 
PRODUCT_PACKAGES += \
    test_server_cpp \
    test_client_cpp 
