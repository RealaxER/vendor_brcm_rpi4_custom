#define LOG_TAG "IR_REMOTE"

#include <hardware_legacy/ir_remote.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <gpiod.h>
#include <unistd.h>

struct IrRemote {
    pthread_t ir_id;
    struct gpiod_chip *chip;
    struct gpiod_line *line;
    uint8_t val;
};

struct IrRemote ir_remote = {
    .ir_id = 0,
    .val = 0,
};


static int ir_get_input() {
    return gpiod_line_get_value(ir_remote.line);
}

void __attribute__ ((constructor)) leds_loaded() {
    ALOGD("Legacy HAL IR REMOTE Module: Loaded");
}

void __attribute__ ((destructor )) leds_unloaded() {
    ALOGD("Legacy HAl IR REMOTE Module: Unloaded");
}

void ir_push(char c) {
    ALOGD("REMOTE KEY: %c\n", c);
}

void convert_code (uint32_t code)
{
    switch (code)
    {
        case (0xFFA25D):
            ir_push('1');
            break;
        
        case (0xFF629D):
            ir_push('2');
            break;			

        case (0xFFE21D):
            ir_push('3');
            break;

        case (0xFF22DD):
            ir_push('4');
            break;

        case (0xFF02FD):
            ir_push('5');
            break;

        case (0xFFC23D):
            ir_push('6');
            break;

        case (0xFFE01F):
            ir_push('7');
            break;

        case (0xFFA857):
            ir_push('8');
            break;

        case (0xFF906F):
            ir_push('9');
            break;

        case (0xFFB04F):
            ir_push('#');
            break;

        case (0XFF6897):
            ir_push('*');
            break;			
                    
        case (0xFF9867):
            ir_push('0');
            break;

        case (0xFF38C7):
            ir_push('K');
            break;

        case (0xFF18E7):
            ir_push('^');
            break;
        
        case (0xFF10EF):
            ir_push('<');
            break;			

        case (0xFF5AA5):
            ir_push('>');
            break;
        
        case (0xFF4AB5):
            ir_push('u');
            break;
        
        default :
            break;
    }
}


void* ir_remote_handle(void* arg) {
    IR_LOG("==============IR REMOTE START========\n");
    while(1) {
        while (ir_get_input());
        uint32_t code = 0;
        uint32_t count = 0;
        /* The START Sequence begin here
        * there will be a pulse of 9ms LOW and
        * than 4.5 ms space (HIGH)
        */
        while (!ir_get_input());  // wait for the pin to go high.. 9ms LOW
        while (ir_get_input());  // wait for the pin to go low.. 4.5ms HIGH
        /* START of FRAME ends here*/

        /* DATA Reception
        * We are only going to check the SPACE after 562.5us pulse
        * if the space is 562.5us, the bit indicates '0'
        * if the space is around 1.6ms, the bit is '1'
        */
        for (int i=0; i<32; i++)
        {
            while (!ir_get_input()); // wait for pin to go high.. this is 562.5us LOW
            while (ir_get_input())  // count the space length while the pin is high
            {
                count++;
                usleep(100);
            }

            if (count > 12) // if the space is more than 1.2 ms
            {
                code |= (1UL << (31-i));   // write 1
            }
            else code &= ~(1UL << (31-i));  // write 0
        }
        convert_code(code);
    }   
     
     return NULL;
}

int ir_init(const char *chipname, unsigned int line_num, const char *name) {
    ir_remote.chip =  gpiod_chip_open_by_name(chipname);
    if (!ir_remote.chip) {
        IR_LOG("ir remote open chip fail");
        return -1;
    }
    
    ir_remote.line = gpiod_chip_get_line(ir_remote.chip, line_num);
    if (!ir_remote.line) {
        IR_LOG("ir get line faisw");
        gpiod_chip_close(ir_remote.chip);
        return -1;
    }

    ir_remote.val = gpiod_line_request_input(ir_remote.line, name);
    if(ir_remote.val < 0){
        IR_LOG("ir get value fail");
        gpiod_chip_close(ir_remote.chip);
        return -1;
    }

    if (pthread_create(&ir_remote.ir_id, NULL, ir_remote_handle, NULL)) {
        perror("Error creating thread ir remote");
        return -1;
    }

    return 0;
}
