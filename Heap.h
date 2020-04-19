class Heap {
	public:
		Heap();
		Heap(uint64_t size);
		Heap (uint64_t size, void* ptr); 
		void* getMallocPointer();
        void* getChunkPointer();
		uint64_t getUserSize();
        uint64_t getMallocSize();
        uint64_t getMallocPrevSize();
        uint64_t getNumber();
        uint64_t getBK();
        uint64_t getFD();
        bool getPrevInUse();
        bool getMmapped();
        bool getNonMainArena();
        void reallocated(); // means used again from malloc after free

		bool freeChunk();
        bool isFree();
        void showInfo(bool);
        void dumpHeapContent();


	private:
		void setSizeChunk();
        void setFD();
        void setBK();
        void * _chunk_ptr; //the real chunk ptr
		void * _malloc_ptr;
		uint64_t * _malloc_size_ptr;
		uint64_t * _malloc_prev_size_ptr;
		uint64_t _number; // number of chunk, from 0 to N
		bool _free; 
		uint64_t _user_size;
		uint64_t* _bk;
		uint64_t* _fd;

};
