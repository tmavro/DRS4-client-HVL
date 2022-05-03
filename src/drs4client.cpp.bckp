#include <math.h>

#ifdef _MSC_VER

#include <windows.h>

#elif defined(OS_LINUX)

#define O_BINARY 0

#include <unistd.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <errno.h>

#define DIR_SEPARATOR '/'

#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "strlcpy.h"
#include "DRS.h"

// ---------------------------------------------------------------------------

#include <iostream>
#include <chrono>

using namespace std;
int initScan(DRS*, DRSBoard*);
int runNumberEvents(DRSBoard*);
void runAmountTime(DRSBoard*, std::chrono::seconds);

int main() //(int argc, char* argv[]) //TODO Implement arguments
{
    // // Check the number of parameters
    // if (argc < 2) {
    //     show_usage(argv[0]);
    //     return 1;
    // }
    // // Print the user's name:
    // std::cout << argv[0] << "says hello, " << argv[1] << "!" << std::endl;
    // return 0;

    DRS *drs;
    DRSBoard *b;

    drs = new DRS();

    // TODO Uncomment if you actually want to force running program with board connected
    //if (initScan(drs, b) == 0) return 0; // Initialize board, returns 0 if no boards found. 

    // TODO: actually respond to arguments here
    int argument = 1; // Argument denotes timed(1) or numbered(0) run
    if (argument == 1) {
        std::chrono::seconds sekunder(10);

        auto start = std::chrono::steady_clock::now();
        runAmountTime(sekunder, b);
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
    }
    else {

        runNumberEvents(b);
    }

    delete drs;
}


int runNumberEvents(DRSBoard* b){
    float time_array[8][1024];
    float wave_array[8][1024];
    /* open file to save waveforms */
   
    FILE* f;
    f = fopen("data.txt", "w");
    if (f == NULL) {
        perror("ERROR: Cannot open file \"data.txt\"");
        return 1;
    }


   /* repeat ten times */
   for (int j=0 ; j<10 ; j++) {

      /* start board (activate domino wave) */
      b->StartDomino();

      /* wait for trigger */
      printf("Waiting for trigger...");
      
      fflush(stdout);
      while (b->IsBusy());

      /* read all waveforms */
      b->TransferWaves(0, 8);

      /* read time (X) array of first channel in ns */
      b->GetTime(0, 0, b->GetTriggerCell(0), time_array[0]);

      /* decode waveform (Y) array of first channel in mV */
      b->GetWave(0, 0, wave_array[0]);

      /* read time (X) array of second channel in ns
       Note: On the evaluation board input #1 is connected to channel 0 and 1 of
       the DRS chip, input #2 is connected to channel 2 and 3 and so on. So to
       get the input #2 we have to read DRS channel #2, not #1. */
      b->GetTime(0, 2, b->GetTriggerCell(0), time_array[1]);

      /* decode waveform (Y) array of second channel in mV */
      b->GetWave(0, 2, wave_array[1]);

      /* Save waveform: X=time_array[i], Yn=wave_array[n][i] */
      fprintf(f, "Event #%d ----------------------\n  t1[ns]  u1[mV]  t2[ns] u2[mV]\n", j);
      for (int i=0 ; i<1024 ; i++)
         fprintf(f, "%7.3f %7.1f %7.3f %7.1f\n", time_array[0][i], wave_array[0][i], time_array[1][i], wave_array[1][i]);

      /* print some progress indication */
      printf("\rEvent #%d read successfully\n", j);
   }

   fclose(f);

   return 1;
}

void parseTimeArgumet() {

}

void runAmountTime(std::chrono::seconds sekund, DRSBoard* b)

    float time_array[8][1024];
    float wave_array[8][1024];
    /* open file to save waveforms */

    FILE* f;
    f = fopen("data.txt", "w");
    if (f == NULL) {
        perror("ERROR: Cannot open file \"data.txt\"");
        return 1;
    }
{
    std::chrono::time_point<std::chrono::system_clock> end;

    end = std::chrono::system_clock::now() + sekund; // this is the end point

    while (std::chrono::system_clock::now() < end) // still less than the end?
    {
        //std::cout << "Running" << std::endl;


        /* start board (activate domino wave) */
        b->StartDomino();

        /* wait for trigger */
        printf("Waiting for trigger...");

        fflush(stdout);
        while (b->IsBusy());

        /* read all waveforms */
        b->TransferWaves(0, 8);

        /* read time (X) array of first channel in ns */
        b->GetTime(0, 0, b->GetTriggerCell(0), time_array[0]);

        /* decode waveform (Y) array of first channel in mV */
        b->GetWave(0, 0, wave_array[0]);

        /* read time (X) array of second channel in ns
         Note: On the evaluation board input #1 is connected to channel 0 and 1 of
         the DRS chip, input #2 is connected to channel 2 and 3 and so on. So to
         get the input #2 we have to read DRS channel #2, not #1. */
        b->GetTime(0, 2, b->GetTriggerCell(0), time_array[1]);

        /* decode waveform (Y) array of second channel in mV */
        b->GetWave(0, 2, wave_array[1]);

        /* Save waveform: X=time_array[i], Yn=wave_array[n][i] */
        fprintf(f, "Event #%d ----------------------\n  t1[ns]  u1[mV]  t2[ns] u2[mV]\n", j);
        for (int i = 0; i < 1024; i++)
            fprintf(f, "%7.3f %7.1f %7.3f %7.1f\n", time_array[0][i], wave_array[0][i], time_array[1][i], wave_array[1][i]);

        /* print some progress indication */
        printf("\rEvent #%d read successfully\n", j);
    }
}

int initScan(DRS* drs, DRSBoard* b){
   /* do initial scan */

   /* show any found board(s) */
   for (int i=0 ; i<drs->GetNumberOfBoards() ; i++) {
      b = drs->GetBoard(i);
      printf("Found DRS4 evaluation board, serial #%d, firmware revision %d\n", 
         b->GetBoardSerialNumber(), b->GetFirmwareVersion());
   }

   /* exit if no board found */
   if (drs->GetNumberOfBoards() == 0) {
      printf("No DRS4 evaluation board found\n");
      return 0;
   }

   /* continue working with first board only */
   b = drs->GetBoard(0);

   /* initialize board */
   b->Init();

   /* set sampling frequency */
   b->SetFrequency(5, true);

   /* enable transparent mode needed for analog trigger */
   b->SetTranspMode(1);

   /* set input range to -0.5V ... +0.5V */
   b->SetInputRange(0);

   /* use following line to set range to 0..1V */
   //b->SetInputRange(0.5);
   
   /* use following line to turn on the internal 100 MHz clock connected to all channels  */
   b->EnableTcal(1);

   /* use following lines to enable hardware trigger on CH1 at 50 mV positive edge */
   if (b->GetBoardType() >= 8) {        // Evaluaiton Board V4&5
      b->EnableTrigger(1, 0);           // enable hardware trigger
      b->SetTriggerSource(1<<0);        // set CH1 as source
   } else if (b->GetBoardType() == 7) { // Evaluation Board V3
      b->EnableTrigger(0, 1);           // lemo off, analog trigger on
      b->SetTriggerSource(0);           // use CH1 as source
   }
   b->SetTriggerLevel(0.05);            // 0.05 V
   b->SetTriggerPolarity(false);        // positive edge
   
   /* use following lines to set individual trigger elvels */
   //b->SetIndividualTriggerLevel(1, 0.1);
   //b->SetIndividualTriggerLevel(2, 0.2);
   //b->SetIndividualTriggerLevel(3, 0.3);
   //b->SetIndividualTriggerLevel(4, 0.4);
   //b->SetTriggerSource(15);
   
   b->SetTriggerDelayNs(0);             // zero ns trigger delay
   
   /* use following lines to enable the external trigger */
   //if (b->GetBoardType() == 8) {     // Evaluaiton Board V4
   //   b->EnableTrigger(1, 0);           // enable hardware trigger
   //   b->SetTriggerSource(1<<4);        // set external trigger as source
   //} else {                          // Evaluation Board V3
   //   b->EnableTrigger(1, 0);           // lemo on, analog trigger off
   // }

   return 1;
}

// static void show_usage(std::string name)
// {
//     std::cerr << "Usage: " << argv[0] << " <option(s)> SOURCES"
//               << "Options:\n"
//               << "\t-h,--help\t\tShow this help message\n"
//               << "\t-d,--destination DESTINATION\tSpecify the destination path"
//               << std::endl;
// }
