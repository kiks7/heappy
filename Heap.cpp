#include <cstdio>
#include <cinttypes>
#include "Heap.h"
#include <cstdlib>
#include <string.h>
#include "Colors.h"



#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

uint64_t NUMBER = 0;
void printHexQword(void* start_address, void* end_address);


Heap::Heap(){}
Heap::Heap(uint64_t size){
	// get size, call malloc and initialize memory
	_user_size = size;
	_malloc_ptr = malloc(size);
    _chunk_ptr = _malloc_ptr - 0x10;
    memset(_malloc_ptr,0x41, size);
	printf("\n[**] Allocated. malloc return ptr --> 0x%x", _malloc_ptr);
    this->setSizeChunk();
	_number = NUMBER;
	NUMBER++;
}

Heap::Heap(uint64_t size, void* malloc_ptr, Heap* list, uint64_t n_chunks){
	// In order to control resue of freed chunk we need to malloc from main
	// so get the pointer from malloc as arg
	_user_size = size;
	_malloc_ptr = malloc_ptr;
    // TODO: think not 0x10 btw to verify
    _chunk_ptr = _malloc_ptr - 0x10;
    memset(_malloc_ptr,0x41, size);
	printf("\n[**] Allocated. malloc return ptr --> 0x%x", _malloc_ptr);
    this->setSizeChunk();
    this->setBK();
    this->setFD();
	_number = NUMBER;
	NUMBER++;
}

// getters
uint64_t Heap::getUserSize() { return _user_size;}
void* Heap::getMallocPointer() { return _malloc_ptr;}
void* Heap::getChunkPointer() { return _chunk_ptr;}
uint64_t Heap::getNumber() { return _number;}
uint64_t Heap::getBK() { return *_bk;}
uint64_t Heap::getFD() { return *_fd;}
bool Heap::isFree() { return _free;}
bool Heap::isDeleted(){return _deleted;}
void* Heap::getNextChunk() { return this->getMallocPointer() + this->getMallocSize();}
uint64_t Heap::getMallocSize() { 
    if(CHECK_BIT(*_malloc_size_ptr, 0))
        return *_malloc_size_ptr - 0x1;
    else
        return *_malloc_size_ptr;
}
uint64_t Heap::getMallocPrevSize() { return *_malloc_prev_size_ptr;}

// Setters

void Heap::resumeChunk(){_deleted=0;} 

void Heap::setSizeChunk(){
	_malloc_size_ptr = (uint64_t*) _malloc_ptr-0x1; // 0x1 cuz uint64_t is 8 bytes
	_malloc_prev_size_ptr = (uint64_t*) _malloc_ptr-0x2; // 0x1 cuz uint64_t is 8 bytes
}

void Heap::deleteChunk(){_deleted = 1;}

void Heap::setBK(){
    _bk = (uint64_t *) _malloc_ptr + 0x1;
    //printf("\n[Debug] BK is: %x", *_bk);
}

void Heap::setFD(){
    _fd =(uint64_t *) _malloc_ptr;
    //printf("\n[Debug] FD is: %x", *_fd);
}


bool Heap::getPrevInUse(){ return CHECK_BIT(*_malloc_size_ptr, 0);}
bool Heap::getMmapped(){ return CHECK_BIT(this->getMallocSize(), 1);}
bool Heap::getNonMainArena(){ return CHECK_BIT(this->getMallocSize(), 2);}


bool Heap::freeChunk(){
	// free an heap chunk
	free(_malloc_ptr);
    _free = true;
	printf("\n[**] Freed %x ",_malloc_ptr);
}

bool Heap::freeChunk(Heap *list, uint64_t n_chunks){
    // free a chunk and merge successive chunks
    uint64_t temp_size = this->getMallocSize();
	free(_malloc_ptr);
    _free = true;
    this->mergedChunks(list, n_chunks);
    // Now, if the size change, means the next chunk is also freed and they are merged
    // but, if the prev chunk is freed, then the prev chunk suze is modified
    // We need to put delete flag on this merged chunk 
	printf("\n[**] Freed %x ",_malloc_ptr);
}

bool Heap::mergedChunks(Heap *list, uint64_t n_chunks){
    // when allocating a chunk or freeing a new one, be sure to align the memory
    // A linked list could be the best solution (as it works with them) but I can bypass it in this way :P
    for(int i=0; i < n_chunks; i++){
        /* Check if the prev or next chunks are merged */
        //get the ptr where the chunk ends
        void* end_list_chunk = list[i].getMallocPointer() + list[i].getMallocSize();
        // if the prev one is merging us
        if( this->getMallocPointer() > list[i].getMallocPointer() && this->getMallocPointer() < end_list_chunk){
            // The previous chunk has merged this one, then this one is not existing anymore
            this->deleteChunk();
        }
        if( this->getMallocPointer() < list[i].getMallocPointer() && (this->getMallocPointer()+this->getMallocSize()) > list[i].getMallocPointer()){
            list[i].deleteChunk();
        }
        /* Check that the chunk hasn been resized after a malloc, and resume removed chunks*/
        // only the next one, 

        //printf("\n[D] %lx <= %lx", this->getMallocPointer() + this->getMallocSize() , list[i].getMallocPointer());
        bool end = 0;

        if(this->getNextChunk() == list[i].getMallocPointer()){
            printf("\nSAMEEEE");
            if((this->getMallocPointer() + this->getMallocSize()) <= list[i].getMallocPointer()){
                if(list[i].isDeleted())
                    list[i].resumeChunk();
            }
        }

    }

}


void Heap::showInfo(bool verbose){

	// Idk how to use macro in order to use a color function in a printf
	// so here it is
	// TODO: improve with a macro plz
    if(!this->isDeleted()){
        if(!this->isFree()){
            printf(BWhite("\n[Chunk %d -------------------------------------"), this->getNumber());
            // Just print info about the current chunk
            printf(BWhite("\n| Chunk pointer: \t 0x%lx"), this->getChunkPointer());
            printf(BWhite("\n| Malloc pointer:\t 0x%lx"), this->getMallocPointer());
            printf(BWhite("\n| User size:     \t 0x%lx (%d)"), this->getUserSize(), this->getUserSize());
            printf(BWhite("\n| Malloc size:   \t 0x%lx (%d\) //0x%lx"), this->getMallocSize(), this->getMallocSize(), *_malloc_size_ptr);
            printf(BWhite("\n| PREV_INUSE:    \t "));
            if(this->getPrevInUse())
                printf(BWhite("Y"));
            else
                printf((BWhite("N")));
            printf(BWhite("\n| Prev chunk size:   \t 0x%x (%d) "), this->getMallocPrevSize(), this->getMallocPrevSize());
            printf(BWhite("\n| Free:    \t\t "));
            printf(BWhite("N"));
            printf(BWhite("\n| bk:\t 0x%lx "), this->getBK());
            printf(BWhite("\n| fd:\t 0x%lx "), this->getFD());


        } else {
            printf(BRed("\n[Chunk %d -------------------------------------"), this->getNumber());
            // Just print info about the current chunk
            printf(BRed("\n| Chunk pointer: \t 0x%lx"), this->getChunkPointer());
            printf(BRed("\n| Malloc pointer:\t 0x%lx"), this->getMallocPointer());
            printf(BRed("\n| User size:     \t 0x%lx (%d)"), this->getUserSize(), this->getUserSize());
            printf(BRed("\n| Malloc size:   \t 0x%lx (%d\) // 0x%lx"), this->getMallocSize(), this->getMallocSize(), *_malloc_size_ptr);
            printf(BRed("\n| PREV_INUSE:    \t "));
            if(this->getPrevInUse())
                printf(BRed("Y"));
            else
                printf((BRed("N")));
            printf(BRed("\n| Prev chunk size:   \t 0x%x (%d) "), this->getMallocPrevSize(), this->getMallocPrevSize());

            printf(BRed("\n| Free:    \t\t "));
            printf(BRed("Y"));
            printf(BGreen("\n| bk:\t 0x%lx "), this->getBK());
            printf(BCyan("\n| fd:\t 0x%lx "), this->getFD());

            }
        if (verbose)
            this->dumpHeapContent();
        if(this->isFree())
            printf(BRed("\n|--------------------------------------------- "), this->getNumber());
        else
            printf(BWhite("\n|--------------------------------------------- "), this->getNumber());
    }
}


void Heap::dumpHeapContent(){
    // dump heap content as HEX
    // from _chunkc_ptr to (_chunkc_ptr + size)
    void* ptr = this->getChunkPointer();
    // 0x10 is the chunk header, part of the defined malloc size defined in the header
    void* end_malloc_ptr = ((ptr + this->getMallocSize()))-0x1;

    printf(BYellow("\nHexdump ( 0x%lx --> 0x%lx )"), ptr, end_malloc_ptr);
    printf(BYellow("\n[--------------------------------"));
    printHexQword(ptr, end_malloc_ptr);
    printf(BYellow("\n--------------------------------]"));
    //printf("\n[D] malloc_ptr --> %lx", ptr);
    //printf("\n[D] end_malloc_ptr --> %lx", end_malloc_ptr);
}

void Heap::reallocated(Heap* list, uint64_t n_chunks){
    this->mergedChunks(list, n_chunks);
   _free = false;
}



// Useful
void printHexQword(void* start_address, void* end_address){
    while(start_address <= end_address){
        //printf("\n0x%lx: 0x%016lx 0x%016lx", start_address,  *(uint64_t *) start_address, *(uint64_t*) start_address + sizeof(uint64_t));
        //printf(BYellow("\n0x%lx:")" "Yellow("0x%016lx 0x%016lx"), start_address ,*(int64_t *) start_address, *(uint64_t *) ((void*) start_address + sizeof(uint64_t)));
        printf(BYellow("\n0x%lx:")" "Yellow("0x%016lx"), start_address ,*(int64_t *) start_address);
        start_address += sizeof(uint64_t); // go to the next QWORD
    }
}



void Heap::test(Heap *list, int n_chunks){
    // n_chunks is the list of allocated chunks
    printf("[!] This is jus a test function\n");

    for(int i=0; i < n_chunks; i++){
        printf("\nTest: %x", list[i]);
    }
}
