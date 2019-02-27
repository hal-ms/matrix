#include "Display.h"
namespace DotMat
{
    /* --- コンストラクタ --- */    
    Display::Display(PinName clk, PinName lat, PinName oe) : _clk(clk), _lat(lat), _oe(oe)
    {
        this->_clk = LOW;
        this->_lat = LOW;
        this->_oe  = LOW;
    }
    
    /* --- デストラクタ --- */ 
    Display::~Display()
    {
        if(this->_r1 != NULL) delete this->_r1;
        if(this->_g1 != NULL) delete this->_g1;
        if(this->_b1 != NULL) delete this->_b1;
        if(this->_r2 != NULL) delete this->_r2;
        if(this->_g2 != NULL) delete this->_g2;
        if(this->_g2 != NULL) delete this->_g2;
    }
    
    /* --- RowBusの設定 --- */
    void Display::SetRowBus(BusOut *bus)
    {
        this->_bus = bus;
        *this->_bus = LOW;
    }
    
    /* --- RGBピンの設定 --- */
    void Display::SetRGBPin(PinName r1, PinName g1, PinName b1, PinName r2, PinName g2, PinName b2)
    {
        this->_r1 = new DigitalOut(r1);
        this->_g1 = new DigitalOut(g1);
        this->_b1 = new DigitalOut(b1);
        this->_r2 = new DigitalOut(r2);
        this->_g2 = new DigitalOut(g2);
        this->_b2 = new DigitalOut(b2);
    }
        
    /* --- 表示メソッド --- */
    void Display::Show(DotMat::RawBuffer &buffer, bool visible) {
        for( int8_t row = 0; row < DOTMAT_ROW_SECT ; row++ ) {   
            this->WriteRow(buffer, row);
            this->_oe = LOW;    // 表示
            wait_us(100);       // *** 値を小さくすると暗くなる ***
            this->_oe = HIGH;   // 非表示
        }
    }
    
    /* --- 1セクション分(2行/1枚 * 2枚分) データ転送 --- */
    void Display::WriteRow(DotMat::RawBuffer &buffer, int8_t row)
    {
        *this->_bus = row & 0x0F;
        for( int col = 0; col < DOTMAT1_WIDTH + DOTMAT2_WIDTH; col++) {
            int8_t  data1, data2;
            int     _row1, _row2, _col;
             
            if( col < DOTMAT1_WIDTH ){
                // 1枚目
                _row1 = row * buffer.width;
                _row2 = ( row + DOTMAT_ROW_SECT ) * buffer.width;
                _col  = col;
                
                data1 = buffer[_row1 + _col];
                data2 = buffer[_row2 + _col];
            }else{
                // 2枚目
                _row1 = ( row + DOTMAT_HEIGHT ) * buffer.width;
                _row2 = ( row + DOTMAT_HEIGHT + DOTMAT_ROW_SECT ) * buffer.width;
                _col  = col - DOTMAT1_WIDTH;
                
                data1 = buffer[_row1 + _col];
                data2 = buffer[_row2 + _col]; 
            }
            
            // 上段
            *this->_r1 = ( data1 >> 0 ) & 0x01;
            *this->_g1 = ( data1 >> 1 ) & 0x01;
            *this->_b1 = ( data1 >> 2 ) & 0x01;
            
            // 下段
            *this->_r2 = ( data2 >> 0 ) & 0x01;
            *this->_g2 = ( data2 >> 1 ) & 0x01;
            *this->_b2 = ( data2 >> 2 ) & 0x01;
            
            // データを詰める      
            this->_clk = HIGH;
            this->_clk = LOW;
            
        }
    
        // 転送
        this->_lat = HIGH;
        this->_lat = LOW;
    }
}