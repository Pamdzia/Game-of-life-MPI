#ifndef LIFEPARALLELIMPLEMENTATION_H
#define LIFEPARALLELIMPLEMENTATION_H

#include "Life.h"
#include <mpi.h>

class LifeParallelImplementation : public Life { //rozszerzenie klasy Life o mozliwosc przetwarzania rownoleglego
public:
    LifeParallelImplementation(); //konstruktor bezparam, inicjalizuje srodowisko MPI, ustawia zmienne 
    void oneStep() override; //zaimplementowanie rownoleglej wersji kroku symulacji
    int numberOfLivingCells() override; // Zaimplementowanie metody zwracającej liczbę żywych komórek
    double averagePollution() override; // Zaimplementowanie metody zwracającej średnie zanieczyszczenie
    void beforeFirstStep() override;
    void afterLastStep() override;

private:
    void realStep(); //obliczenia dla przydzielonej czesci siatki
    void exchangeBorderRows(); //wymiana informacji o brzegowych wierszach/kolumnach miedzy procesami

    int procId; //przechowanie identyfikatora (rank) procesu
    int numProcs; //calkowita liczba procesow MPI
};

#endif // LIFEPARALLELIMPLEMENTATION_H