#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_NUM_PAGES 4194304 //Equals 16GB assuming 4KB pages
#define PAGE_SIZE 4096 //in bytes
#define DEBUG_FLAG 0

#include <sys/mman.h>
#define NR_mmap 9

int main(const int argc, const char *argv[]) {
	unsigned long MB_to_alloc = 2048; //How many MB we want to use for the test
	unsigned long page = 0; //for iterating through all allocated pages
	unsigned long total_pages = 0;
	unsigned long val = ~0; //dummy variable
	unsigned long word = 0; //for iterating through words in a page
	unsigned long **page_array = NULL; //to store pointers to each page we allocate
	int iter = 1; //counter
	unsigned long pageCount = 0;
	unsigned long wordsPerPage = PAGE_SIZE/sizeof(val);
	int part = 0;

	for (part = 0; part < 2; part++) {
		printf("\n==========================================================\nPart: %0d\n", part+1);

		//allocate the huge array for storing page address pointers
		page_array = (unsigned long **) syscall(NR_mmap, NULL, MAX_NUM_PAGES*sizeof(unsigned long *), (PROT_READ | PROT_WRITE), (MAP_ANONYMOUS | MAP_PRIVATE), -1, 0);
		if (page_array == MAP_FAILED) {
			printf("Error: Could not allocate page array using mmap. Terminating.\n");
			perror("mmap");
			return 1;
		}

		page = 0;
		iter = 1;
		
		printf("Hogging memory using mmap...\n");
		while ((page < MAX_NUM_PAGES) && (page < MB_to_alloc*1024*1024/PAGE_SIZE) && (*(page_array+page) = (unsigned long *) syscall(NR_mmap, NULL, PAGE_SIZE, (PROT_READ | PROT_WRITE), (MAP_ANONYMOUS | MAP_PRIVATE), -1, 0)) != MAP_FAILED) 
			page++;
		if (*(page_array+page) == MAP_FAILED) {
			printf("Error: Failed to mmap page number %lu, at %0.2f MB.\n", page, (double)page*PAGE_SIZE/(1024*1024));
			perror("mmap");
		}

		total_pages = page;
		printf("We allocated %lu pages == %lu KB == %0.2f MB == %0.2f GB.\n\n", total_pages, total_pages*PAGE_SIZE/1024, (double)total_pages*PAGE_SIZE/(1024*1024), (double)total_pages*PAGE_SIZE/(1024*1024*1024));
		printf("Writing pattern: 0x%X %X\n", (unsigned int) (val>>32), (unsigned int) val);

		for (iter = 0; iter < 10; iter++) {
			for (page = 0; page < total_pages; page++)
				for (word = 0; word < wordsPerPage; word++) { //for each word in the page
					*(*(page_array+page)+word) = val; //Write all 1s to the word
#if DEBUG_FLAG == 1	
						fprintf(stderr, "page %d, word %d: page_array+page == 0x%X, *(page_array+page) == 0x%X, *(page_array+page)+word == 0x%X, *(*(page_array+page)+word) == 0x%X\n", page, word, page_array+page, *(page_array+page), *(page_array+page)+word, *(*(page_array+page)+word));
#endif
				}
			
			printf("Finished write iteration %d\n", iter);
		}

		//free memory here
		printf("\nFreeing memory...\n");
		for (page = 0; page < total_pages; page++)
			munmap(*(page_array+page), PAGE_SIZE);
		munmap(page_array, MAX_NUM_PAGES);
		if (part == 0) {
			printf("Pausing for 5s...\n");
			sleep(5);
		}
	}
	
	printf("Done.\n");

	return 0; 
}


