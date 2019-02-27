#include "Animation.h"

namespace DotMat{
    
    /* --- コンストラクタ --- */
    Animation::Animation(int animation) {
        this->_animationData = new AnimationData*[animation];
        this->_animationSize = new int[animation];
        this->_animationType = new AnimationType[animation];
        
        this->_flow             = NULL;
        this->_currentBuffer    = 0;
        this->_currentAnimation = 0;
        this->_currentFlow      = 0;
    }
    
    /* --- デストラクタ --- */
    Animation::~Animation() {
        if(this->_animationData != NULL) delete[] this->_animationData;
        if(this->_animationSize != NULL) delete[] this->_animationSize;
        if(this->_animationType != NULL) delete[] this->_animationType;
    }    
    
    /* --- アニメーションデータの登録 --- */
    void Animation::SetAnimation(int index, AnimationType type, int size, AnimationData* animation){
        this->_animationData[index] = animation;
        this->_animationSize[index] = size;
        this->_animationType[index] = type;
    }

    /* --- バッファの設定 --- */ 
    void Animation::SetBuffer(DotMat::Buffer *buffer) {
        this->_buffer = buffer;
    }

    /* --- 再生バッファの取得 --- */   
    DotMat::RawBuffer& Animation::GetCurrent() {
        return (*this->_buffer)[this->_currentBuffer];
    }
    
    /* --- アニメーション切り替え時間の取得 --- */
    float Animation::GetDuration() {
        AnimationData* animation = this->_animationData[this->_currentAnimation];
        return animation[this->_currentBuffer].duration;
    }
    
    AnimationType Animation::GetAnimationType(){
        return this->_animationType[this->_currentAnimation];
    }
    
    /* --- アニメーションスタート --- */
    bool Animation::Start(int size, int* flow) {
        if( flow == NULL || this->_flow == flow ) return false;
        
        this->_flow = flow;
        this->_flowSize = size;
        this->_currentFlow = 0;
        
        this->_currentAnimation = this->_flow[this->_currentFlow];
        this->_LoadAnimation();
        
        return true;
    }
    
    /* --- フレーム更新 --- */
    void Animation::Update() {
        int size = this->_animationSize[this->_currentAnimation];
        AnimationType type = this->_animationType[this->_currentAnimation];
        this->_currentBuffer++;
        if(this->_currentBuffer >= size) {
            switch( type ){
                case Once:  
                    this->_currentBuffer = size - 1;
                    break;
                case Loop:
                    this->_currentBuffer = 0;
                    break;
                case Cutin:
                    this->Next();
                    break;
                default:    break;
            }
        }
    }
    
    /* --- 次のアニメーションへ --- */
    bool Animation::Next() {
        this->_currentFlow++;
        
        if( this->_currentFlow >= this->_flowSize ){
            this->_currentFlow = this->_flowSize - 1;
            return false;
        }
        
        this->_currentAnimation = this->_flow[this->_currentFlow];
        this->_LoadAnimation();
        
        return true;
    }
    
    
    /* --- アニメーションデータの取得 --- */
    void Animation::_LoadAnimation() {
        AnimationData* animation = this->_animationData[this->_currentAnimation];
        
        // アニメーションデータをバッファに転送
        for(int count = 0; count < this->_animationSize[this->_currentAnimation]; count++) {
            (*this->_buffer)[count].ReadFromFile(animation[count].path);
        }
        
        // 再生バッファを先頭に戻す
        this->_currentBuffer = 0;
    }
}