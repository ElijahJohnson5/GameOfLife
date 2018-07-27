#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "SDL2/SDL.h" 
#include "sdl.h"
#include "life.h"

/** 
 * @file gl.c
 * @breif implements the game of life and renders it to the screen, uses command line arguments for anything the user wants to enter
 * @details A structure is used to hold most of the data for the matrices and some base values are set inside of that structure if the user does not enter any command line arguments. Rendering is done by displaying one grid and figuring out the next generation on the other then displaying the second grid. Each grid is cleared before being filled with the next generation.
 * @author Elijah Johnson
 * @date 12/5/2016
 * @bug Klein does not work with starting coordinates on the edges, otherwise everything works as intended
 * @todo get klein working on edges and break up this file into better functions
 */

int main(int argc, char *argv[])
{
        /* intial settings for game of life */
        /* either 2, 4, 8, or 16 */
	int m = 9999;
	int n = 9999;
        int i;
        unsigned char **first_matrix = NULL; /* matrices being used to store 1's and 0's what will be rendered to the screen */
        unsigned char **second_matrix = NULL;
        FILE *starting_condition = NULL; /*file the user wants */
        char buf[SIZE];
        char *filepath = NULL;
        char directory[SIZE] = "Conway_Life/";
        int c;
        int matrix = 0;
        struct sdl_info_t sdl_info; /* this is needed to graphically display the game */
        struct data_t *matrix_data = malloc(sizeof(struct data_t));

        if (!matrix_data) {
                printf("Malloc failed exiting\n");
                exit(1);
        }

        init_struct(matrix_data);

        if (argc == 1) {
                printf("Usage: \n./life -w width -h height -e edge -r red -g green -b blue");
                printf(" -s sprite size -f filename -o starting position -H help\n");
        } /*print usage if no other args were entered */
        

        /*procsses all of the arguments */
        while ((c = getopt(argc, argv, ":w:h:e:r:g:b:s:f:o:H")) != -1) {
                        switch (c) {
                        case 'w':
                                if (atoi(optarg) > 1) {
                                        matrix_data->width = atoi(optarg);
                                }
                                break; /*sets width and height */
                        case 'h':
                                if (atoi(optarg) > 1) {
                                        matrix_data->height = atoi(optarg);
                                }
                                break;
                        case 'e':
                                if (strncmp("hedge", optarg, strlen(optarg)) == 0) {
                                        matrix_data->edge = 1;
                                } else if (strncmp("torus", optarg, strlen(optarg)) == 0) {
                                        matrix_data->edge = 2; /*sets the edge */
                                } else if (strncmp("klein", optarg, strlen(optarg)) == 0) {
                                        matrix_data->edge = 3;
                                }

                                break;
                        case 'r':
                                matrix_data->red = atoi(optarg);

                                matrix_data->red = (matrix_data->red > 255) ? 255 : (matrix_data->red < 0) ? 0 : matrix_data->red;
                                break;
                        case 'g':
                                matrix_data->green = atoi(optarg); /*color of sprite the user wants */

                                matrix_data->green = (matrix_data->green > 255) ? 255 : (matrix_data->green < 0) ? 0 : matrix_data->green;
                                break;
                        case 'b':
                                matrix_data->blue = atoi(optarg);

                                matrix_data->blue = (matrix_data->blue > 255) ? 255 : (matrix_data->blue < 0) ? 0 : matrix_data->blue;
                                break;
                        case 's':
                                matrix_data->sprite_size = atoi(optarg);

                                if (matrix_data->sprite_size != 2 && matrix_data->sprite_size != 4 && matrix_data->sprite_size != 8 && matrix_data->sprite_size != 16) {
                                        printf("Sprite size not a valid size, valid sizes include 2, 4, 8, and 16\n");
                                        printf("Resetting to default size\n");
                                        matrix_data->sprite_size = 8; /*size of sprite they want */
                                }

                                m = matrix_data->row_matrix / 2;
                                n = matrix_data->col_matrix / 2;
                                break;
                        case 'f':
                                errno = 0;
                                filepath = strncat(directory, optarg, strlen(optarg));
                                starting_condition = fopen(filepath, "r");
                                /*file they want the program to run with */

                                if (!starting_condition) {
                                        fprintf(stderr, "%s: argument to option '-f' failed: %s\n", argv[0], strerror(errno));
                                        exit(1);
                                }
                                break;
                        case 'o':
                                m = atoi(strtok(optarg, ","));
                                n = atoi(strtok(NULL, ","));
                                
                                if ((m < 0 || m >= matrix_data->row_matrix) && matrix_data->edge == 1) {
                                        printf("Invalid starting x coordinate for the hedge edge\nResetting to default\n");
                                        m = matrix_data->row_matrix / 2;
                                }
                                if ((n < 0 || n >= matrix_data->col_matrix) && matrix_data->edge == 1) {
                                        printf("Invalid starting y coordinate for the hedge edge\nResetting to default\n");
                                        n = matrix_data->col_matrix / 2;
                                } /*starting coordinates for the file */


                                break;
                        case 'H':
                                printf("Usage: \n./life -w width -h height -e edge -r red -g green -b blue");
                                printf(" -s sprite size -f filename -o starting position -H help\n");
                                printf("w: width of screen that you want\nh: height of screen that you want\n");
                                printf("e: type of edge either hedge, torus, or klein\n");
                                printf("r: red value in rgb, between 255 and 0\n");
                                printf("g: green value in rgb, between 255 and 0\nb: blue value in rgb, between 255 and 0\n");
                                printf("s: size of the sprite you want can be 2, 4, 8, or 16\n");
                                printf("f: file from which the initial pattern will be taken\n");
                                printf("o: x,y starting positions, entered with no space and a comma\n");
                                printf("H: help menu display\n");
                                exit(1);
                                break;
                        case ':':
                                fprintf(stderr, "%s: option '-%c' requires an argument\n", argv[0], optopt);
                                exit(1); /*error processing */
                        case '?':
                        default:
                                printf("Illegal option %c - ignored\n", optopt);
                                printf("Usage: \n./life -w width -h height -e edge -r red -g green -b blue");
                                printf(" -s sprite size -f filename -o starting position -H help\n");

                                break;
                        }
        }

        if (m == 9999) {
                m = matrix_data->row_matrix / 2;
        }

        if (n == 9999) {
                n = matrix_data->col_matrix / 2;
        }

        if (!starting_condition) {
                /*default file is a glider */
                 filepath = strncat(directory, "glider_106.lif", strlen("glider_106.lif"));
        }


        /* intilize the matrices */
        first_matrix = init_matrix(matrix_data->row_matrix, matrix_data->col_matrix);
        second_matrix = init_matrix(matrix_data->row_matrix, matrix_data->col_matrix);
        /* set up SDL -- works with SDL2 */
	init_sdl_info(&sdl_info, matrix_data->width, matrix_data->height, matrix_data->sprite_size, matrix_data->red, matrix_data->green, matrix_data->blue);

	/* your life initialization code here */

        starting_condition = fopen(filepath, "r");
        if (matrix_data->edge == 1 && n == (matrix_data->col_matrix) / 2) {
                n = 2 * matrix_data->sprite_size;
        }

        parse_file(starting_condition, buf, first_matrix, filepath, m, n, matrix_data);

        sdl_render_life(&sdl_info, first_matrix);
        check_board(first_matrix, second_matrix, matrix_data);
        matrix = 1;
        /* Main loop: loop forever. */
	while (1)
	{
		/* your game of life code goes here  */		

		/* change the  modulus value to slow the rendering */
		if (SDL_GetTicks() % 35 == 0) {
                        if (matrix == 0) {
                                /*filp back and forth between the two matrices */
                                set_zero(second_matrix, matrix_data->row_matrix, matrix_data->col_matrix);
			        sdl_render_life(&sdl_info, first_matrix);
                                check_board(first_matrix, second_matrix, matrix_data);
                                matrix = 1;
                        } else {
                                set_zero(first_matrix, matrix_data->row_matrix, matrix_data->col_matrix);
                                sdl_render_life(&sdl_info, second_matrix);
                                check_board(second_matrix, first_matrix, matrix_data);
                                matrix = 0;
                        }
                }
                 /* Poll for events, and handle the ones we care about. 
                  * You can click the X button to close the window
                  */
		SDL_Event event;
		while (SDL_PollEvent(&event)) 
		{
			switch (event.type) 
			{
			case SDL_KEYDOWN:
				break;
			case SDL_KEYUP:
                        /* If escape is pressed, return (and thus, quit) */
				if (event.key.keysym.sym == SDLK_ESCAPE)
					return 0;
				break;
			case SDL_QUIT:
				return(0);
			}
		}
	}

        /* free all allocated memory */
        fclose(starting_condition);
        for (i = matrix_data->col_matrix; i > 0; i--) {
                free(first_matrix[i]);
                free(second_matrix[i]);
        }
        free(first_matrix);
        free(second_matrix);
	return 0;
}
