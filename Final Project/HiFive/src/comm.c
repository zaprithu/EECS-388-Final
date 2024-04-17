#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "eecs388_lib.h"
#define SERVO_PULSE_MAX 2400
#define SERVO_PULSE_MIN 544     
#define SERVO_PERIOD    20000  

void auto_brake(int devid)
{
    gpio_write(GREEN_LED, 0); // Turn off green LED initially
    gpio_write(RED_LED, 0); // Turn off red LED initially
    
    if (devid > 200) {
        gpio_write(GREEN_LED, 1); // Turn on green LED light only
    } 
    else if (devid > 100) {
        gpio_write(GREEN_LED, 1); // Turn on green LED light
        gpio_write(RED_LED, 1); // Turn on red LED light (for yellow LED appearance)
    }
    else if (devid > 60) {
        gpio_write(RED_LED, 1); // Turn on red LED light only
    }
    else {
        // Blink the red LED
        while (1) {
            gpio_write(RED_LED, 1);
            delay(100); // LED is on for 100 milliseconds
            gpio_write(RED_LED, 0);
            delay(100); // LED is off for 100 milliseconds

            // Update `devid` value by reading new data from the sensor here

            if (devid > 60) {
                break; // Exit loop if devid is greater than 60
            }
        }
    }
}

int read_from_pi(int devid)
{
    // Task-2: 
    // You code goes here (Use Lab 09 for reference)
    // After performing Task-2 at dnn.py code, modify this part to read angle values from Raspberry Pi.

}

void steering(int gpio, int pos)
{
    // Task-3: Implement servo control as specified in the project document. Refer to Lab 05 for PWM implementation details.
    
    // Calculate the duty cycle 'on' time in microseconds. This is the time the servo signal line should be high.
    // SERVO_PULSE_MIN and SERVO_PULSE_MAX are constants that define the pulse width in microseconds
    // corresponding to the servo's minimum (0 degrees) and maximum (180 degrees) positions.
    int onTime = SERVO_PULSE_MIN + pos * (SERVO_PULSE_MAX - SERVO_PULSE_MIN) / 180; 
    
    // Calculate the 'off' time by subtracting the 'on' time from the servo's total period.
    // SERVO_PERIOD is a constant representing the total period of the servo PWM signal in microseconds.
    int offTime = SERVO_PERIOD - onTime;
    
    // Set the GPIO pin high to signal the start of the 'on' period.
    gpio_write(gpio, ON);

    // Wait for the 'on' period to end.
    delay_usec(onTime);

    // Set the GPIO pin low to signal the start of the 'off' period.
    gpio_write(gpio, OFF);

    // Wait for the 'off' period to end before the next cycle begins.
    delay_usec(offTime);
}


int main()
{
    // initialize UART channels
    ser_setup(0); // uart0
    ser_setup(1); // uart1
    int pi_to_hifive = 1; //The connection with Pi uses uart 1
    int lidar_to_hifive = 0; //the lidar uses uart 0
    
    printf("\nUsing UART %d for Pi -> HiFive", pi_to_hifive);
    printf("\nUsing UART %d for Lidar -> HiFive", lidar_to_hifive);
    
    //Initializing PINs
    gpio_mode(PIN_19, OUTPUT);
    gpio_mode(RED_LED, OUTPUT);
    gpio_mode(BLUE_LED, OUTPUT);
    gpio_mode(GREEN_LED, OUTPUT);

    printf("Setup completed.\n");
    printf("Begin the main loop.\n");

    while (1) {

        auto_brake(lidar_to_hifive); // measuring distance using lidar and braking
        int angle = read_from_pi(pi_to_hifive); //getting turn direction from pi
        printf("\nangle=%d", angle) 
        int gpio = PIN_19; 
        for (int i = 0; i < 10; i++){
            // Here, we set the angle to 180 if the prediction from the DNN is a positive angle
            // and 0 if the prediction is a negative angle.
            // This is so that it is easier to see the movement of the servo.
            // You are welcome to pass the angle values directly to the steering function.
            // If the servo function is written correctly, it should still work,
            // only the movements of the servo will be more subtle
            if(angle>0){
                steering(gpio, 180);
            }
            else {
                steering(gpio,0);
            }
            
            // Uncomment the line below to see the actual angles on the servo.
            // Remember to comment out the if-else statement above!
            // steering(gpio, angle);
        }

    }
    return 0;
}
