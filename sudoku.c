#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "bitboard.h"
#include "sudoku.h"
#include <time.h>
#include <math.h>
#include <unistd.h>

void pprint_bitboard81(Bitboard96 bb)
{
    printf("\n");
    pprint_bitboard96(bb, '*', 0, 81, 9);
    printf("\n");
}

Bitboard96 row_bitboard(int row_index)
{
    Bitboard96 bb = newBitboard96();
    for (int i = row_index * 9; i < (row_index + 1) * 9; i++)
    {
        set_bit(&bb, i);
    }
    return bb;
}

Bitboard96 col_bitboard(int col_index)
{
    Bitboard96 bb = newBitboard96();
    for (int i = col_index; i < 81; i += 9)
    {
        set_bit(&bb, i);
    }
    return bb;
}

int get_row_from_index(int i)
{
    return floor(i / 9);
}

int get_col_from_index(int i)
{
    return i % 9;
}

int count_zeros(Bitboard96 num)
{
    int count = 0;
    __int128 mask = ~0;

    while (num > 0)
    {
        if ((num & mask) == 0)
        {
            count++;
        }
        num >>= 1;
        // printf("%u\n", count);
    }

    return count;
}

int count_ones(__int128 num)
{
    int count = 0;
    __int128 mask = 1;

    while (num > 0)
    {
        if ((num & mask) != 0)
        {
            count++;
        }
        num >>= 1;
    }

    return count;
}

Bitboard96 get_available_rows(Bitboard96 *bb)
{
    Bitboard96 result = 0;
    for (int i = 0; i < 9; i++)
    {
        Bitboard96 _row_bb = row_bitboard(i);

        // printf("for row %u \n", i);
        // pprint_bitboard81(_row_bb);
        Bitboard96 available = _row_bb & *bb;
        if (count_ones(available) == 9)
        {
            // printf("available to place at row %u ()", 8);
            result |= available;
        }
        // printf("zeors %u", count_ones(available));
        // pprint_bitboard81(available);
    }

    return result;
}

Bitboard96 get_available_cols(Bitboard96 *bb)
{
    Bitboard96 result = 0;
    for (int i = 0; i < 9; i++)
    {

        // printf("for col %u \n", i);
        Bitboard96 _col_bb = col_bitboard(i);
        Bitboard96 available = _col_bb & *bb;

        // pprint_bitboard81(available);
        if (count_ones(available) == 9)
        {
            result |= available;
        }
    }

    return result;
}

int is_solved(Sudoku *sudoku)
{
    if (count_zeros(sudoku->empty) == 81)
    {
        printf("!!! SOLVED!");
        return 1;
    }
    return 0;
}

Sudoku *solve(Sudoku *sudoku)
{
    // printf("-<");
    // sleep(1);

    if (is_solved(sudoku))
    {
        printf("solved!!!");
        pprint_sudoku(*sudoku);
        return sudoku;
    }

    for (int i = 0; i < 9; i++)
    {
        // printf("for %u \n", i);
        Bitboard96 _available_squares = ~(sudoku->boards[i]);
        // pprint_bitboard81(_available_squares);
        // printf("empties: \n");
        // pprint_bitboard81(sudoku->empty);
        Bitboard96 _available_in_row = get_available_rows(&_available_squares);
        // printf("row mask: \n");
        // pprint_bitboard81(_available_in_row);

        // pprint_bitboard81(_available_in_row);
        Bitboard96 _available_in_col = get_available_cols(&_available_squares);
        // printf("col mask: \n");
        // pprint_bitboard81(_available_in_col);

        // pprint_bitboard81(_available_in_col);
        Bitboard96 _fill_matrix = _available_in_row & _available_in_col & sudoku->empty;
        Bitboard96 _og_fill_matrix = _fill_matrix;

        if (_fill_matrix == 0) return NULL;
        // printf("fill matrix: \n");

        // pprint_bitboard96(_fill_matrix, '*', 0, 96, 8);
        pprint_bitboard81(_fill_matrix);
        int index_of_first_position = index_of_fist_one(_fill_matrix);
        printf("INDEX OF FIRST ONE: %u\n", index_of_first_position);
        int current_index = index_of_first_position;

        while (_fill_matrix != 0 && current_index >= 0 && current_index < 81)
        {
            printf("-> go in \n");
            // printf("_fill_matrix is valid! %u", i);
            // printf("current index: %u", current_index);
            // pprint_bitboard81(_fill_matrix);
            // if (index_of_first_position > 81)
            // {
            //     printf("this should not happen (i: %u) \n", i);
            //     printf("empties: \n");
            //     pprint_bitboard81(sudoku->empty);

            //     printf("col mask: \n");
            //     pprint_bitboard81(_available_in_col);

            //     printf("fill matrix: \n");
            //     pprint_bitboard81(_fill_matrix);

            //     pprint_sudoku(*sudoku);
            //     exit(1);
            //     // return NULL;
            // }

            // printf("i: %u", index_of_first_position);
            Sudoku temp_sudoku = *sudoku;

            // printf("index of first one: %u \n", index_of_first_position);

            Bitboard96 fill = oneHotBitboard96(current_index);

            pprint_sudoku(temp_sudoku);
            printf("[%u: i] fill! (%u) current-index \n", i, current_index);
            // printf("-> fill is\n");
            // pprint_bitboard81(fill);
            // pprint_bitboard81(_og_fill_matrix);
            // pprint_bitboard81(_fill_matrix);
            // pprint_sudoku(temp_sudoku);
            // if (current_index == 63) {
            //     exit(1);
            // }

            // printf("fill matrix: \n");
            // pprint_bitboard81(fill);
            // Bitboard96 _fill = fill | temp_sudoku.boards[i];
            temp_sudoku.boards[i] |= fill;
            temp_sudoku.empty &= ~fill;

            Sudoku *result = solve(&temp_sudoku);
            if (result != NULL)
            {
                return result;
            }

            // _fill_matrix >>= (index_of_first_position + 1);
            _fill_matrix >>= (index_of_first_position + 1);
            // pprint_bitboard81(_fill_matrix);
            index_of_first_position = index_of_fist_one(_fill_matrix);
            current_index += index_of_first_position;
            // printf("[%u] next -> \nfill! current-index: %u\n (index of first position: %u) (_fill_matrix: %llu%016llu)", i, current_index, index_of_first_position, _fill_matrix >> 64, _fill_matrix);
            // pprint_bitboard81(_og_fill_matrix);
        };
    }

    printf("\n");
    pprint_sudoku(*sudoku);
    printf("\n");

    // printf(">-");
    return NULL;
}

Sudoku newSudoku()
{
    Sudoku sudoku;
    for (int i = 0; i < 9; i++)
    {
        sudoku.boards[i] = newBitboard96();
    }

    sudoku.empty = ~0;
    for (int i = 0; i < 9; i++)
    {
        set_bit(&sudoku.boards[i], i * 9 + i);
        clear_bit(&sudoku.empty, i * 9 + i);
    }
    // sudoku = shuffle(sudoku);
    return sudoku;
}

/*

    fill sq x with one:
        1) square must not be occupied
            (bb[0] | bb[1] | bb[2])[sq] ... === 0)
        2) row_bitboard(row_index) & one_bitboard === 0
        2) col_bitboard(row_index) & col_bitboard === 0

*/

// Bitboard96 ROW_BITBOARDS[] = {
//     {
//         high : 0b11111111100000000000000000000000,
//         low : 0
//     },
//     {
//         high : 0b11111111100000000000000,
//         low : 0
//     },
//     {
//         high : 0b11111111100000,
//         low : 0
//     },
//     {
//         high : 0b11111,
//         low : 0b1111000000000000000000000000000000000000000000000000000000000000
//     }
// }

//...

/*


0000 0000 0000 0000
0000 0000 0001 1111
1111
000 000 000
000 000 000

000 000 000
000 000 000
000 000 000
*/

void shuffle_board(Bitboard96 *board)
{
}

void pprint_sudoku(Sudoku sudoku)
{
    for (int yy = 0; yy < 9; yy++)
    {
        for (int xx = 0; xx < 9; xx++)
        {
            int is_empty = 1;
            for (int n = 0; n < 9; n++)
            {
                if (is_bit_set(&sudoku.boards[n], yy * 9 + xx) == 1)
                {
                    printf(" %u ", n + 1);
                    is_empty = 0;
                    break;
                }
            }

            if (is_empty == 1)
            {
                printf(" _ ");
            }
        }
        printf("\n");
    }

    // printf("empty bit: ");
    // pprint_bitboard81(sudoku.empty);
    // printf("\n");
}
