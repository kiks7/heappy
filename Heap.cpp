#include <cstdio>
#include <cinttypes>
#include "Heap.h"
#include <cstdlib>
#include <string.h>
#include "Colors.h"




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

Heap::Heap(uint64_t size, void* malloc_ptr){
	// In order to control resue of freed chunk we need to malloc from main
	// so get the pointer from malloc as arg
	_user_size = size;
	_malloc_ptr = malloc_ptr;
    // TODO: think not 0x10 btw to verify
    _chunk_ptr = _malloc_ptr - 0x10;
    memset(_malloc_ptr,0x41, size);
	printf("\n[**] Allocated. malloc return ptr --> 0x%x", _malloc_ptr);
    this->setSizeChunk();
	_number = NUMBER;
	NUMBER++;
}


// getters
uint64_t Heap::getUserSize() { return _user_size;}
uint64_t Heap::getMallocSize() { return *_malloc_size_ptr;}
void* Heap::getMallocPointer() { return _malloc_ptr;}
void* Heap::getChunkPointer() { return _chunk_ptr;}
uint64_t Heap::getNumber() { return _number;}
uint64_t Heap::getBK() { return *_bk;}
uint64_t Heap::getFD() { return *_fd;}
bool Heap::isFree() { return _free;}

// Setters
void Heap::setSizeChunk(){
	_malloc_size_ptr = (uint64_t*) _malloc_ptr-0x1; // 0x1 cuz uint64_t is 8 bytes
}

void Heap::setBK(){
    _bk = (uint64_t *) _malloc_ptr;
    //printf("\n[Debug] BK is: %x", *_bk);
}

void Heap::setFD(){
    _fd =(uint64_t *) _malloc_ptr + 0x1;
    //printf("\n[Debug] FD is: %x", *_fd);
}

bool Heap::freeChunk(){
	// free an heap chunk
	free(_malloc_ptr);
    _free = true;
    this->setBK();
    this->setFD();
	printf("\n[**] Freed %x ",_malloc_ptr);
}


void Heap::showInfo(bool verbose){

	// Idk how to use macro in order to use a color function in a printf
	// so here it is
	// TODO: improve with a macro plz
    if(!this->isFree()){
		printf(BWhite("\n[Chunk %d -------------------------------------"), this->getNumber());
		// Just print info about the current chunk
		printf(BWhite("\n| Chunk pointer: \t 0x%lx"), this->getChunkPointer());
		printf(BWhite("\n| Malloc pointer:\t 0x%lx"), this->getMallocPointer());
		printf(BWhite("\n| User size:     \t 0x%lx (%d)"), this->getUserSize(), this->getUserSize());
		printf(BWhite("\n| Malloc size:   \t 0x%lx (%d\)"), this->getMallocSize(), this->getMallocSize());
		printf(BWhite("\n| Free: "));
		printf(BWhite("N"));

	} else {
		printf(BRed("\n[Chunk %d -------------------------------------"), this->getNumber());
		// Just print info about the current chunk
		printf(BRed("\n| Chunk pointer: \t 0x%lx"), this->getChunkPointer());
		printf(BRed("\n| Malloc pointer:\t 0x%lx"), this->getMallocPointer());
		printf(BRed("\n| User size:     \t 0x%lx (%d)"), this->getUserSize(), this->getUserSize());
		printf(BRed("\n| Malloc size:   \t 0x%lx (%d\)"), this->getMallocSize(), this->getMallocSize());
		printf(BRed("\n| Free: "));
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

void Heap::reallocated(){
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

