MEGATRON_DATAS=../megatron_datas/gpt2_files

python3 tools/preprocess_data.py \
       --input $MEGATRON_DATAS/wikitext-103/wiki.train.json \
       --output-prefix my-gpt2 \
       --vocab-file $MEGATRON_DATAS/gpt2-vocab.json \
       --dataset-impl mmap \
       --tokenizer-type GPT2BPETokenizer \
       --merge-file $MEGATRON_DATAS/gpt2-merges.txt \
       --append-eod \
       --workers 20 \
       --chunk-size 25
