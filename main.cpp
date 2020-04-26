#include <string.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <cstdio>
#include <cinttypes>
#include "Heap.h"
#include "Colors.h"
#include <unistd.h>
#include <array>

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
/* 
 * Input heap size and program will alloc it
 * then add the possibility to free random heap by indexes
 *
 * */

void clear(){
    system("clear");
    //printf("GDB");
}

void help(){
	fprintf(stdout, "\n------------");
	fprintf(stdout, "\n1. Allocate a new heap chunk");
	fprintf(stdout, "\n2. Free an heap chunk");
	fprintf(stdout, "\n3. Print chunk info");
	fprintf(stdout, "\n4. Print heap layout");
	fprintf(stdout, "\n------------\n");
}

void help_game(){
	fprintf(stdout, "[-------------------");
	fprintf(stdout, "\nCommands:");
	fprintf(stdout, "\nmalloc <size>");
	fprintf(stdout, "\nfree <chunk_number>");
	fprintf(stdout, "\nshow <chunk_number>");
	fprintf(stdout, "\n-------------------]\n");
}


int choose_index(uint limit){
    //choose the index and make sure to not chose a non existing one
    uint result = 9999;
    while(result >= limit){
        std::cout << "Index > ";
        std::cin >> result;
    }
    return result;
}

void press(){
    // Just continue the execution
    std::cout << std::endl << " Press Any Key to continue .." << std::endl;
    getchar();
}

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

int manual_loop(){
 	// Alloc something and put something in it
	int32_t heap_size = 0;
	Heap heap_array[100];

	uint64_t * heap_ptr[100];


	int i = 0; // heap index
	int command;
	int index; // used to reference a heap chunk
                // TODO: Maybe integer over here btw
    clear();
    void* temp_malloc; //used as temporary pointer for malloc
    bool reused=false;
	while(true){
		// Infinite loop
		help();
		fprintf(stdout, "\n[?] Command > ");
		scanf("%d", &command);

		switch(command){
		case 1:
            clear();
			fprintf(stdout, "\n[?] Heap size to allocate: ");
			scanf("%d", &heap_size);
            // I need to perform the malloc here.
            // Because i need to update freed chunks
            temp_malloc = malloc(heap_size);
            // and now check that the chunk has not been already inserted
            reused=false;
            for(int j=0; j < i; j++){
                if(heap_array[j].getMallocPointer() == temp_malloc){
                    heap_array[j].reallocated(heap_array, i);
                    reused=true;
                }
            }
            if (!reused){
                heap_array[i] = Heap(heap_size, temp_malloc, heap_array, i);
			    i++;
            }
			break;
		case 2:
            clear();
            index = choose_index(i);
			heap_array[index].freeChunk();
			break;
		case 3:
            clear();
            index = choose_index(i);
            heap_array[index].showInfo(true);
			break;
		case 4:
            // print heap layout starting from the first allocated to the last one
            // print also the first and the last allocated heap address
            clear();
            printf("\n[*] First allocated chunk: 0x%x", heap_array[0].getMallocPointer());
            printf("\n[*] Last allocated chunk: 0x%x", heap_array[i-1].getMallocPointer());
            for(int j=0; j < i; j++){
                heap_array[j].showInfo(false);
            }


		default:
			fprintf(stdout, "\nCommand not found");
			break;
	}

	}
   
}

int game_loop(){
    setbuf(stdout, NULL);
	clear();	

	std::cout << "Input your chunks. For example, if you want to play with 3 chunks of size 10,20 and 30 the just enter: 10 20 30" << std::endl;
    std::cout << "Tell me chunk sizes > ";
    std::string input_chunks;
    std::cin.ignore();
    std::getline (std::cin, input_chunks);
    char * spl_chunks;
    uint64_t chunks [100];
    int i_heap; //heap allocated. TODO: change in this in other mode
    Heap heap_array[100];
    void* temp_malloc; //used as temporary pointer for malloc
    spl_chunks = strtok((char *)input_chunks.c_str()," " );
    int i=0;
    while (spl_chunks != NULL)
    {
        if(i < (sizeof(chunks)/sizeof(uint64_t)))
            chunks[i] = atoi(spl_chunks);
        else
            printf("\nLimit chunks reached (%x). Not putting %d",sizeof(chunks)/sizeof(uint64_t), atoi(spl_chunks));
        //printf ("%s)\n",spl_chunks);
        spl_chunks = strtok(NULL, " ");
        i++;
    }
    uint64_t chunks_len = i;
    std::cout << std::endl << "These will be your chunks: " << std::endl;
    for(int i=0; i < chunks_len; i++){
        printf("\n[%d] %d", i, chunks[i]);
    }
    press();
    // now alloc what requested
    for(int i=0; i < chunks_len; i++){
        printf("\n[*] Allocating %d for chunk %d .. ", chunks[i], i);
        temp_malloc = malloc(chunks[i]);
        heap_array[i_heap] = Heap(chunks[i], temp_malloc, heap_array, i_heap);
        i_heap++;
    }
    printf("Everything allocated, let's start !");
    // now all chunks are allocated. need to start the game
    // will be a while true with everytime show the heap and possibility to free/alloc new heaps
    int index;
    bool reused;
    uint32_t heap_size = 0;
    std::string cmd_delimiter = " ";
    std::string cmd_first;
    std::string cmd_second;
    std::string cmd_temp;
    int cmd=0;
    while(true){
        // CORE INFINITE LOOP
        // everything initialized, can start with the core
        // show help and chunks
        clear();
        help_game();
        printf("\n[*] First allocated chunk: 0x%x", heap_array[0].getMallocPointer());
        printf("\n[*] Last allocated chunk: 0x%x", heap_array[i_heap-1].getMallocPointer());
        // show chunks info
        for(int j=0; j < i_heap; j++){
            heap_array[j].showInfo(false);
            printf("\n");
            }


        std::cout << "Command > ";
        std::string command;
        std::getline(std::cin, command);

        // get the cmd part
        cmd_first = command.substr(0, command.find(cmd_delimiter));
        cmd = 0;
        if(cmd_first.compare("malloc") == 0)
            cmd = 1;

        if(cmd_first.compare("free") == 0)
            cmd = 2;

        if(cmd_first.compare("show") == 0)
            cmd = 3;

        // get the parameter part
        if(command.find(cmd_delimiter) != -1){
            // or it will cause a runtime exception
            cmd_temp = command.substr(command.find(cmd_delimiter), command.size()+1);
            cmd_second = cmd_temp.substr(0, command.find(cmd_delimiter));
        }
        else{
            // missing the 2nd argument. 777 will throw bad syntax
            if(cmd != 0)
                cmd = 777;

        }
		//just a test
		//heap_array[0].test(heap_array, i_heap);
		//sleep(10);
        switch(cmd){
            case 1:
                //alloc
                heap_size = std::atoi(cmd_second.c_str());
                //printf("[*] Allocating %d ", heap_size);
                temp_malloc = malloc(heap_size);
                // and now check that the chunk has not been already inserted
                reused=false;
                for(int j=0; j < i_heap; j++){
                    if(heap_array[j].getMallocPointer() == temp_malloc){
                        heap_array[j].reallocated(heap_array, i_heap);
                        reused=true;
                    }
                }
                if (!reused){
                    heap_array[i_heap] = Heap(heap_size, temp_malloc, heap_array, i_heap);
                    i_heap++;
                }

                break;
            case 2:
                // free
                index = std::atoi(cmd_second.c_str());
                if(index < i_heap)
                    heap_array[index].freeChunk(heap_array, i_heap);
                else
                    printf("[-] Invalid chunk index .. ");
                    sleep(1);
                break;
            case 3:
                // examine
                index = std::atoi(cmd_second.c_str());
                if(index < i_heap){
                    heap_array[index].showInfo(true);
                    press();
                }
                else {
                    printf("[-] Inalid chunk index .. ");
					sleep(1);
				}
                break;
            case 777:
                printf("Bad syntax ..");
                sleep(1);
                break;
            default:
                printf("Command not found ..");
                sleep(1);
                break;
        }

    }

}

int main(){
	clear();
    printf("1 for manual mode\n2 for demo mode\n> ");
    int choose;
    scanf("%d", &choose);
    if(choose == 1)
        manual_loop();
    else if(choose == 2)
        game_loop();
}
