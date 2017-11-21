#ifndef GRID_H_
#define GRID_H_

#include "cell.h"
#include "cell_stack.h"

typedef struct Grid {

	int rows; //number of rows in the game grid
	int columns; //number of columns in the game grid
	Cell **cells;

} Grid;

Grid *CreateGrid(int columns, int rows, double cell_size, int colors) {
	Grid *grid = malloc(sizeof(Grid));
	grid->rows = rows;
	grid->columns = columns;

	grid->cells = (Cell **) malloc(sizeof(Cell*) * grid->rows);


	for (int i = 0; i < grid->rows; i++) {
		grid->cells[i] = (Cell *) malloc(sizeof(Cell) * grid->columns);
	}

	for (int i = 0; i < grid->rows; i++) {
		for (int j = 0; j < grid->columns; j++) {
			InitCell(&grid->cells[i][j], j * cell_size, i * cell_size, 
					i, j, cell_size, colors);
		}
	}

	return grid;
}

void DrawGrid(Grid* grid) {
	for (int i = 0; i < grid->rows; i++) {
		for (int j = 0; j < grid->columns; j++) {
			DrawCell(&grid->cells[i][j]);
		}
	}
}

void DestroyGrid(Grid* grid) {
	for (int i = 0; i < grid->rows; i++) {
		free(grid->cells[i]);
	}

	free(grid->cells);
	free(grid);
}

void DeactivateCell(Grid* grid, int cell_row, int cell_col) {
	grid->cells[cell_row][cell_col].visible = false;

}

int FloodClear(Grid* grid, int start_row, int start_col) {
	Cell *current_cell = &grid->cells[start_row][start_col];
	Color target_color = current_cell->color;
	CellStack *cell_stack = CreateCellStack();
	int cells_changed = 0;

	CellStackPush(cell_stack, current_cell);

	while (!IsCellStackEmpty(cell_stack)) {
		current_cell = CellStackPop(cell_stack);
		if (current_cell->color == target_color && current_cell->visible) {
			current_cell->visible = false;
			cells_changed++;
			if (current_cell->row > 0) {
				CellStackPush(cell_stack, 
						&grid->cells[current_cell->row - 1][current_cell->col]);

			}
			if (current_cell->row < grid->rows - 1) {
				CellStackPush(cell_stack, 
						&grid->cells[current_cell->row + 1][current_cell->col]);
				
			}
			if (current_cell->col > 0) {
				CellStackPush(cell_stack,
						&grid->cells[current_cell->row][current_cell->col-1]);

			}
			if (current_cell->col < grid->columns - 1) {
				CellStackPush(cell_stack,
						&grid->cells[current_cell->row][current_cell->col+1]);
				
			}
		}
	}

	if (cells_changed == 1) {
		grid->cells[start_row][start_col].visible = true;
	}

	DestroyCellStack(cell_stack);

	return cells_changed;
}

int FloodCount(Grid* grid, int start_row, int start_col) {

	Cell *current_cell = &grid->cells[start_row][start_col];
	Color target_color = current_cell->color;
	CellStack *cell_stack = CreateCellStack();
	int cell_count = 0;

	CellStackPush(cell_stack, current_cell);

	while (!IsCellStackEmpty(cell_stack)) {
		current_cell = CellStackPop(cell_stack);
		if (current_cell->color == target_color && current_cell->visible &&
				!current_cell->counted) {
			current_cell->counted = true;
			cell_count++;
			if (current_cell->row > 0) {
				CellStackPush(cell_stack, 
						&grid->cells[current_cell->row - 1][current_cell->col]);
			}
			if (current_cell->row < grid->rows - 1) {
				CellStackPush(cell_stack, 
						&grid->cells[current_cell->row + 1][current_cell->col]);
			}
			if (current_cell->col > 0) {
				CellStackPush(cell_stack,
						&grid->cells[current_cell->row][current_cell->col-1]);
			}
			if (current_cell->col < grid->columns - 1) {
				CellStackPush(cell_stack,
						&grid->cells[current_cell->row][current_cell->col+1]);
			}
		}
	}

	for (int i = 0; i < grid->rows; i++) {
		for (int j = 0; j < grid->columns; j++) {
			grid->cells[i][j].counted = false;
		}
	}


	DestroyCellStack(cell_stack);

	return cell_count;
}

bool CheckGameOver(Grid* grid) {
	for (int i = 0; i < grid->rows; i++) {
		for (int j = 0; j < grid->columns; j++) {
			if (grid->cells[i][j].visible && FloodCount(grid, i, j) > 1) {
				return false;
			}
		}
	}
	return true;
}

bool CheckAllCleared(Grid *grid) {
	for (int i = 0; i < grid->rows; i++) {
		for (int j = 0; j < grid->columns; j++) {
			if (grid->cells[i][j].visible) {
				return false;
			}
		}
	}
	return true;
}


void CollapseCellsDown(Grid* grid) {
	bool finished = false;

	while (!finished) {
		finished = true;

		for (int i = 0; i < grid->rows; i++) {
			for (int j = 0; j < grid->columns; j++) {
				if (i < grid->rows - 1) {
					if (grid->cells[i][j].visible && !grid->cells[i+1][j].visible) {
						grid->cells[i][j].visible = false;
						grid->cells[i+1][j].visible = true;
						grid->cells[i+1][j].color = grid->cells[i][j].color;
						finished = false;
					}
				}
			}
		}
	}
}

void CollapseCellsLeft(Grid* grid) {
	bool finished = false;
	bool column_empty = true;
	bool start_column_empty = false;

	int start_column = grid->columns - 1;

	while (!finished) {
		finished = true;
		for (int i = 0; i < grid->rows; i++) {
			if (grid->cells[i][start_column].visible) {
				column_empty = false;
			}
		}
		if (column_empty) {

			finished = false;
			start_column--;
		}
	}

	finished = false;
	column_empty = false;

	while (!finished) {
		finished = true;
		for (int j = start_column; j > 0; j--) {
			column_empty = true;
			start_column_empty = true;
			for (int i = 0; i < grid->rows; i++) {
				if (grid->cells[i][j-1].visible) {
					column_empty = false;
				}
			}
			for (int i = 0; i < grid->rows; i++) {
				if (grid->cells[i][j].visible) {
					start_column_empty = false;
				}
			}

			if (column_empty && !start_column_empty) {
				for (int i = 0; i < grid->rows; i++) {
					grid->cells[i][j - 1].visible = grid->cells[i][j].visible;
					grid->cells[i][j].visible = false;
					grid->cells[i][j-1].color = grid->cells[i][j].color;
				}
				finished = false;
			}
		}
	}
}


#endif
