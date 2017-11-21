#ifndef BUTTON_H_
#define BUTTON_H_

typedef enum ButtonType {
	EASY_BUTTON, 
	MEDIUM_BUTTON, 
	HARD_BUTTON, 
	SAVE_SCORE_BUTTON,
	PLAY_AGAIN_BUTTON
} ButtonType;

typedef struct Button {
	double x;
	double y;
	double width;
	double height;
	int font_size;
	char *text;
	ButtonType type;
} Button;

Button *CreateButton(double x, double y, const char *text, 
		int font_size, ButtonType type) {

	Button *new_button = malloc(sizeof(Button));
	new_button->x = x;
	new_button->y = y;
	new_button->font_size = font_size;
	new_button->width = strlen(text) * font_size + 10;
	new_button->height = font_size + 10;
	new_button->text = strdup(text);
	new_button->type = type;
	
	return new_button;
}

void DrawButton(Button *button, ALLEGRO_FONT *font) {

	al_draw_filled_rectangle(button->x, button->y, button->x + button->width,
			button->y + button->height, al_map_rgb(0, 0, 255));
	al_draw_rectangle(button->x, button->y, button->x + button->width,
			button->y + button->height, al_map_rgb(255, 255, 255), 5);
	al_draw_text(font, al_map_rgb(0, 0, 0), button->x + 5,
			button->y + 5, 0, button->text); 
}

void DestroyButton(Button *button) {
	free(button->text);
	free(button);
}
#endif
