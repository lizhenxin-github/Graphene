#include "cache_driver.h"
#include "IO_smart_iterator.h"
#include <stdlib.h>
#include <sched.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <asm/mman.h>
#include "pin_thread.h"
#include <algorithm>
#include "get_vert_count.hpp"
#include "get_col_ranger.hpp"

#define PIN_CPU

inline bool is_active(index_t vert_id,
					  sa_t criterion,
					  sa_t *sa, sa_t *sa_prev)
{
	// if(sa[vert_id]==criterion)
	return true;
	// else
	//	return false;
}

int main(int argc, char **argv)
{
	std::cout << "Format: /path/to/exe "
			  << "#row_partitions #col_partitions thread_count "
			  << "/path/to/beg_pos_dir /path/to/csr_dir "
			  << "beg_header csr_header num_chunks "
			  << "chunk_sz (#bytes) concurr_IO_ctx "
			  << "max_continuous_useless_blk ring_vert_count num_buffs\n";

	if (argc != 14)
	{
		fprintf(stdout, "Wrong input\n");
		exit(-1);
	}

	// Output input
	for (int i = 0; i < argc; i++)
		std::cout << argv[i] << " ";
	std::cout << "\n";

	const int row_par = atoi(argv[1]);
	const int col_par = atoi(argv[2]);
	const int NUM_THDS = atoi(argv[3]);
	const char *beg_dir = argv[4];
	const char *csr_dir = argv[5];
	const char *beg_header = argv[6];
	const char *csr_header = argv[7];
	const index_t num_chunks = atoi(argv[8]);
	const size_t chunk_sz = atoi(argv[9]);
	const index_t io_limit = atoi(argv[10]);
	const index_t MAX_USELESS = atoi(argv[11]);
	const index_t ring_vert_count = atoi(argv[12]);
	const index_t num_buffs = atoi(argv[13]);

	assert(NUM_THDS == (row_par * col_par * 2));
	sa_t *vector = NULL;
	sa_t *vector_res = NULL;
	index_t *comm = new index_t[NUM_THDS];
	vertex_t **front_queue_ptr;
	index_t *front_count_ptr;
	vertex_t *col_ranger_ptr;

	const index_t vert_count = get_vert_count(comm, beg_dir, beg_header, row_par, col_par);
	get_col_ranger(col_ranger_ptr, front_queue_ptr,
				   front_count_ptr, beg_dir, beg_header,
				   row_par, col_par);

	vector = (sa_t *)mmap(NULL, sizeof(sa_t) * vert_count,
						  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB | MAP_HUGE_2MB, 0, 0);

	vector_res = (sa_t *)mmap(NULL, sizeof(sa_t) * vert_count,
							  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB | MAP_HUGE_2MB, 0, 0);
	if (vector == MAP_FAILED)
	{
		perror("mmap");
		exit(-1);
	}

	// init rev_odeg and rank value
	for (index_t i = 0; i < vert_count; i++)
		vector[i] = rand() % vert_count;

	if (chunk_sz & (sizeof(vertex_t) - 1))
	{
		std::cout << "Page size wrong\n";
		exit(-1);
	}

	char cmd[256];
	sprintf(cmd, "%s", "iostat -x 1 -k > iostat_spmv.log&");
	std::cout << cmd << "\n";
	// exit(-1);

	int *semaphore_acq = new int[1];
	int *semaphore_flag = new int[1];
	char *spin_lock = new char[vert_count];
	memset(spin_lock, 0, sizeof(char) * vert_count);

	semaphore_acq[0] = 0;
	semaphore_flag[0] = 0;

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

	sa_t *sa_dummy;
	IO_smart_iterator **it_comm = new IO_smart_iterator *[NUM_THDS];

	double tm = 0;
#pragma omp parallel  \
num_threads(NUM_THDS) \
	shared(vector, comm)
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
		comp_t *neighbors, *sources;
		vertex_t dest, source;
		index_t *beg_pos = NULL;

		// use all threads in 1D partition manner.
		index_t step_1d = vert_count / NUM_THDS;
		index_t beg_1d = tid * step_1d;
		index_t end_1d = beg_1d + step_1d;
		if (tid == NUM_THDS - 1)
			end_1d = vert_count;

#ifdef PIN_CPU
		// 需要保证IO线程与计算线程在同一个numa节点上
		pin_thread(socket_all, tid);
#endif

		if ((tid & 1) == 0)
		{
			IO_smart_iterator *it_temp =
				new IO_smart_iterator(
					true,
					front_queue_ptr,
					front_count_ptr,
					col_ranger_ptr,
					comp_tid, comm,
					row_par, col_par,
					beg_dir, csr_dir,
					beg_header, csr_header,
					num_chunks,
					chunk_sz,
					vector, sa_dummy, beg_pos,
					num_buffs,
					ring_vert_count,
					MAX_USELESS,
					io_limit,
					&is_active);

			it_comm[tid] = it_temp;
			it_comm[tid]->is_bsp_done = false;
		}
#pragma omp barrier
		IO_smart_iterator *it = it_comm[(tid >> 1) << 1];

		// if(!tid) system((const char *)cmd);
#pragma omp barrier

		if (tid == 0)
			std::cout << "Start SpMV computation ... \n";
		while (true)
		{
			index_t front_count = 0;
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

				// not early terminate
				it->reqt_blk_count = 1; //
										// as long as not 0
			}
			double convert_tm;
			double ltm = wtime();
			if ((tid & 1) == 0)
			{
				it->is_bsp_done = false;
				convert_tm = wtime();
				convert_tm = wtime() - convert_tm;
			}
			else
				it->is_io_done = false;
#pragma omp barrier

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
							vertex_t nebr = pinst->buff[beg];
							__sync_fetch_and_add(vector_res + vert_id, vector[nebr]);
						}
						++vert_id;

						if (vert_id >= it->row_ranger_end)
							break;
						if (beg_pos[vert_id - it->row_ranger_beg] - blk_beg_off > num_verts)
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
				//			while((chunk_id = it->cd->circ_load_chunk->de_circle())
				//					== -1)
				//			{
				//				if(it->is_bsp_done)
				//				{
				//					chunk_id = it->cd->circ_load_chunk->de_circle();
				//					break;
				//				}
				//			}
				//			it->wait_io_time += (wtime() - blk_tm);

				//			if(chunk_id == -1) break;
				//			struct chunk *pinst = it->cd->cache[chunk_id];
				//			index_t blk_beg_off = pinst->blk_beg_off;
				//			index_t num_verts = pinst->load_sz;
				//			vertex_t vert_id = pinst->beg_vert;

				//			//process one chunk
				//			while(true)
				//			{
				//				index_t beg = beg_pos[vert_id - it->row_ranger_beg]
				//					- blk_beg_off;
				//				index_t end = beg + beg_pos[vert_id + 1 -
				//					it->row_ranger_beg]-
				//					beg_pos[vert_id - it->row_ranger_beg];

				//				//possibly vert_id starts from preceding data block.
				//				//there by beg<0 is possible
				//				if(beg<0) beg = 0;

				//				if(end>num_verts) end = num_verts;
				//				for( ;beg<end; ++beg)
				//				{
				//					vertex_t nebr = pinst->buff[beg];
				//					__sync_fetch_and_add(vector_res + vert_id, vector[nebr]);
				//				}
				//				++vert_id;

				//				if(vert_id >= it->row_ranger_end) break;
				//				if(beg_pos[vert_id - it->row_ranger_beg]
				//						- blk_beg_off > num_verts)
				//					break;
				//			}

				//			pinst->status = EVICTED;
				//			assert(it->cd->circ_free_chunk->en_circle(chunk_id)!= -1);
				//		}
				//	}
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

					it->load_kv_vert_full(level);
					// it->load_key_iolist(level);
				}
			}
		finish_point:
#pragma omp barrier

			ltm = wtime() - ltm;
			if (tid == 0)
				tm += ltm;
#pragma omp barrier
			comm[tid] = it->cd->fetch_sz;
#pragma omp barrier
			index_t total_sz = 0;
			for (int i = 0; i < NUM_THDS; ++i)
				total_sz += comm[i];
			total_sz >>= 1; // total size doubled

			if (!tid)
				std::cout << "@level-" << (int)level
						  << "-font-leveltime-converttm-iotm-waitiotm-waitcomptm-iosize: "
						  << front_count << " " << ltm << " " << convert_tm << " " << it->io_time
						  << "(" << it->cd->io_submit_time << "," << it->cd->io_poll_time << ") "
						  << " " << it->wait_io_time << " " << it->wait_comp_time << " "
						  << total_sz << "\n";
			++level;
			break;
		}

		// if(!tid)system("killall iostat");

		if (!tid)
			std::cout << "Total time: " << tm << " second(s)\n";

		if ((tid & 1) == 0)
			delete it;
	}
	munmap(vector, sizeof(sa_t) * vert_count);
	delete[] comm;
	return 0;
}
