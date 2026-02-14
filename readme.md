JC Zaragoza  
CMPM 123 - Game Programming  
February 2026

I implemented this on Windows 10 using Visual Studio 2019+ and CMake.

When starting this project, I needed to understand the existing game engine architecture that was built around board games. The framework gave me abstractions that I had to learn to work with properly. The key concepts were Bits (visual sprites representing game pieces that inherit from Sprite), BitHolders (containers on the board that can hold a Bit, also inheriting from Sprite), and the Grid system (a 2D array of ChessSquares that manages board layout). The code in TicTacToe.cpp and Checkers.cpp had helpful comments and patterns that helped me with my implementation. I spent time studying how the Grid classes worked, especially the directional methods and the forEachSquare iterator pattern, which made it easier to work with Connect 4's bigger board.

The actionForEmptyHolder() function handles the core gameplay mechanic where pieces drop to the lowest available row in a column. When a player clicks any square in a column, I calculate which row the piece should actually land in by scanning from the bottom up until I find an empty spot. This is handled by the getLowestAvailableRow() helper function.

One challenge was making sure pieces couldn't be placed in full columns. The function returns -1 when a column is full, and the action fails without placing a piece. This keeps the game from breaking when players try impossible moves.

A lot of the logic for the win and draw conditions were majorly based on the code we used for the tic tac toe file. Seeing all the possibilites, then deciding what kind of moves you can make (if there are any) was a big way I got the conditinos for the game right.

ANIMATION------------------------------------

For the animation extra credit, I needed to make pieces fall into place instead of appearing instantly. The framework already had an animation system through the Bit class's moveTo() method, which I saw when studying the Bit.cpp implementation.

When a piece is placed, I create it at a position above the board (100 pixels higher than the top row) and place it in the target square. Then I call moveTo() with the final position. The Bit's update() method, which is called automatically during drawFrame(), handles the gradual movement. The piece's _moving flag stays true until it reaches its destination, creating a smooth falling animation.

AI IMPLEMENTATION----------------------------

 Since the game framework already supported AI through the gameHasAI() and updateAI() methods, I had to implement the decision-making logic that determines which column the AI should choose.

The core of my AI uses the minimax algorithm with alpha-beta pruning. It works by recursively simulating future game states and evaluating which moves lead to the best outcomes. I implemented minimax() which takes a board state string, search depth, alpha/beta bounds, and a maximizing flag.

The algorithm alternates between maximizing (AI trying to win) and minimizing (opponent trying to win) layers. At each level, it tries every possible column placement, generates a new state string with that move applied, and recursively evaluates that position. Alpha-beta pruning cuts off branches that can't possibly affect the final decision, which significantly speeds up the search.
The updateAI() function is called each frame when it's the AI's turn. First, it checks if any pieces are currently animating and waits for them to finish. Then it calls getBestColumn(), which tries each possible column, simulates placing a piece there, and runs minimax on the resulting state. The AI searches 5 moves deep by default, which provides good strategic play without taking too long. 

One important detail is that the AI evaluates positions from its own perspective but needs to switch perspectives at each recursion level. The maximizing flag ensures the algorithm correctly alternates between trying to maximize the AI's score and minimize it for the opponent.

REFLECTION--------------------------------------

Working with this framework taught me a lot about designing modular game systems. The abstraction layers between Game, Grid, BitHolder, and Bit made it straightforward to implement new game logic without rewriting low-level rendering or input handling code. The Grid class was nicely desiged with its directional helpers and iteration functions. Being able to call forEachSquare() with lambdas made many operations concise and readable. The built-in animation system also saved significant time compared to implementing my own interpolation logic. The AI implementation helped me learn how minimax explores the game tree and how evaluation functions guide decision making in complex state spaces.

If I were to work more on this, I could add difficulty levels by adjusting the search depth, implement opening book strategies for faster early-game moves, or add a threat detection system that prioritizes immediate defensive moves. The modular structure would make any of these additions relatively straightforward.Overall, this project demonstrated how good framework design enables faster development of game logic while keeping clean code throughout.


