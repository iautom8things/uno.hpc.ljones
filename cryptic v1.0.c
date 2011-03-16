/*
 * File:   main.c
 * Author: Theodore
 * Version: 1.0
 *
 * Comments will be added later. I think the program basically works.
 * Scroll down to the main function adjust the input and output files
 * if you want to test this program.
 * (Netbeans has been giving me a hard time automatically accessing console
 * arguments so you will have to change the input/ouput file names in the
 * code for now until I figure out how Netbeans implements it)
 *
 * Note that my algorithm is different than the one we discussed on Mon so
 * it might be tough to try to figure out what I did here without the comments.
 *
 * Created on March 14, 2011, 8:24 PM
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
const int BUFF = 128;
const int ALPH = 26;
long *possDeck = NULL;
int **samples = NULL;
char **answers = NULL;
int samCount, ansCount, length;
long allPoss;

void resetPossDeck(){
	int i;
	for(i = 0; i < length; i++)
		possDeck[i] = allPoss;
}

void measureData(FILE *fp) {
    int i;
    char line[BUFF];
    char temStr[BUFF];
    int temCorr;
    samCount = 0;
    length = 0;
    while(fgets(line, BUFF, fp) != NULL) {
        if(sscanf(line, "%s %d", temStr, &temCorr) != 2)
            continue;
        if(length == 0)
            length = strlen(temStr);
        if(strlen(temStr) != length)
            continue;
        if(temCorr > length)
        	continue;
        samCount++;
    }
    rewind(fp);
    possDeck = (long *) malloc(sizeof(long) * length);
    allPoss = 0;
    for(i = 0; i < ALPH; i++){
		allPoss += 1 << i;
	}
}

void sortData() {
	int i, j;
	int *temp;
	for(i = 0; i < samCount - 1; i++){
		for(j = i + 1; j < samCount; j++) {
			if(samples[i][length] < samples[j][length]) {
				temp = samples[i];
				samples[i] = samples[j];
				samples[j] = temp;
			}
		}
	}
}

void resetComb(int index, int offset, int start) {
    int i;
    int correct = samples[index][length];
    for(i = start; i < correct; i++){
        samples[index][length + 1 + i] = offset++;
    }
}

int nextComb(int index) {
    int i, curr, offset, start;
    int correct = samples[index][length];
    int last = correct + length;
    if(correct == 0)
        return 0;
    for(i = 0; i < correct; i++) {
        curr = last - i;
        if(samples[index][curr] != length - 1 - i) {
            samples[index][curr]++;
            if(curr != last) {
                offset = samples[index][curr] + 1;
                start = curr - length;
                resetComb(index, offset, start);
            }
            break;
        } 
        if(curr == length + 1)
            return 0;
    }
    return 1;
}

void readData(char *filename) {
    int i;
    int count = 0;
    int correct;
    int *value = NULL;
    char valStr[BUFF];
    char curr[BUFF];
    FILE *fp = fopen(filename, "r");
    samCount = 0;
    if(fp == NULL)
        return;
    measureData(fp);
    samples = (int **) malloc(sizeof(int *) * samCount);
    while(fgets(curr, BUFF, fp) != NULL) {
        if(sscanf(curr, "%s %d", valStr, &correct) != 2)
            continue;
        if(strlen(valStr) != length)
            continue;
        if(correct > length)
        	continue;
        value = (int *) malloc(sizeof(int) * (length + 1 + correct));
        for(i = 0; i < length; i++) {
            if(valStr[i] < 'a' || valStr[i] > 'z')
                break;
            value[i] = valStr[i] - 'a';
        }

        if(i != length) {
            free(value);
            continue;
        }

        value[length] = correct;
        samples[count] = value;
        resetComb(count++, 0, 0);
    }
    sortData();
}

void dispData() {
    int i,j, corr;
    for(i = 0; i < samCount; i ++){
        printf("Val: ");
        for(j = 0; j < length; j++)
            printf("%c ", samples[i][j]+'a');
        corr = samples[i][length];
        printf("   Cor: %2d   Pos:", corr);
        for(j = 0; j < corr; j++)
        	printf(" %2d", samples[i][length + 1 + j]);
        printf("\n");
    }
}

int nextTrial() {
    int i;
    for(i = 0; i < samCount; i++) {
        if(nextComb(i))
            break;
        else
            resetComb(i, 0, 0);
    }
    if(i == samCount)
        return 0;
    else
        return 1;
}

void appendChar(char *str, char ch) {
	int len = strlen(str);
	str[len] = ch;
	str[len+1] = '\0';
}

int extractAnswers(){
    char pool[length][ALPH + 1];
    int *index = (int *) calloc(length, sizeof(int));
    char ch;
    char *ans;
    long temp;
    int count, position,offset, i, j;
    int total = 1;
    for(i = 0; i < length; i++){
        count = 0;
        temp = possDeck[i];
        pool[i][0] = '\0';
        while(temp != 0){
            position = trunc(log2(temp));
            ch = position + 'a';
            temp -= 1 << position;
            count++;
            appendChar(pool[i], ch);
        }
        total *= count;
        //printf("i = %d, count = %d\n", i, count);
    }
    offset = ansCount;
    ansCount += total;
    answers = realloc(answers, sizeof(char *) * ansCount);
    //printf("Total answers %d\n", total);

    for(i = 0; i < total; i++) {
        ans = (char *) malloc (sizeof(char) * (length + 1));
        ans[length] = '\0';
        for(j = 0; j < length; j++) {
            ans[j] = pool[j][index[j]];
        }
        answers[offset + i] = ans;
        for(j = 0; j < length; j++) {
            if(index[j] != strlen(pool[j])-1) {
                index[j]++;
                break;
            }
            else {
                index[j] = 0;
            }
        }
    }

    free(index);
    return total;
}

void calcTrial() {
    int i,j, corr, next;
    resetPossDeck();
    for(i = 0; i < samCount; i++) {
        corr = samples[i][length];
        next = length+1;

        for(j = 0; j < length; j++) {
            if(corr != 0 && next <= length + corr && j == samples[i][next]) {
               //printf("%d & %d = %d\n", possDeck[j], 1 << samples[i][j], possDeck[j] & 1 << samples[i][j]);
                possDeck[j] &= 1 << samples[i][j];
                next++;
            } else {
               // printf("%d & %d = %d\n", possDeck[j], allPoss - (1 << samples[i][j]), possDeck[j] & (allPoss - (1 << samples[i][j])));
                possDeck[j] &= allPoss - (1 << samples[i][j]);
            }
            if(possDeck[j] == 0)
                return;
        }
    }

    //for(i = 0; i < length; i++)
        //printf("Poss(%d): %d\n", i, possDeck[i]);

	extractAnswers();

}

void calcData() {
	ansCount = 0;
	do{
		calcTrial();
	} while(nextTrial());
}

void showAnsw() {
    int i;
    printf(" Answers found: %d\n\n", ansCount);
    for(i = 0; i < ansCount; i++) {
        printf(" %s \n", answers[i]);
    }
    printf("\n");
}

void writeData(char *filename) {
    int i;
    FILE *fp = fopen(filename, "w");
    if(fp == NULL)
        return;
    fprintf(fp, " Answers found: %d\n\n", ansCount);
    for(i = 0; i < ansCount; i++) {
        fprintf(fp, "  %s \n", answers[i]);
    }
    printf(" Answers written in \"%s\"\n\n", filename);

}

int main(int argc, char** argv) {
    //if(argc == 1)
        //return (EXIT_FAILURE);

    char *readfile = "samples.txt";
    char *writefile = "result.txt";
    readData(readfile);
    //dispData();
    calcData();
    writeData(writefile);
    showAnsw();


   // getch();
    return(EXIT_SUCCESS);
}

