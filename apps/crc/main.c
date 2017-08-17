#include <math.h>
#include <stdint.h>
#include <msp430.h> //msp430 specific library
#include "ipos.h" //runtime specific library
#include <string.h>
//Define all tasks in your program
void task1();
void task2();
void task3();
void task4();

//Define all preprocessor variables
#define FALSE   0
#define TRUE    !FALSE

/*
 * Select the CRC standard from the list that follows.
 */
#define CRC_CCITT

#if defined(CRC_CCITT)


#define CRC_NAME      "CRC-CCITT"
#define POLYNOMIAL          0x1021
#define INITIAL_REMAINDER   0xFFFF // Version 1
//#define INITIAL_REMAINDER 0x1D0F // Version 2 (https://www.lammertbies.nl/comm/info/crc-calculation.html)
#define FINAL_XOR_VALUE     0x0000
#define REFLECT_DATA        FALSE
#define REFLECT_REMAINDER   FALSE
#define CHECK_VALUE         0x29B1

#elif defined(CRC16)

typedef unsigned short  crc;

#define CRC_NAME            "CRC-16"
#define POLYNOMIAL          0x8005
#define INITIAL_REMAINDER   0x0000
#define FINAL_XOR_VALUE     0x0000
#define REFLECT_DATA        TRUE
#define REFLECT_REMAINDER   TRUE
#define CHECK_VALUE         0xBB3D

#endif

/*
 * Derive parameters from the standard-specific parameters in crc.h.
 */
#define WIDTH    (8 * sizeof(unsigned short))
#define TOPBIT   (1 << (WIDTH - 1))

#if (REFLECT_DATA == TRUE)
#undef  REFLECT_DATA
#define REFLECT_DATA(X)         ((unsigned char) reflect_task((X), 8))
#else
#undef  REFLECT_DATA
#define REFLECT_DATA(X)         (X)
#endif

#if (REFLECT_REMAINDER == TRUE)
#undef  REFLECT_REMAINDER
#define REFLECT_REMAINDER(X)    ((crc) reflect_task((X), WIDTH))
#else
#undef  REFLECT_REMAINDER
#define REFLECT_REMAINDER(X)    (X)
#endif

//Define all variables passed from task to task as global
char const message[] = "12345"; //global input
__v int result; //global output

//task1 variables
__v uint16_t byte = 0; //reflect_task variables (internal)
__v unsigned long data; //= (message[byte]) << (WIDTH - 8); //reflect_task variables (input)
__v int nBytes = strlen(message); //crcSlow_task variables (input)

//task 2 variables
__v uint16_t bit = 0; //reflect_task variables (internal)
__v unsigned char nBits = 8; //reflect_task variables (input)
__v unsigned short _remainder = INITIAL_REMAINDER; //reflect_task variables (internal)
__v unsigned long reflection = 0x00000000; //reflect_task variables (output)



void reflect()
{
        unsigned char  bit;

        /*
         * Reflect the data about the center bit.
         */
        for (bit = 0; bit < nBits; ++bit)
        {
            /*
             * If the LSB bit is set, set the reflection of it.
             */
            if (data & 0x01)
            {
                reflection |= (1 << ((nBits - 1) - bit));
            }

            data = (data >> 1);
        }

}



void task1()
{
  ++byte;
  if (byte < nBytes)
  {
    data = (message[byte]) << (WIDTH - 8);
  }
  else
  {
    byte = 0;
  }
}

void task2()
{
  ++bit;

  if (bit < nBits)
  {
    if (data & 0x01)
    {
      reflection |= (1 << ((nBits - 1) - bit));
    }
    data = (data >> 1);

    _remainder ^= reflection;
    //os_block(task2);
    os_same();
  }
  else
  {
    bit = 0;
  }

}

void task3()
{
  int bit = 8;
  --bit;
  if (bit > 0)
  {
    if (_remainder & TOPBIT)
    {
        _remainder = (_remainder << 1) ^ POLYNOMIAL;
    }
    else
    {
        _remainder = (_remainder << 1);
    }
  }
}

void task4()
{
  result = _remainder ^ FINAL_XOR_VALUE;
  while(1);
}

// void reflect_task()
// {
//  unsigned char bit;
//
//  /*
//   * Reflect the data about the center bit.
//   */
//  for (bit = 0; bit < nBits; ++bit)
//  {
//      /*
//       * If the LSB bit is set, set the reflection of it.
//       */
//      if (data & 0x01)
//      {
//          reflection |= (1 << ((nBits - 1) - bit));
//      }
//
//      data = (data >> 1);
//  }
//
//  //return (reflection);
//   os_block(reflect_task);
//   os_block();
//
// }    /* reflect() */
//
// void crcSlow_task()
// {
//   crc            _remainder = INITIAL_REMAINDER;
//  int            byte;
//  unsigned char  bit;
//
//     /*
//      * Perform modulo-2 division, a byte at a time.
//      */
//     for (byte = 0; byte < nBytes; ++byte)
//     {
//         /*
//          * Bring the next byte into the .
//          */
//         _remainder ^= (REFLECT_DATA(message[byte]) << (WIDTH - 8));
//
//         /*
//          * Perform modulo-2 division, a bit at a time.
//          */
//         for (bit = 8; bit > 0; --bit)
//         {
//             /*
//              * Try to divide the current data bit.
//              */
//             if (_remainder & TOPBIT)
//             {
//                 _remainder = (_remainder << 1) ^ POLYNOMIAL;
//             }
//             else
//             {
//                 _remainder = (_remainder << 1);
//             }
//         }
//     }
//
//     /*
//      * The final _remainder is the CRC result.
//      */
//     //return (REFLECT_REMAINDER(_remainder) ^ FINAL_XOR_VALUE);
//
//     result = REFLECT_REMAINDER(_remainder) ^ FINAL_XOR_VALUE;
//     os_unblock(crcSlow_task);
//     os_block(reflect_task);
//
// }   /* crcSlow() */

// initialize MSP430
void init()
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings.
    PM5CTL0 &= ~LOCKLPM5;       // Lock LPM5.

    P3DIR |=BIT5;
}

int main(void)
{
  init(); //Initialize runtime

  //Create order of tasks
  //Comment: what is the meaning of taskId?
  //Comment: why tasks are defined as an array?
  taskId tasks[] = {{task1,0}, //Comment: task chain is already defined within tasks - redundant
                    {task2,0},
                    {task3,0},
                    {task2,0},
                    {task4,0}}; //Comment: what does "0" and "1" mean - 1: block, 0: unblock?


  os_addTasks(4,tasks); //Comment: number of tasks in chain should be automatic
  os_scheduler(); //Comment: does it anyway happen that you don't need to call init(), or os_addTasks and/or os_scheduler()? If not - all three functions can be merged into one name

  return 0;
}
