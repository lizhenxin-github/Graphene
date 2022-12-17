#!/bin/bash

# NOTE: alloc hugepage before running, and free hugepages after running.
# cat /proc/meminfo | grep Huge
# echo 60000 > /proc/sys/vm/nr_hugepages
# echo 0 > /proc/sys/vm/nr_hugepages

algorithms=$1
datasets=$2
epochs=$3

chunknum="128"
chunksize="2097152" #2MB


# chunknum=$4
# chunksize=$5

if [ $algorithms = "BFS" ] || [ $algorithms = "ALL" ]; then
    echo "================================================================================================================"
    echo "======================================================BFS======================================================="
    echo "================================================================================================================"
    # run BFS

    if [ $datasets = "Twitter" ] || [ $datasets = "ALL" ]; then
        for ((times = 0; times < $epochs; times++)); do
            echo "====================================================Twitter===================================================="
            # Twitter: 18225348 48506928 39787722
            ./graphene/test/bfs/aio_bfs.bin 6 8 96 \
                /home/zorax/data/Twitter/row_col_6x8 /home/zorax/data/Twitter/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128 18225348
            wait
            ./graphene/test/bfs/aio_bfs.bin 6 8 96 \
                /home/zorax/data/Twitter/row_col_6x8 /home/zorax/data/Twitter/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128 48506928
            wait
            ./graphene/test/bfs/aio_bfs.bin 6 8 96 \
                /home/zorax/data/Twitter/row_col_6x8 /home/zorax/data/Twitter/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128 39787722
            wait
        done
    fi

    if [ $datasets = "Friendster" ] || [ $datasets = "ALL" ]; then
        for ((times = 0; times < $epochs; times++)); do
            echo "====================================================Friendster===================================================="
            # FriendSter: 26737282 41305569 43962717
            ./graphene/test/bfs/aio_bfs.bin 6 8 96 \
                /home/zorax/data/Friendster/row_col_6x8 /home/zorax/data/Friendster/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128 26737282
            wait
            ./graphene/test/bfs/aio_bfs.bin 6 8 96 \
                /home/zorax/data/Friendster/row_col_6x8 /home/zorax/data/Friendster/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128 41305569
            wait
            ./graphene/test/bfs/aio_bfs.bin 6 8 96 \
                /home/zorax/data/Friendster/row_col_6x8 /home/zorax/data/Friendster/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128 43962717
            wait
        done
    fi

    if [ $datasets = "Ukdomain" ] || [ $datasets = "ALL" ]; then
        for ((times = 0; times < $epochs; times++)); do
            echo "====================================================Ukdomain===================================================="
            # Ukdomain: 5699262 64976639 88961668
            ./graphene/test/bfs/aio_bfs.bin 6 8 96 \
                /home/zorax/data/Ukdomain/row_col_6x8 /home/zorax/data/Ukdomain/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128 5699262
            wait
            ./graphene/test/bfs/aio_bfs.bin 6 8 96 \
                /home/zorax/data/Ukdomain/row_col_6x8 /home/zorax/data/Ukdomain/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128 64976639
            wait
            ./graphene/test/bfs/aio_bfs.bin 6 8 96 \
                /home/zorax/data/Ukdomain/row_col_6x8 /home/zorax/data/Ukdomain/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128 88961668
            wait
        done
    fi

    if [ $datasets = "Kron28" ] || [ $datasets = "ALL" ]; then
        for ((times = 0; times < $epochs; times++)); do
            echo "====================================================Kron28===================================================="
            # Kron28: 254655025 39454459 105161820
            ./graphene/test/bfs/aio_bfs.bin 6 8 96 \
                /home/zorax/data/Kron28/row_col_6x8 /home/zorax/data/Kron28/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128 254655025
            wait
            ./graphene/test/bfs/aio_bfs.bin 6 8 96 \
                /home/zorax/data/Kron28/row_col_6x8 /home/zorax/data/Kron28/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128 39454459
            wait
            ./graphene/test/bfs/aio_bfs.bin 6 8 96 \
                /home/zorax/data/Kron28/row_col_6x8 /home/zorax/data/Kron28/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128 105161820
            wait
        done
    fi

    if [ $datasets = "YahooWeb" ] || [ $datasets = "ALL" ]; then
        for ((times = 0; times < $epochs; times++)); do
            echo "====================================================YahooWeb===================================================="
            # YahooWeb: 35005211 10757586 20414463
            ./graphene/test/bfs/aio_bfs.bin 6 8 96 \
                /home/zorax/data/Yahoo/row_col_6x8 /home/zorax/data/Yahoo/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128 35005211
            wait
            ./graphene/test/bfs/aio_bfs.bin 6 8 96 \
                /home/zorax/data/Yahoo/row_col_6x8 /home/zorax/data/Yahoo/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128 10757586
            wait
            ./graphene/test/bfs/aio_bfs.bin 6 8 96 \
                /home/zorax/data/Yahoo/row_col_6x8 /home/zorax/data/Yahoo/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128 20414463
            wait
        done
    fi

    if [ $datasets = "Kron29" ] || [ $datasets = "ALL" ]; then
        for ((times = 0; times < $epochs; times++)); do
            echo "====================================================Kron29===================================================="
            # Kron29: 310059974 104024179 233665123
            ./graphene/test/bfs/aio_bfs.bin 6 8 96 \
                /home/zorax/data/Kron29/row_col_6x8 /home/zorax/data/Kron29/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128 310059974
            wait
            ./graphene/test/bfs/aio_bfs.bin 6 8 96 \
                /home/zorax/data/Kron29/row_col_6x8 /home/zorax/data/Kron29/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128 104024179
            wait
            ./graphene/test/bfs/aio_bfs.bin 6 8 96 \
                /home/zorax/data/Kron29/row_col_6x8 /home/zorax/data/Kron29/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128 233665123
            wait
        done
    fi

    if [ $datasets = "Kron30" ] || [ $datasets = "ALL" ]; then
        for ((times = 0; times < $epochs; times++)); do
            echo "====================================================Kron30===================================================="
            # Kron30: 233665123  980258108 485560280
            ./graphene/test/bfs/aio_bfs.bin 6 8 96 \
                /home/zorax/data/Kron30/row_col_6x8 /home/zorax/data/Kron30/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128 233665123

            ./graphene/test/bfs/aio_bfs.bin 6 8 96 \
                /home/zorax/data/Kron30/row_col_6x8 /home/zorax/data/Kron30/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128 980258108

            ./graphene/test/bfs/aio_bfs.bin 6 8 96 \
                /home/zorax/data/Kron30/row_col_6x8 /home/zorax/data/Kron30/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128 485560280
        done
    fi
fi

if [ $algorithms = "PageRank" ] || [ $algorithms = "ALL" ]; then
    echo "================================================================================================================"
    echo "====================================================PageRank===================================================="
    echo "================================================================================================================"
    # run PageRank

    if [ $datasets = "Twitter" ] || [ $datasets = "ALL" ]; then
        for ((times = 0; times < $epochs; times++)); do
            echo "====================================================Twitter===================================================="
            # Twitter: num_iterations = 10
            ./graphene/test/pagerank/aio_pagerank.bin 6 8 96 \
                /home/zorax/data/Twitter/row_col_6x8 /home/zorax/data/Twitter/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128 10
            wait
        done
    fi

    if [ $datasets = "Friendster" ] || [ $datasets = "ALL" ]; then
        for ((times = 0; times < $epochs; times++)); do
            echo "====================================================Friendster===================================================="
            # FriendSter: num_iterations = 10
            ./graphene/test/pagerank/aio_pagerank.bin 6 8 96 \
                /home/zorax/data/Friendster/row_col_6x8 /home/zorax/data/Friendster/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128 10
            wait
        done
    fi

    if [ $datasets = "Ukdomain" ] || [ $datasets = "ALL" ]; then
        for ((times = 0; times < $epochs; times++)); do
            echo "====================================================Ukdomain===================================================="
            # Ukdomain: num_iterations = 10
            ./graphene/test/pagerank/aio_pagerank.bin 6 8 96 \
                /home/zorax/data/Ukdomain/row_col_6x8 /home/zorax/data/Ukdomain/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128 10
            wait
        done
    fi

    if [ $datasets = "Kron28" ] || [ $datasets = "ALL" ]; then
        for ((times = 0; times < $epochs; times++)); do
            echo "====================================================Kron28===================================================="
            # Kron28: num_iterations = 10
            ./graphene/test/pagerank/aio_pagerank.bin 6 8 96 \
                /home/zorax/data/Kron28/row_col_6x8 /home/zorax/data/Kron28/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128 10
            wait
        done
    fi

    if [ $datasets = "Yahoo" ] || [ $datasets = "ALL" ]; then
        for ((times = 0; times < $epochs; times++)); do
            echo "====================================================Yahoo===================================================="
            # Yahoo: num_iterations = 10
            ./graphene/test/pagerank/aio_pagerank.bin 6 8 96 \
                /home/zorax/data/Yahoo/row_col_6x8 /home/zorax/data/Yahoo/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128 10
            wait
        done
    fi

    if [ $datasets = "Kron29" ] || [ $datasets = "ALL" ]; then
        for ((times = 0; times < $epochs; times++)); do
            echo "====================================================Kron29===================================================="
            # Kron29: num_iterations = 10
            ./graphene/test/pagerank/aio_pagerank.bin 6 8 96 \
                /home/zorax/data/Kron29/row_col_6x8 /home/zorax/data/Kron29/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128 10
            wait
        done
    fi

    if [ $datasets = "Kron30" ] || [ $datasets = "ALL" ]; then
        for ((times = 0; times < $epochs; times++)); do
            echo "====================================================Kron30===================================================="
            # Kron30: num_iterations = 10
            ./graphene/test/pagerank/aio_pagerank.bin 6 8 96 \
                /home/zorax/data/Kron30/row_col_6x8 /home/zorax/data/Kron30/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128 10
            wait
        done
    fi
fi

if [ $algorithms = "WCC" ] || [ $algorithms = "ALL_NO" ]; then
    echo "================================================================================================================"
    echo "======================================================WCC======================================================="
    echo "================================================================================================================"
    # run Weakly Connected Component

    if [ $datasets = "Twitter" ] || [ $datasets = "ALL" ]; then
        for ((times = 0; times < $epochs; times++)); do
            echo "====================================================Twitter===================================================="
            ./graphene/test/wcc/aio_wcc_hybrid.bin 6 8 96 \
                /home/zorax/data/Twitter/row_col_6x8 /home/zorax/data/Twitter/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128
            wait
        done
    fi

    if [ $datasets = "Friendster" ] || [ $datasets = "ALL" ]; then
        for ((times = 0; times < $epochs; times++)); do
            echo "====================================================Friendster===================================================="
            ./graphene/test/wcc/aio_wcc_hybrid.bin 6 8 96 \
                /home/zorax/data/Friendster/row_col_6x8 /home/zorax/data/Friendster/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128
            wait
        done
    fi

    if [ $datasets = "Ukdomain" ] || [ $datasets = "ALL" ]; then
        for ((times = 0; times < $epochs; times++)); do
            echo "====================================================Ukdomain===================================================="
            ./graphene/test/wcc/aio_wcc_hybrid.bin 6 8 96 \
                /home/zorax/data/Ukdomain/row_col_6x8 /home/zorax/data/Ukdomain/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128
            wait
        done
    fi

    if [ $datasets = "Kron28" ] || [ $datasets = "ALL" ]; then
        for ((times = 0; times < $epochs; times++)); do
            echo "====================================================Kron28===================================================="
            ./graphene/test/wcc/aio_wcc_hybrid.bin 6 8 96 \
                /home/zorax/data/Kron28/row_col_6x8 /home/zorax/data/Kron28/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128
            wait
        done
    fi

    if [ $datasets = "Yahoo" ] || [ $datasets = "ALL" ]; then
        for ((times = 0; times < $epochs; times++)); do
            echo "====================================================Yahoo===================================================="
            ./graphene/test/wcc/aio_wcc_hybrid.bin 6 8 96 \
                /home/zorax/data/Yahoo/row_col_6x8 /home/zorax/data/Yahoo/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128
            wait
        done
    fi

    if [ $datasets = "Kron29" ] || [ $datasets = "ALL" ]; then
        for ((times = 0; times < $epochs; times++)); do
            echo "====================================================Kron29===================================================="
            ./graphene/test/wcc/aio_wcc_hybrid.bin 6 8 96 \
                /home/zorax/data/Kron29/row_col_6x8 /home/zorax/data/Kron29/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128
            wait
        done
    fi

    if [ $datasets = "Kron30" ] || [ $datasets = "ALL" ]; then
        for ((times = 0; times < $epochs; times++)); do
            echo "====================================================Kron30===================================================="
            ./graphene/test/wcc/aio_wcc_hybrid.bin 6 8 96 \
                /home/zorax/data/Kron30/row_col_6x8 /home/zorax/data/Kron30/row_col_6x8 \
                out.txt-split_beg out.txt-split_csr \
                $chunknum $chunksize 4 32 16 128
            wait
        done
    fi
fi
