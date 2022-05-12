/* Authors: Thomas B Mavropoulos and Sigve B Heggedal */

/*  ------
     Libs 
    ------ */

//DRS-related
#include <unistd.h> 
#include <ctype.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "strlcpy.h"
#include "DRS.h"

#include <stdio.h>      /* fprint */
#include <string.h>
#include <stdlib.h>
#include <iostream>

#include <iterator>
#include <math.h>
#include <getopt.h>     /* getopt API */
#include <chrono>       /* Time-related */
#include <array>

/*  -------------------
     Define directives 
    ------------------- */ 
#define DIR_SEPARATOR '/'
#define PROGRAM_NAME "drs4client"

/*  ------------------
     Global variables 
    ------------------ */

// Argument options
int argFreq = 5;        // Sampling frequency
int argRange = 0;       // Input range, -0.5 - 0.5 [V]
int argTrigLvl = 0.02;  // Trigger level [V]
int argTrigDelay = 250; // Trigger delay [ns]
int argNumb = 0;        // Numbered run
int argTime = 0;        // Timed run
bool argIgnore = false; // Do not save waveforms
//char* argFile = "data.txt"; //TODO Filename handling

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
    {"ignore-wave", no_argument, NULL, 'w'},
    {"help", no_argument, NULL, 'h'},
    {NULL, 0, NULL, 0}
};

/* ---------
    Methods
   --------- */ 
void runBoardTime(DRSBoard* &b, std::chrono::seconds sekund, FILE* &f, int &c); // Runs board for given amount of time
void runNumbEvents(DRSBoard* &b, int events, FILE* &f, std::chrono::steady_clock::time_point starttime); // Runs board for given amount of events
int initScan(DRS* &drs, DRSBoard* &b); // Checks for board and initializes it 
void showUsage(int status); // Shows help screen with options
void processArgs(int argc, char **argv); // Handles cli arguments
void printArgs(); // Prints the given cli arguments

using namespace std;

int main(int argc, char **argv){
    processArgs(argc, argv); // Processing arguments from cli

    // Initializing board
    DRS *drs; //Boards
    DRSBoard *b; //Board
    drs = new DRS();
    if (initScan(drs, b) == EXIT_FAILURE) return EXIT_FAILURE; // Initialize board, exits if no boards found. 

    printArgs();

    // Run board for a given time or number of events
    if (argTime > 0 && argNumb == 0) {
    // Opens/creates file to save waveform 
    FILE* f;
    f = fopen("data.txt", "w");
    if (f == NULL) {
        perror("Error writing to file");
        delete drs;
        return EXIT_FAILURE;
    }
        // Runs for amount of time in seconds 
        std::chrono::seconds sekunder(argTime); //TODO Implement more complex time handling, not just seconds
        int countEvents = 1; // Counts number of events

        cout << "Starting timed capture...\n"; 
        auto start = std::chrono::steady_clock::now();
        runBoardTime(b, sekunder, f, countEvents);
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        cout << "\nTotal elapsed time: " << elapsed_seconds.count() << "s\n";

        fclose(f);
        delete drs;

        //TODO Python plot magic here, probably?

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

        // Time counter
        std::chrono::steady_clock::time_point starttime = std::chrono::steady_clock::now();
        // repeat argNumb times 
        for (int j=0 ; j<argNumb; j++) {
            runNumbEvents(b, j, f, starttime);
        }

        cout << endl;

        fclose(f);
        delete drs;

        return EXIT_SUCCESS;
    }
    else {
        cout << ("User must choose to either run based on an ammount of time or number of events. Exiting.\n");
        delete drs;
        return(EXIT_FAILURE);
    }
}

// Returns temperature passed from python script
string measureTemp(){
    array<char, 16> buffer;
    string temp;
    FILE* pipe = popen("python3 readtemp.py 2>&1", "r");
    if (!pipe)
    {
        return "Temp error";
    }
    while (fgets(buffer.data(), 16, pipe) != NULL) {
        temp += buffer.data();
    }
    pclose(pipe);

    return temp;
}

// parses cli argument input to correct time format
void parseTimeArgumet() { //TODO Implement
}

void printArgs(){
    printf("Sampling frequency set to %d GHz, input range %d V, trigger level %d V, trigger delay %d ns. \n", 
    argFreq, argRange, argTrigLvl, argTrigDelay);
    if (argTime > 0) printf("Time-based capture set to %d seconds. ", argTime);
    if (argNumb > 0) printf("Event-based capture set to %d events. ", argNumb);
    if (argIgnore) {
        printf("Capturing waveforms turned off.\n");
    }
    else {
        printf("Capturing waveforms turned on.\n");
    }
}

// Simply prints help screen to console
void showUsage(int status){
    if (status != EXIT_SUCCESS) {
        cout << "Show Usage failure: Nothing to see here.";
        exit(status);
    }
    else {
    printf(("Usage: %s [OPTION] \n"), PROGRAM_NAME); //... [FILE]...\n"), PROGRAM_NAME);
    fputs(("Command-line application for timed data acqusition with DRS4 EB.\n"), stdout); 

    fputs(("\
    \n\
    -f, --sampling-frequency Set sampling frequency. Default: 5 [GHz]\n\
    -i, --input-range        input range, ex. input 0 means -0.5 to 0.5. Default: 0\n\
    -l, --trigger-level      Set trigger level in volts. Default: 0.02 [V]\n\
    -d, --trigger-delay      Set trigger delay in ns. Default: 250 [ns] \n\
    -n, --numbered           Run for given number of events. Default: 0\n\
    -t, --timed              Run for given amount of seconds. Default: 0\n\
    -s, --save-to            Define filename (and directory) to store data. Default: data.txt \n\
    -w, --ignore-wave        Do not save waveforms, only time and temp. Default: False \n\
    -h, --help               Shows this help message\n\
    "), stdout);
    printf(("\
            \n\
            Examples:\n\
            %s -t259200      Records events for 72 hours.\n\
            %s -n50 -w       Records 50 events without waveform data.\n\
            "), PROGRAM_NAME, PROGRAM_NAME); 
    }
    cout << endl;
    exit(status);
} 

// Parse command line options 
void processArgs(int argc, char **argv){

    int c;
    while ((c = getopt_long (argc, argv, "f::i::l::d::n::t::wh", long_options, NULL)) != -1) 
    {
        switch (c)
        {
        case 'f': //Set frequency
            argFreq = atoi(optarg);
            break;

        case 'i': //Input range
            argRange = atoi(optarg);
            break;

        case 'l': //Trigger level
            argTrigLvl = atoi(optarg);
            break;

        case 'd': //Trigger delay
            argTrigDelay = atoi(optarg);
            break;

        case 'n': //Numbered run
            argNumb = atoi(optarg);
            break;

        case 't': //Timed run
            argTime = atoi(optarg);
            break;

        /*case 's': //Save-to
            printf("Option s has arg: %s\n", optarg ? optarg : "(none)");
            //argFile = optarg; //TODO Fix this, maybe make a separate method to handle properly
            break;*/

        case 'w': //Ignore waveform
            argIgnore = true;
            break;

        default:
            showUsage(EXIT_SUCCESS);
        }
    }
}

// Board collects data for given amount of events
void runNumbEvents(DRSBoard* &b, int events, FILE* &f, std::chrono::steady_clock::time_point starttime){
    float time_array[8][1024]; // Waveform data, time axis
    float wave_array[8][1024]; // Waveform data, amplitude

    //start board (activate domino wave)
    b->StartDomino();

    //Wait for trigger.
    fflush(stdout);
    while (b->IsBusy());

    //read all waveforms
    b->TransferWaves(0, 8);

    //read time (X) array of first channel in ns
    b->GetTime(0, 0, b->GetTriggerCell(0), time_array[0]);

    //decode waveform (Y) array of first channel in mV
    b->GetWave(0, 0, wave_array[0]);

    double millisec = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - starttime).count();

    //Save data
    fprintf(f, "Event #%d, Temp:%.6s, Time: %.1f ms\n", events+1, measureTemp().c_str(), millisec);
    if (!argIgnore) {
        fprintf(f, "t[ns],u[mV]\n");
        for (int i = 0; i < 1024; i++){
            fprintf(f, "%.3f,%.1f\n", time_array[0][i], wave_array[0][i]);
        }
    }
    //print some progress indication
    printf("\rEvent #%d read successfully", events);
}

// Board collects data for given amount of time
void runBoardTime(DRSBoard* &b, std::chrono::seconds sekund, FILE* &f, int &counter) {

    float time_array[8][1024]; // Waveform data, time axis
    float wave_array[8][1024]; // Waveform data, amplitude

    std::chrono::steady_clock::time_point starttime = std::chrono::steady_clock::now();
    std::chrono::time_point<std::chrono::system_clock> begin = std::chrono::system_clock::now();
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now() + sekund; // Timed end-point

    while (std::chrono::system_clock::now() < end) { // While current time is still less than defined time end-point

        // Start board (activate domino wave) 
        b->StartDomino();

        // Wait for trigger. (This is where the small drift between given run time and actual run time is introduced.)
        fflush(stdout);
        while (b->IsBusy());

        // read all waveforms 
        b->TransferWaves(0, 8);

        // read time (X) array of first channel in ns 
        b->GetTime(0, 0, b->GetTriggerCell(0), time_array[0]);

        // decode waveform (Y) array of first channel in mV 
        b->GetWave(0, 0, wave_array[0]);

        double millisec = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - starttime).count();

        //Save data
        fprintf(f, "Event #%d, Temp: %.6s, Time: %.1f ms\n", counter, measureTemp().c_str(), millisec); 
        if (!argIgnore) {
            fprintf(f, "t[ns],u[mV]\n");
            for (int i = 0; i < 1024; i++){
                fprintf(f, "%.3f,%.1f\n", time_array[0][i], wave_array[0][i]);
            }
        }
        std::chrono::duration<double> elapsed = std::chrono::system_clock::now() - begin;
        printf("\rRunning for %.0f seconds", elapsed.count());

    counter++;
    }
}

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
    b->SetFrequency(argFreq, true);

    // enable transparent mode needed for analog trigger
    b->SetTranspMode(1);

    //set input range to given value, +/- 0.5. Ex. 0 gives a range from -0.5 to 0.5
    b->SetInputRange(argRange);

    // The following lines enables hardware trigger on CH1 at given voltage positive edge
    if (b->GetBoardType() >= 8) {        // Evaluaiton Board V4&5
        b->EnableTrigger(1, 0);          // enable hardware trigger
        b->SetTriggerSource(1<<0);       // set CH1 as source
    } else if (b->GetBoardType() == 7) { // Evaluation Board V3
        b->EnableTrigger(0, 1);          // lemo off, analog trigger on
        b->SetTriggerSource(0);          // use CH1 as source
    }
    b->SetTriggerLevel(argTrigLvl);      // Trigger level. Defualt: 0.05 V
    b->SetTriggerPolarity(false);        // positive edge

    // set trigger delay. Default: zero ns trigger delay
    b->SetTriggerDelayNs(argTrigDelay);
    return EXIT_SUCCESS;
}
