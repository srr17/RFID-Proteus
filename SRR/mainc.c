#include <reg51.h>
#include <string.h>
#include <stdio.h>

#define LCDPORT P1

sbit rs = P1^0;
sbit rw = P1^1;
sbit en = P1^2;
sbit Motor1 = P2^4;
sbit Motor2 = P2^3;
sbit Speaker = P2^6;

char rfid[13], ch = 0; // Buffer to store RFID
int counter1 = 0, counter2 = 0, counter3 = 0;
unsigned char result[5]; // Buffer for formatted attendance count
int i; // Declare the loop variable globally or outside the loop

// Function Prototypes
void delay(int itime);
void daten(void);
void lcddata(unsigned char ch);
void cmden(void);
void lcdcmd(unsigned char ch);
void lcdstring(char *str);
void lcd_init(void);
void uart_init(void);
char rxdata(void);

// Delay Function
void delay(int itime) {
    int i, j;
    for (i = 0; i < itime; i++)
        for (j = 0; j < 1275; j++);
}

// LCD Data Enable
void daten() {
    rs = 1;
    rw = 0;
    en = 1;
    delay(5);
    en = 0;
}

// Send Data to LCD
void lcddata(unsigned char ch) {
    LCDPORT = ch & 0xf0;
    daten();
    LCDPORT = (ch << 4) & 0xf0;
    daten();
}

// LCD Command Enable
void cmden() {
    rs = 0;
    en = 1;
    delay(5);
    en = 0;
}

// Send Command to LCD
void lcdcmd(unsigned char ch) {
    LCDPORT = ch & 0xf0;
    cmden();
    LCDPORT = (ch << 4) & 0xf0;
    cmden();
}

// Send String to LCD
void lcdstring(char *str) {
    while (*str) {
        lcddata(*str++);
    }
}

// LCD Initialization
void lcd_init(void) {
    lcdcmd(0x02); // Initialize LCD
    lcdcmd(0x28); // 4-bit mode, 2-line, 5x7 dots
    lcdcmd(0x0C); // Display ON, Cursor OFF
    lcdcmd(0x01); // Clear Display
}

// UART Initialization
void uart_init() {
    TMOD = 0x20;  // Timer1 in Mode2 (auto-reload)
    SCON = 0x50;  // 8-bit data, 1 start, 1 stop bit
    TH1 = 0xFD;   // 9600 baud rate
    TR1 = 1;      // Start Timer1
}

// Receive Data from UART
char rxdata() {
    while (!RI);   // Wait for data
    ch = SBUF;     // Read received data
    RI = 0;        // Clear RI flag
    return ch;     // Return the character
}

void main() {
    // Initialize peripherals
    Speaker = 1;
    uart_init();
    lcd_init();

    // Welcome Message
    lcdstring("---RFID Based---");
    lcdcmd(0xC0);
    lcdstring("Attendance Proj");
    delay(500);
    lcd_init();
    lcdstring("--EC--");
    lcdcmd(0xC0);
    lcdstring("-150--151--168-");
    delay(400);

    while (1) {
        // Prompt for RFID
        lcdcmd(1); // Clear Display
        lcdstring("Scan Your Card:");
        lcdcmd(0xC0);

        // Receive RFID input
        for (i = 0; i < 12; i++) { // Use the globally declared `i`
            rfid[i] = rxdata();
        }
        rfid[12] = '\0'; // Null-terminate RFID string

        // Display received RFID
        lcdcmd(1);
        lcdstring("Rfid No. is:");
        lcdcmd(0xC0);
        lcdstring(rfid);
        delay(100);

        // Check RFID and perform actions
        if (strncmp(rfid, "10035AD856C1", 12) == 0) {
            counter1++;
            lcdcmd(1);
            lcdstring("Attended: 150");
            lcdcmd(0xC0);
            sprintf(result, "No.: %d", counter1);
            lcdstring(result);

            // Activate Motor
            Motor1 = 1;
            Motor2 = 0;
            delay(300);
            Motor1 = 0;
            Motor2 = 1;
            delay(300);
            Motor1 = 0;
            Motor2 = 0;
        } else if (strncmp(rfid, "1600ADC369A1", 12) == 0) {
            counter2++;
            lcdcmd(1);
            lcdstring("Attended: 151");
            lcdcmd(0xC0);
            sprintf(result, "No.: %d", counter2);
            lcdstring(result);

            // Activate Motor
            Motor1 = 1;
            Motor2 = 0;
            delay(300);
            Motor1 = 0;
            Motor2 = 1;
            delay(300);
            Motor1 = 0;
            Motor2 = 0;
        } else if (strncmp(rfid, "1600ABCD147A", 12) == 0) {
            counter3++;
            lcdcmd(1);
            lcdstring("Attended: 168");
            lcdcmd(0xC0);
            sprintf(result, "No.: %d", counter3);
            lcdstring(result);

            // Activate Motor
            Motor1 = 1;
            Motor2 = 0;
            delay(300);
            Motor1 = 0;
            Motor2 = 1;
            delay(300);
            Motor1 = 0;
            Motor2 = 0;
        } else {
            lcdcmd(1);
            lcdstring("Card Not Found");
            lcdcmd(0xC0);
            lcdstring("Try Another");
            Speaker = 0;
            delay(300);
            Speaker = 1;
        }
    }
}
