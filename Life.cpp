/*
 * Life.cpp
 */

#include "Life.h"
#include "Alloc.h"

Life::Life()
{
}

Life::~Life()
{
}

void Life::setRules(Rules *rules)
{
	this->rules = rules;
}

void Life::setSize(int size)
{
	this->size = size;
	this->size_1 = size - 1;
	this->size_1_squared = size_1 * size_1;
	this->cells = tableAlloc(size);
	this->cellsNext = tableAlloc(size);
	this->pollution = tableAlloc(size);
	this->pollutionNext = tableAlloc(size);
	clearTable(cells, size);
	clearTable(cellsNext, size);
	clearTable(pollution, size);
	clearTable(pollutionNext, size);
}

void Life::bringToLife(int row, int col)
{
	cells[row][col] = 1;
}

// nie zrownoleglac
int Life::liveNeighbours(int row, int col)
{
	return cells[row - 1][col] + cells[row + 1][col] + cells[row][col - 1] + cells[row][col + 1] +
		   cells[row - 1][col - 1] + cells[row - 1][col + 1] + cells[row + 1][col - 1] + cells[row + 1][col + 1];
}

int Life::getPollution(int row, int col)
{
	return pollution[row][col];
}

int Life::getCellState(int row, int col)
{
	return cells[row][col];
}

int **Life::cellsTable()
{
	return cells;
}

int **Life::pollutionTable()
{
	return pollution;
}

void Life::swapTables()
{
	int **tmp;

	tmp = cells;
	cells = cellsNext;
	cellsNext = tmp;

	tmp = pollution;
	pollution = pollutionNext;
	pollutionNext = tmp;
}

int Life::sumTable( int **table ) {
	int sum = 0;
	for ( int row = 1; row < size_1; row++ )
		for( int col = 1; col < size_1; col++ )
			sum += table[ row ][ col ];
	return sum;
}

void Life::beforeFirstStep() {
}

void Life::afterLastStep() {
}