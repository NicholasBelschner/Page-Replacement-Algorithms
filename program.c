#include <stdio.h> 
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int mem_size = 0;
int* memory;
int* page_refs;

//Getting page references from a file and storing them into an array.
int get_refs(char* file_name, int mem_size){
  int num_refs = 0, page_ref, length;
  FILE *file;

  //Opening the file in reading mode.
  file = fopen(file_name, "r");
  if(file == NULL){
    printf("Failed to open the file.\n");
    exit(1);
  }

  //Moving file pointer to the end of the file. 
  fseek(file, 0, SEEK_END);

  //Get the position of the file pointer, which is the length of the file.
  length = ftell(file);

  //Move file pointer back to the beginning of the file
  fseek(file, 0, SEEK_SET);

  //Allocate memory for page references array.
  page_refs = (int*)malloc((length) * sizeof(int));

  //Initialize page references array to -1
  for(int i = 0; i < length; ++i){
    page_refs[i] = -1;
  }

  //Read page references from the filename
  while(fscanf(file, "%d", &page_ref) == 1){
    page_refs[num_refs++] = page_ref;
  }

  //Close file
  fclose(file);

  return num_refs;
}

//Need to call the method get_refs with the file name ane mem size.
//fifo is already in order. Will need to set an array of size mem size.
//Have the oldest page be replaced by the new, and count++ for page fault.
void fifo(char* file_name, int mem_size){
  int arr[mem_size];
  int page_faults = 0;

  // Call get_refs to populate the page_refs array
  int num_refs = get_refs(file_name, mem_size);

  // Initialize the array to hold the current pages in memory
  for (int i = 0; i < mem_size; i++) {
    arr[i] = -1; // -1 indicates an empty frame in memory
  }

  // Simulate FIFO using an index to track the oldest page in memory
  int oldest_index = 0;

    // Iterate through the page references
    for (int i = 0; i < num_refs; i++) {
      int current_page = page_refs[i];
      bool page_found = false;

    // Check if the current page is already in memory
    for (int j = 0; j < mem_size; j++) {
      if (arr[j] == current_page) {
        page_found = true;
        break;
      }
    }

    if (!page_found) {
      // Page fault: Replace the oldest page in memory with the current page
      arr[oldest_index] = current_page;
      oldest_index = (oldest_index + 1) % mem_size; // Circular increment
      page_faults++;
    }
  }

  // Print the number of page faults and the final state of memory
  printf("FIFO: %d page faults\n", page_faults);
  printf("Final state of memory: ");
  for (int i = 0; i < mem_size; i++) {
    printf("%d ", arr[i]);
  }
  printf("\n");
}

void lru(char* file_name, int mem_size){
  int arr[mem_size];
  int lru_order[mem_size]; // Array to track the order of page access
  int page_faults = 0;

  // Call get_refs to populate the page_refs array
  int num_refs = get_refs(file_name, mem_size);

  // Initialize the arrays to hold the current pages in memory and their order of access
  for (int i = 0; i < mem_size; i++) {
    arr[i] = -1; // -1 indicates an empty frame in memory
    lru_order[i] = 0; // Initialize all pages' access order to 0
  }

  int current_order = 1; // Counter to keep track of page access order

  // Iterate through the page references
  for (int i = 0; i < num_refs; i++) {
    int current_page = page_refs[i];
    bool page_found = false;

    // Check if the current page is already in memory
    for (int j = 0; j < mem_size; j++) {
      if (arr[j] == current_page) {
        page_found = true;
        lru_order[j] = current_order++; // Update the access order of the page
        break;
      }
    }

    if (!page_found) {
      // Page fault: Find the least recently used page and replace it with the current page
      int lru_index = 0;
      int lru_value = lru_order[0];

      for (int j = 1; j < mem_size; j++) {
        if (lru_order[j] < lru_value) {
          lru_index = j;
          lru_value = lru_order[j];
        }
      }

      arr[lru_index] = current_page;
      lru_order[lru_index] = current_order++; // Update the access order of the new page
      page_faults++;
    }
  }

  // Print the number of page faults and the final state of memory
  printf("LRU: %d page faults\n", page_faults);
  printf("Final state of memory: ");
  for (int i = 0; i < mem_size; i++) {
    printf("%d ", arr[i]);
  }
  printf("\n");
}

int main(int argc, char* argv[]){

  if (argc != 3){
    printf("Usage: %s pagereffile memorysize\n", argv[0]);
    return 1;
  }

  //Get file name and memory size from command line arguments
  char* file_name = argv[1];
  mem_size = atoi(argv[2]);

  //Check if memory size is within the valid range
  if(mem_size < 1 || mem_size > 10){
    printf("Error: memory size must be between 1 and 10\n");
    return 1;
  }

  //Call page replacement functions
  fifo(file_name, mem_size);
  lru(file_name, mem_size);

  //Free dynamically allocated memory for page references
  free(page_refs);

  return 0;

}
