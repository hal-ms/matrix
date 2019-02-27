#include "mbed.h"
#include "SDFileSystem.h"
#include "DotMat.h"

/* --- ドットマトリクス --- */
BusOut row_bus(D8,D9,PB_13,PC_5);               /* ABCD */
DotMat::Display display(PB_14, PB_15, PB_1);    /* clk, lat, oe */
DotMat::Buffer buffer(64, 64, 10);
DotMat::Animation animation(80);

/* --- SDカード --- */
SDFileSystem sd(D11, D12, D13, D10, "sd");      /* MOSI, MISO, SCLK, CS */

/* --- シリアル通信 --- */
Serial serial(SERIAL_TX, SERIAL_RX);
volatile bool serial_flg = false;

/* --- アニメーション更新タイマ --- */
Ticker  ticker;
volatile bool update_flg = false;

/* --- 状態一覧 --- */
typedef enum _State{
    Wait = 1,   // 待機（初期状態）
    Ready,      // 準備
    Play        // プレイ中
}State;

/* --- 仕事一覧 --- */
typedef enum _Job{
    Neet = 0,   // 仕事なし（初期状態）
    Cook,       // 料理人
    Carpenter,  // 大工
    Monk,       // お坊さん
    Programmer, // プログラマ
    Pianist,    // ピアニスト
    //Judge      // 裁判官
}Job;

State state = Wait;         // 状態 (初期=Wait(仕事待ち状態))
Job  job = Neet;            // 仕事 (初期=Neet(仕事なし))
bool is_enable = false;     // アニメーション可否 (初期=アニメーションしない)
bool is_visible  = true;    // 表示可否 (初期=表示する)
bool next_flg    = false;   // 次アニメーション遷移フラグ      

/* --- アニメーションデータ --- */
DotMat::AnimationData first[] = {
    {"/sd/logo.bin", 0.5f}
};
DotMat::AnimationData ready[] = {
    {"/sd/Ready.bin", 0.5f}
};

/* 料理人(cook) */
DotMat::AnimationData cook1[] = {
    {"/sd/COOK/cook1-1.bin", 0.3f},
    {"/sd/COOK/cook1-2.bin", 0.3f}
};
DotMat::AnimationData cook2[] = {
    {"/sd/COOK/cook2.bin", 2.0f}
};
DotMat::AnimationData cook3[] = {
    {"/sd/COOK/cook3-1.bin", 0.3f},
    {"/sd/COOK/cook3-2.bin", 0.3f}
};
DotMat::AnimationData cook4[] = {
    {"/sd/COOK/cook4.bin", 2.0f}
};
DotMat::AnimationData cook5[] = {
    {"/sd/COOK/cook5-1.bin", 0.3f},
    {"/sd/COOK/cook5-2.bin", 0.3f}
};
DotMat::AnimationData cook6[] = {
    {"/sd/COOK/cook6.bin", 2.0f},
};

/* 大工(Carpenter) */
DotMat::AnimationData carp1[] = {
    {"/sd/CARPENTER/carp1-1.bin", 0.3f},
    {"/sd/CARPENTER/carp1-2.bin", 0.3f}
};
DotMat::AnimationData carp2[] = {
    {"/sd/CARPENTER/carp2.bin", 2.0f},
};
DotMat::AnimationData carp3[] = {
    {"/sd/CARPENTER/carp3-1.bin", 0.3f},
    {"/sd/CARPENTER/carp3-2.bin", 0.3f}
};
DotMat::AnimationData carp4[] = {
    {"/sd/CARPENTER/carp4.bin", 2.0f}
};
DotMat::AnimationData carp5[] = {
    {"/sd/CARPENTER/carp5-1.bin", 0.3f},
    {"/sd/CARPENTER/carp5-2.bin", 0.3f}
};
DotMat::AnimationData carp6[] = {
    {"/sd/CARPENTER/carp6.bin", 2.0f}
};


/* ピアニスト(pianist) */
DotMat::AnimationData pianist1[] = {
    {"/sd/PIANIST/pianist1-1.bin", 0.3f},
    {"/sd/PIANIST/pianist1-2.bin", 0.3f}
};
DotMat::AnimationData pianist2[] = {
    {"/sd/PIANIST/pianist2.bin", 2.0f}
};
DotMat::AnimationData pianist3[] = {
    {"/sd/PIANIST/pianist3-1.bin", 0.3f},
    {"/sd/PIANIST/pianist3-2.bin", 0.3f}
};
DotMat::AnimationData pianist4[] = {
    {"/sd/PIANIST/pianist4.bin", 2.0f}
};
DotMat::AnimationData pianist5[] = {
    {"/sd/PIANIST/pianist5-1.bin", 0.3f},
    {"/sd/PIANIST/pianist5-2.bin", 0.3f}
};
DotMat::AnimationData pianist6[] = {
    {"/sd/PIANIST/pianist6.bin", 2.0f}
};

/* お坊さん(Monk) */
DotMat::AnimationData monk1[] = {
    {"/sd/MONK/monk1-1.bin", 0.3f},
    {"/sd/MONK/monk1-2.bin", 0.3f}
};
DotMat::AnimationData monk2[] = {
    {"/sd/MONK/monk2-1.bin", 0.5f},
    {"/sd/MONK/monk2-3.bin", 0.5f},
    {"/sd/MONK/monk2-5.bin", 0.5f}
};
DotMat::AnimationData monk3[] = {
    {"/sd/MONK/monk3-1.bin", 0.3f},
    {"/sd/MONK/monk3-2.bin", 0.3f}
};
DotMat::AnimationData monk4[] = {
    {"/sd/MONK/monk4-1.bin", 0.3f},
    {"/sd/MONK/monk4-2.bin", 0.3f}
};

/* プログラマ(programmer) */
DotMat::AnimationData prog1[] = {
    {"/sd/PROGRAMMER/prg1-1.bin", 0.3f},
    {"/sd/PROGRAMMER/prg1-2.bin", 0.3f}
};
DotMat::AnimationData prog2[] = {
    {"/sd/PROGRAMMER/prg2-1.bin", 0.3f},
    {"/sd/PROGRAMMER/prg2-2.bin", 0.3f},
    {"/sd/PROGRAMMER/prg2-1.bin", 0.3f},
    {"/sd/PROGRAMMER/prg2-2.bin", 0.3f},
    {"/sd/PROGRAMMER/prg2-1.bin", 0.3f},
    {"/sd/PROGRAMMER/prg2-2.bin", 0.3f},
    {"/sd/PROGRAMMER/prg2-1.bin", 0.3f},
    {"/sd/PROGRAMMER/prg2-2.bin", 0.3f},
    {"/sd/PROGRAMMER/prg2-1.bin", 0.3f},
    {"/sd/PROGRAMMER/prg2-2.bin", 0.3f}
};
DotMat::AnimationData prog3[] = {
    {"/sd/PROGRAMMER/prg3-1.bin", 0.3f},
    {"/sd/PROGRAMMER/prg3-2.bin", 0.3f}
};
DotMat::AnimationData prog4[] = {
    {"/sd/PROGRAMMER/prg4-1.bin", 0.3f},
    {"/sd/PROGRAMMER/prg4-2.bin", 0.3f},
    {"/sd/PROGRAMMER/prg4-1.bin", 0.3f},
    {"/sd/PROGRAMMER/prg4-2.bin", 0.3f},
    {"/sd/PROGRAMMER/prg4-1.bin", 0.3f},
    {"/sd/PROGRAMMER/prg4-2.bin", 0.3f},
    {"/sd/PROGRAMMER/prg4-1.bin", 0.3f},
    {"/sd/PROGRAMMER/prg4-2.bin", 0.3f},
    {"/sd/PROGRAMMER/prg4-1.bin", 0.3f},
    {"/sd/PROGRAMMER/prg4-2.bin", 0.3f}
};
DotMat::AnimationData prog5[] = {
    {"/sd/PROGRAMMER/prg5-1.bin", 0.3f},
    {"/sd/PROGRAMMER/prg5-2.bin", 0.3f}
};
DotMat::AnimationData prog6[] = {
    {"/sd/PROGRAMMER/prg6.bin", 2.0f}
};

/* --- アニメーションフロー --- */
int first_flow[]      = {  0 };
int cook_flow[]       = { 11, 12, 13, 14, 15, 16 };
int pianist_flow[]    = { 21, 22, 23, 24, 25, 26 };
int carpenter_flow[]  = { 31, 32, 33, 34, 35, 36 };
int monk_flow[]       = { 41, 42, 43, 42, 44, 45 };
int programmer_flow[] = { 51, 52, 53, 54, 55, 56 };

/* ----- リセット ----- */
void Reset(){
    state       = Wait;
    job         = Neet;
    is_enable   = false;
    is_visible  = true;
}

/* ----- シリアル受信割り込み処理 ----- */
void SerialRead() {
    uint8_t read_buf = serial.getc();
    serial_flg = true;
    switch( read_buf ){
        /* 仕事関連コマンド */
        case 0x41:  job = Cook;          break;     /* cmd= 'A' */
        case 0x42:  job = Carpenter;     break;     /* cmd= 'B' */
        case 0x43:  job = Monk;          break;     /* cmd= 'C' */
        case 0x44:  job = Programmer;    break;     /* cmd= 'D' */
        case 0x45:  job = Pianist;       break;     /* cmd= 'E' */
        /* アニメーションコマンド */
        case 0x30:  is_enable = false; break;       /* cmd= '0' */
        case 0x31:  is_enable = true;  break;       /* cmd= '1' */
        case 0x20:  next_flg    = true;  break;     /* cmd= ' ' */
        /* 表示・非表示コマンド */
        case 0x2D:  is_visible  = false; break;     /* cmd= '-' */
        case 0x2B:  is_visible  = true;  break;     /* cmd= '+' */
        /* リセットコマンド */
        case 0x1B:
            Reset();
            break;
        default:   serial_flg = false;   break;
    }
}

/* ----- タイマ処理 ----- */
void TimerTick(){
    update_flg = true;
}

/* ----- アニメーション実行処理 ----- */
void PlayAnimation(){
    ticker.attach(&TimerTick, animation.GetDuration());
}

/* ----- アニメーション停止処理 ----- */
void StopAnimation(){
    ticker.detach();
}

/* ----- アニメーション開始処理 ----- */
void StartAnimation(){
    Job _job = job;
    StopAnimation();
    switch( _job ) {
        case Cook:  // 料理人
            animation.Start(F_size(cook_flow), cook_flow); 
            break;
        case Carpenter: // 大工
            animation.Start(F_size(carpenter_flow), carpenter_flow); 
            break;
        case Monk:  // お坊さん
            animation.Start(F_size(monk_flow), monk_flow); 
            break;
        case Programmer:    // プログラマ
            animation.Start(F_size(programmer_flow), programmer_flow); 
            break;
        case Pianist:   // ピアニスト
            animation.Start(F_size(pianist_flow), pianist_flow); 
            break;
        default:
            animation.Start(F_size(first_flow), first_flow);
           break;
    }
    
    if( job != Neet ){
        state = Play;
        is_enable = false;
        is_visible  = false;
        PlayAnimation();
    }
}

/* ----- フレーム更新処理 ----- */
void UpdateAnimation(){
    if( update_flg ){
        if( animation.GetAnimationType() == DotMat::Cutin || is_enable ){
            animation.Update();
            update_flg = false;
            StopAnimation();
            PlayAnimation();
        }
    }
}

/* ----- 次のアニメーションへ ----- */
void NextAnimation(){
    if(!animation.Next()){
        serial.putc(0x00); 
        Reset();
        StartAnimation();
    }
    update_flg = false;
    StopAnimation();
    PlayAnimation();
}

/* ----- 初期化処理 ----- */
void Init(){
    /* シリアル通信設定 */
    serial.attach(SerialRead, Serial::RxIrq);  // 受信割り込み
    
    /* ドットマトリクスPIN設定 */
    display.SetRowBus(&row_bus);
    display.SetRGBPin(D4, D6, D2, D5, D7, D3);  /* r1, g1, b1, r2, g2, b2 */
    
    /* アニメーション設定 */
    animation.SetBuffer(&buffer);
    animation.SetAnimation( 0, DotMat::Once,  A_size(first),    first);
    animation.SetAnimation( 1, DotMat::Once,  A_size(ready),    ready);
    
    animation.SetAnimation(11, DotMat::Loop,  A_size(cook1),    cook1);
    animation.SetAnimation(12, DotMat::Cutin, A_size(cook2),    cook2);
    animation.SetAnimation(13, DotMat::Loop,  A_size(cook3),    cook3);
    animation.SetAnimation(14, DotMat::Cutin, A_size(cook4),    cook4);
    animation.SetAnimation(15, DotMat::Loop,  A_size(cook5),    cook5);
    animation.SetAnimation(16, DotMat::Once,  A_size(cook6),    cook6);
    
    animation.SetAnimation(21, DotMat::Loop,  A_size(pianist1),  pianist1);
    animation.SetAnimation(22, DotMat::Cutin, A_size(pianist2),  pianist2);
    animation.SetAnimation(23, DotMat::Loop,  A_size(pianist3),  pianist3);
    animation.SetAnimation(24, DotMat::Cutin, A_size(pianist4),  pianist4);
    animation.SetAnimation(25, DotMat::Loop,  A_size(pianist5),  pianist5);
    animation.SetAnimation(26, DotMat::Once,  A_size(pianist6),  pianist6);
    
    animation.SetAnimation(31, DotMat::Loop,  A_size(carp1),    carp1);
    animation.SetAnimation(32, DotMat::Cutin, A_size(carp2),    carp2);
    animation.SetAnimation(33, DotMat::Loop,  A_size(carp3),    carp3);
    animation.SetAnimation(34, DotMat::Cutin, A_size(carp4),    carp4);
    animation.SetAnimation(35, DotMat::Loop,  A_size(carp5),    carp5);
    animation.SetAnimation(36, DotMat::Once,  A_size(carp6),    carp6);
    
    animation.SetAnimation(41, DotMat::Loop,  A_size(monk1),    monk1);
    animation.SetAnimation(42, DotMat::Cutin, A_size(monk2),    monk2);
    animation.SetAnimation(43, DotMat::Loop,  A_size(monk3),    monk3);
    animation.SetAnimation(44, DotMat::Loop,  A_size(monk4),    monk4);
    animation.SetAnimation(45, DotMat::Once,  A_size(monk2),    monk2);
    
    animation.SetAnimation(51, DotMat::Loop,  A_size(prog1),    prog1);
    animation.SetAnimation(52, DotMat::Cutin, A_size(prog2),    prog2);
    animation.SetAnimation(53, DotMat::Loop,  A_size(prog3),    prog3);
    animation.SetAnimation(54, DotMat::Cutin, A_size(prog4),    prog4);
    animation.SetAnimation(55, DotMat::Loop,  A_size(prog5),    prog5);
    animation.SetAnimation(56, DotMat::Once,  A_size(prog6),    prog6);
    
    StartAnimation();
}

/* ----- メインルーチン ----- */
int main() {
    Init(); // 初期設定
    
    // 以下ループ
    while(1){
        // シリアル信号受信
        if( serial_flg ){            
            switch( state ){
                case Wait:
                    StartAnimation();
                    break;
                case Ready:
                    break;
                case Play:
                    /*
                    if( is_visible ){
                        // 次のアニメーションへ
                        if( next_flg && is_enable ) {
                            NextAnimation();
                        }
                    }
                    */
                    if( next_flg ) {
                        NextAnimation();
                    }
                    break;
            }
            next_flg   = false;
            serial_flg = false;
        }
            
        // フレーム更新
        if( update_flg ){
            UpdateAnimation();
        }
        
        // マトリクス表示
        if( is_visible ){
            display.Show(animation.GetCurrent()); 
        }
    }
}