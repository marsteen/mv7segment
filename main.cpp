#include "TM1637.h"

extern "C"
{
  #include <xdo.h>
}

#define dio 8 // original 4
#define clk 9 // original 5 pins definitions for TM1637 and can be changed to other ports       


static xdo_t* xdo;
static int Score1;
static int Score2;
  
int setup()
{
  
  if(wiringPiSetup()==-1)
  {
     printf("setup wiringPi failed ! n");
     return 1;
  }

  xdo = xdo_new(NULL);


  
  printf("START setup\n");
  pinMode(clk,INPUT);
  printf("pinMode clk ok\n");
  pinMode(dio,INPUT);
  printf("pinMode dio ok\n");
  delay(200); 
  
  TM1637_init(clk,dio);
  printf("TM1637_init dio ok\n");
  TM1637_set(BRIGHTEST,0x40,0xc0);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
  printf("TM1637_set ok\n");
  
  printf("Digital Tube test code!\n");
  printf("Using DATA=%d, CLK=%d\n", dio, clk);
  return 1;
  
}


enum ESegment
{
  ESEG_0 = 1,
  ESEG_1 = 2,
  ESEG_2 = 4,
  ESEG_3 = 8,
  ESEG_4 = 16,
  ESEG_5 = 32,
  ESEG_6 = 64
};

/*
      0000      
     5    1
     5    1
     5    1
     5    1
      6666
     4    2
     4    2
     4    2
     4    2
      3333

*/

#define NUMBER_0 (ESEG_0 | ESEG_1 | ESEG_2 | ESEG_3 | ESEG_4 | ESEG_5)
#define NUMBER_1 (ESEG_1 | ESEG_2)
#define NUMBER_2 (ESEG_0 | ESEG_1 | ESEG_3 | ESEG_4 | ESEG_6)
#define NUMBER_3 (ESEG_0 | ESEG_1 | ESEG_2 | ESEG_3 | ESEG_6)
#define NUMBER_4 (ESEG_1 | ESEG_2 | ESEG_5 | ESEG_6)
#define NUMBER_5 (ESEG_0 | ESEG_2 | ESEG_3 | ESEG_5 | ESEG_6)
#define NUMBER_6 (ESEG_0 | ESEG_2 | ESEG_3 | ESEG_4 | ESEG_5 | ESEG_6)
#define NUMBER_7 (ESEG_0 | ESEG_1 | ESEG_2)
#define NUMBER_8 (ESEG_0 | ESEG_1 | ESEG_2 | ESEG_3 | ESEG_4 | ESEG_5 | ESEG_6)
#define NUMBER_9 (ESEG_0 | ESEG_1 | ESEG_2 | ESEG_3 | ESEG_5 | ESEG_6)

/*

      3333
     2    4
     2    4
     2    4
     2    4
      6666
     1    5
     1    5
     1    5
     1    5
      0000

*/


#define NUMBER_0_R (ESEG_0 | ESEG_1 | ESEG_2 | ESEG_3 | ESEG_4 | ESEG_5)
#define NUMBER_1_R (ESEG_5 | ESEG_4)
#define NUMBER_2_R (ESEG_0 | ESEG_1 | ESEG_6 | ESEG_4 | ESEG_3)
#define NUMBER_3_R (ESEG_0 | ESEG_5 | ESEG_4 | ESEG_3 | ESEG_6)
#define NUMBER_4_R (ESEG_2 | ESEG_4 | ESEG_5 | ESEG_6)
#define NUMBER_5_R (ESEG_0 | ESEG_2 | ESEG_3 | ESEG_5 | ESEG_6)
#define NUMBER_6_R (ESEG_0 | ESEG_1 | ESEG_5 | ESEG_6 | ESEG_2 | ESEG_3)
#define NUMBER_7_R (ESEG_3 | ESEG_5 | ESEG_4)
#define NUMBER_8_R (ESEG_0 | ESEG_1 | ESEG_2 | ESEG_3 | ESEG_4 | ESEG_5 | ESEG_6)
#define NUMBER_9_R (ESEG_0 | ESEG_2 | ESEG_4 | ESEG_3 | ESEG_5 | ESEG_6)


static const int Numberfield[] =
{
  NUMBER_0,
  NUMBER_1,
  NUMBER_2,
  NUMBER_3,
  NUMBER_4,
  NUMBER_5,
  NUMBER_6,
  NUMBER_7,
  NUMBER_8,
  NUMBER_9  
};

static const int NumberfieldR[] =
{
  NUMBER_0_R,
  NUMBER_1_R,
  NUMBER_2_R,
  NUMBER_3_R,
  NUMBER_4_R,
  NUMBER_5_R,
  NUMBER_6_R,
  NUMBER_7_R,
  NUMBER_8_R,
  NUMBER_9_R 
};

static void ShowNumber(unsigned int n)
{
  int8_t buff[4];
  
  for (int i = 0; i < 4; i++)
  {
    buff[i] = NumberfieldR[n % 10];
    n /= 10;
  }
  TM1637_displayRaw(buff);
}


static void ShowScore(int score1, int score2)
{
  int8_t buff[4];

  buff[0] = NumberfieldR[score1];
  buff[1] = 0;
  buff[2] = 0;
  buff[3] = NumberfieldR[score2];
  TM1637_displayRaw(buff);
}



struct SAniPhase
{
  int8_t d[4];  
};

SAniPhase ani1[] = 
{
  { ESEG_5, 0, 0, 0 },
  { ESEG_4, 0, 0, 0 },
  { ESEG_3, 0, 0, 0 },
  { ESEG_2, 0, 0, 0 },
  { ESEG_1, 0, 0, 0 },
  { 0, ESEG_0, 0, 0 },
  { 0, ESEG_1, 0, 0 },
  { 0, ESEG_2, 0, 0 },
  { 0, 0, ESEG_3, 0 },
  { 0, 0, ESEG_2, 0 },
  { 0, 0, ESEG_1, 0 },
  { 0, 0, 0, ESEG_0 },
  { 0, 0, 0, ESEG_1 },
  { 0, 0, 0, ESEG_2 },

  { -1, 0, 0, 0 }
};

SAniPhase ani2[] = 
{
  { ESEG_0, ESEG_0,      0,      0 },
  {      0, ESEG_0, ESEG_0,      0 },
  {      0,      0, ESEG_0, ESEG_0 },
  {      0,      0,      0, ESEG_0 | ESEG_1 },
  {      0,      0,      0, ESEG_1 | ESEG_2 },  
  {      0,      0,      0, ESEG_2 | ESEG_3 },
  {      0,      0, ESEG_3, ESEG_3 },  
  {      0, ESEG_3, ESEG_3,      0 },
  { ESEG_3, ESEG_3,      0,      0 },
  { ESEG_3 | ESEG_4,     0,      0 },
  { ESEG_4 | ESEG_5,     0,      0 },
  { ESEG_5 | ESEG_0,     0,      0 },
  { -1, 0, 0, 0 }
};


  
static void PlayAnimation(const SAniPhase* ani, int& phase) 
{
  if (ani[phase].d[0] == -1)
  {
     phase = 0;
  }

  TM1637_displayRaw(ani[phase].d);
/*
  for (int i = 0; i < 4; i++)
  {
    TM1637_displayRaw(i, ani[phase].d[i]);
  }
*/
  phase++;
}
  

  
int main()
{
  
  int8_t NumTab[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};//0~9,A,b,C,d,E,F
  int8_t ListDisp[4];
  unsigned char i = 0;
  unsigned char count = 0;
  printf("START main\n");
  delay(150);
  setup();
  int n = 0;
  int phase = 0;
  int pin0;
  int pin0_status;
  int pin1;
  int pin1_status;

  #define BUTTON_PIN0 0
  #define BUTTON_PIN1 3

  pinMode(BUTTON_PIN0,INPUT);
  pinMode(BUTTON_PIN1,INPUT);

  ShowScore(Score1, Score2);

  while(1)
  {

    //printf("n=%d\n", n++);
/*
    unsigned char BitSelect = 0;
    i = count;
    count ++;
    if(count == sizeof(NumTab)) count = 0;
    for(BitSelect = 0; BitSelect < 4; BitSelect++)
    {
      ListDisp[BitSelect] = NumTab[i];
      i ++;
      if(i == sizeof(NumTab)) i = 0;
    }
*/    
   // ShowNumber(n);
    
    //PlayAnimation(ani2, phase);
    //printf("pin=%d\n", digitalRead(0));
    //TM1637_displayRaw(n);

    int pin0 = digitalRead(BUTTON_PIN0);
    if (n > 20)
    if (pin0 > 0)
    {
      if (pin0 != pin0_status)
      {
         Score1++;
         if (Score1 > 9)
         {
           Score1 = 0;
         }
         ShowScore(Score1, Score2);
         //n++;
	 //xdo_send_keysequence_window(xdo, CURRENTWINDOW, "1", 0);
      }
    }

    int pin1 = digitalRead(BUTTON_PIN1);
    if (n > 20)
    if (pin1 > 0)
    {
      if (pin1 != pin1_status)
      {
         Score2++;
         if (Score2 > 9)
         {
           Score2 = 0;
         }
         ShowScore(Score1, Score2);
      }
    }



    pin0_status = pin0;
    pin1_status = pin1;
    n++;

    delay(20);
  }
}
