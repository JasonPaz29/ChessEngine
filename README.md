# Chess Engine v1

A C++ chess engine and playable chess application built with Raylib. This project implements full chess move legality, special chess rules, undo-based game state management, and a basic computer opponent using search and heuristic evaluation.

## Overview

This project started as an attempt to build a chess engine from scratch in C++. The goal was to move beyond basic web apps and work on a more algorithmic, performance-focused project involving game state, recursion, move generation, and optimization.

The current version is a playable chess program with a working engine. It supports legal chess movement, special rules, move undoing, bot move selection, alpha-beta search, and board evaluation.

## Features

* Playable chessboard built with Raylib
* Full legal move validation
* Turn-based movement
* Check detection
* Checkmate detection
* Stalemate detection
* Castling
* En passant
* Pawn promotion
* Move history
* Undo move system
* Greedy move selection
* Minimax / alpha-beta pruning
* Material-based evaluation
* Piece-square table evaluation
* Release-mode benchmarking
* Nodes-per-second tracking

## Tech Stack

* **Language:** C++
* **Graphics Library:** Raylib
* **Build Type:** Debug / Release
* **Core Concepts:** Chess programming, recursion, alpha-beta pruning, board evaluation, move generation, performance benchmarking

## Engine Architecture

The project is separated into two main parts:

1. **Chess Rules / Board Logic**

   * Handles board state
   * Validates piece movement
   * Detects check, checkmate, and stalemate
   * Handles special chess rules
   * Maintains move history and undo functionality

2. **Engine / Search Logic**

   * Generates legal moves
   * Evaluates board positions
   * Searches possible moves
   * Uses alpha-beta pruning to reduce unnecessary search branches
   * Selects the best move based on evaluation score

## Search Algorithm

The engine uses recursive search to evaluate future positions.

The basic process is:

1. Generate all legal moves for the current side.
2. Try each move.
3. Recursively evaluate the opponent’s possible responses.
4. Undo the move.
5. Choose the move with the best resulting evaluation.

Alpha-beta pruning is used to skip branches that cannot affect the final decision.

## Evaluation Function

The evaluation function currently considers:

* Material balance
* Piece values
* Piece-square tables
* Basic positional bonuses

Example piece values:

```cpp
Pawn   = 100
Knight = 300
Bishop = 300
Rook   = 500
Queen  = 900
```

Positive scores favor White. Negative scores favor Black.

## Performance

Current benchmark results:

```text
Release Build NPS: ~700,000 nodes/second
Search Depth Tested: 4
```

Performance improved significantly after switching from debug builds to release builds. (~110k NPS -> ~700k NPS)


## What I Learned

Through this project, I learned about:

* C++ project structure
* Raylib rendering and input handling
* Chess board representation
* Legal move generation
* Special move logic
* Recursive search
* Minimax and alpha-beta pruning
* Board evaluation
* Undo-based search
* Performance benchmarking
* Debug vs release build performance differences
* The difficulty of optimizing chess engines

## Challenges

Some of the hardest parts of this project were:

* Correctly implementing all legal chess rules
* Handling castling, en passant, and promotion
* Detecting check and preventing illegal king moves
* Building a reliable undo system for recursive search
* Debugging search logic
* Improving engine performance
* Understanding why nodes-per-second was bottlenecking

## Current Limitations

This is version 1 of the engine, so there are still areas that could be improved:

* Move generation is not heavily optimized
* No bitboard representation
* No transposition table
* No iterative deepening
* No quiescence search
* No UCI support
* Evaluation function is still basic
* Search depth is limited by performance

## Future Improvements

Possible future version 2 improvements:

* Rewrite move generation using bitboards or a compact board representation
* Add transposition tables
* Add quiescence search
* Add iterative deepening
* Add move ordering in alpha beta pruning
* Add killer move heuristic
* Add history heuristic
* Improve evaluation function
* Add UCI compatibility
* Benchmark against other engines
* Create engine version comparisons

## Why This Project Matters

This project was built to practice lower-level programming, algorithms, and performance-focused development. Unlike a standard CRUD web app, this project required implementing complex game rules, recursive search, state restoration, and optimization. This also helped with learning a low level language for the first time.

The engine is not designed to compete with professional chess engines, but it demonstrates the foundations of chess programming and gives a strong base for a future optimized version.

## How to Run

From the project root:

```bash
# Build the playable Raylib app in release mode
make config=release_x64

# Run the playable chess program
./bin/Release/chess-engine
```

For a debug build:

```bash
make config=debug_x64
./bin/Debug/chess-engine
```

To run the headless engine benchmark without opening a Raylib window:

```bash
make config=release_x64
./bin/Release/chess-engine --bench
```

The benchmark skips rendering, textures, input, frame timing, and the game loop. It prints search depth, best move, evaluation, nodes searched, elapsed time, NPS, and alpha-beta cutoffs.

## Project Status

This project is currently frozen as **v1**.

The next major version may focus on a faster board representation, better search optimization, and more advanced chess engine techniques.
