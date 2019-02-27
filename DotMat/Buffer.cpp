#include "Buffer.h"

namespace DotMat{
    
    /* --- コンストラクタ --- */
    RawBuffer::RawBuffer() {
    }
    
    /* --- デストラクタ --- */
    RawBuffer::~RawBuffer() {
        if(this->_buffer != NULL) delete this->_buffer;
    }
    
    /* --- バッファ生成 --- */
    void RawBuffer::CreateBuffer(int width, int height) {
        this->width = width;
        this->height = height;
        this->_buffer = new int8_t[width * height];
    }
    
    /* --- バッファ情報クリア --- */
    void RawBuffer::Clear() {
        int count;
        for(count = 0; count < this->width * this->height; count++) {
            this->_buffer[count] = 0;
        }
    }
    
    /* --- ファイルからデータの読み込み --- */ 
    void RawBuffer::ReadFromFile(const char *path) {
       this->ReadFromFile(path, 0, (this->width * this->height));
    }
    
    void RawBuffer::ReadFromFile(const char *path, size_t read_offset, size_t read_size) {
        // ファイルオープン
        FILE* fp = fopen(path, "r");
        // チェック
        if(fp == NULL) return;
        
        // 読み込み
        fseek(fp, HEADER_OFFSET, SEEK_SET); // ヘッダ分移動
        fread(&(this->_buffer[read_offset]), read_size, 1, fp);
        
        // ファイルクローズ
        fclose(fp);
    }
    
    /* --- コンストラクタ --- */
    Buffer::Buffer(int width, int height, int size) {
        this->size = size;
        
        this->_rawBuffer = new RawBuffer[size];    
        for(int count = 0; count < size; count++) {
            this->_rawBuffer[count].CreateBuffer(width, height);
        }
        this->AllClear();
    }
    
    /* --- デストラクタ --- */
    Buffer::~Buffer() {
       if(this->_rawBuffer != NULL) delete[] this->_rawBuffer;
    }
    
    /* --- バッファ情報クリア --- */
    void Buffer::AllClear() {
        int count;
        for(count = 0; count < this->size; ++count) {
            this->_rawBuffer[count].Clear();
    }
}
}