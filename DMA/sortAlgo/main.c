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
//DVAR( uint16_t arr[], ={3,1,4,6,9,5,10,8,16,20} );
__p uint16_t arr[] = {3,1,4,6,9,5,10,8,16,20,19,40,16,17,2,41,80,100,5,89};
     uint16_t arr_len = 20;
__p uint16_t i =  0;
__p uint16_t j = 1;

/////  TASKS
void task_inner_loop()
{

    uint16_t in_i = RVAR(i);
    uint16_t in_j = RVAR(j);
    uint16_t arr_i = RVAR( arr[ in_i ]);
    uint16_t arr_j = RVAR(arr[ in_j ]);


    if( arr_i  > arr_j )
    {
        uint16_t temp = arr_j;
        arr_j =  arr_i;
        arr_i =  temp;
    }

    if( in_j < arr_len)
    {
        os_jump(0);
    }


    WVAR( arr[ in_i ] , arr_i);
    WVAR( arr[ in_j ] , arr_j);
    in_j++;
    WVAR(j , in_j);
}

void task_outer_loop()
{
    uint16_t in_i = RVAR(i);
    in_i++;

    if(in_i > arr_len)
    {
        os_unblock(task_finish);
    }

    WVAR(i, in_i);
    WVAR(j, in_i+1);
}

void task_finish()
{
    P3OUT |=BIT5;
    P3OUT &=~BIT5;


    arr[0]=3;
    arr[1]=1;
    arr[2] =4;
    arr[3]= 6;
    arr[4]=9;
    arr[5]=5;
    arr[6]=10;
    arr[7]=8;
    arr[8]=16;
    arr[9]=20;
    arr[10]= 19;
    arr[11]=40;
    arr[12]=16;
    arr[13]=17;
    arr[14]=2;
    arr[15]=41;
    arr[16]=80;
    arr[17]=100;
    arr[18]=5;
    arr[19]=89;

    WVAR(i, 0) ;
    WVAR(j, 1);
    os_block(task_finish);
}

void init()
{
  WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
  // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings.
  PM5CTL0 &= ~LOCKLPM5;       // Lock LPM5.
  P3DIR |=BIT5;
  P1DIR |=BIT0;
}

int main(void) {
    init();

       taskId tasks[] = {  {task_inner_loop,0}, {task_outer_loop,0}, {task_finish,1}};
       //This function should be called only once
       os_addTasks(3, tasks );

       os_scheduler();
       return 0;
}
