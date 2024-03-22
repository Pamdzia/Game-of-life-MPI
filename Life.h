/*
 * Life.h
 */

#ifndef LIFE_H_
#define LIFE_H_

#include "Rules.h"

class Life {
protected:
	Rules *rules;
	int size;
	int size_1;
	int size_1_squared;
	int **cells;
	int **cellsNext;
	int **pollution;
	int **pollutionNext;
	int liveNeighbours( int row, int col );
	int sumTable( int **table );
	void swapTables();
	virtual void realStep() = 0;
public:
	Life();
	virtual ~Life();
	void setRules( Rules *rules );
	void setSize( int size );
	void bringToLife( int row, int col );
	int getCellState( int row, int col );
	int getPollution( int row, int col );

	int **cellsTable();
	int **pollutionTable();

	virtual void beforeFirstStep();
	virtual void afterLastStep();
	virtual int numberOfLivingCells() = 0;
	virtual double averagePollution() = 0;
	virtual void oneStep() = 0;
};

#endif /* LIFE_H_ */