

#define SIZE 8

void fillArray(unsigned char arr[SIZE][SIZE][SIZE], unsigned char value);
void CompressArray(unsigned char CompressArray[SIZE][SIZE][1], unsigned char orginArray[SIZE][SIZE][SIZE]);
bool sendArray(unsigned char CompressArray[SIZE][SIZE][1], int portNo);