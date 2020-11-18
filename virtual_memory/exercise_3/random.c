#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define HIGH 20
#define FREQ 80
#define PAGES 100
#define SAMPLES 20

typedef struct pte {
  int present;
} pte;

void init(int *sequence, int refs, int pages)  {

  int high = (int)(pages*((float)HIGH/100));

  for(int i = 0; i<refs; i++) {
    if(rand()%100 < FREQ) {
      /* the frequently case */
      sequence[i] = rand()%high;        
    } else {
      /* the less frequently case */
      sequence[i] = high + rand()%(pages - high);
    }
  }
}

int simulate(int *sequence, pte *page_table, int refs, int frames, int pages) {

  int hits = 0;
  int allocated = 0;
    
  for(int i = 0; i < refs; i++) {
    int next = sequence[i];
    pte *entry = &page_table[next];

    if(entry->present == 1) {
      hits++;
    } else {
      if(allocated < frames) {
	allocated++;
	entry->present = 1;
      } else {
	pte *evict;
	do {
	  int rnd = rand()%pages;
	  evict = &page_table[rnd];
	} while(evict->present != 1);

	evict->present = 0;
	entry->present = 1;
      }
    }
  }
  
  return hits;
}

void clear_page_table(pte *page_table, int pages) {
    for(int i=0; i < pages; i++) {
      page_table[i].present = 0;
    }
}

int main(int argc, char *argv[]) {

/* the number of references, should be a command line argument */
  int refs = 100000;

  /* the number of pages of a process, should be a command line argument */
  int pages = 100;
  
  int *sequence = (int*)malloc(refs*sizeof(int)); 

  init(sequence, refs, pages);

  pte *page_table = (pte *)malloc(pages*sizeof(pte)); 
  /*  pte page_table[PAGES]; */

  printf("# This is a benchmark of random replacement policy\n");
  printf("# %d page references\n", refs);    
  printf("# %d pages \n", pages);  
  printf("#\n#\n#frames\tratio\n");    
  
  /*  frames is the size of the memory in frames */
  int frames;

  int incr = pages/SAMPLES;
  
  for(frames = incr; frames <= pages; frames += incr) {

    /* clear page tables entries */
    clear_page_table(page_table, pages);

    int hits =  simulate(sequence, page_table, refs, frames, pages);

    float ratio = (float)hits/refs;

    printf("%d\t%.2f\n", frames, ratio);
  } 

  


  return 0;
}