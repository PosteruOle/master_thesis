#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <ctime>

using namespace std;

int main(int argc, char **argv){
    if(argc==1){
        cerr << "Please provide the test size!\n";
        exit(1);
    }
    
    long test_size=atoi(argv[1]);
    unsigned char data; 
    unsigned short crc; 
    srand(time(NULL));
    
    cout << "Test size is equal to: " << test_size << endl;
    FILE *f=fopen("inputs.txt", "w");

    for(long i=0;i<test_size;i++){
        data=rand()%256; crc=rand()%65536;
        fprintf(f, "%d %d\n", data, crc);
    }
    cout << "End of initialization. Test cases are generated." << endl;
    fclose(f);

    return 0;
}