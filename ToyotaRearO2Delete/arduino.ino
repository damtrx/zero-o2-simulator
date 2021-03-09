// http://cho-yaba.punyu.jp/?p=1663
// Generic O2 simulator produces cycling output signal from 0.3 to 0.6 volts

#define MULT 10000 // multiplier for tuning word / step for noise (range of freq.)
#define MMIN 40000 // minimal value of tuning word (this mods the wave hz. This value is for the slow chip. The fast one needs (10 times more))
// Toyota back cat needs Square wave form from qroung 0.145 to 0.85 volts . 
// T is 13 secs (very slow) (6.5 high and 6.5 low)
byte waveform = 0; // waveform
unsigned long M, Mraw, C = 0;
// M is the tuning word, Mraw is tuning word before processing
// C is the 32bits counter
byte offset = 0;       // +/- will move the wave up or down
byte scaleDivisor = 5; // 1==5V range, 2==2.5v range, etc

//THis one is added at 16.05.2020 to fix the low part of the voltage
const unsigned char sine256[] PROGMEM= { 
// square
238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,
238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,
238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,
238,238,238,238,238,238,238,238,238,238,238,238,238,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,
36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,
36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,8};

void setup()
{
    cli(); // Disable global interrupts
    // use 64Mhz clock
    PLLCSR = (1 << PLLE);
    for (int i = 0; i < 100; i++)
        ; // wait a bit
    PLLCSR |= (1 << PCKE);
    // PB1 as output (OC1A)
    // DDRB = (1 << PB1);
    DDRB = (1 << PB1);
    // Overflow interrupt
    TIMSK = (1 << TOIE1);
    // PWM based on OCR1A / toggle output on OC1A=PB1 / prescaler=1 (64MHz)
    TCCR1 = (1 << PWM1A) | (1 << COM1A1) | (1 << CS10);
    sei(); // Enable global interrupts
    OCR1A = 0;
    M = MMIN;
}

ISR(TIMER1_OVF_vect) // Interrupt Service Routine
{
    switch (waveform)
    {

    case 0: // Custom Square

        OCR1A = offset + pgm_read_byte(&sine256[((C += M) >> 24)]) / scaleDivisor;
        break;
    }
}

void loop()
{
}