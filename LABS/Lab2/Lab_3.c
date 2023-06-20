
#include <SI_EFM8BB1_Register_Enums.h>                // SFR declarations

SI_SBIT(LED_Y, SFR_P1, 4);
SI_SBIT(RS, SFR_P0, 6);
SI_SBIT(LCD_SI, SFR_P0, 7);
SI_SBIT(LCD_RST, SFR_P1, 0);
SI_SBIT(LCD_SCK, SFR_P0, 0);
SI_SBIT(EN, SFR_P1, 5);

SI_SBIT(SW1, SFR_P1, 2);
SI_SBIT(SW2, SFR_P0, 5);
SI_SBIT(SW3, SFR_P1, 6);
SI_SBIT(SW4, SFR_P1, 1);


const int SYSCLK = 12250;


void SiLabs_Startup (void)
{
  // Disable the watchdog here
  WDTCN=0xDE;
  WDTCN=0xAD;
}

void T0_Wait_ms (uint8_t ms){
  TCON &= ~0x30;                        //Stop Timer0; Clear TCON_TF0
  TMOD &= ~0x0f;                        //16-bit free run mode
  TMOD |= 0x01;

  CKCON0 |= 0x04;                       //Timer0 counts SYSCLKs

  while(ms){
      TCON_TR0 = 0;                     //Stop Timer0
      TH0 = ((-(SYSCLK)) >> 8);             //Overflow in 1ms
      TL0 = ((-(SYSCLK)) & 0xFF);
      TCON_TF0 = 1;                     //Clear overflow indicator
      TCON_TR0 = 1;                     //Start Timer0
      while(!TCON_TF0);                 //Wait for overflow
      ms--;                             //Update ms counter
  }
  TCON_TR0 = 0;                         //Stop Timer0
}

void initialization (uint8_t input){
  int counter;
  RS = 0;
  LCD_SCK = 0;
  LCD_RST = 0;
  T0_Wait_ms(50);
  LCD_RST = 1;
  for(counter=0; counter < 8; counter++){
      LCD_SI = (input & 0x80);
      LCD_SCK = 1;
      T0_Wait_ms(50);
      input = input << 1;
      LCD_SCK = 0;
  }
  T0_Wait_ms(50);
  EN = 1;
  T0_Wait_ms(50);
  EN = 0;
}

void TIME_SEND (char input){
  int counter;
  RS = 1;
  LCD_SCK = 0;
  LCD_RST = 0;
  T0_Wait_ms(50);
  LCD_RST = 1;
  for(counter=0; counter < 8; counter++){
      LCD_SI = (input & 0x80);
      LCD_SCK = 1;
      T0_Wait_ms(50);
      input = input << 1;
      LCD_SCK = 0;
  }
  T0_Wait_ms(50);
  EN = 1;
  T0_Wait_ms(50);
  EN = 0;
}
volatile long int counter=0;
SI_INTERRUPT(TIMER1_ISR, TIMER1_IRQn){
  static int i=0;
   if(i==16){
       counter++;
     if (LED_Y==0){
       LED_Y=1;}
     else LED_Y=0;
     i=0;
   }
   else{
       i++;
   }
}




int main (void)
{
  int h, m, s;
  char h1, h2, m1, m2, s1, s2;
    CLKSEL=0x10;

    XBR0=0x0;
    XBR1=0x0;
    XBR2=0xC0;

    PRTDRV=0x7;

    P0MDIN=0xFF;
    P0MDOUT=0xFF;
    P0SKIP=0xFF;

    P1MDIN=0xFF;
    P1MDOUT=0xFE;
    P1SKIP=0xFF;
    TMOD=0x10;
    CKCON0=0x0;
    TCON_TR1=1;
    TL1=0;
    TH1=0;
    IE_ET1=1;
    IE_EA=1;

    initialization(0x01);
    initialization(0x038);
    initialization(0x0C);
    initialization(0x06);
    T0_Wait_ms(50);







  while (1) {
      T0_Wait_ms(100);
      h = counter / 3600;
      m = (counter % 3600) / 60;
      s = (counter % 60);
      h1= (h / 10) + '0';
      h2= (h % 10) + '0';
      m1= (m / 10) + '0';
      m2= (m % 10) + '0';
      s1= (s / 10) + '0';
      s2= (s % 10) + '0';
      if (SW3 == 0){
          counter++;
          if (counter >= 86400)
             counter =0;
      }
      if (SW2 == 0){
          counter+=60;
          if (counter >= 86400)
                     counter =0;
      }
      if (SW1 == 0){
          counter+=3600;

          if (counter >= 86400)
            counter =0;

      }
      if (SW4 == 0){
          counter=0;
      }
      initialization(0x080);
      TIME_SEND(h1);
      TIME_SEND(h2);
      TIME_SEND(':');
      TIME_SEND(m1);
      TIME_SEND(m2);
      TIME_SEND(':');
      TIME_SEND(s1);
      TIME_SEND(s2);
  }                             // Spin forever
}
