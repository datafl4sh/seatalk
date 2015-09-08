/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <datafl4sh@toxicnet.eu> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return. datafl4sh.
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <xc.h>

#include "buffer.h"

// FOSC
#pragma config FOSFPR = XT              // Oscillator (XT)
#pragma config FCKSMEN = CSW_FSCM_OFF   // Clock Switching and Monitor (Sw Disabled, Mon Disabled)

// FWDT
#pragma config FWPSB = WDTPSB_16        // WDT Prescaler B (1:16)
#pragma config FWPSA = WDTPSA_512       // WDT Prescaler A (1:512)
#pragma config WDT = WDT_OFF            // Watchdog Timer (Disabled)

// FBORPOR
#pragma config FPWRT = PWRT_OFF         // POR Timer Value (Timer Disabled)
#pragma config BODENV = BORV20          // Brown Out Voltage (Reserved)
#pragma config BOREN = PBOR_OFF         // PBOR Enable (Disabled)
#pragma config MCLRE = MCLR_DIS         // Master Clear Enable (Disabled)

// FGS
#pragma config GWRP = GWRP_OFF          // General Code Segment Write Protect (Disabled)
#pragma config GCP = CODE_PROT_OFF      // General Segment Code Protection (Disabled)

// FICD
#pragma config ICS = ICS_PGD            // Comm Channel Select (Use PGC/EMUC and PGD/EMUD)

#define PRINTF_BUFSIZE      128
#define NMEA_MAX_LENGTH     83
#define MAX_DATA_AGE        3

#define FORMAT_RAW          0
#define FORMAT_NMEA         1
#define FORMAT_JSON         2
#define FORMAT_HUMAN        3

volatile int ledstatus;

volatile unsigned char time;

struct circular_buffer *cb;

void __attribute__((interrupt, auto_psv))
_U2RXInterrupt(void)
{
    PORTBbits.RB11 = 1;
    buffer_write(cb, U2RXREG);

    IFS1bits.U2RXIF = 0;
    PORTBbits.RB11 = 0;
}

void
uart1_init(void)
{
    U1BRG = 25;
    U1MODEbits.PDSEL =  0;  // 8-N-
    U1MODEbits.STSEL =  0;  // 1
    U1MODEbits.ALTIO =  1;  // use alternate pins
    U1MODEbits.UARTEN = 1;  // enable uart
    U1STAbits.UTXEN =   1;  // enable uart transmitter
}

void
uart2_init(void)
{
    U2BRG = 25;
    U2MODEbits.PDSEL =  3;  // 9-N-
    U2MODEbits.STSEL =  0;  // 1
    U2STAbits.ADDEN =   0;
    U2STAbits.UTXEN =   0;  // enable uart transmitter
    U2STAbits.URXISEL = 0;  // interrupt on each character
    IEC1bits.U2RXIE =   1;  // enable interrupt
    U2MODEbits.UARTEN = 1;  // enable uart
}

void
uart1_transmit_char(char c)
{
    while (!U1STAbits.TRMT)
        ;

    U1TXREG = c;
}

void
uart1_transmit_string(char *str)
{
    while (*str)
        uart1_transmit_char(*str++);
}

unsigned char
nmea_checksum(char *str)
{
    unsigned char chk = 0;

    while (*str)
        chk ^= *str++;

    return chk;
}

void
processor_init(void)
{
    INTCON1bits.NSTDIS = 1;
}

void
transmit_raw_buffer(int *data, int len)
{
    int i;
    char sent[5];
    snprintf(sent, 4, "%d: ", len);
    uart1_transmit_string(sent);

    for (i = 0; i < len; i++)
    {
        snprintf(sent, 4, "%03X ", data[i]);
        uart1_transmit_string(sent);
    }
    printf("\n");
}

int
nmea_sprintf(char *str, const char *fmt, ...)
{
    int         ret = 0;
    char        checksum;
    va_list     args;

    va_start(args, fmt);

    ret = vsnprintf(str, NMEA_MAX_LENGTH, fmt, args);
    if (ret > 78)
    {
        *str = '\0';
        return -1; /* insufficient space for checksum and CRLF*/
    }
     va_end(args);
    checksum = nmea_checksum(str);
    ret += snprintf(str+ret, NMEA_MAX_LENGTH-ret, "*%02X\x0d\x0a", checksum);

   
    
    return ret;
}


/***************************************************************************
 * Depth Below Transducer sentence
 *
 *  00  02  YZ  XX XX  Depth below transducer: XXXX/10 feet
 *                     Flags in Y: Y&8 = 8: Anchor Alarm is active
 *                                Y&4 = 4: Metric display units or
 *                                         Fathom display units if
 *                                          followed by command 65
 *                                Y&2 = 2: Used, unknown meaning
 *                    Flags in Z: Z&4 = 4: Transducer defective
 *                                Z&2 = 2: Deep Alarm is active
 *                                Z&1 = 1: Shallow Depth Alarm is active
 *                  Corresponding NMEA sentences: DPT, DBT
 */
int
depth_below_transducer(int *data, int len, int oformat)
{
    char            buf[PRINTF_BUFSIZE+1];
    unsigned int    depth_raw;
    float           depth_meters, depth_feet, depth_fathoms;

    if (len != data[1]+3)
        return 1;

    depth_raw = (data[4] << 8) | data[3];


    if (oformat == FORMAT_NMEA)
    {
        depth_meters = depth_raw/10.0;
        depth_fathoms = 0.54*depth_meters;
        depth_feet = 3.281*depth_meters;
        
        nmea_sprintf(buf, PRINTF_BUFSIZE, "$SDDBT,%.1f,f,%.1f,M,%.1f,F",
                     depth_feet, depth_meters, depth_fathoms);


        uart1_transmit_string(buf);
        return 0;
    }

    if (oformat == FORMAT_JSON)
    {
        return 0;
    }

    if (oformat == FORMAT_HUMAN)
    {
        depth_feet = depth_raw/10.0;
        snprintf(buf, PRINTF_BUFSIZE, "Depth: %f meters\n", depth_feet/3.281);
        uart1_transmit_string(buf);
        return 0;
    }
}

/***************************************************************************
 * Wind data. Seatalk has separate messages for wind angle and wind speed
 * while NMEA0183 carries both informations on the same message. This problem
 * is solved as follows:
 *
 *  1) Process angle or wind message
 *  2) If output format is NMEA
 *  2.1) Save acquired data in the wind_data struct and mark as valid
 *  2.2) Call transmit_wind_data: if both angle and speed are valid
 *       the NMEA message is formatted and is sent, and data is invalidated.
 *  2.3) goto (1)
 */

struct wind_data
{
    float           wd_speed;
    float           wd_angle;
    unsigned char   wd_speed_age;
    unsigned char   wd_angle_age;
};

int
transmit_wind_data(struct wind_data *wd)
{
    char            buf[NMEA_MAX_LENGTH+1];
    char            *valid = "";
    int             ret;

    if ( (time - wd->wd_angle_age < MAX_DATA_AGE) &&
         (time - wd->wd_speed_age < MAX_DATA_AGE) )
        valid = ",A";

    ret = nmea_sprintf(buf, PRINTF_BUFSIZE, "VWMWV,%.1f,R,%.1f,K%s",
                       wd->wd_angle, wd->wd_speed, valid);

    uart1_transmit_string(buf);

    return ret;
}

/***************************************************************************
 * Apparent Wind Angle sentence
 *
 * 10  01  XX  YY  Apparent Wind Angle: XXYY/2 degrees right of bow
 *               Used for autopilots Vane Mode (WindTrim)
 *               Corresponding NMEA sentence: MWV
 */
int
apparent_wind_angle(int *data, int len, int oformat, struct wind_data *wd)
{
    char            buf[PRINTF_BUFSIZE+1];
    int             wa_raw;
    float           wa;

    if (len != data[1]+3)
        return 1;

    wa_raw = (data[2] << 8) | (data[3] & 0xFF);
    wa = wa_raw / 2.0;

    if (oformat == FORMAT_NMEA)
    {
        wd->wd_angle = wa;
        wd->wd_angle_age = time;
        transmit_wind_data(wd);
        return 0;
    }

    if (oformat == FORMAT_JSON)
    {
        return 0;
    }

    if (oformat == FORMAT_HUMAN)
    {
        snprintf(buf, PRINTF_BUFSIZE, "Wind angle: %f deg\n", wa);
        uart1_transmit_string(buf);
        return 0;
    }
}

/***************************************************************************
 * Apparent Wind Speed sentence
 *
 *  11  01  XX  0Y  Apparent Wind Speed: (XX & 0x7F) + Y/10 Knots
 *                  Units flag: XX&0x80=0    => Display value in Knots
 *                              XX&0x80=0x80 => Display value in Meter/Second
 *                  Corresponding NMEA sentence: MWV
 */
int
apparent_wind_speed(int *data, int len, int oformat, struct wind_data *wd)
{
    char            buf[PRINTF_BUFSIZE+1];
    int             ws_raw;
    float           ws;

    if (len != data[1]+3)
        return 1;

    ws_raw = (data[2] & 0x7F)*10 + (data[3] & 0x0F);
    ws = ws_raw / 10.0;

    if (oformat == FORMAT_NMEA)
    {
        wd->wd_speed = ws;
        wd->wd_speed_age = time;
        transmit_wind_data(wd);
        return 0;
    }

    if (oformat == FORMAT_JSON)
    {
        return 0;
    }

    if (oformat == FORMAT_HUMAN)
    {
        snprintf(buf, PRINTF_BUFSIZE, "Wind speed: %f deg\n", ws);
        uart1_transmit_string(buf);
        return 0;
    }
}

/***************************************************************************
 *  20  01  XX  XX  Speed through water: XXXX/10 Knots
 *                  Corresponding NMEA sentence: VHW
 */
int
speed_through_water(int *data, int len, int oformat)
{
    char            buf[PRINTF_BUFSIZE+1];
    int             spd_raw;
    float           spd;

    if (len != data[1]+3)
        return 1;

    spd_raw = data[3] << 8 | data[2];
    spd = spd_raw / 10.0;

    if (oformat == FORMAT_NMEA)
    {

        return 0;
    }

    if (oformat == FORMAT_JSON)
    {
        return 0;
    }

    if (oformat == FORMAT_HUMAN)
    {
        snprintf(buf, PRINTF_BUFSIZE, "Speed through water: %f kts\n", spd);
        uart1_transmit_string(buf);
        return 0;
    }
}

/***************************************************************************
 *  20  01  XX  XX  Speed through water: XXXX/10 Knots
 *                  Corresponding NMEA sentence: VHW
 */
int
water_temperature(int *data, int len, int oformat)
{
    char            buf[PRINTF_BUFSIZE+1];


    if (len != (data[1] & 0x3) + 3)
        return 1;

    if (oformat == FORMAT_NMEA)
    {

        return 0;
    }

    if (oformat == FORMAT_JSON)
    {
        return 0;
    }

    if (oformat == FORMAT_HUMAN)
    {
        snprintf(buf, PRINTF_BUFSIZE, "Water temperature: %d C %d F\n", data[2], data[3]);
        uart1_transmit_string(buf);
        return 0;
    }
}

int
process_seatalk_string(int *data, int len, int oformat)
{
    struct wind_data wd;

    if (oformat == FORMAT_RAW)
    {
        transmit_raw_buffer(data, len);
        return 0;
    }
    
    switch((*data) & 0x00FF)
    {
        case 0x00:
            return depth_below_transducer(data, len, oformat);

        case 0x10:
            return apparent_wind_angle(data, len, oformat, &wd);

        case 0x11:
            return apparent_wind_speed(data, len, oformat, &wd);

        case 0x20:
            return speed_through_water(data, len, oformat);
        
        case 0x23:
            return water_temperature(data, len, oformat);
            
        default:
            return 1;
    }
}

int main(int argc, char** argv) {

    int i,r;
    int rbuf[20];
    int data;
    int format;
    processor_init();
    uart1_init();
    uart2_init();

    TRISDbits.TRISD0 = 0;
    TRISBbits.TRISB12 = 0;
    TRISBbits.TRISB11 = 0;

    TRISBbits.TRISB0 = 1;
    TRISBbits.TRISB1 = 1;
    TRISBbits.TRISB2 = 1;
    ADPCFGbits.PCFG0 = 1;
    ADPCFGbits.PCFG1 = 1;
    ADPCFGbits.PCFG2 = 1;

    ledstatus = 0;

    cb = buffer_init();

    while(1)
    {
        if ( buffer_full(cb) )
            /* Signal buffer overflow */
            PORTDbits.RD0 = 1;

        if ( !buffer_empty(cb) )
        {
            int data;

            /* Get a 'char' from the buffer */
            buffer_read(cb, &data);

            if ( data & 0x0100 )
            {

                format = (~PORTB) & 0x3;
                /* Print the last sentence and begin reading the new one */
                PORTBbits.RB12 = 1;
                //transmit_raw_buffer(rbuf, r);
                process_seatalk_string(rbuf, r, format);
                PORTBbits.RB12 = 0;
                r = 0;
                rbuf[r++] = data;
            }

            else
            {
                /* Accumulate data into the buffer */
                //printf("rx: %x\n",data);
                rbuf[r++] = data;
            }

        }
    }

    return (EXIT_SUCCESS);
}

