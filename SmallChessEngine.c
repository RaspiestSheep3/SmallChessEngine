#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    int startRow;
    int startCol;
    int endRow;
    int endCol;
    int pieceLabel;
    int flags; //1 = capture, 2 = promotion, 3 = check
} ValidMove;

typedef struct {
    ValidMove moves[256]; //Max is 218 moves according to google for a given position for 1 side
    int count;
} MoveList;

//!THIS IS FOR DEBUGGING - REMOVE WHEN MEASURING SIZE 
void ShowBoard(int positions[8][8]) {
    for(int i = 0; i < 8; i++){
        printf("Line %d : %d %d %d %d %d %d %d %d\n", i, positions[i][0], positions[i][1], positions[i][2], positions[i][3], positions[i][4], positions[i][5], positions[i][6], positions[i][7]);
    }

    fflush(stdout); 

    return;
}

void GenerateMovesFromOffset(bool isWhite, int positions[8][8], int startRow, int startCol, int offsetRow, int offsetCol, MoveList *list,int base,int cap, bool canCapture) {
    for(int k = base; k <= cap; k++) {

        //Finding the offset
        int offset[2] = {k*offsetRow, k*offsetCol};

        if( k == 0 || startRow + offset[0] < 0 || startRow + offset[0] > 7 || startCol + offset[1] < 0 || startCol + offset[1] > 7 || (offset[0] == 0 && offset[1] == 0)) continue;

        if((positions[startRow + offset[0]][startCol + offset[1]] > 9 && isWhite) || (positions[startRow + offset[0]][startCol + offset[1]] < 9 && positions[startRow + offset[0]][startCol + offset[1]]> 0 && !isWhite)) {
            //We have hit the opponent
            if(canCapture){
                list->moves[list->count++] = (ValidMove){startRow, startCol, startRow + offset[0], startCol + offset[1] ,positions[startRow][startCol], 1};
                //printf("We have hit opponent at %d %d for piece type %d\n", startRow + offset[0], startCol + offset[1], positions[startRow][startCol]);
            }
            break;
        }
        else if(positions[startRow + offset[0]][startCol + offset[1]] > 0 && 
            ((positions[startRow + offset[0]][startCol + offset[1]] < 9 && isWhite) || 
             (positions[startRow + offset[0]][startCol + offset[1]] > 9 && !isWhite))) {
            //We have hit ourselves
            //printf("We have hit self at %d %d for piece %d\n", startRow + offset[0], startCol + offset[1], positions[startRow][startCol]);
            break;
        }

        else {
            //We have hit an empty space
            list->moves[list->count++] = (ValidMove){startRow, startCol, startRow + offset[0], startCol + offset[1] ,positions[startRow][startCol], 0};
        }
    }
}


void FindAllValidMoves(bool isWhite, int positions[8][8], MoveList *list) {

    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++) {
            //Saving uneeded checks
            if((positions[i][j] == 0) || (positions[i][j] >= 10 && isWhite) || (positions[i][j] < 10 && !isWhite)) continue;

            //Checking the type and calculating accordingly
            int pieceType = positions[i][j] % 9; 

            if(pieceType == 1) {
                //Pawn
                //Procrastinating because double movement + en passant

                //TODO : En Passant
                //Nvm i dont believe in En Passant

                //Checking if it exists on the starting squares for double move
                if((i == 1 && isWhite) || (i == 6 && !isWhite)) GenerateMovesFromOffset(isWhite, positions, i,j, 1,0, list, isWhite ? -2 : 0, isWhite ? 0 : 2, false);
                else GenerateMovesFromOffset(isWhite, positions, i,j, 1,0, list,  isWhite ? -1 : 0, isWhite ? 0 : 1, false);
            
                //Custom dealing with captures because its easier

                if(isWhite && i > 0) {
                    if(j > 0 && positions[i-1][j-1] > 9) list->moves[list->count++] = (ValidMove){i, j, i-1, j-1 ,positions[i][j], 1};
                    if(j < 7 && positions[i-1][j+1] > 9) list->moves[list->count++] = (ValidMove){i, j, i-1, j+1 ,positions[i][j], 1};
                }
                else if(!isWhite && i < 7) {
                    if(j > 0 && positions[i+1][j-1] < 10 && positions[i+1][j-1] > 0) list->moves[list->count++] = (ValidMove){i, j, i+1, j-1 ,positions[i][j], 1};
                    if(j < 7 && positions[i+1][j+1] < 10 && positions[i+1][j+1] > 0) list->moves[list->count++] = (ValidMove){i, j, i+1, j+1 ,positions[i][j], 1};
                }
            }
            else if(pieceType == 2) {
                //Rook
                
                GenerateMovesFromOffset(isWhite, positions, i,j, 0,-1, list, 1, 7, true);
                GenerateMovesFromOffset(isWhite, positions, i,j, -1,0, list, 1, 7, true);
                GenerateMovesFromOffset(isWhite, positions, i,j, 0,1, list, 1, 7, true);
                GenerateMovesFromOffset(isWhite, positions, i,j, 1,0, list, 1, 7, true);
            }

            else if(pieceType == 3) {
                //Knight
                GenerateMovesFromOffset(isWhite, positions, i,j, 2,-1, list, 0, 1, true);
                GenerateMovesFromOffset(isWhite, positions, i,j, 2,1, list, 0, 1, true);
                GenerateMovesFromOffset(isWhite, positions, i,j, 1,2, list, 0, 1, true);
                GenerateMovesFromOffset(isWhite, positions, i,j, 1,-2, list, 0, 1, true);

                GenerateMovesFromOffset(isWhite, positions, i,j, -2,1, list, 1, 1, true);
                GenerateMovesFromOffset(isWhite, positions, i,j, -2,-1, list, 1, 1, true);
                GenerateMovesFromOffset(isWhite, positions, i,j, -1,-2, list, 1, 1, true);
                GenerateMovesFromOffset(isWhite, positions, i,j, -1,2, list, 1, 1, true);
            }

            else if(pieceType == 4) {
                //Bishop

                GenerateMovesFromOffset(isWhite, positions, i,j, -1,-1, list, 1, 7, true);
                GenerateMovesFromOffset(isWhite, positions, i,j, -1,1, list, 1, 7, true);
                GenerateMovesFromOffset(isWhite, positions, i,j, 1,1, list, 1, 7, true);
                GenerateMovesFromOffset(isWhite, positions, i,j, 1,-1, list, 1, 7, true);
            }

            else if(pieceType == 5) {
                //Queen
                GenerateMovesFromOffset(isWhite, positions, i,j, 0,-1, list, 1, 7, true);
                GenerateMovesFromOffset(isWhite, positions, i,j, -1,0, list, 1, 7, true);
                GenerateMovesFromOffset(isWhite, positions, i,j, 0,1, list, 1, 7, true);
                GenerateMovesFromOffset(isWhite, positions, i,j, 1,0, list, 1, 7, true);

                GenerateMovesFromOffset(isWhite, positions, i,j, -1,-1, list, 1, 7, true);
                GenerateMovesFromOffset(isWhite, positions, i,j, -1,1, list, 1, 7, true);
                GenerateMovesFromOffset(isWhite, positions, i,j, 1,1, list, 1, 7, true);
                GenerateMovesFromOffset(isWhite, positions, i,j, 1,-1, list, 1, 7, true);
            }

            else if(pieceType == 6) {
                //King

                //TODO : CASTLING

                GenerateMovesFromOffset(isWhite, positions, i,j, 0,-1, list, 1, 1, true);
                GenerateMovesFromOffset(isWhite, positions, i,j, -1,0, list, 1, 1, true);
                GenerateMovesFromOffset(isWhite, positions, i,j, 0,1, list, 1, 1, true);
                GenerateMovesFromOffset(isWhite, positions, i,j, 1,0, list, 1, 1, true);

                GenerateMovesFromOffset(isWhite, positions, i,j, -1,-1, list, 1, 1, true);
                GenerateMovesFromOffset(isWhite, positions, i,j, -1,1, list, 1, 1, true);
                GenerateMovesFromOffset(isWhite, positions, i,j, 1,1, list, 1, 1, true);
                GenerateMovesFromOffset(isWhite, positions, i,j, 1,-1, list, 1, 1, true);

            }
        }
    }
}

void ResetBoard(int positions[8][8]) {
    int newBoard[8][8] = {
        {11, 12, 13, 14, 15, 13, 12, 11},
        {10, 10, 10, 10, 10, 10, 10, 10},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {2, 3, 4, 5, 6, 4, 3, 2}
    };

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            positions[i][j] = newBoard[i][j];
}

void FindBestMove(int positions[8][8], char *out, bool isWhite){
    MoveList availableMoves = {0};
    FindAllValidMoves(isWhite, positions, &availableMoves);

    //Parsing the 1st move we find

    ValidMove chosenMove = availableMoves.moves[0];
    
    out[0] = 'a' + chosenMove.startCol;
    out[1] = '8' - chosenMove.startRow;
    out[2] = 'a' + chosenMove.endCol;
    out[3] = '8' - chosenMove.endRow;
    out[4] = '\0';
}

int main(void) {
    char input[256];

    bool isWhite = true;

    /*
    0 = Blank, 1 = Pawn, 2 = Rook, 3 = Knight, 4 = Bishop, 5 = Queen, 6 = King
    +0 = White, +9 = Black

    Drawing black on top
    */
    int positions[8][8] = {
        {11, 12, 13, 14, 15, 13, 12, 11},
        {10, 10, 10, 10, 10, 10, 10, 10},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1}, 
        {2, 3, 4, 5, 6, 4, 3, 2},
    };

    while (fgets(input, sizeof(input), stdin)) {
        input[strcspn(input, "\n")] = 0;  // remove \n

        if (strcmp(input, "uci") == 0) printf("id name Mercury\nauthor RS3\nuciok\n");
        else if (strcmp(input, "isready") == 0) printf("readyok\n"); 
        else if(strncmp(input, "position startpos moves", 23) == 0 && strlen(input) > 17) {
            //We need to update the position accordingly
            char* update = input + 23 + 1;

            ResetBoard(positions);
            isWhite = true;
            
            //Every entry is of size 4, with a spacer of 1
            for(int i = 0; i < strlen(update); i+=5) {
                isWhite = !isWhite;

                positions['8'- update[i + 3]][update[i + 2] - 'a'] = positions['8' - update[i + 1]][update[i] - 'a'];
                positions['8'- update[i + 1]][update[i] - 'a'] = 0;
            }
        }

        else if(strcmp(input, "go") == 0){
            char move[5];
            FindBestMove(positions,move, isWhite);
            printf("bestmove %s\n", move);
        } 
        
        else if(strcmp(input, "quit") == 0) return 0;

         //!TEMP 
        //ShowBoard(positions);


        fflush(stdout); 
    }

}

//CD Command : C:\\Users\\iniga\\source\\repos\\SmallChessEngine
//GCC Command : gcc -Os -s -o engine.exe SmallChessEngine.c
//UPX command : C:\\Users\\iniga\\UPX\\upx-5.0.2-win64\\upx.exe upx --best --ultra-brute engine.exe