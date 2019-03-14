//==============================================================================
// Copyright (C) John-Philip Taylor
// tyljoh010@myuct.ac.za
//
// This file is part of the EEE4084F Course
//
// This file is free software: you can pixelComponentistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
//==============================================================================

#include "Prac2.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
//void bubblesort(JPEG out[6],int startInt, int split);
void bubblesort(int startInt, int split);

//------------------------------------------------------------------------------

// This is each thread's "main" function.  It receives a unique ID
void* Thread_Main(void* Parameter){
 int ID = *((int*)Parameter);

 bubblesort(ID,Thread_Count);

 return 0;
}
//------------------------------------------------------------------------------
void swap(int *xp, int *yp) 
{ 
    int temp = *xp; 
    *xp = *yp; 
    *yp = temp; 
} 

// Bubble sort function
void bubbleSort(int arr[], int n) 
{ 
   int i, j; 
   bool swapped; 
   for (i = 0; i < n-1; i++) 
   { 
     swapped = false; 
     for (j = 0; j < n-i-1; j++) 
     { 
        if (arr[j] > arr[j+1]) 
        { 
           swap(&arr[j], &arr[j+1]); 
           swapped = true; 
        } 
     } 
  
     // IF no two elements were swapped by inner loop, then break 
     if (swapped == false) 
        break; 
   } 
} 

/* This function takes last element as pivot, places 
   the pivot element at its correct position in sorted 
    array, and places all smaller (smaller than pivot) 
   to left of pivot and all greater elements to right 
   of pivot */
int partition (int arr[], int low, int high) 
{ 
    int pivot = arr[high];    // pivot 
    int i = (low - 1);  // Index of smaller element 
  
    for (int j = low; j <= high- 1; j++) 
    { 
        // If current element is smaller than or 
        // equal to pivot 
        if (arr[j] <= pivot) 
        { 
            i++;    // increment index of smaller element 
            swap(&arr[i], &arr[j]); 
        } 
    } 
    swap(&arr[i + 1], &arr[high]); 
    return (i + 1); 
} 

/* The main function that implements QuickSort 
 arr[] --> Array to be sorted, 
  low  --> Starting index, 
  high  --> Ending index */
void quickSort(int arr[], int low, int high) 
{ 
    if (low < high) 
    { 
        /* pi is partitioning index, arr[p] is now 
           at right place */
        int pi = partition(arr, low, high); 
  
        // Separately sort elements before 
        // partition and after partition 
        quickSort(arr, low, pi - 1); 
        quickSort(arr, pi + 1, high); 
    } 
} 
int main(int argc, char** argv){
 ofstream myfile;
 string threads = "Times"+ std::to_string(Thread_Count)+".txt";
 myfile.open (threads.c_str());
 int j;
 int meanRuns = 10;

 // Initialise everything that requires initialisation
 tic();
 pthread_mutex_init(&Mutex, 0);

 // Read the input image
 if(!Input.Read("Data/greatwall.jpg")){
  printf("Cannot read image\n");
  return -1;
 }

 // Allocated RAM for the output image
 if(!Output.Allocate(Input.Width, Input.Height, Input.Components)) return -2;

 // Need to create arrays for the color value of a pixel and those surrounding it
 // Pixel values outside the image boundary will be taken as 0
 // Colour arrays initialised with zeros
int meanTimeSeq = 0;
printf("Starting sequential sort\n");
for(int z = 0; z<meanRuns; z++){
    tic();
    bubblesort(0,1);
    float ts = toc()/1e-3;
    printf("Time of sequential sort = %lg ms\n\n", (double)ts);
    meanTimeSeq+=ts;
}
myfile << "Sequential Mean Execution Time:\n";
myfile << meanTimeSeq/meanRuns;
myfile << "\n";

if(!Output.Write("Data/SeqOutput.jpg")){
  printf("Cannot write image\n");
  return -3;
 }
 //Spawn threads...
 int       Thread_ID[Thread_Count]; // Structure to keep the tread ID
 pthread_t Thread   [Thread_Count]; // pThreads structure for thread admin
 int meanThreadRunTime = 0;
 for(int z = 0; z<meanRuns; z++){
    tic();
    for(j = 0; j < Thread_Count; j++){
        Thread_ID[j] = j;
        pthread_create(Thread+j, 0, Thread_Main, Thread_ID+j);
    }


    //tic();
    // Wait for threads to finish
    for(j = 0; j < Thread_Count; j++){
        if(pthread_join(Thread[j], 0)){
            pthread_mutex_lock(&Mutex);
            printf("Problem joining thread %d\n", j);
            pthread_mutex_unlock(&Mutex);
    }
    }

    //No more active threads, so no more critical sections requipixelComponent
    printf("All threads have quit\n");
    float tt = toc()/1e-3;
    printf("Time taken for threads to run = %lg ms\n", tt);
    meanThreadRunTime += tt;
 }
 myfile << "Mean Multi-Threaded Runtime: \n";
 myfile << meanThreadRunTime/meanRuns;
 myfile << "\n";
//Write the output image
if(!Output.Write("Data/ThreadOutput.jpg")){
  printf("Cannot write image\n");
  return -3;
 }

 return 0;
}

void bubblesort(int startInt, int split){
//out[0].Allocate(Input.Width, Input.Height, Input.Components);
//Output2.Allocate(Input.Width, Input.Height/split, Input.Components);
//JPEG Output2;
//Output2.Allocate(Input.Width, Input.Height, Input.Components);
int x, y;
int j, k;
int start = startInt*(ceil(Input.Height/split));
int end = start + ceil(Input.Height/split);
int index;
for(y = start; y < end; y++){
    for(x = 0; x < Input.Width*Input.Components; x++){
        //memset(pixelComponent, 0, sizeof(pixelComponent)); 
        int pixelComponent[81];
        int k=0;
        //printf("x: %d y: %d \n", x,y);
        // Rxample row = [R,G,B,R,G,B,...,B]
        // Color arrays index corresponds to pixel and surrounding cells as follows: |0|1|2|
        //                                                                           |3|4|5|
        //                                                                           |6|7|8|
        // First check for edge cases
        // Starting with left border
        //index = 40;
        //printf("x %d   y %d \n",x,y);
        //pixelComponent[index] = Input.Rows[y][x];
        
        for (int j = -4; j <= 4; j++) //4 down & up four from y
            {
                for (int i = (-4 * Input.Components); i <= (4 * Input.Components); i += 3, k++) //each 1 = 3
                {

                    if (((y + j) < 0) || ((y + j) >= (Input.Height)))
                    {
                        pixelComponent[k] = 0;
                    }
                    else if (((x + i) < 0) || ((x + i) >= (Input.Components * Input.Width)))
                    {
                        pixelComponent[k] = 0;
                    }
                    else
                    {
                        pixelComponent[k] = Input.Rows[y + j][x + i];
                    }
                }
            }
    bubbleSort(pixelComponent, sizeof(pixelComponent)/sizeof(pixelComponent[0]));
    //quickSort(pixelComponent,0,80);
	Output.Rows[y][x] = pixelComponent[39];
    }
 }


}






//------------------------------------------------------------------------------
