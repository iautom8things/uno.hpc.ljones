/*

 Author: Theodore
 Version: 2.0
 Created on March 23, 2011, 10:06 AM
 Completed on March 25, 2011, 7.34 AM

FOREWORDS
--------------------------------------------------------------------------------
FUCK YES! I've finally nailed it. Two nights of little sleep were worth it! 
Like Manuel said, the algorithm I described at the meeting would take days for a 
single computer to run, given the input of the magnitude provided in the 
mastermind problem. In fact, I let the program run overnight, and it could not 
complete the next morning. Therefore, that algorithm is only appropriate for 
simpler inputs. In other words, it works correctly but very slowly and 
ineffectively.

The new algorithm used in this version of the program is much more refined. 
It still reuses certain elements from the old algorithm like bitmask, combination 
traversing and so on. The main difference is that it tracks and saves the state 
of every sample level, and uses this info to decide whether we need to check the 
lower sample levels before any calculation for the lower levels is done. I call 
this algorithm shadowing due to the resemblance it has with how sunlight is 
filtered through layers of leaves before it reaches the ground.


ALGORITHM EXPLANATION
--------------------------------------------------------------------------------
Consider a simple example of input:

Level 0: abcdefxd 3
Level 1: wxyabzgm 3
Level 2: obdexmfg 2

Like the old algorithm, the first 3 letters of Level 0 (i.e. 'abc') are first 
selected as a possible combination for this level. However, unlike the old 
algorithm,  'wxy' won't be considered as a possible combination for Level 1 
since 'abc' of Level 0 has already cast a shadow on the first 3 letters of every 
level below it, and claimed that the first three letters must be a, b and c. That 
is, lower levels must conform to this claim.

That leaves Level 1 with only 'abzgm' as possible positions to be picked, and we 
only have to pick 3 out of 5 positions instead of 3 out of 8 positions. Now the 
first possible combination for Level 1 is 'abz.' This combination of Level 1, 
in turn, casts a shadow onto the Level 2. Level 2 is now under the two shadows 
created by Level 0 and Level 1. Let's see how this affects the selection of 
combinations of Level 2.

Since Level 0 has claimed that the first 3 positions must be 'abc' and Level 1 
has just claimed that the next 3 positions must be 'abz,' Level 2 cannot choose 
any of the first 6 positions to be correct UNLESS it has the appropriate letter 
for the position it tries to select. Case in point, the 2nd position of Level 2 
is 'b', which still can be selected as a correct position for this level since it 
still conforms to enforcement defined by Level 0. In fact, the 2nd position, 'b', 
is not only possible to be selected but it also MUST be selected or else 'b' will 
not be a possible letter for this position which creates a conflict with what 
Level 0 defined. In other words, the 2nd position of Level 2 is locked down it as 
a correct position and every combination on this level must include the 2nd 
position as a correct one.

That leaves Level 2 with only one position left to pick. Since the first 6 
positions can't be picked (or some of them are already locked down like the 2nd 
position), we are left with the last two, 'fg'. That is, instead of picking 2 
out of 8 like in the old algorithm, we now narrow it down to pick 1 out of 2.

NOTE: Even positions not picked to be correct cast some sort of shadows onto 
lower level positions and we must track them as well. The above example is only 
concerned with shadows created by correct positions.


ALOGRITHM AND COMPUTATIONAL COMPLEXITY COMPARISION
--------------------------------------------------------------------------------
The example described above could roughly give you an idea of how much faster the 
new algorithm is compared to the old one:

Using the old algorithm we will have to check: 
    C(8, 3) x C(8, 3) x C(8, 2) = 56 x 56 x 28 = 87808 Trials
Using the new algorithm we will have to check: 
    C(8, 3) x C(5, 3) x C(2, 1) = 56 x 10 x 2  =  1120 Trials
    
For the actually input given in the mastermind problem, the old algorithm will 
have to go through 1.68E31 trials versus the rough estimation of 1.14E10 trials 
that the new program would have to go through.

However, the new algorithm will also benefit from the fact that the result of 
possibility calculation is saved on each level and is only recalculated when the 
combination on the level has been changed (In the old program, possibility 
calculation is done on every level every time). The combined effects of these 
improvements allow a single computer to finish the program in just 5 seconds 
(Which would take the old program 1.58E14 years - yes, longer than the age of 
the universe)

The tradeoff of the new algorithm is that it uses three recursive functions that 
call each other to keep track of the state of every level. This makes the program 
a lot more complicated and harder to debug and the program potentially consumes a 
lot memory due to a large stacks of recursive functions. Fortunately, it only 
claimed at most 720KB during several test runs.


    More comments will be added for complex functions, especially the three
    inter-recursive functions: adjustToShadow(), castShadow() and nextComb().
*/

//------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

typedef struct Sample {
    long *shad;       // shadow at the layer of the sample
    long *fltr;       // filter created by fixed positions of the sample

    long *data;       // array of bitmasked letters of the sample
    int   corr;       // number of correct positions = locLen + marLen below

    int  *spots;      // current available marker spots (indices of *data)
    int   spoLen  ;   // number of available marker spots

    int  *locked;     // current locked correct positions (indices of *data)
    int   locLen  ;   // number of rigid correct positions

    int  *markers;    // markers of loose correct positions (indices of *spots)
    int   marLen  ;   // number of markers
} Sam;
typedef Sam *ptSam;


const int BUFF = 128; // Buffer size for each line of input
const int ALPH =  26; // Alphabet size
#define ORIGIN (1 << ALPH) - 1 // Bitmask of all 1's (i.e. 1111111111...111)
long *ground = NULL;    // The shadow cast onto the ground by the lowest sample
Sam  *samples = NULL;   // array of nonzero samples
char **answers = NULL;  // array of all answers
int samCount;           // number of samples
int ansCount;           // number of answers
int length;             // length of each sample's data (i.e. number of letters)
//------------------------------------------------------------------------------

//* Extract a bitmask for letters stored in it and return a string of these letters
char* demask(long bitmask, int is4show) {
    int position = 0;
    int count = 0;
    char *str = (char *) malloc(sizeof(char) * (ALPH + 1));
    while(bitmask != 0) {
        if(bitmask % 2 == 1)
            str[count++] = 'a' + position;
        else if(is4show)
            str[count++] = ' ';
        bitmask >>= 1;
        position++;
    }
    str[count] = '\0';
    return str;
} //----------------------------------------------------------------------------


//* Bubble sort by the descending order of the number of correct positions in
// each sample. This helps higher levels cast bigger shadows, which eliminate
// impossible combination on lower levels faster.
void sortData() {
    int i, j;
    Sam temp;
    for(i = 0; i < samCount - 1; i++){
        for(j = i + 1; j < samCount; j++) {
            if(samples[i].corr < samples[j].corr) {
                temp = samples[i];
                samples[i] = samples[j];
                samples[j] = temp;
            }
        }
    }
} //----------------------------------------------------------------------------


//* Read input from a text file. Smaller steps are described inside the function
int readData(char *filename) {
    int i;
    char line[BUFF];        // line buffer for reading from a text file
    char data[BUFF];        // current sample's data

    int corr;               // number of correct position
    int lgth;               // length of current sample
    Sam *sam;               // temporary sample
    long *firstShd = NULL;  // shadow of the first line of sample

    // reset global data
    free(ground); ground = NULL;
    free(samples); samples = NULL;
    free(answers); answers = NULL;
    samCount = ansCount = length = 0;

    // open file for reading
    FILE *fp = fopen(filename, "r");
    if(fp == NULL)
        return 0;

    // For each non-null line found:
    while(fgets(line, BUFF, fp) != NULL) {
        // Scan and check if it has two valid tokens (a string and a number)
        if(sscanf(line, "%s %d", data, &corr) != 2)
            continue;
        lgth = strlen(data);    // extract the length of the data
        // Use the length of the first valid sample as the common length
        if(length == 0 && lgth >= corr) {
            length = lgth;
            // Once the common length is defined, allocate for the ground shadow
            // and initiate values for the first shadow.
            firstShd = (long *) malloc(sizeof(long) * length);
            ground  = (long *) malloc(sizeof(long) * length);
            for(i = 0; i < length; i++)
                firstShd[i] = ORIGIN;
        }
        // Other samples with invalid or different lenghts are ignored
        else if(lgth != length || lgth < corr)
            continue;
        strlwr(data);    // convert data to lowercase
        // Use zero samples' data to shape the first shadow
        if(corr ==0)
            for(i = 0; i < length; i++)
                firstShd[i] &= ORIGIN - (1 << (data[i] - 'a'));
        // Non-zero samples will be assembled on an array
        else {
            // adjust the array of samples accordingly
            samCount++;
            samples = realloc(samples, sizeof(Sam) * samCount);

            // create a new sample with allocated memory and initial values
            sam = &samples[samCount - 1];
            sam->data = (long *) malloc(sizeof(long) * length);
            sam->shad = (long *) malloc(sizeof(long) * length);
            sam->fltr = (long *) malloc(sizeof(long) * length);
            sam->locked = sam->markers = sam->spots = NULL;
            sam->locLen = sam->marLen = sam->spoLen = 0;

            // set values for data and corr
            sam->corr = corr;
            for(i = 0; i < length; i++)
                sam->data[i] = 1 << (data[i] - 'a');
        }
    }
    
    // Sort Data before apply the first shadow
    sortData(); 
    
    // Shadow on the first level is the shadow formed by zero samples
    if(samCount > 0)
        samples[0].shad = firstShd;
    return 1;
} //----------------------------------------------------------------------------

//* Display the data collected.
void dispData() {
    int i, j;
    Sam sam;
    char *str;
    printf("\n Non-zero samples accepted:\n\n");
    for(i = 0; i < samCount; i++) {
        sam = samples[i];
        printf(" Sample %2d:    ", i);
        for(j = 0; j < length; j++)
            printf("%c", ((int)trunc(log2(sam.data[j]))) + 'a');
        printf(" %2d \n", sam.corr);
    }

    printf("\n Shadow cast by zero samples:\n\n        -> ");
    for(i = 0; i < ALPH; i++)
        printf("%c", i + 'a');
        printf(" <-");
    printf("\n           ");
    for(i = 0; i < ALPH; i++)
          printf("-", i + 'a');
    
    for(i = 0; i < length; i++) {
        str = demask(samples[0].shad[i], 1);
        printf("\n col %2d:   %s", i, str);
        free(str);
    }
} //----------------------------------------------------------------------------



int adjustToShadow(int index) {
    int i;
    Sam *sam = &samples[index];
    sam->spoLen = sam->locLen = sam->marLen = 0;

    // Reset filter and adjust setting for the sample at each column
    for(i = 0; i < length; i++) {
        // Set locked positions
        if(sam->shad[i] == sam->data[i]) {
            if(sam->locLen == sam->corr)
                return 0;
            sam->locLen++;
            sam->locked = realloc(sam->locked, sizeof(int) * sam->locLen);
            sam->locked[sam->locLen - 1] = i;
            sam->fltr[i] = sam->data[i];
        // Set spots
        } else if ((sam->shad[i] & sam->data[i]) != 0) {
            sam->spoLen++;
            sam->spots = realloc(sam->spots, sizeof(int) * sam->spoLen);
            sam->spots[sam->spoLen - 1] = i;
            // Set markers
            if(sam->marLen < (sam->corr - sam->locLen)) {
                sam->marLen++;
                sam->markers = realloc(sam->markers, sizeof(int) * sam->marLen);
            }
            sam->fltr[i] = ORIGIN;
        // The rest are incorrect positions
        } else {
            sam->fltr[i] = ORIGIN - sam->data[i];
        }
    }
    if(sam->locLen + sam->marLen != sam->corr)
        return 0;
    for(i = 0; i < sam->marLen; i++)
        sam->markers[i] = sam->marLen - i - 1;
    //dispSamp(" ADJT", index);
    //dispFltr(index);
    //if(index != 0)
        return castShadow(index);
} //----------------------------------------------------------------------------

int castShadow(int index) {
    //dispSamp(" CAST ", index);
    int i;
    int spot;           // index stored in a marked spot
    int next;       // index of the next marker
    Sam sam = samples[index];
    long *shadow;

    // Check if we are trying to cast the last shadow
    if(index == samCount - 1)
        shadow = ground;
    else
        shadow = samples[index+1].shad;

    // Apply filter of fixed positions
    for(i = 0; i < length; i++)
        if((shadow[i] = sam.shad[i] & sam.fltr[i]) == 0)
            return nextComb(index);
       
    // Apply filter of variable positions
    next = sam.marLen - 1;
    for(i = 0; i < sam.spoLen; i++) {
        spot = sam.spots[i];
        // Marked positions
        if(next >= 0 && i == sam.markers[next]) {
            next--;
            if((shadow[spot] = sam.shad[spot] & sam.data[spot]) == 0)
                return nextComb(index);
        // Unmarked positions
        } else
            if((shadow[spot] = sam.shad[spot] & (ORIGIN - sam.data[spot])) == 0)
                return nextComb(index);
    }

    // Return 1 will terminate the recursive call from the root to indicate that
    // the light has reached the ground successfully
    if(index == samCount - 1)
        return 1;
    if(adjustToShadow(index + 1))
        return 1;
    // Else, try the next combination on the same level
    else
        return nextComb(index);
} //----------------------------------------------------------------------------


int nextComb(int index) {
    //dispSamp(" NEXT ", index);
    int i, j;
    int spot;
    int marker;
    Sam *sam = &samples[index];

    // Return a failure flag if there is no spot for markers to fill
    if(sam->spoLen == 0)
        return 0;

    // Loop through the correct-position markers from lowest to higest order
    for(i = 0; i < sam->marLen; i++) {
        // Shift the current marker to the next spot on the right if possible;
        if(sam->markers[i] < sam->spoLen - i - 1) {
            sam->markers[i]++;
            // Lower order markers must adjust to the shifting
            if(i != 0) {
                spot = sam->markers[i] + 1; // the next spot on the right
                marker  = i - 1; // the immediate lower order marker
                for(j = marker; j >= 0; j--)
                    sam->markers[j] = spot++;
            }
            break; // break when the next combination has been set up
        }
    }
    // Return a failure flag when run out of combinations
    if(i == sam->marLen)
        return 0;
    // recursive call to same-level decision-maker function
    return castShadow(index);
} //----------------------------------------------------------------------------

int nextTrial() {
    int i;
    Sam *sam;
    for(i = samCount - 1; i >= 0; i--) {
        sam = &samples[i];
        if(nextComb(i))
            break;
    }
    if(i == -1)
        return 0;
    return 1;
} //----------------------------------------------------------------------------

int calcData() {
    int i, j, offset;
    char *pool[length];
    int count;
    int *index = (int *) calloc(length, sizeof(int));
    char *ans;
    printf("\n\n\n\tCALCULATION STARTED... \n");
    if(adjustToShadow(0) == 0)
        return 0;
    do {
        count = 1;
        for(i = 0; i < length; i++) {
            pool[i] = demask(ground[i], 0);
            count *= strlen(pool[i]);
        }
        offset = ansCount;
        ansCount += count;
        answers = realloc(answers, sizeof(char *) * ansCount);

        for(i = 0; i < count; i++) {
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
    } while(nextTrial());
    free(index);
    printf("\n\t        DONE!");
    return 1;
} //----------------------------------------------------------------------------

int showAnsw() {
    int i;
    printf("\n\n");
    for(i = 0; i < ansCount; i++) {
        printf("\t  %s \n", answers[i]);
    }
    printf("\n\t  Total Found: %d\n\n", ansCount);
} //----------------------------------------------------------------------------

int main(int argc, char** argv) {
    //if(argc == 1)
        //return (EXIT_FAILURE);
    char *readfile = "samples.txt";
    char *writefile = "result.txt";
    readData(readfile);
    dispData();
    calcData();
    showAnsw();

    getch();
    return(EXIT_SUCCESS);
}