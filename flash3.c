typedef volatile unsigned int ioreg;

// for displaying on the ocilliscope
#define SPI_SR   (ioreg *) 0xfffe0010 // SPI status register
#define PIO_PDR (ioreg *) 0xfffff404 // PIO disable register
#define PIO_ASR (ioreg *) 0xfffff470 // PIO A select register
#define SPI_CR   (ioreg *) 0xfffe0000 // SPI Control Register
#define SPI_MR   (ioreg *) 0xfffe0004 // SPI Mode Register
#define SPI_TDR  (ioreg *) 0xfffe000c // SPI Transmit Data Register
#define SPI_CSR0 (ioreg *) 0xfffe0030 // SPI Chip Select Register 0
#define PMC_PCER (ioreg *) 0xFFFFFC10
//reading analogue inputs
#define ADC_CR   (ioreg *) 0xfffd8000 // ADC control register
#define ADC_MR   (ioreg *) 0xfffd8004 // ADC mode register
#define ADC_CHER (ioreg *) 0xfffd8010 // ADC channel enable register
#define ADC_SR   (ioreg *) 0xfffd801c // ADC status register
#define ADC_CDR4 (ioreg *) 0xfffd8040 // ADC channel 4 data register
#define ADC_CDR5 (ioreg *) 0xfffd8044 // ADC channel 5 data register

//defining the x y co-ordinates and the range of the board
#define X_port 0x4000
#define Y_port 0xc000
#define maxLength 1023
#define maxWidth 1023
#define maxPaddle 500
#define minPaddle 100
#define paddleLength 20
#define paddleHeight 510

//function to reset the registers
void resetRegisters(){

    LowLevelInit();
    *PIO_PDR = 0x7800; // disable bits 11 - 14 for SPI (See manual section 10.4)
    *PIO_PDR = 0x7800; // disable bits 11 - 14 for SPI
    *PIO_ASR = 0x7800; // enable peripheral A for bits 11 - 14
    *PMC_PCER = 0x34; // enable PIOA, ADC and SPI


    *SPI_CR = 0x80;    // reset the SPI
    *SPI_CR = 0x1;     // enable the SPI
    *SPI_MR = 0x11;    // set SPI to master, disable fault detection
    *SPI_CSR0 = 0x183; // set clock polarity = 1, clock phase = 1, Bits per transfer = 16, serial clock baud rate = 1
    *SPI_TDR = 0xd002; // Set reference voltage to 2.048V in control register

    *ADC_CR = 0x1;    // reset the ADC
    *ADC_CHER = 0x30; // enable analog channels 4 and 5
    *ADC_MR = 0x030b0400; // sample+holdtime = 3, startup = b, prescale = 4
}

void check();
//function to check for sterilisation
void check() {
  while(*SPI_SR & 1 == 0) {
  }
}

//vertical displacements of left and right paddles from bottom:
int leftY;
int rightY;

int channel4() {
    *ADC_CR = 0x2;  //start conversion
    while(*ADC_SR & 0x10 == 0) {
        //wait for channel to complete conversion
    }
    int paddle = *ADC_CDR4;
    return paddle;
}

int channel5() {
    *ADC_CR = 0x2;  //start conversion
    while(*ADC_SR & 0x20 == 0) {
        //wait for channel to complete conversion
    }
    int paddle = *ADC_CDR5;
    return paddle;
}

void controllingPaddles(){

    int paddleLeft = channel4();
    int paddleRight = channel5();
    //paddleLeft = 200;
    //left
    paddle(100, paddleLeft, paddleLength, paddleHeight);

    //right paddle
    paddle(880, paddleRight, paddleLength, paddleHeight);

}

//function to create a horizontal line
void horizontal(int x, int y, int length){
    int i;
    for (i = 0; i <= length; i++) {
        *SPI_TDR = X_port + ((x + i)<<2);
        check();
        *SPI_TDR = Y_port + (y<<2);
        check();
    }
}

//function to create a vertical line
void vertical(int x, int y, int length){
    int i;
    for (i = 0; i <= length; i++) {
        *SPI_TDR = X_port + (x<<2);
        check();
        *SPI_TDR = Y_port + ((y + i)<<2);
        check();
    }
}

//creating the border of the game
void square(int x, int y, int length, int height ){

    vertical(x, y, height);
    vertical(x + length, y, height);
    horizontal(x, y, length);
    horizontal(x , y + height, length);

}

//creating the paddles of the game
void paddle(int x, int y, int l, int h ){
    square(x, y, l, h);
}

//creating a single co-ordinate point
void point(int x, int y){
    *SPI_TDR = X_port + (x<<2);
    check();
    *SPI_TDR = Y_port + (y<<2);
    check();
}

//co-ordinates of the ball
int ball_x;
int ball_y;

void ball(int x, int y, int radius){
    //draws a circle that represents a ball using formula x^2 + y^2 = r^2
    int i;
    int j;
    for (i = 0; i < (2 * radius); i++){
        for (j = 0; j < (2 * radius); j++){
            if (((i * i) + (j * j)) < (radius * radius)){
                point(x + i, y + j);
            }
        }
    }
}

//resets the game every time a player scores
void resetGame(){
    ball_x = maxLength/2;
    ball_y = maxWidth/2;
}

void main() {

    resetRegisters();
    resetGame();

  while (1) {

    square(0, 0, 1023, 1023);
    square(500, 500, 20, 20);

    controllingPaddles();

  }

}
