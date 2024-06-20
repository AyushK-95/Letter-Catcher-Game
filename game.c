/* This files provides address values that exist in the system */

#define SDRAM_BASE            0xC0000000
#define FPGA_ONCHIP_BASE      0xC8000000
#define FPGA_CHAR_BASE        0xC9000000

/* Cyclone V FPGA devices */
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050
#define TIMER_BASE            0xFF202000
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030
#define PS2_BASE			  0xFF200100

/* VGA colors */
#define WHITE 0xFFFF
#define YELLOW 0xFFE0
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define GREY 0xC618
#define PINK 0xFC18
#define ORANGE 0xFC00
#define BLACK 0x0

/* Screen size. */
#define RESOLUTION_X 320
#define RESOLUTION_Y 240

/* Constants for animation */

#define NUM_LETTERS 3

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

volatile int pixel_buffer_start; // global variable
volatile int* PS2_ptr = (int*)PS2_BASE;


// ***************** Function Prototypes ******************* //
void clear_screen(void);
void plot_pixel(int x, int y, short int line_color);
void plot_letter(int x, int y, short int line_color, char letter);
void plot_pixel_big(int x1, int y1,int x2,int y2, short int line_color);
void getFromKeyboard(unsigned char *keyboardValue);
void wait_for_vsync(void);
void write_char(int x, int y, char c);
// ********************************************************** //


// **************** LETTERS STRUCTURE *************************//
struct LetterInfo
{
	char letter;
	
	int x_pos;
	int y_pos;
	
	int dx;
	int dy;	
};
 
//Array of the structs
struct LetterInfo array[NUM_LETTERS];

// ********************************************************* //

int main(void)
{
    
	// random data to start
	for(int i = 0; i < NUM_LETTERS; i++){
		
		// letters
		array[i].letter = (rand() % 26) + 97; // ascii from a to z lowercase
		
		// initial position
		array[i].x_pos = rand() % 300 + 5;
		array[i].y_pos = 8; // actual size is to 239, but starting letters only in top half of screen
		
		// initial motion
		array[i].dx= 0;
		array[i].dy = rand() % 2 + 1;
		
	}
	volatile int* pixel_ctrl_ptr = (int *)0xFF203020;
	
    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();

    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;

    clear_screen(); // pixel_buffer_start points to the pixel buffer

    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    clear_screen(); // pixel_buffer_start points to the pixel buffer

	bool playGame = true;
	volatile int* SW_PTR = (int*)SW_BASE;
	int switchContents;
	
	
	// the game is played here
	while(1)
	{
		// check for reset
		switchContents = *SW_PTR;
		int count=0;
		while (playGame)
		{	
			// check for a pause or a reset
			switchContents = *SW_PTR; // switchContents is contents of switch
			
			while(switchContents == 2) // infinite while loop when game is paused, key 1
			{
				switchContents = *SW_PTR; // switchContents is contents of switch
			}
			switchContents = *SW_PTR;
			
			while(switchContents == 4) // key 2 for resetting the game
			{
				count=0;
				playGame = true;
			
				// random data to reset all
				for(int i = 0; i < NUM_LETTERS; i++)
				{
		
					// letters
					array[i].letter = (rand() % 26) + 97; // ascii from a to z lowercase
			
					// initial position
					array[i].x_pos = rand() % 300 + 5;
					array[i].y_pos = 8; // actual size is to 239, but starting letters only in top half of screen
				
					// initial motion
					array[i].dx= 0;
					//array[i].dy = rand() % 2 + 1;
					array[i].dy = count/5 + 1;
				}
				switchContents = *SW_PTR;
			}
			
			clear_screen();
			
			// drawing the threshold game line
			plot_pixel_big(0, 230, 319,231, RED);
						
			unsigned char keyboardValue = 0;
			
			// code for updating the locations of boxes
			for(int i = 0; i < NUM_LETTERS; i++)
			{
			
				getFromKeyboard(&keyboardValue);

				if((keyboardValue == 0x1c && array[i].letter == 'a') || (keyboardValue == 0x32 && array[i].letter == 'b') || (keyboardValue == 0x21 && array[i].letter == 'c') || (keyboardValue == 0x23 && array[i].letter == 'd') || (keyboardValue == 0x24 && array[i].letter == 'e') || (keyboardValue == 0x2b && array[i].letter == 'f') || (keyboardValue == 0x34 && array[i].letter == 'g') || (keyboardValue == 0x33 && array[i].letter == 'h') || (keyboardValue == 0x43 && array[i].letter == 'i') || (keyboardValue == 0x3b && array[i].letter == 'j') || (keyboardValue == 0x42 && array[i].letter == 'k') || (keyboardValue == 0x4b && array[i].letter == 'l') || (keyboardValue == 0x3a && array[i].letter == 'm') || (keyboardValue == 0x31 && array[i].letter == 'n') || (keyboardValue == 0x44 && array[i].letter == 'o') || (keyboardValue == 0x4d && array[i].letter == 'p') || (keyboardValue == 0x15 && array[i].letter == 'q') || (keyboardValue == 0x2d && array[i].letter == 'r') || (keyboardValue == 0x1b && array[i].letter == 's') || (keyboardValue == 0x2c && array[i].letter == 't') || (keyboardValue == 0x3c && array[i].letter == 'u') || (keyboardValue == 0x2a && array[i].letter == 'v') || (keyboardValue == 0x1d && array[i].letter == 'w') || (keyboardValue == 0x22 && array[i].letter == 'x') || (keyboardValue == 0x35 && array[i].letter == 'y') || (keyboardValue == 0x1a && array[i].letter == 'z'))
				{
					count++;
					// the key for a specifc letter is pressed so can now reset this letter to something new letters
					array[i].letter = (rand() % 26) + 97; // ascii from a to z lowercase
			
					// initial position
					array[i].x_pos = rand() % 300 + 5;
					array[i].y_pos = rand() % 10 + 5; // actual size is to 239, but starting letters only in top half of screen
			
					// initial motion
					array[i].dx= 0;
					//array[i].dy = rand() % 2 + 1;
					array[i].dy = count/5 + 1;
					//draw new letter
					plot_letter(array[i].x_pos, array[i].y_pos, CYAN, array[i].letter);
					
				}
				else
				{
					// hitting boundaries, need to change directions
					if(array[i].y_pos > 229)
						playGame = false;
				
					if(playGame)
					{
						// increment for new line position
						array[i].y_pos = array[i].y_pos + array[i].dy;
						array[i].x_pos = array[i].x_pos + array[i].dx;
					
						//draw new letter
						plot_letter(array[i].x_pos, array[i].y_pos, CYAN, array[i].letter);
			
					}
				}
				
			}
			
			if(playGame)
			{
				wait_for_vsync(); // swap front and back buffers on VGA vertical sync		
				pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
			}
		}
		
		if(!playGame)
		{
			//The game over screen
			
			clear_screen();
			// game over letters
			
			// G 
			plot_pixel_big(60, 20,80,30, WHITE);
			plot_pixel_big(50, 30,60,60, WHITE);
			plot_pixel_big(60, 60,80,70, WHITE);
			plot_pixel_big(80, 50,90,60, WHITE);
			plot_pixel_big(70,40,90,50, WHITE);

			// A
			plot_pixel_big(110, 30,120,70, WHITE);
			plot_pixel_big(120, 20, 140,30, WHITE);
			plot_pixel_big(120,40,140,50,WHITE);
			plot_pixel_big(140, 30, 150,70, WHITE);
			
			// M
			plot_pixel_big(170, 30,180,70,WHITE);
			plot_pixel_big(180, 20, 190,30, WHITE);
			plot_pixel_big(190, 30, 200,40, WHITE);
			plot_pixel_big(200, 20, 210,30, WHITE);
			plot_pixel_big(210, 30,220,70,WHITE);
			
			// E
			plot_pixel_big(240,20,250,70,WHITE);
			plot_pixel_big(250, 20, 270,30,WHITE);
			plot_pixel_big(250, 40, 260,50, WHITE);
			plot_pixel_big(250, 60, 270,70,WHITE);
		
			// O
			plot_pixel_big(50, 100, 60,130,WHITE);
			plot_pixel_big(60, 90,80,100, WHITE);
			plot_pixel_big(60, 130,80,140, WHITE);
			plot_pixel_big(80, 100, 90,130,WHITE);
			
			// V
			plot_pixel_big(110, 90, 120,130,WHITE);
			plot_pixel_big(120, 130, 140,140,WHITE);
			plot_pixel_big(140, 90, 150,130, WHITE);
			
			// E
			plot_pixel_big(170,90,180,140,WHITE);
			plot_pixel_big(180, 90,200,100,WHITE);
			plot_pixel_big(180, 110, 190,120, WHITE);
			plot_pixel_big(180, 130, 200,140,WHITE);
			
			// R
			plot_pixel_big(220, 90, 230,140,WHITE);
			plot_pixel_big(230, 90, 250,100,WHITE);
			plot_pixel_big(250, 100, 260,110, WHITE);
			plot_pixel_big(230, 110, 250,120,WHITE);
			plot_pixel_big(250, 120, 260,140,WHITE);
			
			// !
			plot_pixel_big(280, 90, 290, 120,WHITE);
			plot_pixel_big(280, 130, 290,140, WHITE);
			
			wait_for_vsync(); // swap front and back buffers on VGA vertical sync		
			pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
			
		}
		
		while(!playGame)
		{
			//End of game loop, checking for a reset...
			switchContents = *SW_PTR;
			if(switchContents == 4) // key 2 for resetting the game
			{
				count=0;
				playGame = true;
			
				// random data to reset all
				for(int i = 0; i < NUM_LETTERS; i++){
			
					// letters
					array[i].letter = (rand() % 26) + 97; // ascii from a to z lowercase
			
					// initial position
					array[i].x_pos = rand() % 300 + 5;
					array[i].y_pos = 8; // actual size is to 239, but starting letters only in top half of screen
				
					// initial motion
					array[i].dx= 0;
					//array[i].dy = rand() % 2 + 1;
					array[i].dy = count/5 + 1;
				}
			}
	    }
	}
}



/******************************SUBROUTINES****************************************************/

void clear_screen(void)
{
	for(int x= 0; x< 320; x++){
		for(int y= 0; y< 240; y++){
			plot_pixel(x, y, BLACK);
		}
	}
}

void write_char(int x, int y, char c) 
{
    // VGA character buffer
    volatile char * character_buffer = (char * )(0x09000000 + (y << 7) + x);
    * character_buffer = c;
}

//Enhanced for bigger pixels and letters
void plot_letter(int x, int y, short int line_color, char letter)
{
    if(letter == 'a')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 1) << 1)) = line_color;
	}
	else if(letter == 'b')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 4) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 4) << 10) + ((x + 1) << 1)) = line_color;
		
	}
	else if(letter == 'c')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 2) << 1)) = line_color;
		
	}
	else if(letter == 'd')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 1) << 1)) = line_color;
		
	}
	else if(letter == 'e')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 4) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 4) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 4) << 10) + ((x + 2) << 1)) = line_color;
	}
	else if(letter == 'f')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 2) << 1)) = line_color;
		
	}
	else if(letter == 'g')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 4) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 4) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 1) << 1)) = line_color;
	}
	else if(letter == 'h')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 2) << 1)) = line_color;
	}
	else if(letter == 'i')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 2) << 1)) = line_color;
		
	}
	else if(letter == 'j')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 1) << 1)) = line_color;
	}
	else if(letter == 'k')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 2) << 1)) = line_color;
		
	}
	else if(letter == 'l')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
	}
	else if(letter == 'm')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 3) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 4) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 4) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 4) << 1)) = line_color;
	}
	else if(letter == 'n')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 3) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 3) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 3) << 1)) = line_color;
	}
	else if(letter == 'o')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 2) << 1)) = line_color;
		
	}
	else if(letter == 'p')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 1) << 1)) = line_color;
		
	}
	else if(letter == 'q')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 2) << 1)) = line_color;
		
	}
	else if(letter == 'r')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
	}
	else if(letter == 's')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 3) << 1)) = line_color;
		
	}
	else if(letter == 't')
	{
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 1) << 1)) = line_color;
		
	}
	else if(letter == 'u')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 2) << 1)) = line_color;
		
	}
	else if(letter == 'v')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 2) << 1)) = line_color;
		
	}
	else if(letter == 'w')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 3) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 3) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 3) << 1)) = line_color;
		
	}
	else if(letter == 'x')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;		
	}
	else if(letter == 'y')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x - 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 2) << 1)) = line_color;
	}
	else //for z
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 2) << 1)) = line_color;
		
	}
	
}

//Regular plot pixel
void plot_pixel(int x, int y, short int line_color)
{
    *(short int*)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

//Big plot pixel
void plot_pixel_big(int x1, int y1,int x2,int y2, short int line_color)
{
    for(int x_pos = x1; x_pos < x2; x_pos++)
	{
		for(int y_pos = y1; y_pos < y2; y_pos++)
		{
			plot_pixel(x_pos, y_pos, line_color);
		}
	}
}

void wait_for_vsync(void)
{
	volatile int* pixel_ctrl_ptr = (int*) 0xFF203020; // pixel controller
	register int status;
	
	*pixel_ctrl_ptr = 1; // start synchronization
	
	status = *(pixel_ctrl_ptr + 3); // status register of the pixel controller
	while((status & 0x01) != 0){
		status = *(pixel_ctrl_ptr + 3);
	}
}

//Get input from Keyboard
void getFromKeyboard(unsigned char *keyboardValue){
	volatile int * PS2_ptr = (int *) 0xFF200100;
	int data = *PS2_ptr;
	*keyboardValue = data & 0xFF;
	
	while(data & 0x8000){
		data = *PS2_ptr;	
	}	
}
