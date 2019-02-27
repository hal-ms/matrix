#ifndef DOT_MAT_ANIMATION
#define DOT_MAT_ANIMATION

#include "Buffer.h"
#include "Display.h"

namespace DotMat {
    typedef enum _AnimationType{
        Once,   // ループせず待機
        Loop,   // 繰り返す
        Cutin   // 次のアニメーションに行く
    }AnimationType;
    
    struct AnimationData {
        const char* path;   // .binパス
        float duration;     // アニメーション切り替え時間
    };
        
    #define A_size(v)   ( sizeof(v) / sizeof(DotMat::AnimationData) )
    #define F_size(v)   ( sizeof(v) / sizeof(int) )
    
    class Animation{
        public:
            Animation(int animation);       // Constructor
            ~Animation();                   // Destructor
            
            void SetAnimation(int index, AnimationType type, int size, AnimationData* animation);
            //void Start();
            bool Start(int size, int* flow);
            void Update();
            bool Next();
            void SetBuffer(DotMat::Buffer* buffer);
            DotMat::RawBuffer& GetCurrent();
            float GetDuration();
            AnimationType GetAnimationType();
           
        private:
            AnimationData**     _animationData;
            int*                _animationSize;
            AnimationType*      _animationType;
            int*                _flow;
            int                 _flowSize;
            DotMat::Buffer*     _buffer;
            
            int                 _currentBuffer;
            int                 _currentAnimation;
            int                 _currentFlow;
            
            void _LoadAnimation();
    };
}

#endif