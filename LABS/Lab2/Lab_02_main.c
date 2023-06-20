//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <SI_EFM8BB1_Register_Enums.h>                // SFR declarations
#include <stdbool.h>


 SI_SBIT(LED_Y, SFR_P1, 4);
 SI_SBIT(MUX_A, SFR_P1, 2);
 SI_SBIT(MUX_B, SFR_P0, 5);
 SI_SBIT(MUX_C, SFR_P0, 0);

 SI_SBIT(BCD_A, SFR_P1, 5);
 SI_SBIT(BCD_B, SFR_P1, 6);
 SI_SBIT(BCD_C, SFR_P1, 1);
 SI_SBIT(BCD_D, SFR_P0, 6);
//-----------------------------------------------------------------------------
void SiLabs_Startup (void)
{

  WDTCN =0xDE;
  WDTCN = 0xAD;
}
unsigned int count = 0;
volatile bool state = false;
int div = 50;
char symbol[] ={1,5,0,5,2,3} ;


void Diode_Mode_Off(){
    MUX_A = 0;
    MUX_B = 0;
    MUX_C = 0;
}

void Diode_Mode_On(bool Diode) {

  if(Diode == true){ //red
      MUX_A = 1;
      MUX_B = 1;
      MUX_C = 1;
  }
  else{ //yellow
      MUX_A = 0;
      MUX_B = 1;
      MUX_C = 1;
  }
}

void Diode_Mode_On_And_Off(bool State) {
  if(State == true){
      Diode_Mode_On(false);
  }
  else{
      Diode_Mode_On(true);
  }
}

//Display Section

void selectedDisplay(char display){
  MUX_A = display & 0x01;
  MUX_B = display & 0x02;
  MUX_C = display & 0x04;
}

void selectedSymbol(char value){
  BCD_A = value & 0x01;
  BCD_B = value & 0x02;
  BCD_C = value & 0x04;
  BCD_D = value & 0x08;
}


void printOnDisplay(char display, char symbol){
  selectedDisplay(display);
  selectedSymbol(symbol);
}


void clearDisplay(char display){
  selectedDisplay(display);
  selectedSymbol(0);
}


void Display_Mode_On_And_Off(bool State, char Display, char symbol) {
  if(State == true){
      printOnDisplay(Display, symbol);
  }
  else{

      clearDisplay(1);
      //printOnDisplay(Display, symbol);
  }
}

SI_INTERRUPT(TIMER0_ISR, TIMER0_IRQn){
  TL0 = 250;
  TH0 = 250;
  ++count;
  state = true;
  if(count ==1){
      Display_Mode_On_And_Off(state,0,symbol[0]);
      }
  else if (count == 2){
      Display_Mode_On_And_Off(state, 1, symbol[1]);
        }
  else if (count == 3){
      Display_Mode_On_And_Off(state, 2, symbol[2]);
        }
  else if (count == 4){
      Display_Mode_On_And_Off(state, 3, symbol[3]);
        }
  else if (count == 5){
      Display_Mode_On_And_Off(state, 4, symbol[4]);
        }
  else if (count >= 6){
      Display_Mode_On_And_Off(state, 5, symbol[5]);
            count = 0;}
}
int main (void)
{

    Diode_Mode_Off();
    //system clock configuration
    CLKSEL = 0x0;

    //crossbar configuration
    XBR0 = 0x0;
    XBR1 = 0x0;
    XBR2 = 0xC0;

    //port drive configuration
    PRTDRV = 0x7;

    //port configuration
    P0MDIN = 0xFF;  //all P0 pins as outputs
    P0MDOUT = 0xFF; //all P0 pins as push pull outputs
    P0SKIP = 0xFF;  //all P0 pins skipped by the crossbar

    P1MDIN = 0xFF;
    P1MDOUT = 0xFE; //configure P1.0 as open drain for input mode
    P1SKIP = 0xFF;

    TMOD = 0x1;
    CKCON0= 0x0;
    TCON_TR0 = 1;
    IE_ET0 = 1;
    IE_EA = 1;






  while (1) {

  }                             // Spin forever
}
