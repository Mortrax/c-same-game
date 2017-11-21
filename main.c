#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "grid.h"
#include "button.h"

//GLOBALS=========================================
const int WIDTH = 700;
const int HEIGHT = 700;
const int CELL_SIZE = 70;
const int FONT_SIZE = 20;
const int NUM_BUTTONS = 3;
const double FPS = 60;

typedef enum GameState {GAME_MENU, PLAYING, GAME_OVER} GameState;
typedef enum GameDifficulty {EASY, MEDIUM, HARD} GameDifficulty;

//PROTOTYPES======================================

void DrawGameMenu(const char *menu_text, Button *menu_buttons[], 
		const int num_buttons, ALLEGRO_FONT *menu_font);


int main(int argc, char* argv[]) {
	//allegro variables
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_FONT *georgia_font = NULL;

	srand(time(NULL));

	//primitive variables
	GameState game_state = GAME_MENU;
	GameDifficulty game_difficulty;
	bool redraw = false;
	bool done = false;
	bool saved_score = false;
	int player_score = 0;
	int num_cleared = 0;
	char score_string[50];
	char game_over_string[10];
	char menu_string[21]; 
	char you_win_string[8];
	sprintf(score_string, "Score: %d", player_score);
	sprintf(game_over_string, "GAME OVER");
	sprintf(menu_string, "WELCOME TO SAME GAME");
	sprintf(you_win_string, "YOU WIN");

	//game objects
	Grid *grid = NULL;
	Button *menu_buttons[NUM_BUTTONS];
	Button *end_game_buttons[2];
	menu_buttons[0] = CreateButton(WIDTH/4, HEIGHT/2, "EASY", FONT_SIZE, EASY_BUTTON);
	menu_buttons[1] = CreateButton(WIDTH/2, HEIGHT/2, "MEDIUM", FONT_SIZE, MEDIUM_BUTTON);
	menu_buttons[2] = CreateButton(3 * WIDTH/4, HEIGHT/2, "HARD", FONT_SIZE, HARD_BUTTON);
	
	end_game_buttons[0] = CreateButton(WIDTH/2 - (10 * FONT_SIZE/2), HEIGHT/2 + 100, 
			"PLAY AGAIN", FONT_SIZE, PLAY_AGAIN_BUTTON);
	end_game_buttons[1] = CreateButton(WIDTH/2 - (10 * FONT_SIZE/2), HEIGHT/2 + 150, 
			"SAVE SCORE", FONT_SIZE, SAVE_SCORE_BUTTON);

	FILE *save_file = fopen("high_scores.txt", "a+");

	if (!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}

	if (!al_init_primitives_addon()) {
		fprintf(stderr, "failed to initialize primitives addon!\n");
		return -1;
	}

	if (!al_init_font_addon()) {
		fprintf(stderr, "failed to initialize font addon!\n");
		return -1;
	}

	if (!al_init_ttf_addon()) {
		fprintf(stderr, "failed to initialize ttf addon!\n");
		return -1;
	}

	if (!al_install_mouse()) {
		fprintf(stderr, "failed to initialize mouse!\n");
		return -1;
	}

	display = al_create_display(WIDTH, HEIGHT);

	if (!display) {
		fprintf(stderr, "failed to create display!\n");
		return -1;
	}

	timer = al_create_timer(1.0 / FPS);

	if (!timer) {
		fprintf(stderr, "failed to create timer!\n");
		al_destroy_display(display);
		return -1;
	}

	event_queue = al_create_event_queue();

	if (!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_timer(timer);
		al_destroy_display(display);
		return -1;
	}

	georgia_font = al_load_font("Georgia.ttf", FONT_SIZE, 0);

	if (!georgia_font) {
		fprintf(stderr, "failed to load score_font!\n");
		al_destroy_event_queue(event_queue);
		al_destroy_timer(timer);
		al_destroy_display(display);
		return -1;
	}

	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_mouse_event_source());

	al_start_timer(timer);

	while (!done) {
	
		ALLEGRO_EVENT ev;

		al_wait_for_event(event_queue, &ev);

		if (ev.type == ALLEGRO_EVENT_TIMER) {
			redraw = true;
		}
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			done = true;
		}
		else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			if (game_state == GAME_MENU) {
				int mouse_x = ev.mouse.x;
				int mouse_y = ev.mouse.y;
				for (int i = 0; i < NUM_BUTTONS; i++) {
					if (mouse_x > menu_buttons[i]->x &&
						mouse_x < menu_buttons[i]->x + menu_buttons[i]->width &&
						mouse_y > menu_buttons[i]->y &&
						mouse_y < menu_buttons[i]->y + menu_buttons[i]->height) {
						if (menu_buttons[i]->type == EASY_BUTTON) {
							game_difficulty = EASY;
							grid = CreateGrid(WIDTH/CELL_SIZE, HEIGHT/CELL_SIZE,
									CELL_SIZE, 3);
						}
						else if (menu_buttons[i]->type == MEDIUM_BUTTON) {
							game_difficulty = MEDIUM;
							grid = CreateGrid(WIDTH/CELL_SIZE, HEIGHT/CELL_SIZE,
									CELL_SIZE, 4);
						}
						else if (menu_buttons[i]->type == HARD_BUTTON) {
							game_difficulty = HARD;
							grid = CreateGrid(WIDTH/CELL_SIZE, HEIGHT/CELL_SIZE,
									CELL_SIZE, 5);
						}
						game_state = PLAYING;
					}
				}
			}

			else if (game_state == PLAYING) {

				int mouse_x = ev.mouse.x;
				int mouse_y = ev.mouse.y;
				int column = mouse_x / CELL_SIZE;
				int row = mouse_y / CELL_SIZE;

				if (grid->cells[row][column].visible) {
					num_cleared = FloodClear(grid, row, column);
					player_score += (num_cleared - 1) * (num_cleared - 1);
					sprintf(score_string, "Score: %d", player_score);
					CollapseCellsDown(grid);
					CollapseCellsLeft(grid);
					if (CheckGameOver(grid)) {
						game_state = GAME_OVER;
					}
				}
			}

			else if (game_state == GAME_OVER) {

				int mouse_x = ev.mouse.x;
				int mouse_y = ev.mouse.y;
				for (int i = 0; i < 2; i++) {
					if (mouse_x > end_game_buttons[i]->x &&
						mouse_x < end_game_buttons[i]->x + end_game_buttons[i]->width &&
						mouse_y > end_game_buttons[i]->y &&
						mouse_y < end_game_buttons[i]->y + end_game_buttons[i]->height) {
						if (end_game_buttons[i]->type == PLAY_AGAIN_BUTTON) {
							Grid *old_grid = grid;
							DestroyGrid(old_grid);
							grid = NULL;
							game_state = GAME_MENU;
							player_score = 0;
							saved_score = false;

							sprintf(score_string, "Score: %d", player_score);
						}
						else if (end_game_buttons[i]->type == SAVE_SCORE_BUTTON &&
								 CheckAllCleared(grid) && !saved_score) {
							fprintf(save_file, score_string);
							fprintf(save_file, "\n");
							saved_score = true;
						}

					}
				}
			}
		}

		if (redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false;
			al_clear_to_color(al_map_rgb(0, 0, 0));
			if (game_state == GAME_MENU) {
				DrawGameMenu(menu_string, menu_buttons, NUM_BUTTONS,
						georgia_font);
			}
			else if (game_state == PLAYING) {

				DrawGrid(grid);
				al_draw_text(georgia_font, al_map_rgb(255, 0, 255), 5, 5, 0, score_string);
			}
			else if (game_state == GAME_OVER) {

				DrawGrid(grid);
				al_draw_text(georgia_font, al_map_rgb(255, 0, 255), 5, 5, 0, score_string);
				al_draw_text(georgia_font, al_map_rgb(255, 0, 255),
						(WIDTH/2) - (strlen(game_over_string)/2) * FONT_SIZE, 
						HEIGHT/2, 0, game_over_string);
				DrawButton(end_game_buttons[0], georgia_font);

				if (CheckAllCleared(grid)) {
					al_draw_text(georgia_font, al_map_rgb(255, 0, 255),
							(WIDTH/2) - (strlen(you_win_string)/2) * FONT_SIZE,
							(HEIGHT/2) + (FONT_SIZE*2), 0, you_win_string);
					if (!saved_score) {

						DrawButton(end_game_buttons[1], georgia_font);
					}
				}
			}
			al_flip_display();
		
		}
	
	}

	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_font(georgia_font);
	for (int i = 0; i < NUM_BUTTONS; i++) {
		DestroyButton(menu_buttons[i]);
	}
	for (int i = 0; i < 2; i++) {
		DestroyButton(end_game_buttons[i]);
	}
	if (grid != NULL) {

		DestroyGrid(grid);
	}
	fclose(save_file);


	return 0;
}

void DrawGameMenu(const char *menu_text, Button *menu_buttons[], 
		const int num_buttons, ALLEGRO_FONT *menu_font) {
	al_draw_text(menu_font, al_map_rgb(255, 0, 255), 
			WIDTH/2 - (strlen(menu_text)/2) * FONT_SIZE,
			50, 0, menu_text);
	for (int i = 0; i < num_buttons; i++) {
		DrawButton(menu_buttons[i], menu_font);
	}
}

