#ifndef DOT_MAT_DISPLAY
#define DOT_MAT_DISPLAY

#include "mbed.h"
#include "Buffer.h"

#define HIGH            1
#define LOW             0

#define DOTMAT_HEIGHT   ( 32 )  // マトリクス高さ
#define DOTMAT1_WIDTH   ( 64 )  // マトリクス幅（1枚目）
#define DOTMAT2_WIDTH   ( 64 )  // マトリクス幅（2枚目）

#define DOTMAT_ROW_SECT ( 16 )  // 行セクション数(2byte;16)

namespace DotMat {
    class Display {
    public:
        Display(PinName clk, PinName lat, PinName oe);  // Constructor
        ~Display(); // Destructor
        
        void SetRowBus(BusOut *bus);
        void SetRGBPin(PinName r1, PinName g1, PinName b1, PinName r2, PinName g2, PinName b2);
        void Show(DotMat::RawBuffer &buffer, bool visible=true); 
    
    private:
        BusOut*         _bus;
        DigitalOut      _clk;
        DigitalOut      _lat;
        DigitalOut      _oe;
        DigitalOut*     _r1;
        DigitalOut*     _g1;
        DigitalOut*     _b1;
        DigitalOut*     _r2;
        DigitalOut*     _g2;
        DigitalOut*     _b2;
        
        void WriteRow(DotMat::RawBuffer &buffer, int8_t row);
    };
}

#endif