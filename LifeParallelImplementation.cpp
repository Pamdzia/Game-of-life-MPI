#include "LifeParallelImplementation.h"
#include<iostream>

LifeParallelImplementation::LifeParallelImplementation()
{
    //int procId, numProcs;
    MPI_Comm_rank(MPI_COMM_WORLD, &procId); //okresla rank porcesu i przypisuje do zmiennej procId
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs); //pobiera calkowita liczbe procesow i przypisueje do zmiennej numProc
    // Dodatkowa inicjalizacja jeśli jest potrzebna

    // Debug: Wyświetl rank i liczbę procesów
    //std::cout << "Proces " << procId << " z " << numProcs << " procesów." << std::endl;
}

void LifeParallelImplementation::beforeFirstStep() {
int rowSize = size; // Rozmiar jednego wiersza

    for (int row = 0; row < size; ++row) {
        //std::cout << "Proces " << procId << " przed MPI_Bcast dla wiersza " << row << std::endl;
        MPI_Bcast(cells[row], rowSize, MPI_INT, 0, MPI_COMM_WORLD);}

    for (int row = 0; row < size; ++row) {
        //std::cout << "Proces " << procId << " przed MPI_Bcast dla wiersza " << row << std::endl;
        MPI_Bcast(pollution[row], rowSize, MPI_INT, 0, MPI_COMM_WORLD);}}

void LifeParallelImplementation::oneStep()
{
    realStep(); // wywolanei metody ktora wykonuje faktyczne obliczenia dla przydzielonej czesci siatki
    //swapTables(); //aktualizacja stanu siatki komorek w kazdym kroku (siatka gotowa na koeljny krok symulacji)
    // Dodatkowe działania po kroku symulacji jeśli są potrzebne
}

void LifeParallelImplementation::realStep()
{


int rowsPerProcess = size_1 / numProcs; // Podstawowa liczba wierszy na proces
int extraRows = size_1 % numProcs; // Dodatkowe wiersze do rozdysponowania

int startRow, endRow;

if (procId < extraRows) {
    // Procesy z niższymi ID obsługują jeden dodatkowy wiersz
    startRow = procId * (rowsPerProcess + 1);
    endRow = startRow + rowsPerProcess; // Dodatkowy wiersz jest już wliczony
} else {
    // Procesy z wyższymi ID obsługują podstawową liczbę wierszy
    startRow = procId * rowsPerProcess + extraRows;
    endRow = startRow + rowsPerProcess - 1;
}

if(procId==0){startRow=startRow+1;}

    //int startRow = 1 + procId * (size_1 - 2) / numProcs; // oblicznie startRow i endRow, okreslaja zakres wierszy siatki nad ktorymi dany proces
    //int endRow = (procId + 1) * (size_1 - 2) / numProcs; // ma pracowac, podzial rownomierny pomiedzy wszystkimi procesami

    int upRank = procId == 0 ? MPI_PROC_NULL : procId - 1; //jak rank=0 to znacze ze proces nie ma procesu wyzej - MPI_PROC_NULL
    int downRank = procId == numProcs - 1 ? MPI_PROC_NULL : procId + 1; //jak ma rank ostatni to nie ma ponizej - MPI_PROC_NULL

    //if(downRank == MPI_PROC_NULL){endRow=endRow+1;}

    // Debug: Wyświetl zakres wierszy dla każdego procesu
    //std::cout << "Proces " << procId << " oblicza wiersze od tablica  " << startRow << " do " << endRow <<"  down rank tablica  "<<downRank<< std::endl;


    int currentState, currentPollution;
	for (int row = startRow; row <= endRow; row++)
		for (int col = 1; col < size_1; col++)
		{
			currentState = cells[row][col];
			currentPollution = pollution[row][col];
			cellsNext[row][col] = rules->cellNextState(currentState, liveNeighbours(row, col),
													   currentPollution);
			pollutionNext[row][col] =
				rules->nextPollution(currentState, currentPollution, pollution[row + 1][col] + pollution[row - 1][col] + pollution[row][col - 1] + pollution[row][col + 1],
									 pollution[row - 1][col - 1] + pollution[row - 1][col + 1] + pollution[row + 1][col - 1] + pollution[row + 1][col + 1]);
		}


    exchangeBorderRows(); // po zakonczeniu obliczen dla swojego segmentu siatki kazdy proces wywoluje funkcje wymiany informacji wierszy brzegowych
}

void LifeParallelImplementation::exchangeBorderRows() //przydatne na kolejna iteracje
{
    // stale do tagowania wiadomosci MPI
    const int TAG_SEND_UP = 0;
    const int TAG_SEND_DOWN = 1;
    const int TAG_RECV_UP = 2;
    const int TAG_RECV_DOWN = 3;

    int upRank = procId == 0 ? MPI_PROC_NULL : procId - 1; //jak rank=0 to znacze ze proces nie ma procesu wyzej - MPI_PROC_NULL
    int downRank = procId == numProcs - 1 ? MPI_PROC_NULL : procId + 1; //jak ma rank ostatni to nie ma ponizej - MPI_PROC_NULL
//std::cout << "upRank  " << upRank <<"   downRank   "<< downRank  << std::endl;

int rowsPerProcess = size_1 / numProcs; // Podstawowa liczba wierszy na proces
int extraRows = size_1 % numProcs; // Dodatkowe wiersze do rozdysponowania

int firstRow, lastRow;

if (procId < extraRows) {
    // Procesy z niższymi ID obsługują jeden dodatkowy wiersz
    firstRow = procId * (rowsPerProcess + 1);
    lastRow = firstRow + rowsPerProcess; // Dodatkowy wiersz jest już wliczony
} else {
    // Procesy z wyższymi ID obsługują podstawową liczbę wierszy
    firstRow = procId * rowsPerProcess + extraRows;
    lastRow = firstRow + rowsPerProcess - 1;
}

    
if(procId==0){firstRow=firstRow+1;}
//if(downRank == MPI_PROC_NULL){lastRow=lastRow+1;}
//std::cout << "Proces " << procId << "   firstRow przesylanie   " << firstRow <<"   last przesylanie   "<< lastRow << std::endl;

    // Bufory do wysyłania i odbierania danych (przechowuja wiersze wysylane do sasiednich procesow)
    int numColumns = size;  // gdzie 'size' jest wartością przekazaną do tableAlloc
    //int* sendBufferUp = cells[firstRow]; // wskaznik wskazuje na numer wiersza (czyli na caly wiersz)
    //int* sendBufferDown = cells[lastRow]; 
    int recvBufferUp[numColumns];// wartosci ze wspolnego wiersza (gorny) - z tego wiersza odpowiednia liczba komorek, przygotowana do odebrania procesu
    int recvBufferDown[numColumns]; // -||- dolny [bo po kolei przekazujemy caly wiersz cell a w tym biforze ja zapisujemy]


    
    int recvBufferUpPollution[numColumns];
    int recvBufferDownPollution[numColumns];

    MPI_Barrier(MPI_COMM_WORLD);

//std::cout << "num columns   " << numColumns << std::endl;

MPI_Barrier(MPI_COMM_WORLD);
//std::cout << "tuuuuuu" << std::endl;

/*
// Najpierw wysyłanie
    for(int col=0; col<numColumns; col++){
   if (procId == 1) {
    MPI_Send(&cells[firstRow][col], 1, MPI_INT, upRank, TAG_SEND_UP, MPI_COMM_WORLD);
    //std::cout << "Proces " << procId << " wysyła górny wiersz do " << upRank << std::endl;
} else if (procId == 0) {
    MPI_Recv(&recvBufferUp[col], 1, MPI_INT, downRank, TAG_SEND_UP, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //std::cout << "Proces " << procId << " odbiera górny wiersz od " << downRank << std::endl;
}}
*/

//std::cout << "gorne wiersze " <<  std::endl;
if (procId %2== 0 && upRank != MPI_PROC_NULL) {
    MPI_Send(cellsNext[firstRow], numColumns, MPI_INT, upRank, TAG_SEND_UP, MPI_COMM_WORLD);
    MPI_Send(pollutionNext[firstRow], numColumns, MPI_INT, upRank, 4, MPI_COMM_WORLD);
    //std::cout << "Proces " << procId << " wysyła górny wiersz do " << upRank << std::endl;
} else if (procId %2!= 0 && downRank != MPI_PROC_NULL ) {
    MPI_Recv(recvBufferUp, numColumns, MPI_INT, downRank, TAG_SEND_UP, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(recvBufferUpPollution, numColumns, MPI_INT, downRank, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //std::cout << "Proces " << procId << " odbiera górny wiersz od " << downRank << std::endl;
}

MPI_Barrier(MPI_COMM_WORLD);

if (procId %2!= 0 ) {
    MPI_Send(cellsNext[firstRow], numColumns, MPI_INT, upRank, TAG_SEND_UP, MPI_COMM_WORLD);
    MPI_Send(pollutionNext[firstRow], numColumns, MPI_INT, upRank, 4, MPI_COMM_WORLD);
    //std::cout << "Proces " << procId << " wysyła górny wiersz do " << upRank << std::endl;
} else if (procId %2== 0 && downRank != MPI_PROC_NULL ) {
    MPI_Recv(recvBufferUp, numColumns, MPI_INT, downRank, TAG_SEND_UP, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(recvBufferUpPollution, numColumns, MPI_INT, downRank, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //std::cout << "Proces " << procId << " odbiera górny wiersz od " << downRank << std::endl;
}

//std::cout << "BARIERA " <<  std::endl;
    MPI_Barrier(MPI_COMM_WORLD);
//std::cout << "PO BARIERA " <<  std::endl;

//std::cout << "dolne wiersze " <<  std::endl;
    if (procId %2== 0 && downRank != MPI_PROC_NULL) {
        //std::cout << "weszlam proc 0 " <<  std::endl;
        MPI_Send(cellsNext[lastRow], numColumns, MPI_INT, downRank, TAG_SEND_DOWN, MPI_COMM_WORLD);
        MPI_Send(pollutionNext[lastRow], numColumns, MPI_INT, downRank, 5, MPI_COMM_WORLD);
        //std::cout << "Proces " << procId << " wysyła dolny wiersz do " << downRank << std::endl;
    }

    // Następnie odbieranie
    if (procId %2!= 0 ) {
        //std::cout << "weszlam proc 1 " <<  std::endl;
        MPI_Recv(recvBufferDown, numColumns, MPI_INT, upRank, TAG_SEND_DOWN, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(recvBufferDownPollution, numColumns, MPI_INT, upRank, 5, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //std::cout << "Proces " << procId << " odbiera dolny wiersz od " << upRank << std::endl;
    }


MPI_Barrier(MPI_COMM_WORLD);

    if (procId %2!= 0 && downRank != MPI_PROC_NULL ) {
        //std::cout << "weszlam proc 0 " <<  std::endl;
        MPI_Send(cellsNext[lastRow], numColumns, MPI_INT, downRank, TAG_SEND_DOWN, MPI_COMM_WORLD);
        MPI_Send(pollutionNext[lastRow], numColumns, MPI_INT, downRank, 5, MPI_COMM_WORLD);
        //std::cout << "Proces " << procId << " wysyła dolny wiersz do " << downRank << std::endl;
    }

    // Następnie odbieranie
    if (procId %2== 0 && upRank != MPI_PROC_NULL) {
        //std::cout << "weszlam proc 1 " <<  std::endl;
        MPI_Recv(recvBufferDown, numColumns, MPI_INT, upRank, TAG_SEND_DOWN, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(recvBufferDownPollution, numColumns, MPI_INT, upRank, 5, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //std::cout << "Proces " << procId << " odbiera dolny wiersz od " << upRank << std::endl;
        }
MPI_Barrier(MPI_COMM_WORLD);
swapTables();
        // Aktualizacja górnego brzegu siatki
    if (upRank != MPI_PROC_NULL) { //jezeli nie jest to ten dolny albo gorny proces
    //std::cout << "Proces " << procId << " aktualizuje gore   "<<firstRow -1<< std::endl;
        for (int col = 1; col < size_1; col++) {
            cells[firstRow -1 ][col] = recvBufferDown[col];
        }
    }

    // Aktualizacja dolnego brzegu siatki
    if (downRank != MPI_PROC_NULL) {
        //std::cout << "Proces " << procId << " aktualizuje dol  "<< lastRow + 1<<std::endl;
        for (int col = 1; col < size_1; col++) {
            cells[lastRow + 1 ][col] = recvBufferUp[col];
        }
    }

    // Synchronizacja procesów po wymianie danych
    MPI_Barrier(MPI_COMM_WORLD);

        // Aktualizacja górnego brzegu siatki
    if (upRank != MPI_PROC_NULL) { //jezeli nie jest to ten dolny albo gorny proces
        for (int col = 1; col < size_1; col++) {
            pollution[firstRow -1][col] = recvBufferDownPollution[col];
        }
    }

    // Aktualizacja dolnego brzegu siatki
    if (downRank != MPI_PROC_NULL) {
        for (int col = 1; col < size_1; col++) {
            pollution[lastRow + 1][col] = recvBufferUpPollution[col];
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

}

void LifeParallelImplementation::afterLastStep() {
 
    //int startRow = 1 + procId * (size_1 - 2) / numProcs; // oblicznie startRow i endRow, okreslaja zakres wierszy siatki nad ktorymi dany proces
    //int endRow = (procId + 1) * (size_1 - 2) / numProcs; // ma pracowac, podzial rownomierny pomiedzy wszystkimi procesami

int rowsPerProcess = size_1 / numProcs; // Podstawowa liczba wierszy na proces
int extraRows = size_1 % numProcs; // Dodatkowe wiersze do rozdysponowania

int firstRow, lastRow;

if (procId < extraRows) {
    // Procesy z niższymi ID obsługują jeden dodatkowy wiersz
    firstRow = procId * (rowsPerProcess + 1);
    lastRow = firstRow + rowsPerProcess; // Dodatkowy wiersz jest już wliczony
} else {
    // Procesy z wyższymi ID obsługują podstawową liczbę wierszy
    firstRow = procId * rowsPerProcess + extraRows;
    lastRow = firstRow + rowsPerProcess - 1;
}


int upRank = procId == 0 ? MPI_PROC_NULL : procId - 1; //jak rank=0 to znacze ze proces nie ma procesu wyzej - MPI_PROC_NULL
int downRank = procId == numProcs - 1 ? MPI_PROC_NULL : procId + 1; //jak ma rank ostatni to nie ma ponizej - MPI_PROC_NULL
if(procId==0){firstRow=firstRow+1;}


  // Przygotowanie lokalnych danych do wysłania
int localDataSize = (lastRow - firstRow+1) * (size_1 -1);
int* localDataToSend = new int[localDataSize];

int localDataSizePollution = (lastRow - firstRow+1) * (size_1 -1);
int* localDataToSendPollution = new int[localDataSizePollution];

//printf("Proces %d: startRow = %d, endRow = %d, wysyłanie %d elementów\n", procId, firstRow, lastRow, localDataSize);

MPI_Barrier(MPI_COMM_WORLD);
int index = 0;
for (int i = firstRow; i <= lastRow; i++) {
    for (int j = 1; j < size_1; j++) {
        localDataToSend[index] = cells[i][j];
        index++;
    }
}

int indeks = 0;
for (int i = firstRow; i <= lastRow; i++) {
    for (int j = 1; j < size_1; j++) {
        localDataToSendPollution[indeks] = pollution[i][j];
        indeks++;
    }
}

//std::cout<<"index   "<<index<<std::endl;

MPI_Barrier(MPI_COMM_WORLD);

// Alokacja bufora odbiorczego w procesie root
int* recvBuffer = new int[size * size];
int* recvBufferPollution = new int[size * size];
//if (procId == 0) {
    //recvBuffer = new int[size * size]; // 'size' to całkowita liczba wierszy
    //printf("Proces Root (%d): Rozmiar bufora odbiorczego = %d\n", procId, size * size);
//}

// Wydrukowanie wielkości danych do wysłania przez każdy proces
//printf("Proces %d: Wysyłanie %d elementów\n", procId, localDataSize);

int* sendcounts = new int[numProcs];
int* displs = new int[numProcs];

// Wypełnienie sendcounts
for (int i = 0; i < numProcs; i++) {
    int start = i * (size_1 / numProcs);
    int end = start + (size_1 / numProcs) - 1;
    //if(i == numProcs-1){
      //  end=end+1;
    //}
    if(i==0){
        start=start+1;
    }
    sendcounts[i] = (end - start+1) * (size_1 -1);
    //std::cout<<"start  "<<start<<"   end   "<<end<<"   size  "<<size<<"  sendcounts[i]   "<<sendcounts[i]<<std::endl;
}

//MPI_Barrier(MPI_COMM_WORLD);

MPI_Allgather(&localDataSize, 1, MPI_INT, sendcounts, 1, MPI_INT, MPI_COMM_WORLD);
MPI_Allgather(&localDataSizePollution, 1, MPI_INT, sendcounts, 1, MPI_INT, MPI_COMM_WORLD);

// Obliczenie displs
displs[0] = 0;  // Pierwszy proces zawsze zaczyna od 0
for (int i = 1; i < numProcs; i++) {
    displs[i] = displs[i - 1] + sendcounts[i - 1];
}

//MPI_Barrier(MPI_COMM_WORLD);

if ( procId == 0 ) {
     for ( int i = 0; i < numProcs; i++ ) {
       //std::cout << "Proces " << i << " ma " << sendcounts[ i ] << " danych do przekazania, przesuniecie " << displs[ i ] << std::endl;
     }
     //std::cout << "Wszystkie dane to " << allData << std::endl;
   }

   //double * bigBuffer4allData = new double[ allData ];

// Wykonaj MPI_Gatherv
MPI_Allgatherv(localDataToSend, localDataSize, MPI_INT, 
            recvBuffer, sendcounts, displs, MPI_INT, MPI_COMM_WORLD);

MPI_Allgatherv(localDataToSendPollution, localDataSize, MPI_INT, 
            recvBufferPollution, sendcounts, displs, MPI_INT, MPI_COMM_WORLD);

delete[] sendcounts;
delete[] displs;


// Wykonaj MPI_Gather
//MPI_Gather(localDataToSend, localDataSize, MPI_INT, 
    //       recvBuffer, localDataSize, MPI_INT, 
     //      0, MPI_COMM_WORLD);

// Zwolnienie zasobów lokalnych
delete[] localDataToSend;
delete[] localDataToSendPollution;
//MPI_Barrier(MPI_COMM_WORLD);
// Proces root teraz posiada wszystkie dane w recvBuffer
if (procId == 0) {
    // Przepisywanie danych z recvBuffer do cells
    index = 0;
    for (int i = 1; i < size_1; i++) {
        for (int j = 1; j < size_1; j++) {
            cells[i][j] = recvBuffer[index];
            index++;
        }
    }

    indeks = 0;
    for (int i = 1; i < size_1; i++) {
        for (int j = 1; j < size_1; j++) {
            pollution[i][j] = recvBufferPollution[indeks];
            indeks++;
        }
    }

    delete[] recvBuffer;
    delete[] recvBufferPollution;
}

}

int LifeParallelImplementation::numberOfLivingCells() {
   // int count = 0;
   // for (int row = 1; row < size_1; row++) {
     //   for (int col = 1; col < size_1; col++) {
      //      if (cells[row][col] == 1) {
       //         count++;
       //     }
      //  }
   // }
   // return count;

   return sumTable( cells );
}
double LifeParallelImplementation::averagePollution() {
   // int totalPollution = 0;
  //  int cellCount = 0;
   // for (int row = 1; row < size_1; row++) {
    //    for (int col = 1; col < size_1; col++) {
      //      totalPollution += pollution[row][col];
      //      cellCount++;
    //    }
  //  }
   // return cellCount > 0 ? static_cast<double>(totalPollution) / cellCount : 0.0;

   return (double)sumTable( pollution ) / size_1_squared / rules->getMaxPollution();
}
