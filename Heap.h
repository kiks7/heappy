class Heap {
	public:
		Heap();
		Heap(uint64_t size);
		Heap (uint64_t size, void* ptr, Heap*, uint64_t);
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
        void reallocated(Heap *, uint64_t); // means used again from malloc after free
        bool mergedChunks(Heap*, uint64_t);

        bool isDeleted();
		bool freeChunk();
		bool freeChunk(Heap*, uint64_t);
        bool isFree();
        void showInfo(bool);
        void dumpHeapContent();
        void test(Heap*, int n_chunks); // just a test function
        void* getNextChunk();


	private:
		void setSizeChunk();
        void setFD();
        void setBK();
        void deleteChunk();
        void resumeChunk();
        void * _chunk_ptr; //the real chunk ptr
		void * _malloc_ptr;
		uint64_t * _malloc_size_ptr;
		uint64_t * _malloc_prev_size_ptr;
		uint64_t _number; // number of chunk, from 0 to N
		bool _free;
		uint64_t _user_size;
		uint64_t* _bk;
		uint64_t* _fd;
        bool _deleted=0; // 1 if deleted (merged from a previous chunk

};
