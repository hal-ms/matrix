#ifndef DOT_MAT_BUFFER
#define DOT_MAT_BUFFER

#include "mbed.h"

#define HEADER_OFFSET   ( 2 )

namespace DotMat{
    class RawBuffer{
        public:
            int width;
            int height;
            
            RawBuffer();    // Constructor
            ~RawBuffer();   // Destructor
            
            void CreateBuffer(int width, int height);
            void Clear();
            void ReadFromFile(const char *path);
            void ReadFromFile(const char *path, size_t read_offset, size_t read_size);
            // void ReadFromFile(const char *path, size_t file_x, size_t file_y, size_t read_x, size_t read_y, size_t size_x, size_t size_y);
            // void CopyTo(DotMat::RawBuffer *buffer);
            // void CopyTo(DotMat::RawBuffer *buffer, size_t src_x, size_t src_y, size_t dst_x, size_t dst_y, size_t size_x, size_t size_y);
       
            const int8_t & operator[](int index) const { return this->_buffer[index]; }
            int8_t & operator[](int index) { return this->_buffer[index]; }
        private:
            int8_t* _buffer;
    };
    
    class Buffer {
        public:
            int size;
            
            Buffer(int width, int height, int size);    // Constructor
            ~Buffer();  // Destructor
            void AllClear();
            
            const RawBuffer & operator[](int index) const { return this->_rawBuffer[index]; }
            RawBuffer & operator[](int index) { return this->_rawBuffer[index]; }
        private:
            RawBuffer* _rawBuffer;
    };
}

#endif