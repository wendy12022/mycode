#!/usr/bin/python3
# coding=utf-8
#
# Copyright (C) 2023-2024. Huawei Technologies Co., Ltd. All rights reserved.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# ===============================================================================

import numpy as np
import torch

def gen_golden_data_simple():
    input_x = torch.randn(8,2048,device="cpu", dtype=torch.bfloat16)
    input_y = torch.randn(8,2048,device="cpu", dtype=torch.bfloat16)
    golden = (input_x + input_y)
    x = input_x.detach().contiguous().cpu().to(torch.bfloat16)
    with open("./input/input_x.bin", "wb") as f:
        f.write(x.view(torch.uint8).numpy().tobytes())
    y = input_y.detach().contiguous().cpu().to(torch.bfloat16)
    with open("./input/input_y.bin", "wb") as f:
        f.write(y.view(torch.uint8).numpy().tobytes())
    z = golden.detach().contiguous().cpu().to(torch.bfloat16)
    with open("./output/golden.bin", "wb") as f:
        f.write(z.view(torch.uint8).numpy().tobytes())
    '''input_x.numpy().view(torch.uint16).tofile("./input/input_x.bin")
    input_y.numpy().view(torch.uint16).tofile("./input/input_y.bin")
    golden.numpy().view(torch.uint16).tofile("./output/golden.bin")'''
    '''input_x = torch.empty(8,2048,device="cpu").uniform_(1,100).to(torch.float16)
    input_y = torch.empty(8,2048,device="cpu").uniform_(1,100).to(torch.float16)
    golden = (input_x + input_y)
    input_x.numpy().tofile("./input/input_x.bin")
    input_y.numpy().tofile("./input/input_y.bin")
    golden.numpy().tofile("./output/golden.bin")'''
    


if __name__ == "__main__":
    gen_golden_data_simple()
