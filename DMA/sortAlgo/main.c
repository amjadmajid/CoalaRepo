#include <msp430.h> 
#include <ipos.h>

/*
void sortAlgo(int arr[], int arrLen){

    for (int i=0; i< arrLen-1;i++){

        for(int j=i+1; j < arrLen; j++){
            if(arr[i] >  arr[j]) { //ascending order
                int temp = arr[j];
                arr[j] = arr[i];
                arr[i] = temp;
            }
        }

    }

}

*/
///// function Prototypes
void task_finish();
void task_outer_loop();
void task_inner_loop();

////// Global variables
unsigned arr[]= {3,1,4,6,9,5,10,8,16,20,19,40,16,17,2,41,80,100,5,89};
unsigned arr_len = 20;
unsigned i =  0 ;
unsigned j = 1;

/////  TASKS
void task_inner_loop()
{
    if(arr[i] > arr[j])
    {
        int temp = arr[j];
        arr[j] = arr[i];
        arr[i] = temp;
    }

    if( j < arr_len)
    {
        os_jump(0);
    }
    j++;
}

void task_outer_loop()
{
    i++;
    j=i+1;

    if(i > arr_len)
    {
        os_unblock(task_finish);
    }
}

void task_finish()
{

    if( (arr[19] == 100)  && (arr[0] ==1) )
        {
            P3OUT |=BIT5;
            P3OUT &=~BIT5;
        }

//    P4OUT |= BIT0;
//    __delay_cycles(500);
//    P4OUT &= ~BIT0;

//    arr[0]=3;
//    arr[1]=1;
//    arr[2] =4;
//    arr[3]= 6;
//    arr[4]=9;
//    arr[5]=5;
//    arr[6]=10;
//    arr[7]=8;
//    arr[8]=16;
//    arr[9]=20;
//    arr[10]= 19;
//    arr[11]=40;
//    arr[12]=16;
//    arr[13]=17;
//    arr[14]=2;
//    arr[15]=41;
//    arr[16]=80;
//    arr[17]=100;
//    arr[18]=5;
//    arr[19]=89;

    i =  0 ;
    j = 1;
    os_block(task_finish);
}

void init()
{
  WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
  // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings.
  PM5CTL0 &= ~LOCKLPM5;       // Lock LPM5.
  P3OUT = 0;
  P4OUT = 0;
  P3DIR |=BIT5;
  P4DIR |=BIT0;
}

int main(void) {
    init();

//    P4OUT |= BIT0;
//    __delay_cycles(5000);
//    P4OUT &= ~BIT0;

       taskId tasks[] = {  {task_inner_loop,0}, {task_outer_loop,0}, {task_finish,1}};
       //This function should be called only once
       os_addTasks(3, tasks );

       os_scheduler();
       return 0;
}
