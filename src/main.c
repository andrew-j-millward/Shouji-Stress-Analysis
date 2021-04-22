/* Include Files */
#include "main.h"
#include "stdio.h" 
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <immintrin.h>

char* simulateErrorRead(char ReadSeq[], double ErrorFrequency, int ReadLength) {
    double rand_tmp = 0;
    //printf("Out1: %s %lf\n", ReadSeq, ErrorFrequency);
    for (int i=0; i<ReadLength; i++) {
        double rand_tmp = ((double)(rand()%10000))/10000;
        if (rand_tmp <= ErrorFrequency) {
            int rand_adj = rand()%4;
            //printf("change %d %d %c\n", i, rand_adj, ReadSeq[i]);
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
    //printf("Out2: %s %lf %lf\n", ReadSeq, ErrorFrequency, rand_tmp);
    return ReadSeq;
}

char* simulateSizeMismatch(char ReadSeq[], double ErrorFrequency, int ReadLength) {
    int rand_tmp = 0;
    int shift = ErrorFrequency*ReadLength;
    //printf("Out1: %s %lf\n", ReadSeq, ErrorFrequency);
    for (int i=0; i<ReadLength; i++) {
        int rand_tmp = rand()%2; // Beginning or end

        // Delete shift beginning characters (X)
        if (rand_tmp == 0) {
            for (int j=0; j<shift; j++) {
                ReadSeq[j] = 'X';
            }
        }

        // Delete shift end characters (X)
        else if (rand_tmp == 1) {
            for (int j=0; j<shift; j++) {
                ReadSeq[ReadLength-j-1] = 'X';
            }
        }
    }
    //printf("Out2: %s %lf %lf\n", ReadSeq, ErrorFrequency, rand_tmp);
    return ReadSeq;
}

int main(int argc, const char * const argv[]) {

	if (argc!=6){
		printf("missing argument..\n./main [DebugMode] [GridSize] [ReadLength] [ReadFile] [# of reads]\n");
		exit(-1);
	}

	int DebugMode=atoi(argv[1]);
	int GridSize=atoi(argv[2]);
	int ReadLength = atoi(argv[3]); 

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
    int max_error_threshold = 5;

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
                for (i = 1; i <= atoi(argv[5]); i++) {
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

                    strcpy(ReadSeq, simulateSizeMismatch(ReadSeq, ErrorFrequency, ReadLength));
                    //strcpy(ReadSeq, simulateErrorRead(ReadSeq, ErrorFrequency, ReadLength));

                    begin1 = clock();
                    Accepted1 = Shouji(ReadLength, RefSeq, ReadSeq, ErrorThreshold, GridSize, DebugMode);
                    /*if (Accepted1==1){
                        EdlibAlignResult resultEdlib1 = edlibAlign(RefSeq, ReadLength, ReadSeq, ReadLength, edlibNewAlignConfig(-1, EDLIB_MODE_NW, EDLIB_TASK_PATH, NULL, 0)); // with alignment
                        edlibFreeAlignResult(resultEdlib1);
                    }*/
                    end1 = clock();

                    
                    
                    /////////////////////////////////////////////////////////////////////////////////////////////////
                    /////////////////////////////////////////////////////////////////////////////////////////////////
                    /////////////////////////////////////////////////////////////////////////////////////////////////


                    //NWAccepted = Accepted8;

                    if (Accepted1==0  ){//&& NWAccepted==1
                        FN1++;
                        //SlidingWindow(ReadLength, RefSeq, ReadSeq, ErrorThreshold, GridSize, 1);
                    }
                    else if (Accepted1==1 ){//&& NWAccepted==0){
                        FP1++;		
                    }

                    time1 = time1 + (end1 - begin1);
                }

                time_spent1 = (double)time1 / CLOCKS_PER_SEC;

                //printf("Fastest implementation of Myers’s bit-vector algorithm (Edlib 2017):\n");
                //printf("CPU Time(seconds): %5.4f,    Accepted Mapping: %d,    Rejected Mapping: %d\n", time_spent8, FP8,FN8);
                //printf("----------------------------------------------------------------- \n");
                //printf("Filter Name \t    CPU Time(seconds) \t\t FPs# \t FNs# \n");
                printf(" %d \t\t %d \t\t %5.4f \t %10d \t\t\t %d\n", ErrorThreshold, innerLoopPar, time_spent1, FP1,FN1);


                fclose(fp);
            }
        }
	}
	return 0;
}
