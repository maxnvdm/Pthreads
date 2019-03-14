//==============================================================================
// Copyright (C) John-Philip Taylor
// tyljoh010@myuct.ac.za
//
// This file is part of the EEE4084F Course
//
// This file is free software: you can redistribute it and/or modify
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

//void bubblesort(JPEG out[6],int startInt, int split);
void bubblesort(int startInt, int split);

//------------------------------------------------------------------------------

// This is each thread's "main" function.  It receives a unique ID
void* Thread_Main(void* Parameter){
 int ID = *((int*)Parameter);

//  pthread_mutex_lock(&Mutex);
//   printf("Hello from thread %d\n", ID);
//  pthread_mutex_unlock(&Mutex);

//  for(int j = 0; j < 1e6; j++); // Do some heavy calculation

//  pthread_mutex_lock(&Mutex);
//   printf("Thread %d: I QUIT!\n", ID);
//  pthread_mutex_unlock(&Mutex);

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

int main(int argc, char** argv){
 int j;

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
printf("Starting sequential sort\n");
tic();
bubblesort(0,1);
printf("Time of sequential sort = %lg ms\n\n", (double)toc()/1e-3);
if(!Output.Write("Data/SeqOutput.jpg")){
  printf("Cannot write image\n");
  return -3;
 }
 // Spawn threads...
 int       Thread_ID[Thread_Count]; // Structure to keep the tread ID
 pthread_t Thread   [Thread_Count]; // pThreads structure for thread admin
tic();
 for(j = 0; j < Thread_Count; j++){
  Thread_ID[j] = j;
  pthread_create(Thread+j, 0, Thread_Main, Thread_ID+j);
 }

 // Printing stuff is a critical section...
//  pthread_mutex_lock(&Mutex);
//   printf("Threads created :-)\n");
//  pthread_mutex_unlock(&Mutex);

 //tic();
 // Wait for threads to finish
 for(j = 0; j < Thread_Count; j++){
  if(pthread_join(Thread[j], 0)){
   pthread_mutex_lock(&Mutex);
    printf("Problem joining thread %d\n", j);
   pthread_mutex_unlock(&Mutex);
  }
 }

 // No more active threads, so no more critical sections required
 printf("All threads have quit\n");
 printf("Time taken for threads to run = %lg ms\n", toc()/1e-3);

// Write the output image
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
int red[9];
int start = startInt*(ceil(Input.Height/split));
int end = start + ceil(Input.Height/split);

for(y = start; y < end; y++){
    for(x = 0; x < Input.Width*Input.Components; x++){
        //memset(red, 0, sizeof(red)); 
        for(int a=0; a < 10; a++){
            red[a]=0;
        }
        //printf("x: %d y: %d \n", x,y);
        // Rxample row = [R,G,B,R,G,B,...,B]
        // Color arrays index corresponds to pixel and surrounding cells as follows: |0|1|2|
        //                                                                           |3|4|5|
        //                                                                           |6|7|8|
        // First check for edge cases
        // Starting with left border
        if(x<4){
            // Check if we're on a corner pixel
            if(y<1){
                red[4] = Input.Rows[y][x];
                red[5] = Input.Rows[y][x+3];
                red[7] = Input.Rows[y+1][x];
                red[8] = Input.Rows[y+1][x+3];
            }
            else if (y==Input.Height-1){
                red[1] = Input.Rows[y-1][x];
                red[2] = Input.Rows[y-1][x+3];
                red[4] = Input.Rows[y][x];
                red[5] = Input.Rows[y][x+3];
            }
            // Left border case
            else{
                red[1] = Input.Rows[y-1][x];
                red[2] = Input.Rows[y-1][x+3];
                red[4] = Input.Rows[y][x];
                red[5] = Input.Rows[y][x+3];
                red[7] = Input.Rows[y+1][x];
                red[8] = Input.Rows[y+1][x+3];
            }
        }
        // Check right border
        else if (x>=Input.Width*Input.Components-3) {
            if(y<1){
                red[4] = Input.Rows[y][x];
                red[3] = Input.Rows[y][x-3];
                red[6] = Input.Rows[y+1][x-3];
                red[7] = Input.Rows[y+1][x];
            }
            else if (y==Input.Height-1){
                red[0] = Input.Rows[y-1][x-3];
                red[1] = Input.Rows[y-1][x];
                red[4] = Input.Rows[y][x];
                red[3] = Input.Rows[y][x-3];
            }
            else{
                red[0] = Input.Rows[y-1][x-3];
                red[1] = Input.Rows[y-1][x];
                red[4] = Input.Rows[y][x];
                red[7] = Input.Rows[y+1][x];
                red[3] = Input.Rows[y][x-3];
                red[6] = Input.Rows[y+1][x-3];
            }
        }
        else if (y==0) {
            red[3] = Input.Rows[y][x-3];
            red[4] = Input.Rows[y][x];
            red[5] = Input.Rows[y][x+3];
            red[6] = Input.Rows[y+1][x-3];
            red[7] = Input.Rows[y+1][x];
            red[8] = Input.Rows[y+1][x+3];
        }
        else if (y==Input.Height-1){
            red[0] = Input.Rows[y-1][x-3];
            red[1] = Input.Rows[y-1][x];
            red[2] = Input.Rows[y-1][x+3];
            red[4] = Input.Rows[y][x];
            red[5] = Input.Rows[y][x+3];
            red[3] = Input.Rows[y][x-3];
            }
        
        // Non-border case
        else{
            red[0] = Input.Rows[y-1][x-3];
            red[1] = Input.Rows[y-1][x];
            red[2] = Input.Rows[y-1][x+3];
            red[3] = Input.Rows[y][x-3];
            red[4] = Input.Rows[y][x];
            red[5] = Input.Rows[y][x+3];
            red[6] = Input.Rows[y+1][x-3];
            red[7] = Input.Rows[y+1][x];
            red[8] = Input.Rows[y+1][x+3];
        }
        // for(int z=0;z<10;z++){
        //     printf(" %d ",red[z]);
        // }
        // printf("\n");
        bubbleSort(red, sizeof(red)/sizeof(red[0]));
        // for(int z=0;z<10;z++){
        //     printf(" %d ",red[z]);
        // }
        // printf("\n");
        //Output2.Rows[y][x] = red[4];
	Output.Rows[y][x] = red[4];
    }
 }

 //if(!Output2.Write("Data/Output2.jpg")){
//if(!Output.Write("Data/Output2.jpg")){
//  printf("Cannot write image\n");
// }

}






//------------------------------------------------------------------------------
