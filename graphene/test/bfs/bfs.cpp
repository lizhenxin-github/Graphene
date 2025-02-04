#include "cache_driver.h"
#include "IO_smart_iterator.h"
#include <stdlib.h>
#include <sched.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <asm/mman.h>
#include "pin_thread.h"
#include "get_vert_count.hpp"
#include "get_col_ranger.hpp"

#define PIN_CPU

inline bool is_active(index_t vert_id,
					  sa_t criterion,
					  sa_t *sa, sa_t *prior)
{
	return (sa[vert_id] == criterion);
	//		return true;
	//	else
	//		return false;
}

int main(int argc, char **argv)
{
	std::cout << "Format: /path/to/exe "
			  << "#row_partitions #col_partitions thread_count "
			  << "/path/to/beg_pos_dir /path/to/csr_dir "
			  << "beg_header csr_header num_chunks "
			  << "chunk_sz (#bytes) concurr_IO_ctx "
			  << "max_continuous_useless_blk ring_vert_count num_buffs source\n";

	if (argc != 15)
	{
		fprintf(stdout, "Wrong input\n");
		exit(-1);
	}

	// Output input
	for (int i = 0; i < argc; i++)
		std::cout << argv[i] << " ";
	std::cout << "\n";

	const int row_par = atoi(argv[1]);	// 切分的行数
	const int col_par = atoi(argv[2]);	// 切分的列数
	const int NUM_THDS = atoi(argv[3]); // 工作线程总数，等于row_par*col_par*2
	const char *beg_dir = argv[4];
	const char *csr_dir = argv[5];
	const char *beg_header = argv[6];
	const char *csr_header = argv[7];
	const index_t num_chunks = atoi(argv[8]); // buffer中chunk数量
	const size_t chunk_sz = atoi(argv[9]);
	const index_t io_limit = atoi(argv[10]);
	const index_t MAX_USELESS = atoi(argv[11]); // bitmap合并中最大能不连续的大小
	const index_t ring_vert_count = atoi(argv[12]);
	const index_t num_buffs = atoi(argv[13]);
	vertex_t root = (vertex_t)atol(argv[14]);

	assert(NUM_THDS == (row_par * col_par * 2));
	sa_t *sa = NULL;
	index_t *comm = new index_t[NUM_THDS]; // 每个分区的结点数量？
	vertex_t **front_queue_ptr;
	index_t *front_count_ptr;
	vertex_t *col_ranger_ptr;

	const index_t vert_count = get_vert_count(comm, beg_dir, beg_header, row_par, col_par);
	get_col_ranger(col_ranger_ptr, front_queue_ptr,
				   front_count_ptr, beg_dir, beg_header,
				   row_par, col_par);

	sa = (sa_t *)mmap(NULL, sizeof(sa_t) * vert_count,
					  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB | MAP_HUGE_2MB, 0, 0);
	//| MAP_HUGETLB , 0, 0);
	if (sa == MAP_FAILED)
	{
		perror("mmap");
		exit(-1);
	}

	const index_t vert_per_blk = chunk_sz / sizeof(vertex_t);
	if (chunk_sz & (sizeof(vertex_t) - 1))
	{
		std::cout << "Page size wrong\n";
		exit(-1);
	}
	sa_t *sa_dummy = NULL;

	char cmd[256];
	sprintf(cmd, "%s", "iostat -x 1 -k > iostat_bfs.log&");
	std::cout << cmd << "\n";

	int *semaphore_acq = new int[1];
	int *semaphore_flag = new int[1];

#ifdef PIN_CPU
	//***************pin cpu ***************************//

	// hardware information for server YQ3.
	// available: 2 nodes (0-1)
	// node 0 cpus: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71
	// node 0 size: 64092 MB
	// node 0 free: 2644 MB
	// node 1 cpus: 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95
	// node 1 size: 64470 MB
	// node 1 free: 347 MB

	int socket_one[48] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71};
	int socket_two[48] = {24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95};

	int socket_all[96] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95};
#endif

	memset(sa, INFTY, sizeof(sa_t) * vert_count);
	sa[root] = 0;
	IO_smart_iterator **it_comm = new IO_smart_iterator *[NUM_THDS];

	double tm = 0;
#pragma omp parallel                        \
num_threads(NUM_THDS)                       \
	shared(sa, root, comm, front_queue_ptr, \
		   front_count_ptr, col_ranger_ptr)
	{
		std::stringstream travss;
		travss.str("");
		travss.clear();

		std::stringstream fetchss;
		fetchss.str("");
		fetchss.clear();

		std::stringstream savess;
		savess.str("");
		savess.clear();

		sa_t level = 0;
		int tid = omp_get_thread_num();
		int comp_tid = tid >> 1;
		comp_t *neighbors;
		index_t *beg_pos;

#ifdef PIN_CPU
		// 需要保证IO线程与计算线程在同一个numa节点上
		// printf("*****************tid==%d\n", tid);
		pin_thread(socket_all, tid);
#endif

		index_t prev_front_count = 0;
		index_t front_count = 0;

		if ((tid & 1) == 0)
		{
			IO_smart_iterator *it_temp =
				new IO_smart_iterator(
					front_queue_ptr,
					front_count_ptr,
					col_ranger_ptr,
					comp_tid, comm,
					row_par, col_par,
					beg_dir, csr_dir,
					beg_header, csr_header,
					num_chunks,
					chunk_sz,
					sa, sa_dummy, beg_pos,
					num_buffs,
					ring_vert_count,
					MAX_USELESS,
					io_limit,
					&is_active);

			front_queue_ptr[comp_tid][0] = root;
			front_count_ptr[comp_tid] = 1;

			prev_front_count = front_count_ptr[comp_tid];
			it_comm[tid] = it_temp;
			it_comm[tid]->is_bsp_done = false;
		}
#pragma omp barrier
		IO_smart_iterator *it = it_comm[(tid >> 1) << 1];

		if (!tid)
			system((const char *)cmd);
		double convert_tm = 0;
		while (true)
		{
			front_count = 0;
			//- Framework gives user block to process
			//- Figures out what blocks needed next level
			if ((tid & 1) == 0)
			{
				it->io_time = 0;
				it->wait_io_time = 0;
				it->wait_comp_time = 0;
				it->cd->io_submit_time = 0;
				it->cd->io_poll_time = 0;
				it->cd->fetch_sz = 0;
				it->cd->load_chunk_count = 0;

				it->cd->useful_vert_count = 0;
			}

			double ltm = wtime();
			convert_tm = wtime();
			if ((tid & 1) == 0)
			{
				it->is_bsp_done = false;
				if ((prev_front_count * 100.0) / vert_count > 2.0)
					it->req_translator(level);
				else
				{
					it->req_translator_queue();
				}
			}
			else
				it->is_io_done = false;
#pragma omp barrier
			convert_tm = wtime() - convert_tm;

			if ((tid & 1) == 0)
			{
				while (true)
				{
					int chunk_id = -1;
					double blk_tm = wtime();
					while ((chunk_id = it->cd->circ_load_chunk->de_circle()) == -1)
					{
						if (it->is_bsp_done)
						{
							chunk_id = it->cd->circ_load_chunk->de_circle();
							break;
						}
					}
					it->wait_io_time += (wtime() - blk_tm);

					if (chunk_id == -1)
						break;
					struct chunk *pinst = it->cd->cache[chunk_id];
					index_t blk_beg_off = pinst->blk_beg_off;
					index_t num_verts = pinst->load_sz;
					vertex_t vert_id = pinst->beg_vert;

					// process one chunk
					while (true)
					{
						if (sa[vert_id] == level)
						{
							index_t beg = beg_pos[vert_id - it->row_ranger_beg] - blk_beg_off;
							index_t end = beg + beg_pos[vert_id + 1 - it->row_ranger_beg] -
										  beg_pos[vert_id - it->row_ranger_beg];

							// possibly vert_id starts from preceding data block.
							// there by beg<0 is possible
							if (beg < 0)
								beg = 0;

							if (end > num_verts)
								end = num_verts;
							for (; beg < end; ++beg)
							{
								it->cd->useful_vert_count++;

								vertex_t nebr = pinst->buff[beg];
								if (sa[nebr] == INFTY)
								{
									sa[nebr] = level + 1;
									if (front_count <= it->col_ranger_end - it->col_ranger_beg)
										it->front_queue[comp_tid][front_count] = nebr;
									front_count++;
								}
							}
						}
						++vert_id;

						if (vert_id >= it->row_ranger_end)
							break;
						if (beg_pos[vert_id - it->row_ranger_beg] //?
								- blk_beg_off >
							num_verts)
							break;
					}

					pinst->status = EVICTED;
					assert(it->cd->circ_free_chunk->en_circle(chunk_id) != -1);
				}

				// work-steal
				//	for(int ii = tid - (col_par * 2); ii <= tid + (col_par * 2) ; ii += (col_par *4))
				//	{
				//		if(ii < 0 || ii >= NUM_THDS) continue;
				//		IO_smart_iterator* it_work_steal = it_comm[ii];
				//		while(true)
				//		{
				//			int chunk_id = -1;
				//			double blk_tm = wtime();
				//			while((chunk_id = it_work_steal->cd->circ_load_chunk->de_circle())
				//					== -1)
				//			{
				//				if(it_work_steal->is_bsp_done)
				//				{
				//					chunk_id = it_work_steal->cd->circ_load_chunk->de_circle();
				//					break;
				//				}
				//			}
				//			it_work_steal->wait_io_time += (wtime() - blk_tm);

				//			if(chunk_id == -1) break;
				//
				//			//printf("%dhelps%d-for%d\n", tid, ii, chunk_id);
				//			struct chunk *pinst = it_work_steal->cd->cache[chunk_id];
				//			index_t blk_beg_off = pinst->blk_beg_off;
				//			index_t num_verts = pinst->load_sz;
				//			vertex_t vert_id = pinst->beg_vert;

				//			//process one chunk
				//			while(true)
				//			{
				//				if(sa[vert_id] == level)
				//				{
				//					index_t beg = it_work_steal->beg_pos_ptr[vert_id - it_work_steal->row_ranger_beg]
				//						- blk_beg_off;
				//					index_t end = beg + it_work_steal->beg_pos_ptr[vert_id + 1 -
				//						it_work_steal->row_ranger_beg]-
				//						it_work_steal->beg_pos_ptr[vert_id - it_work_steal->row_ranger_beg];

				//					//possibly vert_id starts from preceding data block.
				//					//there by beg<0 is possible
				//					if(beg<0) beg = 0;

				//					if(end>num_verts) end = num_verts;
				//					for( ;beg<end; ++beg)
				//					{
				//						vertex_t nebr = pinst->buff[beg];
				//						if(sa[nebr] == INFTY)
				//						{
				//							sa[nebr]=level+1;
				//							if(front_count <= it->col_ranger_end - it->col_ranger_beg)
				//								it->front_queue[comp_tid][front_count] = nebr;
				//							front_count++;
				//						}
				//					}
				//				}
				//				++vert_id;

				//				if(vert_id >= it_work_steal->row_ranger_end) break;
				//				if(it_work_steal->beg_pos_ptr[vert_id - it_work_steal->row_ranger_beg]
				//						- blk_beg_off > num_verts)
				//					break;
				//			}

				//			pinst->status = EVICTED;
				//			assert(it_work_steal->cd->circ_free_chunk->en_circle(chunk_id)!= -1);
				//		}
				//	}

				it->front_count[comp_tid] = front_count;
			}
			else
			{
				while (it->is_bsp_done == false)
				{
					// if(it->circ_free_buff->get_sz() == 0)
					//{
					//	printf("worked\n");
					//	int curr_buff = it->next(-1);
					//	assert(curr_buff != -1);
					//	neighbors = it -> buff_dest[curr_buff];
					//	index_t buff_edge_count = it -> buff_edge_count[curr_buff];
					//	//nebr_chk += buff_edge_count;
					//	for(long i = 0;i < buff_edge_count; i++)
					//	{
					//		vertex_t nebr = neighbors[i];
					//		if(sa[nebr]==INFTY)
					//		{
					//			//printf("new-front: %u\n", nebr);
					//			sa[nebr]=level+1;
					//			front_count++;
					//		}
					//	}
					//	it->circ_free_buff->en_circle(curr_buff);
					// }

					it->load_key(level);
					// it->load_key_iolist(level);
				}
			}
		finish_point:
			comm[tid] = front_count;
#pragma omp barrier
			front_count = 0;
			for (int i = 0; i < NUM_THDS; ++i)
				front_count += comm[i];

			ltm = wtime() - ltm;
			if (!tid)
				tm += ltm;

#pragma omp barrier
			comm[tid] = it->cd->fetch_sz;
#pragma omp barrier
			index_t total_sz = 0;
			long useful_vert_count_sum = 0;

			for (int i = 0; i < NUM_THDS; ++i)
			{
				total_sz += comm[i];

				useful_vert_count_sum += it_comm[(i >> 1) << 1]->cd->useful_vert_count;
			}
#pragma omp barrier // 防止tid!=0的线程清空it状态，从而让上面统计出错。

			total_sz >>= 1; // total size doubled
			useful_vert_count_sum >>= 1;

			if (!tid)
				std::cout << "@level-" << (int)level
						  << "-font-leveltime-converttm-iotm-waitiotm-waitcomptm-iosize-usefulvert-chunk_utilization: "
						  << front_count << " " << ltm << " " << convert_tm << " " << it->io_time
						  << "(" << it->cd->io_submit_time << "," << it->cd->io_poll_time << ") "
						  << " " << it->wait_io_time << " " << it->wait_comp_time << " "
						  << total_sz << " "
						  << useful_vert_count_sum << " " << (1.0 * useful_vert_count_sum * sizeof(vertex_t) / total_sz) << "\n";

			if (front_count == 0 || level > 254)
				break;
			prev_front_count = front_count;
			front_count = 0;
			++level;
		}
		if (!tid)
			system("killall iostat");

		if (!tid)
			std::cout << "Total time: " << tm << " second(s)\n";

		if ((tid & 1) == 0)
			delete it;
	}

	munmap(sa, sizeof(sa_t) * vert_count);
	delete[] comm;
	//	gpu_freer(keys_d);
	return 0;
}
