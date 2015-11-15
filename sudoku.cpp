// sudoku.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NUM_CL_ROW  4
#define MAX_NUM_CL_COL  4
#define MAX_SUDOKU_SIZE	16

#define NOT_SET		0
#define IS_SET		1
#define AVAIL		0

#define SUCCESS		1
#define FAILURE		0

typedef struct
{
	bool IsSet;
	unsigned char Digit;
	unsigned char Maybe[MAX_SUDOKU_SIZE + 1];
} CELL;

typedef struct
{
	CELL Cell[MAX_NUM_CL_ROW][MAX_NUM_CL_COL];
} SQUARE;

int Num_Sq_Row = 3;
int Num_Sq_Col = 3;
int Num_Cl_Row = 3;
int Num_Cl_Col = 3;
int Sudoku_Size = 9;
int Total_Sum = (((Sudoku_Size*Sudoku_Size) + Sudoku_Size)/2);

void SetCell(SQUARE ** Square, int SquareRow, int SquareCol, int CellRow, int CellCol, unsigned char Number)
{
	int i;
	int j;

	Square[SquareRow][SquareCol].Cell[CellRow][CellCol].IsSet = IS_SET;
	Square[SquareRow][SquareCol].Cell[CellRow][CellCol].Digit = Number;

	for (i = 0; i < Num_Sq_Col; i++)
	{
		for (j = 0; j < Num_Cl_Col; j++)
			Square[SquareRow][i].Cell[CellRow][j].Maybe[Number]++;
	}

	for (i = 0; i < Num_Sq_Row; i++)
	{
		for (j = 0; j < Num_Cl_Row; j++)
			Square[i][SquareCol].Cell[j][CellCol].Maybe[Number]++;
	}

	for (i = 0; i < Num_Cl_Row; i++)
	{
		for (j = 0; j < Num_Cl_Col; j++)
			Square[SquareRow][SquareCol].Cell[i][j].Maybe[Number]++;
	}

}

void UnSetCell(SQUARE ** Square, int SquareRow, int SquareCol, int CellRow, int CellCol, unsigned char Number)
{
	int i;
	int j;

	Square[SquareRow][SquareCol].Cell[CellRow][CellCol].IsSet = NOT_SET;
	Square[SquareRow][SquareCol].Cell[CellRow][CellCol].Digit = 0;

	for (i = 0; i < Num_Sq_Col; i++)
	{
		for (j = 0; j < Num_Cl_Col; j++)
			Square[SquareRow][i].Cell[CellRow][j].Maybe[Number]--;
	}

	for (i = 0; i < Num_Sq_Row; i++)
	{
		for (j = 0; j < Num_Cl_Row; j++)
			Square[i][SquareCol].Cell[j][CellCol].Maybe[Number]--;
	}

	for (i = 0; i < Num_Cl_Row; i++)
	{
		for (j = 0; j < Num_Cl_Col; j++)
			Square[SquareRow][SquareCol].Cell[i][j].Maybe[Number]--;
	}

}

int GetAllAvail(int * Random, SQUARE ** Square, int SquareRow, int SquareCol, int CellRow, int CellCol, bool Randomize)
{
	int i = 1;
	int j = 0;
	int Temp;
	int Src;
	int Dest;

	while(i <= Sudoku_Size)
	{
		if (Square[SquareRow][SquareCol].Cell[CellRow][CellCol].Maybe[i] == AVAIL)
		{
			Random[j] = i;
			j++;
		}
		i++;
	}

	if (j > 0 && Randomize)
	{
		for (i = 0; i < Sudoku_Size; i++)
		{
			Src = rand() % j;
			Dest = rand() % j;
			
			if (Src == Dest)
				Dest = (Src + 1) % j;

			Temp = Random[Dest];
			Random[Dest] = Random[Src];
			Random[Src] = Temp;
		}
	}

	return j;
}

void GetNextAvailCell(SQUARE ** Square, 
					  int SquareRow, 
					  int SquareCol, 
					  int CellRow, 
					  int CellCol,
					  int * NewSquareRow, 
					  int * NewSquareCol, 
					  int * NewCellRow, 
					  int * NewCellCol)
{
	while ( SquareRow < Num_Sq_Row && SquareCol < Num_Sq_Col &&
			CellRow < Num_Cl_Row && CellCol < Num_Cl_Col &&
			Square[SquareRow][SquareCol].Cell[CellRow][CellCol].IsSet == IS_SET)
	{
		CellCol++;

		if (CellCol >= Num_Cl_Col)
		{
			CellCol = 0;
			CellRow++;
		}

		if (CellRow >= Num_Cl_Row)
		{
			CellRow = 0;
			SquareCol++;

			if (SquareCol >= Num_Sq_Col)
			{
				SquareCol = 0;
				SquareRow++;
			}
		}
	}

	* NewSquareRow = SquareRow;  
	* NewSquareCol = SquareCol; 
	* NewCellRow = CellRow;
	* NewCellCol = CellCol;
}

int CheckUnique (SQUARE ** Square, int SquareRow, int SquareCol, int CellRow, int CellCol, int * NumSolutions)
{
	int NewSquareRow;
	int NewSquareCol;
	int NewCellRow;
	int NewCellCol;
	int Total;
	int j = 0;
	int Random[MAX_SUDOKU_SIZE];

	if (SquareRow >= Num_Sq_Row)
		return SUCCESS;

	if ((Total = GetAllAvail(Random, Square, SquareRow, SquareCol, CellRow, CellCol, false)) <= 0)
		return FAILURE;

	SetCell(Square, SquareRow, SquareCol, CellRow, CellCol, Random[j]);

	GetNextAvailCell(Square,
					 SquareRow,
					 SquareCol,
					 CellRow,
					 CellCol,
					 &NewSquareRow,
					 &NewSquareCol,
					 &NewCellRow,
					 &NewCellCol);

	while (true)
	{
        if (CheckUnique(Square, NewSquareRow, NewSquareCol, NewCellRow, NewCellCol, NumSolutions) == SUCCESS)
            (*NumSolutions)++;

		UnSetCell(Square, SquareRow, SquareCol, CellRow, CellCol, Random[j]);

        if (*NumSolutions > 1)
            return FAILURE;

		j++;
		if (j >= Total)
			return FAILURE;

		SetCell(Square, SquareRow, SquareCol, CellRow, CellCol, Random[j]);
	}

	return SUCCESS;
}

int Solve (SQUARE ** Square, int SquareRow, int SquareCol, int CellRow, int CellCol)
{
	int NewSquareRow;
	int NewSquareCol;
	int NewCellRow;
	int NewCellCol;
	int Total;
	int j = 0;
	int Random[MAX_SUDOKU_SIZE];

	if (SquareRow >= Num_Sq_Row)
		return SUCCESS;

	if ((Total = GetAllAvail(Random, Square, SquareRow, SquareCol, CellRow, CellCol, false)) <= 0)
		return FAILURE;

	SetCell(Square, SquareRow, SquareCol, CellRow, CellCol, Random[j]);

	GetNextAvailCell(Square,
					 SquareRow,
					 SquareCol,
					 CellRow,
					 CellCol,
					 &NewSquareRow,
					 &NewSquareCol,
					 &NewCellRow,
					 &NewCellCol);

	while (Solve(Square, NewSquareRow, NewSquareCol, NewCellRow, NewCellCol) == FAILURE)
	{
		UnSetCell(Square, SquareRow, SquareCol, CellRow, CellCol, Random[j]);

		j++;
		if (j >= Total)
			return FAILURE;

		SetCell(Square, SquareRow, SquareCol, CellRow, CellCol, Random[j]);
	}

	return SUCCESS;
}

int Generate (SQUARE ** Square, int SquareRow, int SquareCol, int CellRow, int CellCol)
{
	int NewSquareRow;
	int NewSquareCol;
	int NewCellRow;
	int NewCellCol;
	int Total;
	int j = 0;
	int Random[MAX_SUDOKU_SIZE];

	if (SquareRow >= Num_Sq_Row)
		return SUCCESS;

	if ((Total = GetAllAvail(Random, Square, SquareRow, SquareCol, CellRow, CellCol, true)) <= 0)
		return FAILURE;

	SetCell(Square, SquareRow, SquareCol, CellRow, CellCol, Random[j]);

	GetNextAvailCell(Square,
					 SquareRow,
					 SquareCol,
					 CellRow,
					 CellCol,
					 &NewSquareRow,
					 &NewSquareCol,
					 &NewCellRow,
					 &NewCellCol);

	while (Generate(Square, NewSquareRow, NewSquareCol, NewCellRow, NewCellCol) == FAILURE)
	{
		UnSetCell(Square, SquareRow, SquareCol, CellRow, CellCol, Random[j]);

		j++;
		if (j >= Total)
			return FAILURE;

		SetCell(Square, SquareRow, SquareCol, CellRow, CellCol, Random[j]);
	}

	return SUCCESS;
}

int PreProcess(SQUARE ** Square)
{
	int i;
	int j;
	int k;
	int l;
    int Found;
    int Total = 1;
    int GrandTotal = 0;
    int Cl_Row;
    int Cl_Col;

    while (Total)
    {
        Total = 0;
        for (i = 0; i < Num_Sq_Row; i++)
        {
            for (j = 0; j < Num_Sq_Col; j++)
            {
                for (k = 1; k <= Sudoku_Size; k++)
                {
                    Found = 0;
                    for (l = 0; l < Num_Cl_Row * Num_Cl_Col; l++)
                    {
                        if (Square[i][j].Cell[l / Num_Cl_Col][l % Num_Cl_Col].Maybe[k] == AVAIL)
                        {
                            Found++;
                            if (Found > 1)
                                break;
                            else
                            {
                                Cl_Row = l / Num_Cl_Col;
                                Cl_Col = l % Num_Cl_Col;
                            }
                        }
                    }

                    if (Found == 1)
                    {
                        SetCell(Square, i, j, Cl_Row, Cl_Col, k);
                        Total++;
                    }
                }
            }
        }
        GrandTotal++;
    }

    return GrandTotal;
}

void PostProcess(SQUARE ** Square)
{
    int i;
    int j;
    int k;
    int l;
    int NewSquareRow;
    int NewSquareCol;
    int NewCellRow;
    int NewCellCol;
    int Num;
    int Temp;
    int Total = Num_Sq_Row * Num_Cl_Col * Num_Cl_Row * Num_Sq_Col;
    int Progress = 0;

    if (Sudoku_Size < MAX_SUDOKU_SIZE)
    {
        for(i = Num_Sq_Row - 1; i >= 0; i--)
        {
            for (k = 0; k < Num_Cl_Col; k++)
            {
                for(j = 0;j < Num_Cl_Row; j++)
                {
                    for (l = Num_Sq_Col - 1; l >= 0; l--)
                    {
                        Temp = Square[i][l].Cell[j][k].Digit;
                        UnSetCell(Square, i, l, j, k, Temp);
                        Num = 0;

                        GetNextAvailCell(Square,
                            0,
                            0,
                            0,
                            0,
                            &NewSquareRow,
                            &NewSquareCol,
                            &NewCellRow,
                            &NewCellCol);

                        CheckUnique(Square, NewSquareRow, NewSquareCol, NewCellRow, NewCellCol, &Num);

                        if (Num > 1)
                            SetCell(Square, i, l, j, k, Temp);

                        Progress++;
                        printf("Generating Unique Puzzle : %3.3f%% %c",((float)Progress * 100) / (float)Total, 13);
                    }
                }
            }
        }
    }
    else
    {
        for(i = 0; i < Num_Sq_Row; i++)
        {
            for (k = 0; k < Num_Cl_Col; k++)
            {
                for(j = 0;j < Num_Cl_Row; j++)
                {
                    for (l = 0; l < Num_Sq_Col; l++)
                    {
                        Temp = Square[i][l].Cell[j][k].Digit;
                        UnSetCell(Square, i, l, j, k, Temp);

                        Num = 0;

                        GetNextAvailCell(Square,
                            i,
                            0,
                            0,
                            0,
                            &NewSquareRow,
                            &NewSquareCol,
                            &NewCellRow,
                            &NewCellCol);

                        CheckUnique(Square, NewSquareRow, NewSquareCol, NewCellRow, NewCellCol, &Num);

                        if (Num > 1)
                            SetCell(Square, i, l, j, k, Temp);

                        Progress++;
                        printf("Generating Unique Puzzle : %3.3f%% %c",((float)Progress * 100) / (float)Total, 13);
                    }
                }
            }
        }
    }
    printf("\n\n");
}

bool CheckWin (SQUARE ** Square)
{
	int i;
	int j;
	int k;
	int l;
	int sum;

	sum = 0;
	for(i = 0; i < Num_Sq_Row; i++)
	{
		for (k = 0; k < Num_Cl_Row; k++)
		{
			for(j = 0;j < Num_Sq_Col; j++)
			{
				for (l = 0;l < Num_Cl_Col; l++)
				{
					if (Square[i][j].Cell[k][l].Digit < 1 || Square[i][j].Cell[k][l].Digit > Sudoku_Size)
						return false;
					sum += Square[i][j].Cell[k][l].Digit;
				}
			}
			if (sum != Total_Sum)
				return false;
			sum = 0;
		}
	}

	sum = 0;
	for(i = 0; i < Num_Sq_Col; i++)
	{
		for (k = 0; k < Num_Cl_Col; k++)
		{
			for(j = 0;j < Num_Sq_Row; j++)
			{
				for (l = 0;l < Num_Cl_Row; l++)
				{
					if (Square[j][i].Cell[l][k].Digit < 1 || Square[j][i].Cell[l][k].Digit > Sudoku_Size)
						return false;
					sum += Square[j][i].Cell[l][k].Digit;
				}
			}
			if (sum != Total_Sum)
				return false;
			sum = 0;
		}
	}

	sum = 0;
	for(i = 0; i < Num_Sq_Row; i++)
	{
		for (k = 0; k < Num_Sq_Col; k++)
		{
			for(j = 0;j < Num_Cl_Row; j++)
			{
				for (l = 0;l < Num_Cl_Col; l++)
				{
					if (Square[i][k].Cell[j][l].Digit < 1 || Square[i][k].Cell[j][l].Digit > Sudoku_Size)
						return false;
					sum += Square[i][k].Cell[j][l].Digit;
				}
			}
			if (sum != Total_Sum)
				return false;
			sum = 0;
		}
	}

	return true;
}

int ReadInput(SQUARE ** Square, char * InputFile)
{
	FILE * input;
	char Number;
	int Row = 0;
	int Column = 0;

	if ((input = fopen(InputFile, "r")) == NULL)
		return -1;
	
    while((Number = fgetc(input)) != EOF && Row < Sudoku_Size)
	{
		if ((Number >= 0x30 && Number <= 0x39) ||
            (Number >= 0x41 && Number <= 0x47 ))
		{
			if (Number != 0x30)
			{
                if (Number >= 0x41 && Number <= 0x47 )
                    Number -= 0x37;
                else
				    Number -= 0x30;

				SetCell(Square, 
						Row / Num_Cl_Row, 
						Column / Num_Cl_Col, 
						Row % Num_Cl_Row, 
						Column % Num_Cl_Col, 
						Number);
			}

			Column++;
			if (Column >= Sudoku_Size)
			{
				Column = 0;
				Row++;
			}
		}
	}

    fclose(input);

    if (Row < Sudoku_Size)
        return -1;

	return 0;
}

void PrintSudoku(SQUARE ** Square)
{
	int i;
	int j;
	int k;
	int l;

	for(i = 0; i < Num_Sq_Row; i++)
	{
		for (k = 0; k < Num_Cl_Row; k++)
		{
			for(j = 0;j < Num_Sq_Col; j++)
			{
				for (l = 0;l < Num_Cl_Col; l++)
				{
                    if (Square[i][j].Cell[k][l].Digit > 9)
					    printf("%c ",Square[i][j].Cell[k][l].Digit + 0x37);
                    else
                        printf("%d ",Square[i][j].Cell[k][l].Digit);
				}
			}
			printf("\n");
		}
	}
}

void PrintHelp(void)
{
    printf("Usage: sudoku [-s/-cu/-g] [Size] <[FileName]>\n");
    printf("     : -s  : Solve a Puzzle\n");
    printf("     : -cu : Check if a Puzzle has a Unique Solution\n");
    printf("     : -g  : Create a Puzzle with a Unique Solution\n");
    printf("     : [FileName] is Optional, It Contains a Puzzle to Solve\n");
    printf("     : If [FileName] is NOT provided it will generate a puzzle\n");
}

int main(int argc, char* argv[])
{
	SQUARE ** Square;
	int NewSquareRow;
	int NewSquareCol;
	int NewCellRow;
	int NewCellCol;
    int Num;
	int i;

    if (argc < 3)
    {
        PrintHelp();
        return 0;
    }

    Sudoku_Size = atoi(argv[2]);

    switch(Sudoku_Size)
    {
    case 4:
        Num_Sq_Row = 2;
        Num_Sq_Col = 2;
        Num_Cl_Row = 2;
        Num_Cl_Col = 2;
        break;
    case 6:
        Num_Sq_Row = 3;
        Num_Sq_Col = 2;
        Num_Cl_Row = 2;
        Num_Cl_Col = 3;
        break;
    case 9:
        Num_Sq_Row = 3;
        Num_Sq_Col = 3;
        Num_Cl_Row = 3;
        Num_Cl_Col = 3;
        break;
    case 12:
        Num_Sq_Row = 4;
        Num_Sq_Col = 3;
        Num_Cl_Row = 3;
        Num_Cl_Col = 4;
        break;
    case 16:
        Num_Sq_Row = 4;
        Num_Sq_Col = 4;
        Num_Cl_Row = 4;
        Num_Cl_Col = 4;
        break;
    default:
        return 0;
    }

    Total_Sum = (((Sudoku_Size*Sudoku_Size) + Sudoku_Size)/2);

	Square = (SQUARE **) malloc (sizeof(SQUARE *) * Num_Sq_Row);

	for (i = 0; i < Num_Sq_Row; i++)
	{
		Square[i] = (SQUARE *) malloc (sizeof(SQUARE) * Num_Sq_Col);
		memset(Square[i], NOT_SET, sizeof(SQUARE) * Num_Sq_Col);
	}

	if (stricmp(argv[1], "-s") == 0 && argc == 4)
	{
		if (ReadInput(Square, argv[3]) < 0)
			return 0;

        printf("\nGenerating Solution. Please Wait ...\n\n");

        PreProcess(Square);

        GetNextAvailCell(Square,
					 0,
					 0,
					 0,
					 0,
					 &NewSquareRow,
					 &NewSquareCol,
					 &NewCellRow,
					 &NewCellCol);

        Solve(Square, NewSquareRow, NewSquareCol, NewCellRow, NewCellCol);

		if (CheckWin (Square))
			PrintSudoku(Square);
		else
			printf("Unable to Generate Solution.\n");
	}
	else
    if (stricmp(argv[1], "-cu") == 0 && argc == 4)
    {
		if (ReadInput(Square, argv[3]) < 0)
			return 0;

        printf("\nGenerating Solution(s). Please Wait ...\n\n");

		GetNextAvailCell(Square,
					 0,
					 0,
					 0,
					 0,
					 &NewSquareRow,
					 &NewSquareCol,
					 &NewCellRow,
					 &NewCellCol);

        Num = 0;
        CheckUnique(Square, NewSquareRow, NewSquareCol, NewCellRow, NewCellCol, &Num);

        if (Num > 1)
            printf("More than One Solution Found.\n", Num);
        else
        if (Num == 1)
            printf("Only One Solution Found.\n", Num);
        else
            printf("No Solution Found.\n", Num);
    }
    else
    if (stricmp(argv[1], "-g") == 0)
	{
        printf("\nGenerating Solution. Please Wait ...\n\n");

		srand( (unsigned)time( NULL ) );

		Generate(Square, 0, 0, 0, 0);

		if (CheckWin (Square))
		{
			PrintSudoku(Square);
			printf("\n");

			PostProcess(Square);
			PrintSudoku(Square);
		}
		else
			printf("Unable to Generate Solution.\n");
	}

	for (i = 0; i < Num_Sq_Row; i++)
	{
		free(Square[i]);
	}

    free(Square);

    printf("\nPress Enter to Continue...");
    getchar();

	return 0;
}
