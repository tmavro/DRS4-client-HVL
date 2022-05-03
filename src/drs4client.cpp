/*  ------
     Libs 
    ------ */

#include <unistd.h> //DRS-related
#include <ctype.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "strlcpy.h"
#include "DRS.h"

#include <stdio.h>      /* fprint */
#include <string.h>
#include <stdlib.h>

#include <iterator>
#include <math.h>

#include <getopt.h>     /* getopt API */

#include <iostream>
#include <chrono>       /* Time-related */

/*  -------------------
     Define directives 
    ------------------- */ 
#define DIR_SEPARATOR '/'
#define PROGRAM_NAME "DRS4-client"
#define AUTHORS \
proper_name("Thomas B. Mavropoulos"), \
proper_name("Sigve B. Heggedal")

/*  ------------------
     Global variables 
    ------------------ */

// Argument options
int argFreq = 5;        // Sampling frequency
int argRange = 0;       // Input range, -0.5 - 0.5 [V]
int argTrigLvl = 0.05;   // Trigger level [V]
int argTrigDelay = 0;   // Trigger delay [ns]
int argNumb = 0;        // Numbered run
int argTime = 0;        // Timed run
//char* argFile = "data.txt"; 

//TODO decide on sane defaults! 

// Long argument options
static struct option const long_options[] =
{
    {"sampling-frequency", optional_argument, NULL, 'f'},
    {"input-range", optional_argument, NULL, 'i'},
    {"trigger-level", optional_argument, NULL, 'l'},
    {"trigger-delay", optional_argument, NULL, 'd'},
    {"numbered", optional_argument, NULL, 'n'},
    {"timed", optional_argument, NULL, 't'},
    {"save-to", optional_argument, NULL, 's'},
    {"help", no_argument, NULL, 'h'},
    {NULL, 0, NULL, 0}
};

/* ---------
    Methods
   --------- */ 
//TODO implement with DRS classes!
//void runBoardTime(std::chrono::seconds); // Runs board for given amount of time  //TODO Where did the relevant code disappear!?
void runNumbEvents(DRSBoard* &b, int events, FILE* &f); // Runs board for given amount of events
int initScan(DRS* &drs, DRSBoard* &b); // Checks for board and initializes it 
void showUsage(int status); // Shows help screen with options
void processArgs(int argc, char **argv); // Handles cli arguments

using namespace std;

int main(int argc, char **argv){

    processArgs(argc, argv); // Processing arguments from cli

    // Initializing board
    DRS *drs; //Boards
    DRSBoard *b; //Board
    drs = new DRS();
    if (initScan(drs, b) == EXIT_FAILURE) return EXIT_FAILURE; // Initialize board, exits if no boards found. 

    // Run board for a given time or number of events
    if (argTime > 0 && argNumb == 0) {

      // Opens file to save waveform 
      FILE* f;
      f = fopen("data.txt", "w");
      if (f == NULL) {
        perror("Error writing to file");
        delete drs;
        return EXIT_FAILURE;
      }

        // Runs for amount of time in seconds 
        std::chrono::seconds sekunder(argTime);
        auto start = std::chrono::steady_clock::now();
        //runBoardTime(sekunder);
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        std::cout << "Elapsed time: " << elapsed_seconds.count() << "s\n";

        fclose(f);
        delete drs;
        return EXIT_SUCCESS;
    }
    else if (argNumb > 0 && argTime == 0){
        FILE* f; // For storing waveforms
        f = fopen("data.txt", "w");
        if (f == NULL) {
            perror("ERROR: Cannot open file \"data.txt\"");
            delete drs;
            return EXIT_FAILURE;
        }
        // repeat argNumb times 
        for (int j=0 ; j<argNumb; j++) {
         runNumbEvents(b, j, f);
        }

        fclose(f);
        delete drs;

        return EXIT_SUCCESS;
    }
    else {
      cout << ("\nMust choose to either run based on an ammount of time or number of events. Exiting.\n");
      delete drs;
      return(EXIT_FAILURE);
    }
}

// parses cli argument input to correct time format
void parseTimeArgumet() { //TODO Implement
}

// Simply prints help screen to console
void showUsage(int status) 
{
  if (status != EXIT_SUCCESS) {
      cout << "Show Usage failure: Nothing to see here.";
      exit(status);
  }
  else {
    printf(("Usage: %s [OPTION]... [FILE]...\n"), PROGRAM_NAME);
    fputs(("Program description goes here \n"), stdout); // TODO: Mind-blowing and succinct desc

    fputs(("\
    \n\
    -f, --sampling-frequency equivalent to -vET\n\
    -i, --input-range        input range, ex. input 0 means -0.5 to 0.5. Default: 0\n\
    -l, --trigger-level      Set trigger level in volts. Default: 0.05\n\
    -d, --trigger-delay      Set trigger delay in ns. Default: 0 \n\
    -n, --numbered           Run for given number of events. Default: 0\n\
    -t, --timed              Run for given amount of seconds. Default: 0\n\
    -s, --save-to            Define filename (and directory) to store data. Default: \n\
    -h, --help               Shows this help message"), stdout);
    printf(("\
            \n\
            Examples:\n\
            %s -save-to=.file.txt            Saves file.txt to subdir folder\n\
            %s                               More information here\n"), PROGRAM_NAME, PROGRAM_NAME); // TODO Usage examples
  }
  exit(status);
} 

// Parse command line options 
void processArgs(int argc, char **argv){

  int c;
  while ((c = getopt_long (argc, argv, "f::i::l::d::n::t::h", long_options, NULL)) != -1)
    {
      switch (c)
        {
        case 'f': //Set frequency
          printf("Option f has arg: %s\n", optarg ? optarg : "(none)");
          argFreq = atoi(optarg);
          break;

        case 'i': //Input range
          printf("Option i has arg: %s\n", optarg ? optarg : "(none)");
          argRange = atoi(optarg);
          break;

        case 'l': //Trigger level
          printf("Option l has arg: %s\n", optarg ? optarg : "(none)");
          argTrigLvl = atoi(optarg);
          break;

        case 'd': //Trigger delay
          printf("Option d has arg: %s\n", optarg ? optarg : "(none)");
          argTrigDelay = atoi(optarg);
          break;

        case 'n': //Numbered run
          printf("Option n has arg: %s\n", optarg ? optarg : "(none)");
          argNumb = atoi(optarg);
          break;

        case 't': //Timed run
          printf("Option t has arg: %s\n", optarg ? optarg : "(none)");
          argTime = atoi(optarg);
          break;
                
        case 's': //Save-to
          printf("Option s has arg: %s\n", optarg ? optarg : "(none)");
          //argFile = optarg; //TODO Fix this, maybe make a separate method to handle properly
          break;

        default:
          showUsage(EXIT_SUCCESS);
        }
    }
}

// Board collects data for given amount of events
void runNumbEvents(DRSBoard* &b, int events, FILE* &f){
    float time_array[8][1024]; // Waveform data, time axis
    float wave_array[8][1024]; // Waveform data, amplitude

    //start board (activate domino wave)
    b->StartDomino();

    //wait for trigger 
    printf("Waiting for trigger...");

    fflush(stdout);
    while (b->IsBusy());

    //read all waveforms
    b->TransferWaves(0, 8);

    //read time (X) array of first channel in ns
    b->GetTime(0, 0, b->GetTriggerCell(0), time_array[0]);

    //decode waveform (Y) array of first channel in mV
    b->GetWave(0, 0, wave_array[0]);

    //read time (X) array of second channel in ns
    //  Note: On the evaluation board input #1 is connected to channel 0 and 1 of
    //  the DRS chip, input #2 is connected to channel 2 and 3 and so on. So to
    //  get the input #2 we have to read DRS channel #2, not #1. 
    b->GetTime(0, 2, b->GetTriggerCell(0), time_array[1]);

    //decode waveform (Y) array of second channel in mV 
    b->GetWave(0, 2, wave_array[1]);

    //Save waveform: X=time_array[i], Yn=wave_array[n][i]
    fprintf(f, "Event #%d ----------------------\n  t1[ns]  u1[mV]  t2[ns] u2[mV]\n", events);
    for (int i=0 ; i<1024 ; i++)
        fprintf(f, "%7.3f %7.1f %7.3f %7.1f\n", time_array[0][i], wave_array[0][i], time_array[1][i], wave_array[1][i]);

    //print some progress indication
    printf("\rEvent #%d read successfully\n", events);

}

/*
// Board collects data for given amount of time
void runBoardTime(std::chrono::seconds sekund) {
    std::chrono::time_point<std::chrono::system_clock> end;
    end = std::chrono::system_clock::now() + sekund; // This is the end point

    while (std::chrono::system_clock::now() < end) { // While time is still less than end time
    //std::cout << "Running" << std::endl;


    // start board (activate domino wave) 
    b->StartDomino();

    // wait for trigger 
    printf("Waiting for trigger...");

    fflush(stdout);
    while (b->IsBusy());

    // read all waveforms 
    b->TransferWaves(0, 8);

    // read time (X) array of first channel in ns 
    b->GetTime(0, 0, b->GetTriggerCell(0), time_array[0]);

    // decode waveform (Y) array of first channel in mV 
    b->GetWave(0, 0, wave_array[0]);

    // read time (X) array of second channel in ns
        Note: On the evaluation board input #1 is connected to channel 0 and 1 of
        the DRS chip, input #2 is connected to channel 2 and 3 and so on. So to
        get the input #2 we have to read DRS channel #2, not #1. 
    b->GetTime(0, 2, b->GetTriggerCell(0), time_array[1]);

    // decode waveform (Y) array of second channel in mV 
    b->GetWave(0, 2, wave_array[1]);

    //Save waveform: X=time_array[i], Yn=wave_array[n][i]
    fprintf(f, "Event ----------------------\n  t1[ns]  u1[mV]  t2[ns] u2[mV]\n");
    for (int i = 0; i < 1024; i++)
        fprintf(f, "%7.3f %7.1f %7.3f %7.1f\n", time_array[0][i], wave_array[0][i], time_array[1][i], wave_array[1][i]);

    //print some progress indication 
    printf("\rEvent read successfully\n");
    }
}

  */

// Does a scan for boards and initializes with user-defined parameters (or defaults.) Always picks first board as we only support one board. Returns 0 if no board found. 
int initScan(DRS* &drs, DRSBoard* &b){
  // show any found board(s)
  for (int i=0 ; i<drs->GetNumberOfBoards() ; i++) {
    b = drs->GetBoard(i);
    printf("Found DRS4 evaluation board, serial #%d, firmware revision %d\n", 
        b->GetBoardSerialNumber(), b->GetFirmwareVersion());
  }

  // exit if no board found
  if (drs->GetNumberOfBoards() == 0) {
    printf("No DRS4 evaluation board found\n");
    return EXIT_FAILURE;
  }

  // continue working with first board only
  b = drs->GetBoard(0);

  //initialize board
  b->Init();

  // set sampling frequency
  b->SetFrequency(5, true);

  // enable transparent mode needed for analog trigger
  b->SetTranspMode(1);

  //set input range to given value, +/- 0.5. Ex. 0 gives a range from -0.5 to 0.5
  b->SetInputRange(argRange);
  
  // use following line to turn on the internal 100 MHz clock connected to all channels 
  b->EnableTcal(1);

  // The following lines enables hardware trigger on CH1 at given voltage positive edge
  if (b->GetBoardType() >= 8) {        // Evaluaiton Board V4&5
    b->EnableTrigger(1, 0);           // enable hardware trigger
    b->SetTriggerSource(1<<0);        // set CH1 as source
  } else if (b->GetBoardType() == 7) { // Evaluation Board V3
    b->EnableTrigger(0, 1);           // lemo off, analog trigger on
    b->SetTriggerSource(0);           // use CH1 as source
  }
  b->SetTriggerLevel(0.05);            // Trigger level. Defualt: 0.05 V
  b->SetTriggerPolarity(false);        // positive edge

  // set trigger delay. Default: zero ns trigger delay
  b->SetTriggerDelayNs(0);             
  return EXIT_SUCCESS;
}
