#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#define SIZE 4096

/**
 * @file life.h
 * @breif headers for all of the functions in life.c
 * @author Elijah Johnson
 * @date 12/5/2016
 * @bug none known
 * @todo implement klein bottle
 */

#ifndef LIFE_H_
#define LIFE_H_

struct data_t {
        int row_matrix;
        int col_matrix;
        int edge;
        int height;
        int width;
        int sprite_size;
        unsigned char red;
        unsigned char green;
        unsigned char blue;
};

unsigned char **init_matrix(int rows, int cols);

void check_board(unsigned char **matrix, unsigned char **next_matrix, struct data_t *matrix_data);

int check_surrounding(unsigned char **matrix, int row_alive, int col_alive, int row_matrix, int col_matrix, int edge);

unsigned char **change_board(unsigned char **prev_matrix, unsigned char **next_matrix, int row, int col, int count_surrounding);

unsigned char **fill_board(unsigned char **matrix, int row, int col, int x, int y, int min, int row_matrix, int col_matrix);

void set_zero(unsigned char **matrix, int rows, int cols);

unsigned char **fill_board_torus(unsigned char **matrix, int row, int col, int x, int y, int row_matrix, int col_matrix);

unsigned char **fill_board_klein(unsigned char **matrix, int row, int col, int x, int y, int row_matrix, int col_matrix); 

int find_min_hedge(FILE *starting_condition, char *buf);

void parse_file(FILE *starting_condition, char *buf, unsigned char **matrix, char *filepath, int x, int y, struct data_t *matrix_data);

void init_struct(struct data_t *matrix_data);
#endif
