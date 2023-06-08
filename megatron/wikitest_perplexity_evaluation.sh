
export CUDA_HOME=/home/spack/spack/opt/spack/linux-ubuntu22.04-broadwell/gcc-11.2.0/cuda-11.7.0-rawvxrv3urmsmnddaax3b5q5kblpt5k3
export PATH=/home/spack/spack/opt/spack/linux-ubuntu22.04-broadwell/gcc-11.2.0/cuda-11.7.0-rawvxrv3urmsmnddaax3b5q5kblpt5k3/bin:$PATH

export LD_LIBRARY_PATH=/home/spack/spack/opt/spack/linux-ubuntu22.04-broadwell/gcc-11.2.0/cuda-11.7.0-rawvxrv3urmsmnddaax3b5q5kblpt5k3/lib64:$LD_LIBRARY_PATH

TASK="WIKITEXT103"
export CUDA_DEVICE_MAX_CONNECTIONS=1
MEGATRON_DATAS=../megatron_datas
VALID_DATA=$MEGATRON_DATAS/gpt2_files/wikitext-103/wiki.valid.tokens
VOCAB_FILE=$MEGATRON_DATAS/gpt2_files/gpt2-vocab.json
MERGE_FILE=$MEGATRON_DATAS/gpt2_files/gpt2-merges.txt
CHECKPOINT_PATH=$MEGATRON_DATAS/gpt2_files/megatron_lm_345m_v0.0/latest_checkpointed_iteration.txt

COMMON_TASK_ARGS="--num-layers 24 \
                  --hidden-size 1024 \
                  --num-attention-heads 16 \
                  --seq-length 1024 \
                  --max-position-embeddings 1024 \
                  --fp16 \
                  --vocab-file $VOCAB_FILE"


WORLD_SIZE=1

DISTRIBUTED_ARGS="--nproc_per_node $WORLD_SIZE \
                  --nnodes 1 \
                  --node_rank 0 \
                  --master_addr localhost \
                  --master_port 6000"

torchrun  tasks/main.py \
       --task $TASK \
       $COMMON_TASK_ARGS \
       --valid-data $VALID_DATA \
       --tokenizer-type GPT2BPETokenizer \
       --merge-file $MERGE_FILE \
       --load $CHECKPOINT_PATH \
       --micro-batch-size 8 \
       --log-interval 10 \
       --no-load-optim \
       --no-load-rng
