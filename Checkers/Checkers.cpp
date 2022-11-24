#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define EMPTY 0
#define WHITE 1
#define PLAYER 2
#define COMPUTER 3
#define SELECTED 4

int POS(int x, int y) { return y*8+x; }

struct Board
{
    char cells[64];
};

enum Directions
{
    BACK_LEFT = 0,
    BACK_RIGHT,
    FORWARD_LEFT,
    FORWARD_RIGHT
};

struct ComputerMoves
{
    int count;
    int x;
    int y;
    enum Directions directions[4];
};

void initBoard(struct Board* board)
{
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            board->cells[POS(i, j)] = (i + j) % 2 ? WHITE : EMPTY;
        }
    }
}

void startGame(struct Board* board)
{
    // init computer units

    for (int i = 0; i < 4; ++i)
    {
        board->cells[POS(2 * i, 0)] = COMPUTER;
        board->cells[POS(2 * i + 1, 1)] = COMPUTER;
        board->cells[POS(2 * i, 2)] = COMPUTER;
    }

    // init player units

    for (int i = 0; i < 4; ++i)
    {
        board->cells[POS(2 * i + 1, 5)] = PLAYER;
        board->cells[POS(2 * i, 6)] = PLAYER;
        board->cells[POS(2 * i + 1, 7)] = PLAYER;
    }
}

void printBoard(struct Board* board)
{
    system("cls");

    printf("   0 1 2 3 4 5 6 7\n");
    for (int i = 0; i < 8; ++i)
    {
        printf("%d |", i);
        for (int j = 0; j < 8; ++j)
        {
            char currentCell = board->cells[POS(j, i)];
            switch (currentCell)
            {
            case EMPTY:
                printf("_");
                break;
            case WHITE:
                printf(" ");
                break;
            case PLAYER:
                printf("#");
                break;
            case COMPUTER:
                printf("@");
                break;
            case SELECTED:
                printf("?");
            }
            printf("|");
        }
        printf("\n");
    }
}

void reDrawBoard(struct Board* board, int x, int y)
{
    printBoard(board);
    printf("Select>%d %d\n", x, y);
}

void selectUnit(struct Board* board, int x, int y)
{
    if (board->cells[POS(x, y)] == PLAYER)
        board->cells[POS(x, y)] = SELECTED;
}

void deselectUnit(struct Board* board, int x, int y)
{
    if (board->cells[POS(x, y)] == SELECTED)
        board->cells[POS(x, y)] = PLAYER;
}

int moveDistance(int currentPosX, int currentPosY, int moveX, int moveY)
{
    int max = 0;
    int value = abs(currentPosX - moveX);
    if (value > max)
        max = value;

    value = abs(currentPosY - moveY);
    if (value > max)
        max = value;

    return max;
}

void tryMove(struct Board* board, int currentPosX, int currentPosY, int moveX, int moveY)
{
    if (board->cells[POS(currentPosX, currentPosY)] != PLAYER)
        return;

    if (board->cells[POS(moveX, moveY)] != EMPTY)
        return;

    board->cells[POS(currentPosX, currentPosY)] = EMPTY;
    board->cells[POS(moveX, moveY)] = PLAYER;
}

int shouldBeat(struct Board* board, int currentPosX, int currentPosY, int moveX, int moveY, int attackingPlayer, int attackedPlayer)
{
    int beatedPosX = (currentPosX + moveX) / 2;
    int beatedPosY = (currentPosY + moveY) / 2;

    if (board->cells[POS(currentPosX, currentPosY)] != attackingPlayer)
        return 0;

    if (board->cells[POS(beatedPosX, beatedPosY)] != attackedPlayer)
        return 0;

    if (board->cells[POS(moveX, moveY)] != EMPTY)
        return 0;

    return 1;
}

void beat(struct Board* board, int currentPosX, int currentPosY, int moveX, int moveY, int beatingPlayer)
{
    int beatedPosX = (currentPosX + moveX) / 2;
    int beatedPosY = (currentPosY + moveY) / 2;

    board->cells[POS(beatedPosX, beatedPosY)] = EMPTY;
    board->cells[POS(currentPosX, currentPosY)] = EMPTY;
    board->cells[POS(moveX, moveY)] = beatingPlayer;
}

void playerMove(struct Board* board, int currentPosX, int currentPosY, int moveX, int moveY)
{
    int distance = moveDistance(currentPosX, currentPosY, moveX, moveY);
    switch (distance)
    {
    case 0:
        return;
    case 1:
        tryMove(board, currentPosX, currentPosY, moveX, moveY);
        break;
    case 2:
        if (shouldBeat(board, currentPosX, currentPosY, moveX, moveY, PLAYER, COMPUTER))
            beat(board, currentPosX, currentPosY, moveX, moveY, PLAYER);
        break;
    default:
        return;
    }
}

struct ComputerMoves getAllPossibleMoves(struct Board* board, int x, int y)
{
    struct ComputerMoves moves = { 0 };
    if (x > 0 && y > 0)
    if (board->cells[POS(x - 1, y - 1)] == EMPTY)
        moves.directions[moves.count++] = BACK_LEFT;

    if (x < 7 && y > 0)
    if (board->cells[POS(x + 1, y - 1)] == EMPTY)
        moves.directions[moves.count++] = BACK_RIGHT;

    if (x > 0 && y < 7)
    if (board->cells[POS(x - 1, y + 1)] == EMPTY)
        moves.directions[moves.count++] = FORWARD_LEFT;

    printf("%d", POS(x + 1, y + 1));
    if (x < 7 && y < 7)
    if (board->cells[POS(x + 1, y + 1)] == EMPTY)
        moves.directions[moves.count++] = FORWARD_RIGHT;

    moves.x = x;
    moves.y = y;

    return moves;
}

void computerMove(struct Board* board)
{
    struct ComputerMoves moves[12];
    int count = 0;
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            if (board->cells[POS(i, j)] == COMPUTER)
            {
                moves[count] = getAllPossibleMoves(board, i, j);
                if (moves[count].count)
                    ++count;
            }
        }
    }

    if (!count)
        return;
    int moveNumber = rand() % count;
    int x = moves[moveNumber].x;
    int y = moves[moveNumber].y;

    if (!moves[moveNumber].count)
        return;

    int directionNumber = rand() % moves[moveNumber].count;
    enum Directions direction = moves[moveNumber].directions[directionNumber];

    switch (direction)
    {
    case BACK_LEFT:
        board->cells[POS(x - 1, y - 1)] = COMPUTER;
        break;
    case BACK_RIGHT:
        board->cells[POS(x + 1, y - 1)] = COMPUTER;
        break;
    case FORWARD_LEFT:
        board->cells[POS(x - 1, y + 1)] = COMPUTER;
        break;
    case FORWARD_RIGHT:
        board->cells[POS(x + 1, y + 1)] = COMPUTER;
        break;
    }
    board->cells[POS(x, y)] = EMPTY;
}

int tryBeat(struct Board* board, int attackingPlayer, int attackedPlayer, int isBeat = 1)
{
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            if (board->cells[POS(i, j)] == attackingPlayer)
            {
                if (i > 1 && j > 1)
                if (shouldBeat(board, i, j, i - 2, j - 2, attackingPlayer, attackedPlayer))
                {
                    if (isBeat)
                        beat(board, i, j, i - 2, j - 2, COMPUTER);
                    return 1;
                }
                if (i < 6 && j > 1)
                if (shouldBeat(board, i, j, i + 2, j - 2, attackingPlayer, attackedPlayer))
                {
                    if (isBeat)
                        beat(board, i, j, i + 2, j - 2, COMPUTER);
                    return 1;
                }
                if (i > 1 && j < 6)
                if (shouldBeat(board, i, j, i - 2, j + 2, attackingPlayer, attackedPlayer))
                {
                    if (isBeat)
                        beat(board, i, j, i - 2, j + 2, COMPUTER);
                    return 1;
                }
                if (i < 6 && j < 6)
                if (shouldBeat(board, i, j, i + 2, j + 2, attackingPlayer, attackedPlayer))
                {
                    if (isBeat)
                        beat(board, i, j, i + 2, j + 2, COMPUTER);
                    return 1;
                }
            }
        }
    }

    return 0;
}

int main()
{
    srand(time(0));

    struct Board board;
    initBoard(&board);
    startGame(&board);

    int currentPosX;
    int currentPosY;
    int moveX;
    int moveY;

    while (1)
    {
        printBoard(&board);

        printf("Select>");
        scanf("%d %d", &currentPosX, &currentPosY);
        selectUnit(&board, currentPosX, currentPosY);
        reDrawBoard(&board, currentPosX, currentPosY);

        printf("Move>");
        scanf("%d %d", &moveX, &moveY);
        deselectUnit(&board, currentPosX, currentPosY);

        // player turn

        if (tryBeat(&board, PLAYER, COMPUTER, 0))
        {
            if (moveDistance(currentPosX, currentPosY, moveX, moveY) == 2)
                playerMove(&board, currentPosX, currentPosY, moveX, moveY);
            else
                continue;
        }
        else
        {
            playerMove(&board, currentPosX, currentPosY, moveX, moveY);
        }

        // computer turn

        if (!tryBeat(&board, COMPUTER, PLAYER))
            computerMove(&board);
    }

    return 0;
}
