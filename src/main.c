/* Include Files */
#include "main.h"
#include "stdio.h" 
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <immintrin.h>

char* simulateErrorRead(char ReadSeq[], double ErrorFrequency, int ReadLength, int DebugMode) {
    
    if (DebugMode) {
        printf("Simulating Randomized Read Errors:\nStart: \t%s\n", ReadSeq);
    }
    
    double rand_tmp = 0;
    for (int i=0; i<ReadLength; i++) {
        double rand_tmp = ((double)(rand()%10000))/10000;
        if (rand_tmp <= ErrorFrequency) {
            int rand_adj = rand()%4;
            if (rand_adj == 0) {
                ReadSeq[i] = 'A';
            }
            else if (rand_adj == 1) {
                ReadSeq[i] = 'C';
            }
            else if (rand_adj == 2) {
                ReadSeq[i] = 'G';
            }
            else if (rand_adj == 3) {
                ReadSeq[i] = 'T';
            }
        }
    }

    if (DebugMode) {
        printf("End: \t%s\n", ReadSeq);
    }

    return ReadSeq;
}

char* simulateSizeMismatch(char ReadSeq[], double ErrorFrequency, int ReadLength, int DebugMode) {

    if (DebugMode) {
        printf("Simulating String Size Mismatch:\nStart: \t%s\n", ReadSeq);
    }

    int shift = ErrorFrequency*ReadLength;

    int rand_tmp = rand()%2; // Beginning or end

    // Delete shift beginning characters (X)
    if (rand_tmp == 0) {
        for (int i=0; i<shift; i++) {
            ReadSeq[i] = 'X';
        }
    }

    // Delete shift end characters (X)
    else if (rand_tmp == 1) {
        for (int i=0; i<shift; i++) {
            ReadSeq[ReadLength-i-1] = 'X';
        }
    }

    if (DebugMode) {
        printf("End: \t%s\n", ReadSeq);
    }

    return ReadSeq;
}

char* simulateGap(char ReadSeq[], double ErrorFrequency, int ReadLength, int DebugMode) {

    if (DebugMode) {
        printf("Simulating Gap Insertion for Unknown Characters:\nStart: \t%s\n", ReadSeq);
    }

    double rand_tmp = 0;
    for (int i=0; i<ReadLength; i++) {
        double rand_tmp = ((double)(rand()%10000))/10000;
        if (rand_tmp <= ErrorFrequency) {
            int rand_adj = rand()%4;
            ReadSeq[i] = 'X';
        }
    }

    if (DebugMode) {
        printf("End: \t%s\n", ReadSeq);
    }
    
    return ReadSeq;
}

int main(int argc, const char * const argv[]) {

	if (argc!=7){
		printf("missing argument..\n./main [DebugMode] [GridSize] [ReadLength] [ReadFile] [# of reads] [TestingScheme]\n");
		exit(-1);
	}

	int DebugMode=atoi(argv[1]);
	int GridSize=atoi(argv[2]);
	int ReadLength = atoi(argv[3]); 
    int NumberReads = atoi(argv[5]);

    /*
     * Testing Schemes:
     * 0 - No tests
     * 1 - Erroneous Reads (Character Misreads)
     * 2 - String Size Mismatch
     * 3 - Gap Insertion Errors
     * 4 - Erroneous Reads -> String Size Mismatch
     * 5 - Erroneous Reads -> Gap Insertion Errors
     * 6 - String Size Mismatch -> Gap Insertion Errors
     * 7 - Erroneous Reads -> String Size Mismatch -> Gap Insertion Errors
    */
    int TestingScheme = atoi(argv[6]);

	int n;
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	char *p;
	int j,i;
	int loopPar;
    int innerLoopPar;
	char RefSeq[ReadLength] ;
	char ReadSeq[ReadLength];
	int ErrorThreshold;
    double ErrorFrequency;
	int Accepted1;
	int FP1;
	int FN1;
	clock_t begin1;
	clock_t end1;
	double time1;
	double time_spent1;
    int min_error_threshold = 0; // Percent
    int max_error_threshold = 15;
    int acc_baseline = 0;
    int rej_baseline = 0;

    int *baselineAcceptDict = (int *) calloc(NumberReads, sizeof(int));
    int *alternateAcceptDict = (int *) calloc(NumberReads, sizeof(int));

    printf("Edit Distance \t Read Error \t CPU Time(seconds) \t Alignment_Needed \t Not_Needed \n");
	printf("Threshold \t Frequency \n");
	for (loopPar=0; loopPar<=10; loopPar++) {
        for (innerLoopPar=min_error_threshold; innerLoopPar<=max_error_threshold; innerLoopPar++) {
            ErrorThreshold=(loopPar*ReadLength)/100;
            ErrorFrequency=(double)innerLoopPar/(double)100;
            //printf("\n<-------------------Levenshtein Distance = %d------------------->\n", ErrorThreshold);

            FP1=0;
            FN1=0;
            time1= 0;
            //fp = fopen("/home/alser-xilinx/Desktop/Filters_29_11_2016/ReadRef_39999914_pairs_ERR240727_1_with_NW_2017.fastq", "r");

            fp = fopen(argv[4], "r");
            if (!fp){
                printf("Sorry, the file does not exist or you do not have access permission");
            }
            else {
                for (i = 1; i <= NumberReads; i++) {
                    read = getline(&line, &len, fp);
                    j=1;
                    for (p = strtok(line, "\t"); p != NULL; p = strtok(NULL, "\t")) {
                        if (j==1){
                            for (n = 0; n < ReadLength; n++) {
                                ReadSeq[n]= p[n];
                                //printf("%c",ReadSeq[n]);
                            }
                            //printf(" ");
                        }
                        else if (j==2){
                            for (n = 0; n < ReadLength; n++) {
                                RefSeq[n]= p[n];
                                //printf("%c",RefSeq[n]);
                            }
                            //printf("\n");
                        }
                        j=j+1;
                    }		  

                    if (TestingScheme == 1 || TestingScheme == 4 || TestingScheme == 5 || TestingScheme == 7) {
                        strcpy(ReadSeq, simulateErrorRead(ReadSeq, ErrorFrequency, ReadLength, DebugMode));
                    }
                    
                    if (TestingScheme == 2 || TestingScheme == 4 || TestingScheme == 6 || TestingScheme == 7) {
                        strcpy(ReadSeq, simulateSizeMismatch(ReadSeq, ErrorFrequency, ReadLength, DebugMode));
                    }

                    if (TestingScheme == 3 || TestingScheme == 5 || TestingScheme == 6 || TestingScheme == 7) {
                        strcpy(ReadSeq, simulateGap(ReadSeq, ErrorFrequency, ReadLength, DebugMode));
                    }

                    begin1 = clock();
                    Accepted1 = Shouji(ReadLength, RefSeq, ReadSeq, ErrorThreshold, GridSize, DebugMode);
                    /*if (Accepted1==1){
                        EdlibAlignResult resultEdlib1 = edlibAlign(RefSeq, ReadLength, ReadSeq, ReadLength, edlibNewAlignConfig(-1, EDLIB_MODE_NW, EDLIB_TASK_PATH, NULL, 0)); // with alignment
                        edlibFreeAlignResult(resultEdlib1);
                    }*/
                    end1 = clock();

                    //NWAccepted = Accepted8;

                    if (Accepted1==0  ){//&& NWAccepted==1
                        FN1++;
                        //SlidingWindow(ReadLength, RefSeq, ReadSeq, ErrorThreshold, GridSize, 1);
                    }
                    else if (Accepted1==1 ){//&& NWAccepted==0){
                        FP1++;		
                    }

                    time1 = time1 + (end1 - begin1);

                    if (innerLoopPar == 0) {
                        baselineAcceptDict[i] = Accepted1;
                    }
                    else {
                        alternateAcceptDict[i] = Accepted1;
                    }
                }

                time_spent1 = (double)time1 / CLOCKS_PER_SEC;

                if (innerLoopPar != 0) {
                    for (i=0; i<NumberReads; i++) {
                        if(alternateAcceptDict[i] == baselineAcceptDict[i]) {
                            acc_baseline++;
                        }
                        else {
                            rej_baseline++;
                        }
                    }
                    printf(" %d \t\t %d \t\t %5.4f \t %10d \t\t\t %d %d %d\n", ErrorThreshold, innerLoopPar, time_spent1, FP1,FN1, acc_baseline, rej_baseline);
                }
                else {
                    printf(" %d \t\t %d \t\t %5.4f \t %10d \t\t\t %d\n", ErrorThreshold, innerLoopPar, time_spent1, FP1,FN1);
                }
                acc_baseline = 0;
                rej_baseline = 0;

                //printf("Fastest implementation of Myers’s bit-vector algorithm (Edlib 2017):\n");
                //printf("CPU Time(seconds): %5.4f,    Accepted Mapping: %d,    Rejected Mapping: %d\n", time_spent8, FP8,FN8);
                //printf("----------------------------------------------------------------- \n");
                //printf("Filter Name \t    CPU Time(seconds) \t\t FPs# \t FNs# \n");
                //printf(" %d \t\t %d \t\t %5.4f \t %10d \t\t\t %d\n", ErrorThreshold, innerLoopPar, time_spent1, FP1,FN1);


                fclose(fp);
            }
        }
	}
	return 0;
}
