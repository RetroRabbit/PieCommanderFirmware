// this file contains hardware specific definitions
/*
https://developer.mbed.org/users/mbedAustin/code/BLE_URIBeacon_ScavengerHunt/docs/3c518a637de6/main_8cpp_source.html

*/
    #define M1A_PIN P0_24   // motor 1 A
    #define M1B_PIN P0_25   // motor 1 B
    
    #define M2A_PIN P0_16   // motor 2 A
    #define M2B_PIN P0_17   // motor 2 B
    
    #define LED_PIN P0_22   // heart beat LED
    #define SPR_PIN P0_23   // speaker 
    #define LED_R   P0_3    // LED - red
    #define LED_G   P0_4    // LED - green
    #define LED_B   P0_5    // LED - blue
    #define BMP1    P0_6    // BUMPER1
    #define BMP2    P0_7    // BUMPER2
    #define SCL     P0_2    // I2C-SCL
    #define SDA     P0_19   // I2C-SDA
    
    #define LASER_L   P0_9   // Laser1 - on the left
    #define LASER_R   P0_11  // Laser2 - on the right
    
    #define IRTX_PIN    P0_1   // assigned to expansion socket pin8
    #define IRRX_PIN    P0_0   // assigned to expansion socket pin6
    #define IRMUX_PIN   P0_20  // IR receiver mux select  
        
    #define KHZ_PIN     P0_18   // uart pin used for IR rx
    
    #define baudrate 115200
    


/* Notes
http://developer.mbed.org/teams/Bluetooth-Low-Energy/wiki/UART-access-over-BLE

pc.printf("Hello...");

*/



