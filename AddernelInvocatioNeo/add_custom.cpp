/**
 * @file add_custom.cpp
 *
 * Copyright (C) 2024. Huawei Technologies Co., Ltd. All rights reserved.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#include "kernel_operator.h"

constexpr int32_t TOTAL_LENGTH = 8 * 2048;                            // total length of data
constexpr int32_t USE_CORE_NUM = 8;                                   // num of core used
constexpr int32_t BLOCK_LENGTH = TOTAL_LENGTH / USE_CORE_NUM;         // length computed of each core
constexpr int32_t TILE_NUM = 8;                                       // split data into 8 tiles for each core
constexpr int32_t BUFFER_NUM = 2;                                     // tensor num for each queue
constexpr int32_t TILE_LENGTH = BLOCK_LENGTH / TILE_NUM / BUFFER_NUM; // separate to 2 parts, due to double buffer

class KernelAdd {
public:
    __aicore__ inline KernelAdd() {}
    __aicore__ inline void Init(GM_ADDR x, GM_ADDR y, GM_ADDR z)
    {
        xGm.SetGlobalBuffer((__gm__ bfloat16_t *)x + BLOCK_LENGTH * AscendC::GetBlockIdx(), BLOCK_LENGTH);
        yGm.SetGlobalBuffer((__gm__ bfloat16_t *)y + BLOCK_LENGTH * AscendC::GetBlockIdx(), BLOCK_LENGTH);
        zGm.SetGlobalBuffer((__gm__ bfloat16_t *)z + BLOCK_LENGTH * AscendC::GetBlockIdx(), BLOCK_LENGTH);
        pipe.InitBuffer(inQueueX, BUFFER_NUM, TILE_LENGTH * sizeof(bfloat16_t));
        pipe.InitBuffer(inQueueY, BUFFER_NUM, TILE_LENGTH * sizeof(bfloat16_t));
        pipe.InitBuffer(outQueueZ, BUFFER_NUM, TILE_LENGTH * sizeof(bfloat16_t));
        pipe.InitBuffer(tmpBuffer1, TILE_LENGTH * sizeof(float));
        pipe.InitBuffer(tmpBuffer2, TILE_LENGTH * sizeof(float));
        pipe.InitBuffer(tmpBuffer3, TILE_LENGTH * sizeof(float));
    }
    __aicore__ inline void Process()
    {
        int32_t loopCount = TILE_NUM * BUFFER_NUM;
        for (int32_t i = 0; i < loopCount; i++) {
            CopyIn(i);
            Compute(i);
            CopyOut(i);
        }
    }

private:
    __aicore__ inline void CopyIn(int32_t progress)
    {        
        AscendC::LocalTensor<bfloat16_t> xLocal = inQueueX.AllocTensor<bfloat16_t>();
        AscendC::LocalTensor<bfloat16_t> yLocal = inQueueY.AllocTensor<bfloat16_t>();
        AscendC::DataCopy(xLocal, xGm[progress * TILE_LENGTH], TILE_LENGTH);
        AscendC::DataCopy(yLocal, yGm[progress * TILE_LENGTH], TILE_LENGTH);
        inQueueX.EnQue(xLocal);
        inQueueY.EnQue(yLocal);
        #ifdef ASCENDC_CPU_DEBUG
        if(0 == AscendC::GetBlockIdx())
        {
            AscendC::printf("size=%d\n", xLocal.GetSize());
            xLocal.Print();
        }
        #else
         if(0 == AscendC::GetBlockIdx())
        {
            AscendC::printf("size=%d\n", xLocal.GetSize());
            DumpTensor(xLocal,10101,16);
        }
        #endif
    }
    __aicore__ inline void Compute(int32_t progress)
    {
        AscendC::LocalTensor<bfloat16_t> xLocal = inQueueX.DeQue<bfloat16_t>();    //这里每次计算1个元素吗？？？？   
        AscendC::LocalTensor<bfloat16_t> yLocal = inQueueY.DeQue<bfloat16_t>();        
        AscendC::LocalTensor<bfloat16_t> zLocal = outQueueZ.AllocTensor<bfloat16_t>();

        AscendC::LocalTensor<float> x = tmpBuffer1.Get<float>();        
        AscendC::LocalTensor<float> y = tmpBuffer2.Get<float>();
        AscendC::LocalTensor<float> z = tmpBuffer3.Get<float>();
        AscendC::Cast(x, xLocal,AscendC::RoundMode::CAST_NONE, TILE_LENGTH);
        AscendC::Cast(y, yLocal,AscendC::RoundMode::CAST_NONE, TILE_LENGTH);//bfloat->其他，只能是CAST_NONE

        AscendC::Add(z, x, y, TILE_LENGTH);
        AscendC::Cast(zLocal, z,AscendC::RoundMode::CAST_ROUND, TILE_LENGTH);////反之，不能是CAST_NONE
        //AscendC::Add(zLocal, xLocal, yLocal, TILE_LENGTH);
        outQueueZ.EnQue<bfloat16_t>(zLocal);
        inQueueX.FreeTensor(xLocal);
        inQueueY.FreeTensor(yLocal);
    }
    __aicore__ inline void CopyOut(int32_t progress)
    {
        AscendC::LocalTensor<bfloat16_t> zLocal = outQueueZ.DeQue<bfloat16_t>();
        AscendC::DataCopy(zGm[progress * TILE_LENGTH], zLocal, TILE_LENGTH);
        outQueueZ.FreeTensor(zLocal);
    }

private:
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECIN, BUFFER_NUM> inQueueX, inQueueY;
    AscendC::TQue<AscendC::TPosition::VECOUT, BUFFER_NUM> outQueueZ;
    AscendC::GlobalTensor<bfloat16_t> xGm;
    AscendC::GlobalTensor<bfloat16_t> yGm;
    AscendC::GlobalTensor<bfloat16_t> zGm;
    AscendC::TBuf<AscendC::TPosition::VECCALC> tmpBuffer1, tmpBuffer2, tmpBuffer3;
};

extern "C" __global__ __aicore__ void add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z)
{
    KernelAdd op;
    op.Init(x, y, z);
    op.Process();
}

#ifndef ASCENDC_CPU_DEBUG
void add_custom_do(uint32_t blockDim, void *stream, uint8_t *x, uint8_t *y, uint8_t *z)
{
    add_custom<<<blockDim, nullptr, stream>>>(x, y, z);
}
#endif
