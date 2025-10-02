# IO Module

<img width="320" height="441" alt="io11" src="https://github.com/user-attachments/assets/b5452d8e-400c-4a72-a195-d5b7a7764407" />
<img width="320" height="77" alt="io22" src="https://github.com/user-attachments/assets/99fd135c-8d36-463b-ae3a-f06119cb8ccb" />

Shows info about input / output devices, e.g. gpio pins.

🚧

* Per pin:
    * GPIO = gpio_num;
    * Valid = is_valid;
    * Output = is_output_valid;
    * RTC = is_rtc_gpio;
    * Level = (level >= 0) ? (level ? "HIGH" : "LOW") : "N/A";
    * DriveCap = (drive_result == ESP_OK) ? drive_cap_to_string(drive_cap) : "N/A";
    * PinInfo = ;
