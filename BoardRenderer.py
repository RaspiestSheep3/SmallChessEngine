import subprocess
import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib.animation import FuncAnimation
from datetime import datetime
#Engine stuff
engine = subprocess.Popen(
    ["./engineOld.exe"],
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    stderr=subprocess.DEVNULL,
    text=True,
    bufsize=1
)

engine2 = subprocess.Popen(
    ["./engine.exe"],
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    stderr=subprocess.DEVNULL,
    text=True,
    bufsize=1
)

# --- Constants ---
ROWS, COLS = 8, 8
WHITE_COLOR = "#F0D9B5"
BLACK_COLOR = "#B58863"

# Starting board position
board = [
    ["br","bn","bb","bq","bk","bb","bn","br"],
    ["bp"]*8,
    ["--"]*8,
    ["--"]*8,
    ["--"]*8,
    ["--"]*8,
    ["wp"]*8,
    ["wr","wn","wb","wq","wk","wb","wn","wr"]
]

# Map piece codes to symbols (unicode)
PIECE_SYMBOLS = {
    "wp": "♙","wn": "♘","wb": "♗","wr": "♖","wq": "♕","wk": "♔",
    "bp": "♟","bn": "♞","bb": "♝","br": "♜","bq": "♛","bk": "♚"
}


if(input("2 bots? : ").upper() == "Y"):

    #Loading in existing openings for fun
    opening = input("What opening? : ").lower()
    if(opening == "caro kann"):
        moves = ["e2e4", "c7c6", "d2d4", "d7d5"]
    elif(opening == "queens gambit declined"):
        moves = ["d2d4", "d7d5", "c2c4", "e7e6"]
    elif(opening == "queens gambit accepted"):
        moves = ["d2d4", "d7d5", "c2c4", "d5c4"]
    elif(opening == "french defense"):
        moves = ["e2e4", "e7e6", "d2d4", "d7d5", "b1c3"]
    else:
        moves = []

    # Read engine 1 output

    for i in range(50):
        moves_str = " ".join(moves)
        position_cmd = f"position startpos moves {moves_str}\n"

        print(position_cmd)

        # Send to engine 1
        engine.stdin.write(position_cmd)
        engine.stdin.flush()
        engine.stdin.write("go\n")
        engine.stdin.flush()
        startTime = datetime.now()
        
        if engine.poll() is not None:
            print("Engine exited")
            break
            
        line = engine.stdout.readline()
        print("Engine1 says:", line.strip())
        if not line:
            print("Broke")
            break
        
        while(True):
            line = engine.stdout.readline()
            print("Engine1 says:", line.strip())
            if not line:
                print("Broke")
                break
        
            #Processing the move 
            line = line.strip()
            if line.startswith("bestmove"):
                move = line.split()[1]
                
                print(f"Fetch took {datetime.now() - startTime} for E1")
            
                moves.append(move)
                break

        moves_str = " ".join(moves)
        position_cmd = f"position startpos moves {moves_str}\n"

        print(position_cmd)

        # Send to engine 2
        engine2.stdin.write(position_cmd)
        engine2.stdin.flush()
        engine2.stdin.write("go\n")
        engine2.stdin.flush()
        startTime = datetime.now()
        
        if engine2.poll() is not None:
            print("Engine 2 exited")
            break
        
        while(True):
            line = engine2.stdout.readline()
            print("Engine2 says:", line.strip())
            if not line:
                print("Broke")
                break
        
            #Processing the move 
            line = line.strip()
            if line.startswith("bestmove"):
                move = line.split()[1]
                print(f"Fetch took {datetime.now() - startTime} for E2")
                
                moves.append(move)
                break

    # --- Functions ---
    def move_piece(move, board):
        start_col = ord(move[0]) - ord('a')
        start_row = 8 - int(move[1])
        end_col = ord(move[2]) - ord('a')
        end_row = 8 - int(move[3])
        board[end_row][end_col] = board[start_row][start_col]
        board[start_row][start_col] = "--"

    def draw_board(ax, board):
        ax.clear()
        # Draw squares
        for row in range(ROWS):
            for col in range(COLS):
                color = WHITE_COLOR if (row + col) % 2 == 0 else BLACK_COLOR
                rect = patches.Rectangle((col, 7-row), 1, 1, facecolor=color)
                ax.add_patch(rect)

                piece = board[row][col]
                if piece != "--":
                    ax.text(col+0.5, 7-row+0.5, PIECE_SYMBOLS[piece],
                            ha='center', va='center', fontsize=48)

        ax.set_xlim(0,8)
        ax.set_ylim(0,8)
        ax.set_xticks(range(8))
        ax.set_yticks(range(8))
        ax.set_xticklabels(['a','b','c','d','e','f','g','h'])
        ax.set_yticklabels(['1','2','3','4','5','6','7','8'])
        ax.set_aspect('equal')
        ax.set_title("Chess Engine Moves")
        ax.axis('on')

    # --- Animation ---
    fig, ax = plt.subplots(figsize=(6,6))
    move_index = [0]  # mutable container for FuncAnimation

    def update(frame):
        if move_index[0] < len(moves):
            move_piece(moves[move_index[0]], board)
            move_index[0] += 1
        draw_board(ax, board)
        
    ani = FuncAnimation(fig, update, frames=len(moves)+1, interval=300, repeat=False)
    plt.show()
else:
    userInput = input("Move : ").strip()
    moves = [userInput]
    while(userInput != "quit"):
        moves_str = " ".join(moves)
        position_cmd = f"position startpos moves {moves_str}\n"

        print(position_cmd)

        # Send to engine 1
        engine.stdin.write(position_cmd)
        engine.stdin.flush()
        engine.stdin.write("go\n")
        engine.stdin.flush()
        
        if engine.poll() is not None:
            print("Engine exited")
            break
        
        while(True):
            line = engine.stdout.readline()
            print("Engine1 says:", line.strip())
            if not line:
                print("Broke")
                break
        
            #Processing the move 
            line = line.strip()
            if line.startswith("bestmove"):
                move = line.split()[1]
            
                moves.append(move)
                break
        
        userInput = input("Move : ").strip()
        moves.append(userInput)