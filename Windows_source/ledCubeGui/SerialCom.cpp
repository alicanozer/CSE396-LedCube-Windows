#include <iostream>

#include "rs232.h"

#define SIZE 8
#define BAUDRATE  115200
using namespace std;


void fillArray(unsigned char arr[SIZE][SIZE][SIZE], unsigned char value){
	for (int i = 0; i < SIZE; ++i){
		for (int j = 0; j < SIZE; ++j){
			for (int k = 0; k < SIZE; ++k){
				arr[i][j][k] = value;
			}

		}

	}
}

void CompressArray(unsigned char CompressArray[SIZE][SIZE][1], unsigned char orginArray[SIZE][SIZE][SIZE]){

	for (int i = 0; i < SIZE; ++i){
		for (int j = 0; j < SIZE; ++j){
			for (int k = 0; k < SIZE; ++k){
				if (orginArray[i][j][k]){
					CompressArray[i][j][0] |= (1 << k);
				}
				else{
					CompressArray[i][j][0] &= ~(1 << k);
				}


			}
		}
	}


}
bool sendArray(unsigned char CompressArray[SIZE][SIZE][1], int portNo) {
	for (int i = 0; i < SIZE; ++i){
		for (int j = 0; j < SIZE; ++j){
			if (SendByte(portNo, CompressArray[i][j][0]) == 1){
				cerr << "SendByte Error\n";
				return false;
			}
		}
	}
	return true;

}