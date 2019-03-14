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
// printf("Starting sequential sort\n");
// tic();
// bubblesort(0,1);
// printf("Time of sequential sort = %lg ms\n\n", (double)toc()/1e-3);
// if(!Output.Write("Data/SeqOutput.jpg")){
//   printf("Cannot write image\n");
//   return -3;
//  }
 //Spawn threads...
 int       Thread_ID[Thread_Count]; // Structure to keep the tread ID
 pthread_t Thread   [Thread_Count]; // pThreads structure for thread admin
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

 //No more active threads, so no more critical sections required
 printf("All threads have quit\n");
 printf("Time taken for threads to run = %lg ms\n", toc()/1e-3);

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
        //memset(red, 0, sizeof(red)); 
        int red[81];
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
        //red[index] = Input.Rows[y][x];
        
        for (int j = -4; j <= 4; j++) //4 down & up four from y
            {
                for (int i = (-4 * Input.Components); i <= (4 * Input.Components); i += 3, k++) //each 1 = 3
                {

                    if (((y + j) < 0) || ((y + j) >= (Input.Height)))
                    {
                        red[k] = 0;
                    }
                    else if (((x + i) < 0) || ((x + i) >= (Input.Components * Input.Width)))
                    {
                        red[k] = 0;
                    }
                    else
                    {
                        red[k] = Input.Rows[y + j][x + i];
                    }
                }
            }
    bubbleSort(red, sizeof(red)/sizeof(red[0]));
	Output.Rows[y][x] = red[39];
    }
 }


}






//------------------------------------------------------------------------------
