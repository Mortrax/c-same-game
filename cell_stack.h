#ifndef CELL_STACK_H_
#define CELL_STACK_H_
#include <assert.h>

typedef struct CellStackNode {
	Cell* cell;
	struct CellStackNode *next;
} CellStackNode;

typedef struct CellStack {

	CellStackNode *top;
	int current_size;

} CellStack;

CellStack* CreateCellStack() {
	CellStack* result = malloc(sizeof(CellStack));
	result->top = NULL;
	result->current_size = 0;
	return result;
}

void CellStackPush(CellStack* cell_stack, Cell* cell) {
	if (cell_stack->top == NULL) {
		assert(cell_stack->current_size == 0);
		cell_stack->top = malloc(sizeof(struct CellStackNode));
		cell_stack->top->cell = cell;
		cell_stack->top->next = NULL;
		cell_stack->current_size++;
	}
	else {
		CellStackNode *new_top = malloc(sizeof(struct CellStackNode));
		new_top->cell = cell;
		new_top->next = cell_stack->top;
		cell_stack->top = new_top;
		cell_stack->current_size++;
	}
}

Cell* CellStackPop(CellStack* cell_stack) {
	if (cell_stack->top == NULL) {
		assert(cell_stack->current_size == 0);
		return NULL;
	}
	else {
		Cell* result = cell_stack->top->cell;
		CellStackNode *old_top = cell_stack->top;
		cell_stack->top = cell_stack->top->next;
		old_top->cell = NULL;
		free(old_top);
		cell_stack->current_size--;
		return result;
	}
}

bool IsCellStackEmpty(CellStack* cell_stack) {
	return cell_stack->current_size == 0;
}

void DestroyCellStack(CellStack* cell_stack) {
	CellStackNode *previous_top;
	while (cell_stack->top != NULL) {
		previous_top = cell_stack->top;
		cell_stack->top = cell_stack->top->next;
		free(previous_top);
	}
	free(cell_stack);
}

#endif
