//
//  main.c
//  P2
//
//  Created by Brian Hornak on 3/22/16.
//  Copyright © 2016 Brian Hornak. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

//int * genCards(int rows, int cols, int max);    //generate a list of pairs of cards

int getUserRow(int guessNum);
int getUserCol(int guessNum);
int didMatch(int card1, int card2);     //did we get a match given the value of the two cards and the player
int didCPUMatch(int card1, int card2);  //same thing, but for CPU

//Write the required information to output file 2
//@param round #, user rowGuess1,2, user colGuess1,2, card at guess 1,2, same for CPU
void writeToOutput(FILE * output, int round, int rguess1, int cguess1, int rguess2, int cguess2, int card1, int card2, int cpu_r1, int cpu_c1, int cpu_r2, int cpu_c2, int card3, int card4);

int main(int argc, const char * argv[]) {
    FILE *output1, *output2;
    
    if(argc != 3){
        printf("Please make sure you've entered in the proper arguements\n");
    }
    
    output1 = fopen(argv[1], "w");
    output2 = fopen(argv[2], "w");
    
    
    int row, col, level, seed, highestNum, totalNumCards;
    char misc;
    //int *listOfCards;
    
    printf("Please enter size of board (e.g. 3x6): ");
    scanf("%d%c%d", &row, &misc, &col);
    printf("Please enter the level of the game (0-9): ");
    scanf("%d", &level);
    printf("Please enter the largest card value (1-99): ");
    scanf("%d", &highestNum);
    printf("Please enter the seed of the random number generator (0-9999): ");
    scanf("%d", &seed);
    
    //Creat first output file
    fprintf(output1, "%dx%d", row, col);
    fprintf(output1, "\n%d",level);
    fprintf(output1, "\n%d", highestNum);
    fprintf(output1, "\n%d", seed);
    fclose(output1);
    
    srand(seed);
    
    totalNumCards = row*col;

    //Get our list of cards to pull from and assign to board
    //listOfCards = genCards(row, col, highestNum);
    
    int numCards = (row*col)/2;     //the number of different cards we need to generate
    int listOfCards[numCards];   //an array holding 1 part of the pairs of cards (i.e. one of the numbers in a pair)
    int dupList[numCards];      //the duplicated cards (so we have pairs)
    int finalList[numCards*2];  //combine the two lists to form a final list containing all pairs
    
    int i;
    
    //Randomly pick numbers from 0 to max (inclusive) and put them in the list
    for(i = 0; i < numCards; i++){
        listOfCards[i] = rand() % (highestNum+1);   //plus 1 is to be inclusive
    }
    
    //Duplicate the list so we have pairs of cards (2 of each card)
    int j;
    for(j = 0; j < numCards; j++){
        dupList[j] = listOfCards[j];
    }
    
    int k;
    for(k = 0; k < numCards; k++){
        finalList[k] = listOfCards[k];
    }
    int g;
    for(g = numCards; g < numCards*2; g++){
        finalList[g] = dupList[numCards*2 - g - 1];
    }
    
    //--------------------END OF CARD GENERATION-------------------//
    
    //---------DEBUG-------------//
    int p;
    printf("List of Cards:\n");
    for(p = 0; p < totalNumCards; p++){
        printf("%d ",finalList[p]);
    }
    printf("\n");
    //---------------------------//
    
    
    
    
    //Initialize board with random numbers
    int board[row][col];
    int r,c;
    int randomRow, randomCol;
    
    for(r = 0; r < row; r++){
        for(c = 0; c < col; c++){
            board[r][c] = -1;
        }
    }
    
    int q;
    for(q = 0; q < row*col; q++){
        randomRow = rand() % row;
        randomCol = rand() % col;
        while(board[randomRow][randomCol] >= 0 && board[randomRow][randomCol] <= highestNum){
            //this spot is taken, we need to pick another one
            randomRow = rand() % row;
            randomCol = rand() % col;
        }
        board[randomRow][randomCol] = finalList[q];
    }
    
    /*
                                Assigning a value to each spot on the board
     -Go through each card in listOfCards, and then pick a random place on the board to assign it. But, only assign the card if there isn't one already at that spot. If there is a card at that spot (i.e. it has a value between 1 and highestNum), then pick another random spot to place the card. Then check again...so on and so forth
     */
    
    //Print board for first time
    int m,n,x;
    printf("   ");
    for(x = 0; x < col; x++){
        printf("%3d",x);
    }
    printf("\n\n");
    for(m = 0; m < row; m++){
        printf("%3d", m);
        for(n = 0; n < col; n++){
            printf("  X");
        }
        printf("\n\n");
    }
    
    //Output original board (with cards up) to output file 2
    fprintf(output2,"   ");
    for(x = 0; x < col; x++){
        fprintf(output2,"%4d",x);
    }
    fprintf(output2,"\n\n");
    for(m = 0; m < row; m++){
        fprintf(output2,"%4d", m);
        for(n = 0; n < col; n++){
            fprintf(output2,"%4d",board[m][n]);
        }
        fprintf(output2,"\n\n");
    }
    fclose(output2);
    
    int gameStillGoing = 1;     //0 when the game is over, 1 while it's still going
    int roundNum = 1;           //the round number
    
    //Establish the computer's memory
    //The CPU needs to be able to memorize the value of flipped cards, and their coordinates
    int **memory;   //a 2d array of pointers (aka a pointer of pointers to ints
    int numCardsCanMemorize = row * col * level / 9;
    
    //Make the memory 3 rows (one for card value, one for corresponding row, one for correspong column
    memory = (int **) malloc(sizeof(int *) * 3);
    int a;
    for(a = 0; a < 3; a++){
        //For each of those 3 arrays, allocate memory for the number of cards the CPU can memorize. So, the card value array can hold numCardsCanMemorize cards, etc.
        memory[a] = (int *)malloc(sizeof(int)*numCardsCanMemorize);
    }
    
    for(a = 0; a < numCardsCanMemorize; a++){
        memory[0][a] = -1;
    }
    for(a = 0; a < numCardsCanMemorize; a++){
        memory[1][a] = -1;
    }
    for(a = 0; a < numCardsCanMemorize; a++){
        memory[2][a] = -1;
    }
    //The current index in the CPU's memory (i.e. where we are storing a value,row, or col in it's respective array) We incrememnt this each time we store something as long as it's not full
    int index = 0;
    
    int numUserMatches = 0;
    int numCPUMatches = 0;
    int rowChoice1;
    int colChoice1;
    int rowChoice2;
    int colChoice2;
    
    
    printf("   ");
    for(x = 0; x < col; x++){
        printf("%3d",x);
    }
    printf("\n\n");
    for(m = 0; m < row; m++){
        printf("%3d", m);
        for(n = 0; n < col; n++){
            printf("%3d",board[m][n]);
        }
        printf("\n\n");
    }
    
    
    while(gameStillGoing == 1){
        /*
         
         ****       DEBUG       ****
         
         */
        printf("Card: ");
        for(m = 0; m < numCardsCanMemorize; m++){
            printf("%d  ", memory[0][m]);
        }
        printf("\nRow: ");
        for(m = 0; m < numCardsCanMemorize; m++){
            printf("%d  ",memory[1][m]);
        }
        printf("\nCol: ");
        for(m = 0; m < numCardsCanMemorize; m++){
            printf("%d  ",memory[2][m]);
        }
        
        
        /*
         ****    END OF DEBUG   ****
         */
        
        //Get user's moves
        printf("\nIt's now your turn...\n");
        
        rowChoice1 = getUserRow(1);
        colChoice1 = getUserCol(1);
        //Display the board, revealing the player's choices
        printf("   ");
        for(x = 0; x < col; x++){
            printf("%3d",x);
        }
        printf("\n\n");
        for(m = 0; m < row; m++){
            printf("%3d", m);
            for(n = 0; n < col; n++){
                if(board[m][n] == -1){
                    printf("   ");
                }
                
                else if(m == rowChoice1 && n == colChoice1){
                    printf("%3d", board[rowChoice1][colChoice1]);
                    //Add that card (and it's location) to CPU memory (IF AVAILABLE)
                    if(index != numCardsCanMemorize){
                        //The memory isn't full, so we can add stuff
                        memory[0][index] = board[rowChoice1][colChoice1];
                        memory[1][index] = rowChoice1;
                        memory[2][index] = colChoice1;
                        index++;
                    }
                }
                else{
                    printf("  X");
                }
            }
            printf("\n\n");
        }
        //Get the other spot to check
        rowChoice2 = getUserRow(2);
        colChoice2 = getUserCol(2);
        if(rowChoice2 == rowChoice1 && colChoice2 == colChoice1){
            rowChoice2 = getUserRow(2);
            colChoice2 = getUserCol(2);
        }
        //Display board with those spaces
        printf("   ");
        for(x = 0; x < col; x++){
            printf("%3d",x);
        }
        printf("\n\n");
        for(m = 0; m < row; m++){
            printf("%3d", m);
            for(n = 0; n < col; n++){
                if(board[m][n] == -1){
                    printf("   ");
                }
                
                else if(m == rowChoice1 && n == colChoice1){
                    printf("%3d", board[rowChoice1][colChoice1]);
                }
                else if(m == rowChoice2 && n == colChoice2){
                    printf("%3d", board[rowChoice2][colChoice2]);
                    //Add that card (and it's location) to CPU memory (IF AVAILABLE), and we haven't already memorized that card
                    if(index != numCardsCanMemorize){
                        //The memory isn't full, so we can add stuff
                        memory[0][index] = board[rowChoice2][colChoice2];
                        memory[1][index] = rowChoice2;
                        memory[2][index] = colChoice2;
                        index++;
                    }
                }
                else{
                    printf("  X");
                }
            }
            printf("\n\n");
        }
        //Now print an updated (or unchanged if the user didn't get a match) board.
        //So first, we check if there was a match
        if(didMatch(board[rowChoice1][colChoice1], board[rowChoice2][colChoice2]) == 1){
            //we got a match!
            //so, "remove" those two cards
            //This happens in two ways:
                //The first is to print those cards as empty spaces
                //The second is to change the value of those cards to -1 (so that we know they're used)
            
            numUserMatches++;
            
            //Get rid of the matched cards from the CPU's memory
            int z, g, f;
            //Card value
            for(z = 0; z < numCardsCanMemorize; z++){
                if(memory[0][z] == board[rowChoice1][colChoice1]){
                    memory[0][z] = memory[0][z+1];  //move everything forward by one to "forget" the matched cards
                }
            }
            //Row
            for(g = 0; g < numCardsCanMemorize; g++){
                if(memory[0][g] == board[rowChoice1][colChoice1]){
                    memory[1][g] = memory[1][g+1];  //move everything forward by one to "forget" the matched cards
                }
            }
            //Column
            for(f = 0; f < numCardsCanMemorize; f++){
                if(memory[0][f] == board[rowChoice1][colChoice1]){
                    memory[2][f] = memory[2][f+1];  //move everything forward by one to "forget" the matched cards
                }
            }
            
            //Change values
            board[rowChoice1][colChoice1] = -1;
            board[rowChoice2][colChoice2] = -1;
            
            
            //print a blank spot in those two locations
            printf("   ");
            for(x = 0; x < col; x++){
                printf("%3d",x);
            }
            printf("\n\n");
            for(m = 0; m < row; m++){
                printf("%3d", m);
                for(n = 0; n < col; n++){
                    if(board[m][n] == -1){
                        printf("   ");
                    }
                    else{
                        printf("  X");
                    }
                }
                printf("\n\n");
            }
        }
        else{
            //we didn't get a match, so print out the board as it was
            printf("   ");
            for(x = 0; x < col; x++){
                printf("%3d",x);
            }
            printf("\n\n");
            for(m = 0; m < row; m++){
                printf("%3d", m);
                for(n = 0; n < col; n++){
                    if(board[m][n] == -1){
                        printf("   ");
                    }
                    else{
                        printf("  X");
                    }
                }
                printf("\n\n");
            }
        }
        
        //Now, the computer moves
        printf("\nIt's now the computer's turn...\n");
        int CPU_RowChoice1 = -1;
        int CPU_ColChoice1 = -1;
        int CPU_RowChoice2 = -1;
        int CPU_ColChoice2 = -1;
        int turnIsOver = 0;
        
        /*
         When making a move, the computer will first look to see if it has any matches in it's memory. If not it chooses a card at random. Then, for it's second guess, it checks to see if anything in it's memory matches the first guess. If not, it chooses randomly, and memorizes both cards (if it has space). If it doesn't have space, it "forgets" the first card.
         
         I will  treat these as 3 cases:
            Case I (Best Case): There is a match already in memory, so just go right to it and flip it over. Do everything within that loop structure (displaying board, updating memory, etc.
            Case II: We don't have any matches in our memory, so randomly choose a card (that we don't already know), then check if we already have a card in our memory that matches that card. Do all updating and stuff in the loop.
            Case III: We don't have any matches, so we just pick two random cards and hope for the best. We have to check for matches after we choose the second card.
         
         Notice that in the first two cases, we are guarenteed to have a match. We know that at some point, there will be a match, so our code can reflect that. In the third case, we need to check for a match (that's the only case where it's necessary to check).
         */
        
        
        /** -----------------------   CASE I   ------------------------**/
        int w, b;
        int noMatches = 1;  //= 0 if we find a match in our memory
        for(w = 0; w < numCardsCanMemorize; w++){
            for(b = w+1; b < numCardsCanMemorize; b++){
                if(memory[0][b] == memory[0][w] && memory[0][b] >= 0 && memory[0][w] >= 0 && turnIsOver == 0){
                    //We've got a match in our memory, flip them
                    CPU_RowChoice1 = memory[1][w];
                    CPU_ColChoice1 = memory[2][w];
                    CPU_RowChoice2 = memory[1][b];
                    CPU_ColChoice2 = memory[2][b];
                    
                    //Display the CPU's first pick
                    printf("   ");
                    for(x = 0; x < col; x++){
                        printf("%3d",x);
                    }
                    printf("\n\n");
                    for(m = 0; m < row; m++){
                        printf("%3d", m);
                        for(n = 0; n < col; n++){
                            if(board[m][n] == -1){
                                printf("   ");
                            }
                            else if(m == CPU_RowChoice1 && n == CPU_ColChoice1){
                                printf("%3d", board[CPU_RowChoice1][CPU_ColChoice1]);
                            }
                            else{
                                printf("  X");
                            }
                        }
                        printf("\n\n");
                    }
                    
                    //Display the CPU's second pick
                    printf("   ");
                    for(x = 0; x < col; x++){
                        printf("%3d",x);
                    }
                    printf("\n\n");
                    for(m = 0; m < row; m++){
                        printf("%3d", m);
                        for(n = 0; n < col; n++){
                            if(board[m][n] == -1){
                                printf("   ");
                            }
                            
                            else if(m == CPU_RowChoice2 && n == CPU_ColChoice2){
                                printf("%3d", board[CPU_RowChoice2][CPU_ColChoice2]);
                            }
                            else{
                                printf("  Q");
                            }
                        }
                        printf("\n\n");
                    }
                    
                    
                    //Forget those cards
                    int z;
                    for(z = 0; z < numCardsCanMemorize; z++){
                        if(memory[0][z] == board[rowChoice1][colChoice1]){
                            memory[0][z] = memory[0][z+1];  //move everything forward by one to "forget" the matched cards
                            memory[1][z] = memory[1][z+1];
                            memory[2][z] = memory[2][z+1];
                        }
                    }
                    
                    //Update the game board
                    board[CPU_RowChoice1][CPU_ColChoice1] = -1;
                    board[CPU_RowChoice2][CPU_ColChoice2] = -1;
                    
                    //print a blank spot in those two locations
                    printf("   ");
                    for(x = 0; x < col; x++){
                        printf("%3d",x);
                    }
                    printf("\n\n");
                    for(m = 0; m < row; m++){
                        printf("%3d", m);
                        for(n = 0; n < col; n++){
                            if(board[m][n] == -1){
                                printf("   ");
                            }
                            else{
                                printf("  W");
                            }
                        }
                        printf("\n\n");
                    }
                    numCPUMatches++;
                    noMatches = 0;
                    turnIsOver = 1;
                    
                }
            }
        }
        
        /** -----------------------   CASE II   ------------------------ **/
        if(turnIsOver == 0){
            //We didn't find any matches, choose a random card as our pick, but one that isn't in our memory already
            CPU_RowChoice1 = rand() % row;
            CPU_ColChoice1 = rand() % col;
            for(m = 0; m < numCardsCanMemorize; m++){
                while(board[CPU_RowChoice1][CPU_ColChoice1] == -1 || (memory[1][m] == CPU_RowChoice1 && memory[2][m] == CPU_ColChoice1)) {
                    //This card has already been removed, pick another
                    CPU_RowChoice1 = rand() % row;
                    CPU_ColChoice1 = rand() % col;
                }
            }
            
            //Display the CPU's first pick
            printf("   ");
            for(x = 0; x < col; x++){
                printf("%3d",x);
            }
            printf("\n\n");
            for(m = 0; m < row; m++){
                printf("%3d", m);
                for(n = 0; n < col; n++){
                    if(board[m][n] == -1){
                        printf("   ");
                    }
                    else if(m == CPU_RowChoice1 && n == CPU_ColChoice1){
                        printf("%3d", board[CPU_RowChoice1][CPU_ColChoice1]);
                    }
                    else{
                        printf("  E");
                    }
                }
                printf("\n\n");
            }
        }
        
        //Now check for a match in our memory
        int i;
        for(i = 0; i < numCardsCanMemorize; i++){
            if(didCPUMatch(board[CPU_RowChoice1][CPU_ColChoice1], memory[0][i] == 1) && turnIsOver == 0){
                //we've got a match - flip it and everything immediately
                CPU_RowChoice2 = memory[1][i];
                CPU_ColChoice2 = memory[2][i];
                
                //Display the CPU's second pick
                printf("   ");
                for(x = 0; x < col; x++){
                    printf("%3d",x);
                }
                printf("\n\n");
                for(m = 0; m < row; m++){
                    printf("%3d", m);
                    for(n = 0; n < col; n++){
                        if(board[m][n] == -1){
                            printf("   ");
                        }
                        
                        else if(m == CPU_RowChoice2 && n == CPU_ColChoice2){
                            printf("%3d", board[CPU_RowChoice2][CPU_ColChoice2]);
                        }
                        else{
                            printf("  R");
                        }
                    }
                    printf("\n\n");
                }
                
                int z;
                for(z = 0; z < numCardsCanMemorize; z++){
                    if(memory[0][z] == board[rowChoice1][colChoice1]){
                        memory[0][z] = memory[0][z+1];  //move everything forward by one to "forget" the matched cards
                        memory[1][z] = memory[1][z+1];
                        memory[2][z] = memory[2][z+1];
                    }
                }
                
                //Update the game board
                board[CPU_RowChoice1][CPU_ColChoice1] = -1;
                board[CPU_RowChoice2][CPU_ColChoice2] = -1;
                
                //print a blank spot in those two locations
                printf("   ");
                for(x = 0; x < col; x++){
                    printf("%3d",x);
                }
                printf("\n\n");
                for(m = 0; m < row; m++){
                    printf("%3d", m);
                    for(n = 0; n < col; n++){
                        if(board[m][n] == -1){
                            printf("   ");
                        }
                        else{
                            printf("  T");
                        }
                    }
                    printf("\n\n");
                }
                numCPUMatches++;
                turnIsOver = 1;
                noMatches = 0;
                break;
            }
        }
        if(noMatches == 1){
            //we don't have any matches in our memory for the first random card, choose another random card (that isn't in our memory and isn't flipped)
            CPU_RowChoice2 = rand() % row;
            CPU_ColChoice2 = rand() % col;
            for(m = 0; m < numCardsCanMemorize; m++){
                while(board[CPU_RowChoice2][CPU_ColChoice2] == -1 || (memory[1][m] == CPU_RowChoice2 && memory[2][m] == CPU_ColChoice2)) {
                    //This card has already been removed, pick another
                    CPU_RowChoice2 = rand() % row;
                    CPU_ColChoice2 = rand() % col;
                }
            }
            //Display the CPU's first pick
            printf("   ");
            for(x = 0; x < col; x++){
                printf("%3d",x);
            }
            printf("\n\n");
            for(m = 0; m < row; m++){
                printf("%3d", m);
                for(n = 0; n < col; n++){
                    if(board[m][n] == -1){
                        printf("   ");
                    }
                    else if(m == CPU_RowChoice1 && n == CPU_ColChoice1){
                        printf("%3d", board[CPU_RowChoice1][CPU_ColChoice1]);
                    }
                    else{
                        printf("  B");
                    }
                }
                printf("\n\n");
            }
            //Display the CPU's second pick
            printf("   ");
            for(x = 0; x < col; x++){
                printf("%3d",x);
            }
            printf("\n\n");
            for(m = 0; m < row; m++){
                printf("%3d", m);
                for(n = 0; n < col; n++){
                    if(board[m][n] == -1){
                        printf("   ");
                    }
                    
                    else if(m == CPU_RowChoice2 && n == CPU_ColChoice2){
                        printf("%3d", board[CPU_RowChoice2][CPU_ColChoice2]);
                    }
                    else{
                        printf("  P");
                    }
                }
                printf("\n\n");
            }
            
            //Now see if we got lucky and our second card is a match
            if(didCPUMatch(board[CPU_RowChoice1][CPU_ColChoice1], board[CPU_RowChoice2][CPU_ColChoice2]) == 1){
                
                numCPUMatches++;
                int z;
                for(z = 0; z < numCardsCanMemorize; z++){
                    if(memory[0][z] == board[rowChoice1][colChoice1]){
                        memory[0][z] = memory[0][z+1];  //move everything forward by one to "forget" the matched cards
                        memory[1][z] = memory[1][z+1];
                        memory[2][z] = memory[2][z+1];
                    }
                }
                
                //Update the game board
                board[CPU_RowChoice1][CPU_ColChoice1] = -1;
                board[CPU_RowChoice2][CPU_ColChoice2] = -1;
                
                //print a blank spot in those two locations
                printf("   ");
                for(x = 0; x < col; x++){
                    printf("%3d",x);
                }
                printf("\n\n");
                for(m = 0; m < row; m++){
                    printf("%3d", m);
                    for(n = 0; n < col; n++){
                        if(board[m][n] == -1){
                            printf("   ");
                        }
                        else{
                            printf("  D");
                        }
                    }
                    printf("\n\n");
                }
            }
            else{
                //We didn't find any matches, so let's memorize these cards (if we have space)
                if(index != numCardsCanMemorize){
                    memory[0][index] = board[CPU_RowChoice1][CPU_ColChoice1];
                    memory[1][index] = CPU_RowChoice1;
                    memory[2][index] = CPU_ColChoice1;
                    index++;
                    memory[0][index] = board[CPU_RowChoice2][CPU_ColChoice2];
                    memory[1][index] = CPU_RowChoice2;
                    memory[2][index] = CPU_ColChoice2;
                    index++;
                }
                else{
                    int i;
                    for(i = 0; i < numCardsCanMemorize; i++){
                        //Shift everything forward by one to "Forget" oldest card and remember new one
                        memory[0][i] = memory[0][i+1];
                        memory[1][i] = memory[1][i+1];
                        memory[2][i] = memory[2][i+1];
                    }
                    memory[0][index] = board[CPU_RowChoice1][CPU_ColChoice1];
                    memory[1][index] = CPU_RowChoice1;
                    memory[2][index] = CPU_ColChoice1;
                    //Now repeat to memorize the second card
                    for(i = 0; i < numCardsCanMemorize; i++){
                        memory[0][i] = memory[0][i+1];
                        memory[1][i] = memory[1][i+1];
                        memory[2][i] = memory[2][i+1];
                    }
                    memory[0][index] = board[CPU_RowChoice2][CPU_ColChoice2];
                    memory[1][index] = CPU_RowChoice2;
                    memory[2][index] = CPU_ColChoice2;
                }
                //we didn't get a match, so print out the board as it was
                printf("   ");
                for(x = 0; x < col; x++){
                    printf("%3d",x);
                }
                printf("\n\n");
                for(m = 0; m < row; m++){
                    printf("%3d", m);
                    for(n = 0; n < col; n++){
                        if(board[m][n] == -1){
                            printf("   ");
                        }
                        else{
                            printf("  A");
                        }
                    }
                    printf("\n\n");
                }
            }
        }
        
        //First, look through our memeory and see if we've got matches
        
//        for(w = 0; w < numCardsCanMemorize; w++){
//            for(b = w+1; b < numCardsCanMemorize; b++){
//                if(memory[0][w] == memory[0][b] && memory[0][w] >= 0){
//                    //We've got two cards that match in our memory, let's flip 'em
//                    CPU_RowChoice1 = memory[1][w];
//                    CPU_ColChoice1 = memory[2][w];
//                    CPU_RowChoice2 = memory[1][b];
//                    CPU_ColChoice2 = memory[2][b];
//                }
//            }
//        }
////        //We didn't find any matches, choose a random card as our pick
////        CPU_RowChoice1 = rand() % row;
////        CPU_ColChoice1 = rand() % col;
////        while(board[CPU_RowChoice1][CPU_ColChoice1] == -1) {
////            //This card has already been removed, pick another
////            CPU_RowChoice1 = rand() % row;
////            CPU_ColChoice1 = rand() % col;
////        }
//        
//        
//        //Display the CPU's first pick
//        printf("   ");
//        for(x = 0; x < col; x++){
//            printf("%3d",x);
//        }
//        printf("\n\n");
//        for(m = 0; m < row; m++){
//            printf("%3d", m);
//            for(n = 0; n < col; n++){
//                if(board[m][n] == -1){
//                    printf("   ");
//                }
//                else if(m == CPU_RowChoice1 && n == CPU_ColChoice1){
//                    printf("%3d", board[CPU_RowChoice1][CPU_ColChoice1]);
//                }
//                else{
//                    printf("  X");
//                }
//            }
//            printf("\n\n");
//        }
//        
//        //Now check if something matches the first card displayed
////        int i;
////        for(i = 0; i < numCardsCanMemorize; i++){
////            if(didCPUMatch(board[CPU_RowChoice1][CPU_ColChoice1], memory[0][i]) == 1){
////                //we've got another card of that value in our memory
////                CPU_RowChoice2 = memory[1][i];
////                CPU_ColChoice2 = memory[2][i];
////            }
////            else{
////                //We don't have a card that matches our random guess, so we must pick another random card
////                CPU_RowChoice2 = rand() % row;
////                CPU_ColChoice2 = rand() % col;
////                while(board[CPU_RowChoice2][CPU_ColChoice2] == -1 && CPU_RowChoice2 != CPU_RowChoice1 && CPU_ColChoice2 != CPU_ColChoice1) {
////                    //This card has already been removed, pick another
////                    CPU_RowChoice2 = rand() % row;
////                    CPU_ColChoice2 = rand() % col;
////                }
////            }
////        }
//        
//        //Display the CPU's second pick
//        printf("   ");
//        for(x = 0; x < col; x++){
//            printf("%3d",x);
//        }
//        printf("\n\n");
//        for(m = 0; m < row; m++){
//            printf("%3d", m);
//            for(n = 0; n < col; n++){
//                if(board[m][n] == -1){
//                    printf("   ");
//                }
//                
//                else if(m == CPU_RowChoice2 && n == CPU_ColChoice2){
//                    printf("%3d", board[CPU_RowChoice2][CPU_ColChoice2]);
//                }
//                else{
//                    printf("  X");
//                }
//            }
//            printf("\n\n");
//        }
        
        //Now that we've officially selected our two cards to guess, we check for matches, and remove them if we've got a match
//        if(didCPUMatch(board[CPU_RowChoice1][CPU_ColChoice1], board[CPU_RowChoice2][CPU_ColChoice2]) == 1){
//            
//            numCPUMatches++;
//            
//            //Get rid of the matched cards from the CPU's memory
//            int z, g, f;
//            //Card value
//            for(z = 0; z < numCardsCanMemorize; z++){
//                if(memory[0][z] == board[rowChoice1][colChoice1]){
//                    memory[0][z] = memory[0][z+1];  //move everything forward by one to "forget" the matched cards
//                }
//            }
//            //Row
//            for(g = 0; g < numCardsCanMemorize; g++){
//                if(memory[0][g] == board[rowChoice1][colChoice1]){
//                    memory[1][g] = memory[1][g+1];  //move everything forward by one to "forget" the matched cards
//                }
//            }
//            //Column
//            for(f = 0; f < numCardsCanMemorize; f++){
//                if(memory[0][f] == board[rowChoice1][colChoice1]){
//                    memory[2][f] = memory[2][f+1];  //move everything forward by one to "forget" the matched cards
//                }
//            }
//            
//            board[CPU_RowChoice1][CPU_ColChoice1] = -1;
//            board[CPU_RowChoice2][CPU_ColChoice2] = -1;
//            
//            //print a blank spot in those two locations
//            printf("   ");
//            for(x = 0; x < col; x++){
//                printf("%3d",x);
//            }
//            printf("\n\n");
//            for(m = 0; m < row; m++){
//                printf("%3d", m);
//                for(n = 0; n < col; n++){
//                    if(board[m][n] == -1){
//                        printf("   ");
//                    }
//                    //else if(m == CPU_RowChoice2 && n == CPU_ColChoice2){
//                        //printf("   ");
//                    //}
//                    else{
//                        printf("  X");
//                    }
//                }
//                printf("\n\n");
//            }
//            
//        }
//        else{
//            //We didn't find any matches, so let's memorize these cards (if we have space)
//            if(index != numCardsCanMemorize){
//                memory[0][index] = board[CPU_RowChoice1][CPU_ColChoice1];
//                memory[1][index] = CPU_RowChoice1;
//                memory[2][index] = CPU_ColChoice1;
//                index++;
//                memory[0][index] = board[CPU_RowChoice2][CPU_ColChoice2];
//                memory[1][index] = CPU_RowChoice2;
//                memory[2][index] = CPU_ColChoice2;
//                index++;
//            }
//            else{
//                int i;
//                for(i = 0; i < numCardsCanMemorize; i++){
//                    //Shift everything forward by one to "Forget" oldest card and remember new one
//                    memory[0][i] = memory[0][i+1];
//                    memory[1][i] = memory[1][i+1];
//                    memory[2][i] = memory[2][i+1];
//                }
//                memory[0][index] = board[CPU_RowChoice1][CPU_ColChoice1];
//                memory[1][index] = CPU_RowChoice1;
//                memory[2][index] = CPU_ColChoice1;
//                //Now repeat to memorize the second card
//                for(i = 0; i < numCardsCanMemorize; i++){
//                    memory[0][i] = memory[0][i+1];
//                    memory[1][i] = memory[1][i+1];
//                    memory[2][i] = memory[2][i+1];
//                }
//                memory[0][index] = board[CPU_RowChoice2][CPU_ColChoice2];
//                memory[1][index] = CPU_RowChoice2;
//                memory[2][index] = CPU_ColChoice2;
//            }
//            //we didn't get a match, so print out the board as it was
//            printf("   ");
//            for(x = 0; x < col; x++){
//                printf("%3d",x);
//            }
//            printf("\n\n");
//            for(m = 0; m < row; m++){
//                printf("%3d", m);
//                for(n = 0; n < col; n++){
//                    if(board[m][n] == -1){
//                        printf("   ");
//                    }
//                    else{
//                        printf("  X");
//                    }
//                }
//                printf("\n\n");
//            }
//            
//        }
        //output to file
        writeToOutput(output2,roundNum, rowChoice1, colChoice1, board[rowChoice1][colChoice1], rowChoice2, colChoice2, board[rowChoice2][colChoice2], CPU_RowChoice1, CPU_ColChoice1, board[CPU_RowChoice1][CPU_ColChoice1], CPU_RowChoice2, CPU_ColChoice2, board[CPU_RowChoice2][CPU_ColChoice2]);

        roundNum++;
        
    }   //END OF WHILE LOOP
    
    fclose(output2);

    return 0;
}


int getUserRow(int guessNum){
    int rowChoice;
    printf("Enter row%d: ",guessNum);
    scanf("%d", &rowChoice);
    return rowChoice;
}
int getUserCol(int guessNum){
    int colChoice;
    printf("Enter col%d: ",guessNum);
    scanf("%d", &colChoice);
    return colChoice;
}
int didMatch(int c1, int c2){
    if (c1 == c2){
        return 1;
    }
    return 0;
}
void writeToOutput(FILE * output,int round, int rguess1, int cguess1, int rguess2, int cguess2, int card1, int card2, int cpu_r1, int cpu_c1, int cpu_r2, int cpu_c2, int card3, int card4){
    fprintf(output, "\n");
    //round   1: (0 0), (1 1), (18 19); (3 4), (2 3), (22 19)
    fprintf(output, "Round %3d: (%d %d), (%d %d), (%d %d); (%d %d), (%d %d), (%d %d)\n", round, rguess1, cguess1, rguess2, cguess2,card1,card2, cpu_r1, cpu_c1, cpu_r2, cpu_c2, card3, card4);
}
int didCPUMatch(int c1, int c2){
    if (c1 == c2){
        return 1;
    }
    return 0;
}



