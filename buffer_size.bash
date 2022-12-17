#!/bin/bash

algorithms=ALL
datasets=ALL
epochs=1

chunknum_set="256"
chunksize="2097152" #2MB

for chunknum in ${chunknum_set[@]}; do
    bash test.bash $algorithms $datasets $epochs $chunknum $chunksize;
    wait;
done
