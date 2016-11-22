// changes made to move move commands to their own command
// 27 oct 16, swapped assigned of the motor channels as PCB now mounted upside down
// 06 nov 16, added i2c interface between this boiard and top (head) pcb
//             - EYES and RGB led supported

#include "mbed.h"
#include "droid_hw.h"
#include "BLEDevice.h"
#include "UARTService.h"
#include <string>

DigitalOut topled(LED2);
DigitalOut bottomled(LED1);
DigitalOut myled (LED_PIN);

DigitalOut ledRed   (LED_R);
DigitalOut ledGreen (LED_G);
DigitalOut ledBlue  (LED_B);
DigitalIn  bumper1  (BMP1);
DigitalIn  bumper2  (BMP2);
DigitalOut laserL   (LASER_L);
DigitalOut laserR   (LASER_R);
DigitalOut irMuxSelect  (IRMUX_PIN);

I2C i2c (SDA, SCL);

DigitalOut khz  (KHZ_PIN);

Serial ir(IRTX_PIN, IRRX_PIN);

    
    
DigitalOut m1a(M1A_PIN);
PwmOut     m1b(M1B_PIN);

DigitalOut m2a(M2A_PIN);
PwmOut     m2b(M2B_PIN);

PwmOut     speaker(SPR_PIN);


enum {
    NONE,
    DRIVE,
    WAGGLE
} action_t;


//InterruptIn event(P0_20);

BLEDevice  ble;
UARTService *uart;
Ticker timer_tick;

const int i2c_addr = 0x9E; // define the I2C Address

char i2c_buffer[30] = {};
uint8_t mac[6] = {};
    
int limit = 20;
int enable = 0;
int msec   = 0;
int m1cmd  = 0;
int m2cmd  = 0;
int m1ttl  = 0;
int m2ttl  = 0;
int rc     = 0; // return code for function calls

int rxchar;

typedef struct
   {
   int wridx;
   int rdidx;
   int cmd[16];
   int ttl[16];
   } motor_msg_q_type;
   
motor_msg_q_type motorLq;
motor_msg_q_type motorRq;

typedef struct
   {
   int wridx;
   int rdidx;
   int cmd[16];
   int on_ttl[16];
   int off_ttl[16];
   } led_msg_q_type;

led_msg_q_type laserLq; // command structure for left laser
led_msg_q_type laserRq; // command structure for right laser



typedef enum {
    WAG_INIT,
    WAG_WAGGLE,
    WAG_DELAY,
    WAG_DONE
} waggle_state;

typedef struct {
    waggle_state state; // state of waggle
    int val;            // intensity of waggle
    int dur;            // duration of waggle
    int t_rem;          // remaining time of waggle
    int per;            // period of oscillation
    int p_mod;
    int p_rem;          // remaining time of current period
    int rep;            // number of repetitions
    int del;            // delay between repetitions
    int del_rem;        // remaining time of delay
    int dir;            // direction of current oscillation
} waggle_t;

const static unsigned MAX_SIZEOF_RX_PAYLOAD = 20;
const static unsigned MAX_SIZEOF_TX_PAYLOAD = 20;
static char           DEVICE_NAME[9]        = {0x44, 0x72, 0x6f, 0x69, 0x64, 0x2d, 0x34, 0x35, 0x00};
static volatile bool  triggerSensorPolling = false;
char rxPayload[MAX_SIZEOF_RX_PAYLOAD] = {0,};
char txPayload[MAX_SIZEOF_TX_PAYLOAD] = {0,};

char rxbuffer[MAX_SIZEOF_RX_PAYLOAD] = {0,};

void sio_putstring (char * string)
{
  int ix;
  for (ix = 0; ix < 15; ix++)
  {
    ir.putc(string[ix]);
  }
}
    
void motor1ctrl (int speed)
{
    if (speed == 0)
    {
        m1a = 1;
        m1b.pulsewidth_us(5080);
        return; 
    }
    
    if (speed >  127)
    {
        speed =  127;
    }
    else if (speed < -127)
    {
        speed = -127;
    }

    if (speed > 0)
    {
        m1a = 1;
        m1b.pulsewidth_us(5080 - speed * 40);
    }
    else
    {
        speed = -speed;
        m1a = 0;
        m1b.pulsewidth_us(speed * 40);
    }
}

void motor2ctrl (int speed)
{
    if (speed == 0)
    {
        m2a = 1;
        m2b.pulsewidth_us(5080);
        return; 
    }
    
    if (speed >  127)
    {
        speed =  127;
    }
    else if (speed < -127)
    {
        speed = -127;
    }

    if (speed > 0)
    {
        m2a = 1;
        m2b.pulsewidth_us(5080 - speed * 40);
    }
    else
    {
        speed = -speed;
        m2a = 0;
        m2b.pulsewidth_us(speed * 40);
    }
}

// This function is called when a character goes into the RX buffer.
void rxCallback() {
    rxchar = ir.getc();
}

void hardwareInit( )
{
    // set up motor control channels

    m1a = 1;
    m2a = 1;
    m1b.period_us(5080);
    m2b.period_us(5080);

    motor1ctrl (0);
    motor2ctrl (0);
    
    speaker.period_us(5080);
    speaker.pulsewidth_us(25);
    
    // initialize the UART for IR operations
    ir.baud(baudrate);
    ir.attach(&rxCallback, Serial::RxIrq);
    
    myled = 0;
    ledRed = 1;
    ledGreen = 1;
    ledBlue  = 1;
    laserL = 1;
    laserR = 1;
}


// When we disconnect from the BT connection, the device must start to re-advertise
void disconnectionCallback(Gap::Handle_t handle, Gap::DisconnectionReason_t reason)
{
    ble.startAdvertising(); // restart advertising
}

// Something is incomming on the BT connection, read data from the TX characteristic
// Batch it up into 20byte chunks and then invoke the processInput method
void onDataWritten(const GattCharacteristicWriteCBParams *params)
{
    if ((uart != NULL) && (params->charHandle == uart->getTXCharacteristicHandle())) {
        uint16_t bytesRead = params->len;

        if (bytesRead < MAX_SIZEOF_RX_PAYLOAD) {
            strncpy(rxPayload, (char *)params->data, MAX_SIZEOF_RX_PAYLOAD - 1);
            //rxPayload[bytesRead] = '\0';
            //FG processInput();                   
            triggerSensorPolling= true;
            
            // copy received data to local buffer
            uint16_t ix;
            for (ix = 0; ix < bytesRead; ix++)
            {
                rxbuffer[ix] = (char) params->data[ix];
            }
                        
            // respond
            // ble.updateCharacteristicValue(uart->getRXCharacteristicHandle(), params->data, bytesRead);
        }
    }
}

// Send something back over the BT connection
void WriteToUART(char STR[])
{
    // RESPOND...    
    ble.updateCharacteristicValue(uart->getRXCharacteristicHandle(), (uint8_t *)STR, strlen(STR));
}

void toggle() 
{
    enable = 1;
}

waggle_t *init_waggle(waggle_t *wag, int val, int dur, int rep = 1, int del = 0)
{
    if (wag == NULL) wag = (waggle_t*)calloc(1, sizeof(waggle_t));
    if (val > 255) val = 255;
    
    wag->val = (val * 64 / 255) + 63;
    wag->dur = dur;
    wag->t_rem = dur;
    wag->per = ((500 - 100) * (255 - val) / 255) + 100;
    wag->per = wag->per - (wag->per % 10);
    wag->p_mod = (wag->dur % (2 * wag->per)) / 2;
    wag->p_mod = wag->p_mod - (wag->p_mod % 10);
    wag->p_rem = wag->per;
    wag->rep = rep;
    wag->del = del - (wag->del % 10);
    wag->del_rem = wag->del;
    wag->dir = 1;
    wag->state = WAG_INIT;
    
    for (int i = 0; i < 16; i++) {
        motorLq.cmd[i] = 0;
        motorLq.ttl[i] = 0;
        motorRq.cmd[i] = 0;
        motorRq.ttl[i] = 0;
    }
    
    return wag;
}

waggle_t *init_shiver(waggle_t *wag, int val, int dur, int rep = 1, int del = 0)
{
    if (wag == NULL) wag = (waggle_t*)calloc(1, sizeof(waggle_t));
    if (val > 255) val = 255;
    
    wag->val = (val * 64 / 255) + 63;
    wag->dur = dur;
    wag->t_rem = dur;
    wag->per = (int)((100 - 50) * (float)(255 - val) / 255) + 50;
    wag->per = wag->per - (wag->per % 10);
    wag->p_mod = (wag->dur % (2 * wag->per)) / 2;
    wag->p_mod = wag->p_mod - (wag->p_mod % 10);
    wag->p_rem = wag->per;
    wag->rep = rep;
    wag->del = del - (wag->del % 10);
    wag->del_rem = wag->del;
    wag->dir = 1;
    wag->state = WAG_INIT;
    
    for (int i = 0; i < 16; i++) {
        motorLq.cmd[i] = 0;
        motorLq.ttl[i] = 0;
        motorRq.cmd[i] = 0;
        motorRq.ttl[i] = 0;
    }
    
    return wag;
}

void exec_waggle(waggle_t *wag)
{
    switch (wag->state) {
    case WAG_INIT:
        if (wag->rep > 0) {
            wag->rep = wag->rep - 1;
            wag->state = WAG_WAGGLE;
            wag->t_rem = wag->dur;
            wag->p_rem = wag->per;
            wag->dir = 1;
        } else {
            wag->state = WAG_DONE;
            break;
        }
    case WAG_WAGGLE:
        if (wag->t_rem >= 10) {
            if (wag->p_rem < 10) {
                if (wag->t_rem > 2*wag->p_mod) {
                    wag->p_rem = wag->per;
                } else {
                    wag->p_rem = wag->p_mod;
                }
                wag->dir = wag->dir * -1;
            } 
            motor1ctrl (wag->dir * wag->val);
            motor2ctrl (-wag->dir * wag->val);
//            motorLq.cmd[0] = wag->dir * wag->val;
//            motorLq.ttl[0] = 10;
//            motorRq.cmd[0] = -wag->dir * wag->val;
//            motorRq.ttl[0] = 10;
            
            wag->t_rem = wag->t_rem - 10;
            wag->p_rem = wag->p_rem - 10;
        } else if (wag->rep > 0) {
            wag->state = WAG_DELAY;
            motor1ctrl (0);
            motor2ctrl (0);
            motorLq.cmd[0] = 0;
            motorLq.ttl[0] = 0;
            motorRq.cmd[0] = 0;
            motorRq.ttl[0] = 0;
            wag->del_rem = wag->del;
        } else {
            wag->state = WAG_DONE;
            motor1ctrl (0);
            motor2ctrl (0);
            motorLq.cmd[0] = 0;
            motorLq.ttl[0] = 0;
            motorRq.cmd[0] = 0;
            motorRq.ttl[0] = 0;
        }
        break;
    case WAG_DELAY:
        if (wag->del_rem >= 10) {
            wag->del_rem = wag->del_rem - 10;
        } else {
            wag->state = WAG_INIT;
        }
        break;
    case WAG_DONE:
    default:
        break;
    }    
}

// This is the main kahuna
int main() {

    char    idata[3];
 
    idata[0]   = 0x16;
    idata[1]   = 0x55;
    idata[2]   = 0x55;
    
    
    
    motorLq.rdidx = 0;
    motorLq.wridx = 0;
    motorLq.cmd[0] = 0;
    motorLq.ttl[0] = 0;
    
    motorRq.rdidx = 0;
    motorRq.wridx = 0;
    motorRq.cmd[0] = 0;
    motorRq.ttl[0] = 0;
    
    laserLq.rdidx = 0;
    laserLq.wridx = 0;
    laserLq.cmd[0] = 0;
    laserLq.on_ttl[0] = 0;
    laserLq.off_ttl[0] = 0;
    
    laserRq.rdidx = 0;
    laserRq.wridx = 0;
    laserRq.cmd[0] = 0;
    laserRq.on_ttl[0] = 0;
    laserRq.off_ttl[0] = 0;

    
    hardwareInit();
    
    ble.init();

    ble.onDisconnection(disconnectionCallback);
    ble.onDataWritten(onDataWritten);
    
    ble.getAddress(0,mac);
    
    idata[0] = mac[0] ^ mac[1];
    idata[1] = mac[2] ^ mac[3];
    idata[2] = mac[4] ^ mac[5];
    idata[0]= idata[0] ^ idata[1] ^ idata[2];
    idata[1] = idata[0];    // make temp copy
    
    idata[0] = (idata[0] & 0xf0) >> 4;
    if (idata[0] > 9) 
    {
        idata[0] = idata[0] + 0x37;
    }
    else
    {
        idata[0] = idata[0] + 0x30;
    }
    
    idata[1] =  idata[1] & 0x0f;
    if (idata[1] > 9) 
    {
        idata[1] = idata[1] + 0x37;
    }
    else
    {
        idata[1] = idata[1] + 0x30;
    }
    DEVICE_NAME[6] = idata[0];
    DEVICE_NAME[7] = idata[1];
 
    
    
    // ble.startAdvertising();
         
    /* setup advertising */
    ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED);
    ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.accumulateAdvertisingPayload(GapAdvertisingData::SHORTENED_LOCAL_NAME,(uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME) - 1);
    // We're using Nordic's custom UART Service, Thanks !
    ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS,(const uint8_t *)UARTServiceUUID_reversed, sizeof(UARTServiceUUID_reversed));

    ble.setAdvertisingInterval(160); /* 100ms; in multiples of 0.625ms. */
    ble.startAdvertising();
    
    timer_tick.attach(&toggle, 0.01); // the address of the function to be attached (toggle)
    //event.fall(&toggle);
    
    uart = new UARTService(ble);
    
                m1cmd = 0;
                m2cmd = 0;
    
            //FG debug motor1ctrl (-100);
            //FG debug motor2ctrl (-100);
            
            //motorRq.cmd[0] = -127;
//            motorRq.ttl[0] = 50;
//            motorLq.cmd[0] = 127;
//            motorLq.ttl[0] = 50;
//            motorRq.cmd[1] = 127;
//            motorRq.ttl[1] = 50;
//            motorLq.cmd[1] = -127;
//            motorLq.ttl[1] = 50;
//            motorRq.cmd[2] = -127;
//            motorRq.ttl[2] = 50;
//            motorLq.cmd[2] = 127;
//            motorLq.ttl[2] = 50;
//            motorRq.cmd[3] = 127;
//            motorRq.ttl[3] = 50;
//            motorLq.cmd[3] = -127;
//            motorLq.ttl[3] = 50;
//            motorRq.cmd[4] = -127;
//            motorRq.ttl[4] = 50;
//            motorLq.cmd[4] = 127;
//            motorLq.ttl[4] = 50;
//            motorRq.cmd[5] = 127;
//            motorRq.ttl[5] = 50;
//            motorLq.cmd[5] = -127;
//            motorLq.ttl[5] = 50;
//            motorRq.cmd[6] = -127;
//            motorRq.ttl[6] = 50;
//            motorLq.cmd[6] = 127;
//            motorLq.ttl[6] = 50;
//            motorRq.cmd[7] = 127;
//            motorRq.ttl[7] = 50;
//            motorLq.cmd[7] = -127;
//            motorLq.ttl[7] = 50;
    
    waggle_t *wag = init_shiver(NULL, 127, 1000, 3, 500);
            
            
    while (true) 
    {
        
        if (enable == 1)
        {
            enable = 0;
            
            exec_waggle(wag);
        
            // process the commands
            if (motorLq.cmd[0] != 0 || motorLq.ttl[0] != 0)
            {
                motorLq.ttl[0] = motorLq.ttl[0] - 10;
                
                motor1ctrl (motorLq.cmd[0]);
                
                if (motorLq.ttl[0] == 0) 
                {
                    motor1ctrl (0);
                    motorLq.cmd[0] = 0;
                }
                else
                {
                    motor1ctrl (motorLq.cmd[0]);
                }
            }
            
            if (motorRq.cmd[0] != 0 || motorRq.ttl[0] != 0)
            {
                motorRq.ttl[0] = motorRq.ttl[0] - 10;
                
                motor2ctrl (motorRq.cmd[0]);
                
                if (motorRq.ttl[0] == 0) 
                {
                    motor2ctrl (0);
                    motorRq.cmd[0] = 0;
                }
                else
                {
                    motor2ctrl (motorRq.cmd[0]);
                }
            }

        
            msec++;
            if (msec == limit)
            {

                //RGB led test code = 6 nov 2016
                
                i2c_buffer[0] = 0x06;
                i2c_buffer[1] = 0x06;
                i2c_buffer[2] = 0x04;
                i2c_buffer[3] = 0x03;   // 3 = toggle
                i2c_buffer[4] = 0x00;
                i2c_buffer[5] = 0x00;
                i2c_buffer[6] = 0x28;
                i2c_buffer[7] = 0x29;
                rc = i2c.write(i2c_addr, i2c_buffer, 8, 0);
                
                
                
                msec = 0;
                myled = !myled;
                
                khz     = 0; //!khz;
                
                irMuxSelect = 0; //!irMuxSelect;
                ir.putc('w');

            }
        }
        
        

            
        // Now check if we have received anyting from the UART && if we are still connected
        //if (triggerSensorPolling && ble.getGapState().connected)
        if (triggerSensorPolling)
        {
            triggerSensorPolling = false;
            
            
            ir.putc (rxbuffer[7]);
            ir.putc (rxbuffer[8]);
            ir.putc (rxbuffer[9]);
            
            
            // process incomming string 
            strcpy(txPayload, "hello");

            
            if (rxbuffer[7] == 'S')   // this is Stop command
            {
                motorLq.cmd[0] = 0;
                motorLq.ttl[0] = 0;
                motorRq.cmd[0] = 0;
                motorRq.ttl[0] = 0;
                motor1ctrl (0);
                motor2ctrl (0);
                if (wag != NULL) wag->state = WAG_DONE;
                ir.putc('S');
                strcpy(txPayload, "S done");
            }
            if (rxbuffer[7] == 'M')                 // this is a motor control command
            {
                if (rxbuffer[8] != 0x00)            // this is motor commandfor left motor
                {
                    motorLq.cmd[0] = (signed char) rxbuffer[8];   // this is left motor speed
                    motorLq.ttl[0]  = (int) ((rxbuffer[12] << 8) | rxbuffer[13]);
                    if (rxbuffer[10] == 0x01)       // check direction
                    {
                        motorLq.cmd[0] = -motorLq.cmd[0];
                    }
                }
                if (rxbuffer[9] != 0x00)            // this is motor commandfor right motor
                {
                    motorRq.cmd[0] = (signed char) rxbuffer[9];   // this is right motor speed
                    motorRq.ttl[0]  = (int) ((rxbuffer[12] << 8) | rxbuffer[13]);
                    if (rxbuffer[11] == 0x01)       // check direction
                    {
                        motorRq.cmd[0] = -motorRq.cmd[0];
                    }
                }

                ir.putc( (unsigned char) motorLq.cmd[0]);
                ir.putc( (unsigned char) motorRq.cmd[0]);
                strcpy(txPayload, "M done");
            }
            if (rxbuffer[7] == 'W')                 // this is a motor control command
            {
                init_waggle(wag, rxbuffer[8], (rxbuffer[9] << 8) | rxbuffer[10], rxbuffer[11], (rxbuffer[12] << 8) | rxbuffer[13]);
                strcpy(txPayload, "W done");
            }
            if (rxbuffer[7] == 0x45)                // this is Eye command
            {
                // note: Left/Right EYE are part of head, and message must be passed thru via I2C interface
                i2c_buffer[0] = 0x0c;           // offset (location) of data in the head data table
                i2c_buffer[1] = 0x06;           // length of payload adata
                i2c_buffer[2] = rxbuffer[9];    // first row data, left / right eye select
                i2c_buffer[3] = rxbuffer[10];   // second row data
                i2c_buffer[4] = rxbuffer[11];   // third row data
                i2c_buffer[5] = rxbuffer[12];   // fourth row data
                i2c_buffer[6] = rxbuffer[13];   // fifth row data
                i2c_buffer[7] = rxbuffer[14];   // sixth row data
                rc = i2c.write(i2c_addr, i2c_buffer, 8, 0);
                strcpy(txPayload, "E done");
            }
            if (rxbuffer[7] == 0x54)   // this is Toggle command
            {
                if ( (rxbuffer[9]) == 0x02) // this is left laser
                {
                    laserLq.cmd[0]     = rxbuffer[10];
                    laserLq.on_ttl[0]  = (int) ((rxbuffer[11] << 8) | rxbuffer[12]);
                    laserLq.off_ttl[0] = (int) ((rxbuffer[13] << 8) | rxbuffer[14]);
                    strcpy(txPayload, "T L done");
                }
                
                if ( (rxbuffer[9]) == 0x03) // this is right laser
                {
                    laserRq.cmd[0]     = rxbuffer[10];
                    laserRq.on_ttl[0]  = (int) ((rxbuffer[11] << 8) | rxbuffer[12]);
                    laserRq.off_ttl[0] = (int) ((rxbuffer[13] << 8) | rxbuffer[14]);
                    strcpy(txPayload, "T R done");
                }
                
                // multicolor LED
                if ( ((rxbuffer[9]) == 0x04) || ((rxbuffer[9]) == 0x05) || ((rxbuffer[9]) == 0x06) )
                {
                    i2c_buffer[0] = 0x06;           // offset (location) of data in the head data table
                    i2c_buffer[1] = 0x06;           // length of payload adata
                    i2c_buffer[2] = rxbuffer[9];    // device selection
                    i2c_buffer[3] = rxbuffer[10];   // command or state
                    i2c_buffer[4] = rxbuffer[11];   // command on time
                    i2c_buffer[5] = rxbuffer[12];
                    i2c_buffer[6] = rxbuffer[13];   // command off time
                    i2c_buffer[7] = rxbuffer[14];
                    rc = i2c.write(i2c_addr, i2c_buffer, 8, 0);
                    strcpy(txPayload, "T B done");
                }
                
            }
            


            //WriteToUART(txPayload);  // respond 
            
            if (laserLq.cmd[0] != 0)
            {
                if (laserLq.cmd[0] == 0x02) laserL = !laserL; // toggle laser
                laserLq.cmd[0] = 0;
                
                i2c.write( 0xC0, idata, 3 );
            }

            if (laserRq.cmd[0] != 0)
            {
                if (laserRq.cmd[0] == 0x02) laserR = !laserR; // toggle laser
                laserRq.cmd[0] = 0;
            }


            

            
        }
        else
        {
            ble.waitForEvent();
        }
            
    }

}




