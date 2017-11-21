#ifndef CELL_H_
#define CELL_H_

typedef enum Color { RED, BLUE, YELLOW, GREEN, ORANGE } Color;

typedef struct Cell {

	double x;
	double y;
	double size;
	int row;
	int col;
	bool visible;
	bool counted;
	Color color;

} Cell;

void InitCell(Cell *cell, double x, double y, 
		double row, double col, double size, int colors) {
	cell->x = x;
	cell->y = y;
	cell->row = row;
	cell->col = col;
	cell->size = size;
	cell->visible = true;
	cell->counted = false;
	cell->color = (Color) rand() % colors;
}

void DrawCell(Cell *cell) {
	ALLEGRO_COLOR color;
	switch(cell->color) {
		case RED:
			color = al_map_rgb(255, 0, 0);
			break;
		case BLUE:
			color = al_map_rgb(0, 0, 255);
			break;
		case YELLOW:
			color = al_map_rgb(255, 255, 0);
			break;
		case GREEN:
			color = al_map_rgb(0, 255, 0);
			break;
		case ORANGE:
			color = al_map_rgb(255, 100, 0);
			break;

	}
	if (cell->visible) {
		al_draw_filled_rectangle(cell->x, cell->y, cell->x + cell->size,
				cell->y + cell->size, color);
		al_draw_rectangle(cell->x, cell->y, cell->x + cell->size,
				cell->y + cell->size, al_map_rgb(0, 0, 0), 2.0); 
	}
}




#endif
