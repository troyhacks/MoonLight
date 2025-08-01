# Pins module

Shows info in gpio pins.

🆕 🚧

* Per pin:
    * GPIO = gpio_num;
    * Valid = is_valid;
    * Output = is_output_valid;
    * RTC = is_rtc_gpio;
    * Level = (level >= 0) ? (level ? "HIGH" : "LOW") : "N/A";
    * DriveCap = (drive_result == ESP_OK) ? drive_cap_to_string(drive_cap) : "N/A";
    * PinInfo = ;
