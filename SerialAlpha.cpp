#include<iostream>  
#include <fstream> // For the file input and output reading of logs. 
#include <string>
#include <vector>

 using namespace std;
     const int row = 100;
     const int col = 100;
     int posinFile = 0; // This will tell us where we are in the file, so we can create a few arrays. 
 
 
 int getfileLen(){
 ifstream file("tester.csv",ifstream::ate | ifstream::binary);
    
    if (file.is_open()) {
    return (file.tellg());
    }
    else{return 0;}
 
 }
 
 
 void createArray(string logFile[row][col]){
     
    ifstream file("tester.csv");
    if (file.is_open()) {

        for (int i = 0; i < row; ++i) {
            for (int j = 0; j < col; ++j) {
                file >> logFile[i][j];
                file.get(); // This part will remove the limiter
            }
        }
    }
    cout << file.tellg();
}

void readArray(string logFile[row][col]){

   for (int i = 0; i < row; ++i) {
            for (int j = 0; j < col; ++j) {
                cout << logFile[i][j] << ' ';
            }
            cout << '\n';
        }
  
}
 
 
 void createVectors(int numberofArrays, int sizeofArrays , vector<vector<string> > &arrays){ // As we are unable to create arrays after runtime, we need to use vectors. 
 
   for (int i = 0; i < numberofArrays; i++)
{
    arrays[i].resize(sizeofArrays);
}
 
 }
 

 
 int main()
 {
     int fileEnd = 0; // This will tell where the file array ends, so the next array can start here. This will avoid SEG faults. 
     int numberofArrays = 10;
     vector<vector<string> >arrays(numberofArrays); // Create a new vector
     auto fileLen = getfileLen; // Change later 
     createVectors(numberofArrays,100,arrays);
     string logFile[row][col]; 
     createArray(logFile);
     readArray(logFile);
 return 0;
 
 
 
 }
 

 
 
 
