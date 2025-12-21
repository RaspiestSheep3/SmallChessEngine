#include <stdio.h>
#include <string.h>

//!THIS IS FOR DEBUGGING - REMOVE WHEN MEASURING SIZE 
void ShowBoard(int positions[8][8]) {
    for(int i = 0; i < 8; i++){
        printf("Line %d : %d %d %d %d %d %d %d %d\n", i, positions[i][0], positions[i][1], positions[i][2], positions[i][3], positions[i][4], positions[i][5], positions[i][6], positions[i][7]);
    }

    fflush(stdout); 

    return;
}

char* FindBestMove(int positions[8][8]){
    return "e2e4\n";
}

int main(void) {
    char input[256];

    /*
    0 = Blank, 1 = Pawn, 2 = Rook, 3 = Knight, 4 = Bishop, 5 = Queen, 6 = King
    +0 = White, +9 = Black

    Drawing white on top
    */
    int positions[8][8] = {
        {2, 3, 4, 5, 6, 4, 3, 2},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {10, 10, 10, 10, 10, 10, 10, 10},
        {11, 12, 13, 14, 15, 13, 12, 11},
    };

    while (fgets(input, sizeof(input), stdin)) {
        input[strcspn(input, "\n")] = 0;  // remove \n

        if (strcmp(input, "uci") == 0) printf("id name Mercury\nauthor RS3\nuciok\n");
        else if (strcmp(input, "isready") == 0) printf("readyok\n"); 
        else if(strncmp(input, "position startpos", 17) == 0 && strlen(input) > 17) {
            //We need to update the position accordingly
            char* update = input + 17 + 1;

            //Every entry is of size 4, with a spacer of 1
            for(int i = 0; i < strlen(update); i+=5) {
                positions[update[i + 3] - '1'][update[i + 2] - 'a'] = positions[update[i + 1] - '1'][update[i] - 'a'];
                positions[update[i + 1] - '1'][update[i] - 'a'] = 0;
            }
        }

        else if(strcmp(input, "go") == 0) printf(" bestmove %s", FindBestMove(positions));
        
        else if(strcmp(input, "quit") == 0) return 0;

         //!TEMP 
        ShowBoard(positions);


        fflush(stdout); 
    }

}
