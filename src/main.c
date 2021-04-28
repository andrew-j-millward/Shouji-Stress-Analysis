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
        rand_tmp = ((double)(rand()%10000))/10000;
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
        rand_tmp = ((double)(rand()%10000))/10000;
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

char* simulateCharacterInsertion(char ReadSeq[], double ErrorFrequency, int ReadLength, int DebugMode) {

    if (DebugMode) {
        printf("Simulating Random Character Insertion:\nStart: \t%s\n", ReadSeq);
    }

    char tmpRead[ReadLength];
    strcpy(tmpRead, ReadSeq);
    int insertions = 0;

    double rand_tmp = 0;
    for (int i=0; i<ReadLength; i++) {
        rand_tmp = ((double)(rand()%10000))/10000;
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
            insertions++;
        }
        else {
            ReadSeq[i] = tmpRead[i-insertions];
        }
    }

    if (DebugMode) {
        printf("End: \t%s\n", ReadSeq);
    }
    
    return ReadSeq;
}

char* simulateCharacterDeletion(char ReadSeq[], double ErrorFrequency, int ReadLength, int DebugMode) {

    if (DebugMode) {
        printf("Simulating Random Character Deletion:\nStart: \t%s\n", ReadSeq);
    }

    char tmpRead[ReadLength];
    strcpy(tmpRead, ReadSeq);
    int deletions = 0;

    double rand_tmp = 0;
    for (int i=0; i<ReadLength; i++) {
        rand_tmp = ((double)(rand()%10000))/10000;
        if (rand_tmp <= ErrorFrequency) {
            deletions++;
        }
        else {
            ReadSeq[i-deletions] = tmpRead[i];
        }
    }

    for (int i=0; i<deletions; i++) {
        ReadSeq[ReadLength-deletions+i] = 'X';
    }

    if (DebugMode) {
        printf("End: \t%s\n", ReadSeq);
    }
    
    return ReadSeq;
}

int main(int argc, const char * const argv[]) {

	if (argc!=8){
		printf("missing argument..\n./main [DebugMode] [GridSize] [ReadLength] [ReadFile] [# of reads] [TestingScheme]\n");
		exit(-1);
	}

	int DebugMode=atoi(argv[1]);
	int GridSize=atoi(argv[2]);
	int ReadLength = atoi(argv[3]); 
    int NumberReads = atoi(argv[5]);

    /*
     * Testing Schemes:
     * 0  - No tests
     * 1  - Substitution Error
     * 2  - Insertion Error
     * 3  - Deletion Error
     * 4  - String Size Mismatch
     * 5  - Gap Insertion Errors
     * 6  - Substitution Error -> Insertion Error
     * 7  - Substitution Error -> Deletion Error
     * 8  - Substitution Error -> String Size Mismatch
     * 9  - Substitution Error -> Gap Insertion Errors
     * 10 - Insertion Error -> Deletion Error
     * 11 - Insertion Error -> String Size Mismatch
     * 12 - Insertion Error -> Gap Insertion Errors
     * 13 - Deletion Error -> String Size Mismatch
     * 14 - Deletion Error -> Gap Insertion Errors
     * 15 - String Size Mismatch -> Gap Insertion Errors
     * 16 - Substitution Error -> Insertion Error -> Deletion Error
     * 17 - Substitution Error -> Insertion Error -> String Size Mismatch
     * 18 - Substitution Error -> Insertion Error -> Gap Insertion Errors
     * 19 - Substitution Error -> Deletion Error -> String Size Mismatch
     * 20 - Substitution Error -> Deletion Error -> Gap Insertion Errors
     * 21 - Substitution Error -> String Size Mismatch -> Gap Insertion Errors
     * 22 - Insertion Error -> Deletion Error -> String Size Mismatch
     * 23 - Insertion Error -> Deletion Error -> Gap Insertion Errors
     * 24 - Insertion Error -> String Size Mismatch -> Gap Insertion Errors
     * 25 - Insertion Error -> Deletion Error -> String Size Mismatch
     * 26 - Deletion Error -> String Size Mismatch -> Gap Insertion Errors
     * 27 - Substitution Error -> Insertion Error -> Deletion Error -> String Size Mismatch
     * 28 - Substitution Error -> Insertion Error -> Deletion Error -> Gap Insertion Errors
     * 29 - Substitution Error -> Insertion Error -> String Size Mismatch -> Gap Insertion Errors
     * 30 - Substitution Error -> Deletion Error -> String Size Mismatch -> Gap Insertion Errors
     * 31 - Insertion Error -> Deletion Error -> String Size Mismatch -> Gap Insertion Errors
     * 32 - Substitution Error -> Insertion Error -> Deletion Error -> String Size Mismatch -> Gap Insertion Errors
    */
    int TestingScheme = atoi(argv[6]);

    /*
     * Output configuration:
     * -1 - Report all outputs to CSV
     * 0-25 - Report specific error threshold to CSV for various edit distances
    */
    int OutputConfig = atoi(argv[7]);

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
    int max_error_threshold = 25;
    int acc_baseline = 0;
    int rej_baseline = 0;
    int false_acc = 0;
    int false_rej = 0;
    double fraction_correct = 0;
    double fraction_false_positive = 0;
    double fraction_false_negative = 0;

    int *baselineAcceptDict = (int *) calloc(NumberReads, sizeof(int));
    int *alternateAcceptDict = (int *) calloc(NumberReads, sizeof(int));

    FILE *fp2;
    char *filename = "output.csv";
    fp2 = fopen(filename, "w+");
    fprintf(fp2,"Edit Distance Threshold,Read Error Frequency,CPU Time (seconds),Alignment_Needed,Not_Needed,Correct Alignment,Incorrect Alignment,Fraction Correct,Fraction False Positive,Fraction False Negative\n");

    printf("Edit Distance \tRead Error \tCPU Time(seconds) \tAlignment_Needed \tNot_Needed \tCorrect \tIncorrect \tFraction \tFraction \tFraction\n");
	printf("Threshold \tFrequency \t\t\t\t\t\t\t\t\tAlignment \tAlignment \tCorrect \tFalse Positive \tFalse Negative\n");
	for (loopPar=0; loopPar<=10; loopPar++) {
        for (innerLoopPar=min_error_threshold; innerLoopPar<=max_error_threshold; innerLoopPar++) {
            ErrorThreshold=(loopPar*ReadLength)/100;
            ErrorFrequency=(double)innerLoopPar/(double)1000;
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

                    if (TestingScheme == 1 || (TestingScheme >= 6 && TestingScheme <= 9) || (TestingScheme >= 16 && TestingScheme <= 21) || (TestingScheme >= 27 && TestingScheme <= 30) || TestingScheme == 32) {
                        strcpy(ReadSeq, simulateErrorRead(ReadSeq, ErrorFrequency, ReadLength, DebugMode));
                    }
                    
                    if (TestingScheme == 2 || TestingScheme == 6 || (TestingScheme >= 10 && TestingScheme <= 12) || (TestingScheme >= 16 && TestingScheme <= 18) || (TestingScheme >= 22 && TestingScheme <= 25) || (TestingScheme >= 27 && TestingScheme <= 29) || TestingScheme == 31 || TestingScheme == 32) {
                        strcpy(ReadSeq, simulateCharacterInsertion(ReadSeq, ErrorFrequency, ReadLength, DebugMode));
                    }

                    if (TestingScheme == 3 || TestingScheme == 7 || TestingScheme == 10 || TestingScheme == 13 || TestingScheme == 14 || TestingScheme == 16 || TestingScheme == 19 || TestingScheme == 20 || TestingScheme == 22 || TestingScheme == 23 || (TestingScheme >= 25 && TestingScheme <= 28) || (TestingScheme >= 30 && TestingScheme <= 32)) {
                        strcpy(ReadSeq, simulateCharacterDeletion(ReadSeq, ErrorFrequency, ReadLength, DebugMode));
                    }

                    if (TestingScheme == 4 || TestingScheme == 8 || TestingScheme == 11 || TestingScheme == 13 || TestingScheme == 15 || TestingScheme == 17 || TestingScheme == 19 || TestingScheme == 21 || TestingScheme == 22 || (TestingScheme >= 24 && TestingScheme <= 27) || (TestingScheme >= 29 && TestingScheme <= 32)) {
                        strcpy(ReadSeq, simulateSizeMismatch(ReadSeq, ErrorFrequency, ReadLength, DebugMode));
                    }

                    if (TestingScheme == 5 || TestingScheme == 9 || TestingScheme == 12 || TestingScheme == 14 || TestingScheme == 15 || TestingScheme == 18 || TestingScheme == 20 || TestingScheme == 21 || TestingScheme == 23 || TestingScheme == 24 || TestingScheme == 26 || (TestingScheme >= 28 && TestingScheme <= 32)) {
                        strcpy(ReadSeq, simulateGap(ReadSeq, ErrorFrequency, ReadLength, DebugMode));
                    }

                    /*double rand11 = ((double)(rand()%3000))/3000;
                    if(rand11 == 0) {
                        printf("%s\n", ReadSeq);
                    }*/

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
                        else if(alternateAcceptDict[i] == 0 && baselineAcceptDict[i] == 1) {
                            false_rej++;
                            rej_baseline++;
                        }
                        else if(alternateAcceptDict[i] == 1 && baselineAcceptDict[i] == 0) {
                            false_acc++;
                            rej_baseline++;
                        }
                    }
                    fraction_correct = (double)acc_baseline/NumberReads; 
                    fraction_false_positive = (double)false_acc/NumberReads;
                    fraction_false_negative = (double)false_rej/NumberReads;
                    printf(" %d \t\t %d \t\t %5.4f \t %10d \t\t\t %d \t\t%d \t\t%d \t\t%lf\t%lf\t%lf\n", ErrorThreshold, innerLoopPar, time_spent1, FP1,FN1, acc_baseline, rej_baseline, fraction_correct, fraction_false_positive, fraction_false_negative);
                    if (OutputConfig == innerLoopPar || innerLoopPar == -1) {
                        fprintf(fp2, "%d,%d,%5.4f,%d,%d,%d,%d,%lf,%lf,%lf\n", ErrorThreshold, innerLoopPar, time_spent1, FP1,FN1, acc_baseline, rej_baseline, fraction_correct, fraction_false_positive, fraction_false_negative);
                    }
                }
                else {
                    printf(" %d \t\t %d \t\t %5.4f \t %10d \t\t\t %d \t\t%d \t\t0 \t\t1.000000\t0.000000\t0.000000\n", ErrorThreshold, innerLoopPar, time_spent1, FP1, FN1, NumberReads);
                    if (OutputConfig == innerLoopPar || innerLoopPar == -1) {
                        fprintf(fp2, "%d,%d,%5.4f,%d,%d,%d,0,1.000000,0.000000,0.000000\n", ErrorThreshold, innerLoopPar, time_spent1, FP1, FN1, NumberReads);
                    }
                }
                acc_baseline = 0;
                rej_baseline = 0;
                false_acc = 0;
                false_rej = 0;

                //printf("Fastest implementation of Myers’s bit-vector algorithm (Edlib 2017):\n");
                //printf("CPU Time(seconds): %5.4f,    Accepted Mapping: %d,    Rejected Mapping: %d\n", time_spent8, FP8,FN8);
                //printf("----------------------------------------------------------------- \n");
                //printf("Filter Name \t    CPU Time(seconds) \t\t FPs# \t FNs# \n");
                //printf(" %d \t\t %d \t\t %5.4f \t %10d \t\t\t %d\n", ErrorThreshold, innerLoopPar, time_spent1, FP1,FN1);


                fclose(fp);
            }
        }
	}
    fclose(fp2);
    free(baselineAcceptDict);
    free(alternateAcceptDict);
	return 0;
}
