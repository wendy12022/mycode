
export CUDA_HOME=/home/spack/spack/opt/spack/linux-ubuntu22.04-broadwell/gcc-11.2.0/cuda-11.7.0-rawvxrv3urmsmnddaax3b5q5kblpt5k3
export PATH=/home/spack/spack/opt/spack/linux-ubuntu22.04-broadwell/gcc-11.2.0/cuda-11.7.0-rawvxrv3urmsmnddaax3b5q5kblpt5k3/bin:$PATH

export LD_LIBRARY_PATH=/home/spack/spack/opt/spack/linux-ubuntu22.04-broadwell/gcc-11.2.0/cuda-11.7.0-rawvxrv3urmsmnddaax3b5q5kblpt5k3/lib64:$LD_LIBRARY_PATH

TASK="WIKITEXT103"
export CUDA_DEVICE_MAX_CONNECTIONS=1
MEGATRON_DATAS=../megatron_datas
DATA_PATH=my-gpt2_text_document
VOCAB_FILE=$MEGATRON_DATAS/gpt2_files/gpt2-vocab.json
MERGE_FILE=$MEGATRON_DATAS/gpt2_files/gpt2-merges.txt
CHECKPOINT_PATH=checkpoints/gpt2_345m
GPT_ARGS="--num-layers 24 \
          --hidden-size 1024 \
          --num-attention-heads 16 \
          --seq-length 1024 \
          --max-position-embeddings 1024 \
          --micro-batch-size 8 \
          --global-batch-size 8 \
          --lr 0.00015 \
          --train-iters 500000 \
          --lr-decay-iters 320000 \
          --lr-decay-style cosine \
          --vocab-file $VOCAB_FILE \
          --merge-file $MERGE_FILE \
          --lr-warmup-fraction .01 \
          --fp16"
OUTPUT_ARGS="--log-interval 10 \
             --save-interval 500 \
             --eval-interval 100 \
             --eval-iters 10 \
             --recompute-activations"
WORLD_SIZE=1
MP_SIZE=1

DISTRIBUTED_ARGS="--nproc_per_node $WORLD_SIZE \
                  --nnodes 1 \
                  --node_rank 0 \
                  --master_addr localhost \
                  --master_port 6000"

export CUDA_DEVICE_MAX_CONNECTIONS=1



torchrun $DISTRIBUTED_ARGS ./pretrain_gpt.py \
                $GPT_ARGS \
                $OUTPUT_ARGS \
                --save $CHECKPOINT_PATH \
                --load $CHECKPOINT_PATH \
                --data-path $DATA_PATH \
                --tensor-model-parallel-size $MP_SIZE \
                --DDP-impl torch
