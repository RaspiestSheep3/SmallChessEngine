#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    int startRow;
    int startCol;
    int endRow;
    int endCol;
    int pieceLabel;
    int flags; //1 = capture
    int promotionMarker; //1 - rook, 2 = knight, 3 = bishop, 4 = queen for promotions
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

void FindTargetedSquares(MoveList *list, bool targetedSquares[8][8]) {
    for(int i = 0; i < list->count; i++) {
        ValidMove targetedMove = list->moves[i];
        if(targetedMove.flags == 1) targetedSquares[targetedMove.endRow][targetedMove.endCol] = true;
    }
}

void GenerateMovesFromOffset(bool isWhite, int positions[8][8], int startRow, int startCol, int offsetRow, int offsetCol, MoveList *list,int base,int cap, bool canCapture) {
    for(int k = base; k <= cap; k++) {

        //Finding the offset
        int offset[2] = {k*offsetRow, k*offsetCol};

        if( k == 0 || startRow + offset[0] < 0 || startRow + offset[0] > 7 || startCol + offset[1] < 0 || startCol + offset[1] > 7 || (offset[0] == 0 && offset[1] == 0)) continue;

        if((positions[startRow + offset[0]][startCol + offset[1]] > 9 && isWhite) || (positions[startRow + offset[0]][startCol + offset[1]] < 9 && positions[startRow + offset[0]][startCol + offset[1]]> 0 && !isWhite)) {
            //We have hit the opponent
            if(canCapture){
                list->moves[list->count++] = (ValidMove){startRow, startCol, startRow + offset[0], startCol + offset[1] ,positions[startRow][startCol], 1, 0};
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
            if(positions[startRow][startCol] % 9 == 1) {
                //Dealing with promotion nonsense
                if((positions[startRow][startCol] == 1 && (startRow + offset[0]) == 0) || (positions[startRow][startCol] == 10 && (startRow + offset[0]) == 7)) {
                    for(int l = 1; l < 5; l++) list->moves[list->count++] = (ValidMove){startRow, startCol, startRow + offset[0], startCol + offset[1] ,positions[startRow][startCol], 0, l};
                }
                //Else no promotion - work as normal
                else list->moves[list->count++] = (ValidMove){startRow, startCol, startRow + offset[0], startCol + offset[1] ,positions[startRow][startCol], 0, 0};
            }
            else list->moves[list->count++] = (ValidMove){startRow, startCol, startRow + offset[0], startCol + offset[1] ,positions[startRow][startCol], 0, 0};
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

                    if(j > 0 && positions[i-1][j-1] > 9){
                        if(i-1 == 0) {
                            //Promotion stuff
                            for(int l = 1; l < 5; l++) list->moves[list->count++] = (ValidMove){i, j, i-1, j-1 ,positions[i][j], 1, l};
                        }
                        else list->moves[list->count++] = (ValidMove){i, j, i-1, j-1 ,positions[i][j], 1, 0};
                    } 
                    if(j < 7 && positions[i-1][j+1] > 9) {
                        if(i-1 == 0) {
                            //Promotion stuff
                            for(int l = 1; l < 5; l++) list->moves[list->count++] = (ValidMove){i, j, i-1, j+1 ,positions[i][j], 1, l};
                        }
                        else list->moves[list->count++] = (ValidMove){i, j, i-1, j+1 ,positions[i][j], 1, 0};
                    }
                }
                else if(!isWhite && i < 7) {
                    if(j > 0 && positions[i+1][j-1] < 10 && positions[i+1][j-1] > 0){
                        if(i+1 == 7){
                            for(int l = 1; l < 5; l++) list->moves[list->count++] = (ValidMove){i, j, i+1, j-1 ,positions[i][j], 1, l};
                        }
                        else list->moves[list->count++] = (ValidMove){i, j, i+1, j-1 ,positions[i][j], 1, 0};
                    }
                    if(j < 7 && positions[i+1][j+1] < 10 && positions[i+1][j+1] > 0){
                        if(i+1 == 7){
                            for(int l = 1; l < 5; l++) list->moves[list->count++] = (ValidMove){i, j, i+1, j+1 ,positions[i][j], 1, l};
                        }
                        else list->moves[list->count++] = (ValidMove){i, j, i+1, j+1 ,positions[i][j], 1, 0};
                    }
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

void FlipMap(float original[8][8], float out[8][8]) {
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++) out[i][j] = original[7-i][j];
    }
}

//Eval function
float EvaluatePosition(int positions[8][8]) {
    //Very basic eval
    //Defining costs
    float costs[6] = {1, 5, 3, 3.5, 9, 10000}; //King is defined as 10k because if the king can be captured its a win
    //People also seem to value bhishops over knights so Ive rewarded having them a bit more
    //I believe that knights are better but o well

    float whiteEval = 0;
    float blackEval = 0;

    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++) {
            if(positions[i][j] > 9) blackEval += costs[positions[i][j] % 9 - 1];
            else if(positions[i][j] > 0) whiteEval += costs[positions[i][j] - 1];
        }
    }

    //Punishing for having the king surrounded with squares it cant run to - smothered or targeted
    //To do this we need to track white squares are targeted and which squares have the king
    //Im going to give a fairly harsh penalty for having the king
    //However, I intend for this to increase linearly as the number of pieces decreases
    //ATM it does not increase at all
    //TODO

    //Reward for mobility
    float mobilityMult = 0.01; //Assuming avg 35 moves per side this rewards 1.75 aka 1/2 a knight
    //Ignore prev comment, im toning it down so it plays samrter
    
    MoveList whiteMoves = {0};
    FindAllValidMoves(true, positions, &whiteMoves);

    MoveList blackMoves = {0};
    FindAllValidMoves(false, positions, &blackMoves);

    whiteEval += mobilityMult * whiteMoves.count;
    blackEval += mobilityMult * blackMoves.count;

    //Position maps - we reward the bot for having pieces in good positions
    //These are likely to be subject to a lot of change
    //TODO reward passed pawns
    float pawnMap[8][8] = {
        {3.000,3.000,3.000,3.000,3.000,3.000,3.000,3.000},
        {0.060,0.080,0.090,0.100,0.100,0.090,0.080,0.060},
        {0.030,0.040,0.045,0.050,0.050,0.045,0.040,0.030},
        {0.010,0.015,0.025,0.030,0.030,0.025,0.015,0.010},
        {0.005,0.010,0.015,0.025,0.025,0.015,0.010,0.005},
        {0.001,0.002,0.005,0.010,0.010,0.005,0.002,0.001},
        {0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000},
        {0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000},
    };

    //Copied from knight again but adding extra punishment for starting squares
    float rookMap[8][8] = {
        {-0.015,-0.010,-0.010,-0.005,-0.005,-0.010,-0.010,-0.015},
        {-0.010,0.000,0.000,0.015,0.015,0.000,0.000,-0.010},
        {-0.010,0.000,0.015,0.030,0.030,0.015,0.000,-0.010},
        {-0.005,0.015,0.030,0.040,0.040,0.030,0.015,-0.005},
        {-0.005,0.015,0.030,0.040,0.040,0.030,0.015,-0.005},
        {-0.010,0.000,0.015,0.030,0.030,0.015,0.000,-0.010},
        {-0.010,0.000,0.000,0.015,0.015,0.000,0.000,-0.010},
        {-0.020,-0.010,-0.010,-0.005,-0.005,-0.010,-0.010,-0.020},
    };

    //Knights want to be centralised but adding extra punishment for starting squares
    float knightMap[8][8] = {
        {-0.015,-0.010,-0.010,-0.005,-0.005,-0.010,-0.010,-0.015},
        {-0.010,0.000,0.000,0.015,0.015,0.000,0.000,-0.010},
        {-0.010,0.000,0.015,0.030,0.030,0.015,0.000,-0.010},
        {-0.005,0.015,0.030,0.040,0.040,0.030,0.015,-0.005},
        {-0.005,0.015,0.030,0.040,0.040,0.030,0.015,-0.005},
        {-0.010,0.000,0.015,0.030,0.030,0.015,0.000,-0.010},
        {-0.010,0.000,0.000,0.015,0.015,0.000,0.000,-0.010},
        {-0.015,-0.020,-0.010,-0.005,-0.005,-0.010,-0.020,-0.015},
    };

    //This is being copied over from the knight map but adding extra punishment for starting squares
    float bishopMap[8][8] = {
        {-0.015,-0.010,-0.010,-0.005,-0.005,-0.010,-0.010,-0.015},
        {-0.010,0.000,0.000,0.015,0.015,0.000,0.000,-0.010},
        {-0.010,0.000,0.015,0.030,0.030,0.015,0.000,-0.010},
        {-0.005,0.015,0.030,0.040,0.040,0.030,0.015,-0.005},
        {-0.005,0.015,0.030,0.040,0.040,0.030,0.015,-0.005},
        {-0.010,0.000,0.015,0.030,0.030,0.015,0.000,-0.010},
        {-0.010,0.000,0.000,0.015,0.015,0.000,0.000,-0.010},
        {-0.015,-0.010,-0.020,-0.005,-0.005,-0.020,-0.010,-0.015},
    };

    //This is also a copy of the knight map without start pushiment
    float queenMap[8][8] = {
        {-0.015,-0.010,-0.010,-0.005,-0.005,-0.010,-0.010,-0.015},
        {-0.010,0.000,0.000,0.015,0.015,0.000,0.000,-0.010},
        {-0.010,0.000,0.015,0.030,0.030,0.015,0.000,-0.010},
        {-0.005,0.015,0.030,0.040,0.040,0.030,0.015,-0.005},
        {-0.005,0.015,0.030,0.040,0.040,0.030,0.015,-0.005},
        {-0.010,0.000,0.015,0.030,0.030,0.015,0.000,-0.010},
        {-0.010,0.000,0.000,0.015,0.015,0.000,0.000,-0.010},
        {-0.015,-0.010,-0.010,-0.005,-0.005,-0.010,-0.010,-0.015},
    };

    //Kings want to be on the ends so im taking the negative of the knight map
    //TODO - Make a lategame map and interpolate
    float kingMap[8][8] = {
        {+0.015,+0.010,+0.010,+0.005,+0.005,+0.010,+0.010,+0.015},
        {+0.010,-0.000,-0.000,-0.015,-0.015,-0.000,-0.000,+0.010},
        {+0.010,-0.000,-0.015,-0.030,-0.030,-0.015,-0.000,+0.010},
        {+0.005,-0.015,-0.030,-0.040,-0.040,-0.030,-0.015,+0.005},
        {+0.005,-0.015,-0.030,-0.040,-0.040,-0.030,-0.015,+0.005},
        {+0.010,-0.000,-0.015,-0.030,-0.030,-0.015,-0.000,+0.010},
        {+0.010,-0.000,-0.000,-0.015,-0.015,-0.000,-0.000,+0.010},
        {+0.015,+0.010,+0.010,+0.005,+0.005,+0.010,+0.010,+0.015},
    };

    float flipMap[8][8] = {0};

    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            int target = positions[i][j];
            if(target == 0) continue;
            
            float (*map)[8] = NULL;

            if(target % 9 == 1)      map = pawnMap;
            else if(target % 9 == 2) map = rookMap;
            else if(target % 9 == 3) map = knightMap;
            else if(target % 9 == 4) map = bishopMap;
            else if(target % 9 == 5) map = queenMap;
            else if(target % 9 == 6) map = kingMap;

            if(target > 9) FlipMap(map, flipMap);
            else memcpy(flipMap, map, sizeof(float) * 64);

            if(target < 9) whiteEval += flipMap[i][j];
            else blackEval += flipMap[i][j];
        }
    }

    return whiteEval - blackEval; //+ve = white winning, -ve = black winning
}

void FindBestMove(int positions[8][8], char *out, bool isWhite){
    MoveList availableMoves = {0};
    FindAllValidMoves(isWhite, positions, &availableMoves);
    
    float evals[256]= {0};

    for(int i = 0; i < availableMoves.count; i++) {
        ValidMove move = availableMoves.moves[i];

        int positionsAlt[8][8];    
        memcpy(positionsAlt, positions, sizeof(int) * 64);

        //Making the move on the alt
        positionsAlt[move.endRow][move.endCol] = move.pieceLabel + move.promotionMarker; //This should properly deal with promostions beacuse most of the time we just add 0
        positionsAlt[move.startRow][move.startCol] = 0;

        //Making every single opposites move
        MoveList availableOtherMoves = {0};
        FindAllValidMoves(!isWhite, positionsAlt, &availableOtherMoves);

        //Checking if one of their moves threatens our king - if it does we cant play this move
        bool targetedSquaresAlt[8][8] = {false};
        FindTargetedSquares(&availableOtherMoves, targetedSquaresAlt);

        //Finding our king
        int kingPosition[2]= {0,0};
        bool found = false;
        for(int i = 0; i < 8 && !found; i++){
            for(int j = 0; j < 8; j++) {
                if((isWhite && positionsAlt[i][j] == 6) || (!isWhite && positionsAlt[i][j] == 15)){
                    kingPosition[0] = i;
                    kingPosition[1] = j;
                    found = true;
                    break;
                }
            }
        }

        if(targetedSquaresAlt[kingPosition[0]][kingPosition[1]]) {
            evals[i] = -1000000000000000; //We are in check so we cant play this move
            continue;
        }

        //Playing all of others moves
        float worstEval = 10000000000000;
        for(int j = 0; j < availableOtherMoves.count; j++) {
            int positionsAlt2[8][8];
            ValidMove otherMove = availableOtherMoves.moves[j];

            memcpy(positionsAlt2, positionsAlt, sizeof(int) * 64);

            //Making the move on the alt
            positionsAlt2[otherMove.endRow][otherMove.endCol] = otherMove.pieceLabel + otherMove.promotionMarker;
            positionsAlt2[otherMove.startRow][otherMove.startCol] = 0;
            
            //Getting an eval for this move combo

            //!FOR THE MOMENT I AM USING 3, 3
            float eval = EvaluatePosition(positionsAlt2);
            if(!isWhite) eval *= -1;

            if(eval < worstEval) worstEval = eval;
        }

        evals[i] = worstEval;
    }

    //Finding the best move
    int index = 0;
    float maxEval = -1000000000000000;
    for(int i = 0; i < availableMoves.count; i++) {
        
        if(maxEval < evals[i]) {
            maxEval = evals[i];
            index = i;
        }
    }

    ValidMove chosenMove = availableMoves.moves[index];
    
    out[0] = 'a' + chosenMove.startCol;
    out[1] = '8' - chosenMove.startRow;
    out[2] = 'a' + chosenMove.endCol;
    out[3] = '8' - chosenMove.endRow;

    if (chosenMove.promotionMarker) {
        char promoChar = 'q';
        if (chosenMove.promotionMarker == 1) promoChar = 'r';
        else if (chosenMove.promotionMarker == 2) promoChar = 'n';
        else if (chosenMove.promotionMarker == 3) promoChar = 'b';
        else if (chosenMove.promotionMarker == 4) promoChar = 'q';

        out[4] = promoChar;
        out[5] = '\0';
    } 
    else {
        out[4] = '\0';
    }
}

int main(void) {
    char input[4096];

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
        else if(strncmp(input, "position startpos moves", 23) == 0 && strlen(input) > 23) {
            //We need to update the position accordingly
            char* update = input + 23 + 1;

            ResetBoard(positions);
            isWhite = true;
            
           //Dividing up the rest of the moves and applying them
           char * token = strtok(update, " ");
           while(token != NULL) {

                isWhite = !isWhite;
                //Appling the move
                //Standard move
                positions['8'-token[3]][token[2] - 'a'] = positions['8' - token[1]][token[0] - 'a'];
                positions['8' - token[1]][token[0] - 'a'] = 0;

                if(strlen(token) == 5) {
                    int base = isWhite ? 1 : 10;
                    //Promoting the pawn
                    if(token[4] == 'q') positions['8'-token[3]][token[2] - 'a'] = 4 + base;
                    else if(token[4] == 'b') positions['8'-token[3]][token[2] - 'a'] = 3 + base;
                    else if(token[4] == 'n') positions['8'-token[3]][token[2] - 'a'] = 2 + base;
                    else if(token[4] == 'r') positions['8'-token[3]][token[2] - 'a'] = 1 + base;
                }

                token = strtok(NULL, " ");
            }
        }

        else if(strcmp(input, "go") == 0){
            char move[6];
            FindBestMove(positions,move, isWhite);
            //ShowBoard(positions);
            printf("bestmove %s\n", move);
            fflush(stdout); 
        } 
        
        else if(strcmp(input, "quit") == 0) return 0;

         //!TEMP 


        fflush(stdout); 
    }

}

//CD Command : cd C:\\Users\\iniga\\source\\repos\\SmallChessEngine
//GCC Command : gcc -Os -s -o engine.exe SmallChessEngine.c
//UPX command : C:\\Users\\iniga\\UPX\\upx-5.0.2-win64\\upx.exe upx --best --ultra-brute engine.exe