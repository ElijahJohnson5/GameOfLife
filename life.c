#include "life.h"

/** 
 * @file life.c
 * @breif Functions to run the game of life. Includes everything from initilizing the matrix to checking the adjacent tiles
 * @details In the function, check_surrounding there are three different cases to check for the different edges. In all three of them the ternerary operator is used to decide where to start checking and then the checking proceeds from there. In the torus case the edges get wrapped around and it checks the opposite side. In the klein case the edges get wrapped around and the Y coordinate gets flipped if it leaves the left or right side of the screen.
 * @author Elijah Johnson
 * @date 12/5/2016
 * @bug Klein bottle does not work with starting coordinates on the edges, otherwise no known bugs.
 * @todo Implement the klein bottle edge and make a struct so not as many things have to be passed to the functions
 */

/**
 * Intilizes a 2d array of chars, with number of rows = to rows and num cols = to cols
 * @param rows the amount of rows in the array
 * @param cols the cols in the array
 * @return the pointer to the 2d array of chars
 */
unsigned char **init_matrix(int rows, int cols)
{
        int i;
        int j;
        unsigned char **a = NULL;

        a = malloc(sizeof(unsigned char *) * rows); /*mallocs the rows */

        if (!a) {
                printf("Malloc failed\n");
                return NULL;
        }

        for (i = 0; i < rows; i++) {
                a[i] = malloc(sizeof(unsigned char) * cols);/*mallocs the cols, if one fails frees everything */

                if (!a[i]) {
                        for (j = i; j > 0; j--) {
                                free(a[j]);

                        }
                        free(a);

                        return NULL;
                }
        }

        for (j = 0; j < rows; j++) {
                for (i = 0; i < cols; i++) { /*sets everything to 0 */
                        a[j][i] = 0;
                }
        }

        return a;
}

/**
 * Checks every cell on the board and the adjacent cells around that cell
 * @param **matrix the matrix to be checked
 * @param **next_matrix the matrix to be filled with the next generation
 * @param *matrix_data a pointer to the main data for the matrix
 */
void check_board(unsigned char **matrix, unsigned char **next_matrix, struct data_t *matrix_data)
{
        int i;
        int j;

        for (i = 0; i <= matrix_data->row_matrix; i++) {
                for (j = 0; j <= matrix_data->col_matrix; j++) {
                        next_matrix = change_board(matrix, next_matrix, i % matrix_data->row_matrix, j % matrix_data->col_matrix, 
                                        check_surrounding(matrix, i % matrix_data->row_matrix, j % matrix_data->col_matrix,
                                                matrix_data->row_matrix, matrix_data->col_matrix, matrix_data->edge));
                }
        }


}

/**
 * Checks the tiles surrounding the one being checked currently based on the edge and keeps count
 * @param **matrix the matrix in which the point is located
 * @param row_alive the row currently being checked
 * @param col_alive the col currently being checked
 * @param row_matrix the maximum row for the matrix
 * @param col_matrix the maximum col for the matrix
 * @param edge the edge either torus klein or hedge, 1 2 or 3
 * @return count the number of alive cells counted around the one being checked
 */
int check_surrounding(unsigned char **matrix, int row_alive, int col_alive, int row_matrix, int col_matrix, int edge)
{
        int count = 0;
        int i;
        int j;
        int start_posX;
        int start_posY;
        int end_posX;
        int end_posY;

        row_matrix--;/* sets row matrix to the correct thing, becuase an array goes from 0 to row_matrix - 1 */
        col_matrix--;

        if (edge == 1) {
                start_posX = (row_alive - 1 < 0) ? row_alive : row_alive - 1;

                start_posY = (col_alive - 1 < 0) ? col_alive : col_alive - 1;

                end_posX = (row_alive + 1 > row_matrix) ? row_alive : row_alive + 1;

                end_posY = (col_alive + 1 > col_matrix) ? col_alive : col_alive + 1;

                for (i = start_posX; i <= end_posX; i++) { /* set a start and end pos for both col and rows and then run through all the possibilties and count */
                        for (j = start_posY; j <= end_posY; j++) {
                                if (i == row_alive && j == col_alive) {
                                        continue;
                                } else if (matrix[i][j] == 1) {
                                        count++;
                                }
                                
                        }
                }
        } else if (edge == 2) {
                /* always goes from -1 to 1 */
                for (i = -1; i < 2; i++) {
                        for (j = -1; j < 2; j++) {
                                start_posX = (row_alive + i < 0) ? row_matrix : (row_alive + i > row_matrix) ? 0 : row_alive + i;
                                start_posY = (col_alive + j < 0) ? col_matrix : (col_alive + j > col_matrix) ? 0 : col_alive + j;

                                if (start_posX == row_alive && start_posY == col_alive) { /* based on if it is an edge cell check the appropriate oposite edge */
                                        continue;
                                } else if (matrix[start_posX][start_posY] == 1) {
                                        count++;
                                }
                                
                        }
                }
                
        } else if (edge == 3) {
                for (i = -1; i < 2; i++) {
                        for (j = -1; j < 2; j++) {
                                start_posX = (row_alive + i < 0) ? row_matrix : (row_alive + i > row_matrix) ? 0 : row_alive + i;
                                start_posY = (col_alive + j < 0) ? col_matrix : (col_alive + j > col_matrix) ? 0 : col_alive + j;

                                if ((start_posX == row_matrix || start_posX == 0) && row_alive + i != start_posX) {
                                        start_posY = (start_posY == col_matrix || start_posY == 0) ? col_matrix - start_posY : col_matrix - start_posY;
                                }

                                if (start_posX == row_alive && start_posY == col_alive) { /* based on if it is an edge cell check the appropriate oposite edge */
                                        continue;
                                } else if (matrix[start_posX][start_posY] == 1) {
                                        count++;
                                }
                                
                        }
                }
        }
        return count;
}

/**
 * changes the board based on the basic rules of the game of life
 * @param **prev_matrix the matrix to be checked for alive and dead cells
 * @param **next_matrix the matrix to be filled based on the number of alive cells surrounding it
 * @param row the row to be changed
 * @param col the col to be changed
 * @param count_surroundings the number of surrounding cells that are alive
 * @return next_matrix the matrix that was updated based on the surroundings
 */
unsigned char **change_board(unsigned char **prev_matrix, unsigned char **next_matrix, int row, int col, int count_surroundings)
{
        if (count_surroundings > 3) {
                next_matrix[row][col] = 0;
        } else if (count_surroundings >= 2 && count_surroundings <= 3 && prev_matrix[row][col] == 1) {
                next_matrix[row][col] = 1;
        } else if (count_surroundings == 3 && prev_matrix[row][col] == 0) {
                next_matrix[row][col] = 1;
        } else if (count_surroundings < 2) {
                next_matrix[row][col] = 0;
        }
        return next_matrix;

}

/**
 * fills the board with a file on the hedge edge
 * @param **matrix the matrix to be filled with 1's
 * @param row the row to be filled
 * @param col the col to be filled
 * @param x the offset to the row that the user wanted
 * @param y the offset to the col that the user wanted
 * @param min the minimum value for the file which is being used to fill the board
 * @param row_matrix the maximum row in the matrix
 * @param col_matrix the maximum col in the matrix
 * @return matrix the filled board
 */
unsigned char **fill_board(unsigned char **matrix, int row, int col, int x, int y, int min, int row_matrix, int col_matrix)
{      
        row += (min > 0) ? min : -min;
        col += (min > 0) ? min : -min;

        if (row + x > row_matrix || row + x < 0) {
                printf("Invalid starting x coordinates\n");
                exit(1);
        } else if (col + y > col_matrix || col + y < 0) { /*if the offset makes it a point off the board it is invalid */
                printf("Invalid starting y coordinates\n");
                exit(1);
        }

        matrix[row + x][col + y] = 1;
        
        return matrix;
}

/**
 * fills the board based on a torus shape
 * @param **matrix the matrix to be filled
 * @param row the row at which to change
 * @param col the col at which to change
 * @param x the offset the user wants
 * @param y the offset of the y coordinate that the user wants
 * @param row_matrix the maximum row in the matrix
 * @param col_matrix the maximum col in the matrix
 * @return matrix the filled board
 */
unsigned char **fill_board_torus(unsigned char **matrix, int row, int col, int x, int y, int row_matrix, int col_matrix)
{
        while (row + x < 0) {
                row += row_matrix;
        }

        while (row + x >= row_matrix) {
                row = row - row_matrix;
        }

        /*changes the row based on what the user entered and uses the torus shape */
        while (col + y < 0) {
                col = col_matrix + col;
        } 
        
        while (col + y >= col_matrix) {
                col = col - col_matrix;
        }

        matrix[row + x][col + y] = 1;
        return matrix;
}

/**
 * fills the board based off of a klein shape
 * @param **matrix the board to be filled with ones based on the row and cols
 * @param row the row to fill the board at
 * @param col the col to fill the board at
 * @param x the x offset that the user wanted
 * @param y the y offset that the user wanted
 * @param row_matrix the total rows in the matrix
 * @param col_matrix the total cols in the matrix
 * @return the matrix which was filled with ones
 */
unsigned char **fill_board_klein(unsigned char **matrix, int row, int col, int x, int y, int row_matrix, int col_matrix) 
{
        while (row + x < 0) {
                row += row_matrix;
                col = col_matrix - col;
        }

        while (row + x >= row_matrix) {
                row = row - row_matrix;
                col = col_matrix - col;
        }

        while (col + y < 0) {
                col += col_matrix;
        }

        while (col + y >= row_matrix) {
                col = col - col_matrix;
        }

        matrix[row + x][col + y] = 1;

        return matrix;
}

/**
 * sets a matrix to all zeros
 * @param **matrix the matrix to be set to zero
 * @param rows the number of rows in the matrix
 * @param cols the number of cols in the matris
 */
void set_zero(unsigned char **matrix, int rows, int cols)
{
        int i;
        int j;

        for (i = 0; i < rows; i++) {
                for (j = 0; j < cols; j++) { /*walks through making each cell 0 */
                        matrix[i][j] = 0;
                }
        }
}

/**
 * finds the minimum value, used when filling a hedge matrix
 * @param *starting_condition the file that is going to be used to fill the board
 * @param *buf a character buffer to read in the file
 * @return the min of the file
 */
int find_min_hedge(FILE *starting_condition, char *buf)
{
        int min = 1000;
        int first;
        int second;
        int i;

        while (fgets(buf, SIZE, starting_condition)) { /*starts reading the file line by line */
                if (buf[0] == '#') {
                        continue;
                }

                i = SIZE;
                while (isspace(buf[i])) {
                        buf[i] = '\0'; /*remove white space */
                        i--;
                }
                first = atoi(strtok(buf, " "));
                second = atoi(strtok(NULL, "\0")); /*break apart the file and store in ints */
                /* determine what min is */
                min = (first <= second) ? (first <= min) ? first : min : (second <= min) ? second : min;
        }

        /*close the file pointer */
        fclose(starting_condition);
        return min;
}

/**
 * walks through a life 106 file and takes out the coordinates, fills a board based on the edge
 * @param *starting_condition a file pointer to the file that is being used to fill the board
 * @param **matrix the matrix to fill with the file instructions
 * @param *filepath the filepath for the file, and for the starting_condition to point to
 * @param x the offset for the x value that the user entered
 * @param y the offset for the y value that the user entered
 * @param *matrix_data a pointer to the structure containing the main data for the matrices
 */
void parse_file(FILE *starting_condition, char *buf, unsigned char **matrix, char *filepath, int x, int y, struct data_t *matrix_data)
{
        int first;
        int second;
        int i;
        
        int min = find_min_hedge(starting_condition, buf);

        starting_condition = fopen(filepath, "r");

        while (fgets(buf, SIZE, starting_condition)) { /*walks through the file that is opened */
                if (buf[0] == '#') {
                        continue;
                }

                i = SIZE;
                while (isspace(buf[i])) { /*removes white space */
                        buf[i] = '\0';
                        i--;
                }
                first = atoi(strtok(buf, " "));
                second = atoi(strtok(NULL, "\0")); /*breaks up the buf based on a space and stores the coordinates in two ints */
                if(matrix_data->edge == 1) {
                        matrix = fill_board(matrix, first, second, x, y, min, matrix_data->row_matrix, matrix_data->col_matrix); /*fills board based on the edge the user wanted */
                } else if (matrix_data->edge == 2) {
                        matrix = fill_board_torus(matrix, first, second, x, y, matrix_data->row_matrix, matrix_data->col_matrix);
                } else if (matrix_data->edge == 3) {
                        matrix = fill_board_klein(matrix, first, second, x, y, matrix_data->row_matrix, matrix_data->col_matrix);
                }
        }

        fclose(starting_condition);
}

void init_struct(struct data_t *matrix_data)
{
        matrix_data->red = 140;
        matrix_data->green = 145;
        matrix_data->blue = 250;
        matrix_data->sprite_size = 8;
        matrix_data->width = 1280;
        matrix_data->height = 720;
        matrix_data->edge = 1;
        matrix_data->row_matrix = matrix_data->width / matrix_data->sprite_size;
        matrix_data->col_matrix = matrix_data->height / matrix_data->sprite_size;
}
