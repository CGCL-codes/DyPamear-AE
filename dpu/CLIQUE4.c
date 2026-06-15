#include <dpu_mine.h>

#ifdef BITMAP
static ans_t __imp_clique4_bitmap(sysname_t tasklet_id, node_t second_index) {
    ans_t ans = 0;
    mram_read(mram_bitmap[second_index], bitmap[tasklet_id], sizeof(bitmap[tasklet_id]));
    for (node_t i = 0; i < bitmap_size; i++) {
        uint32_t tmp = bitmap[tasklet_id][i];
        if (tmp) for (node_t j = 0; j < 32; j++) {
            if (tmp & (1 << j)) {
                node_t third_index = (i << 5) + j;
                mram_read(mram_bitmap[third_index], bitmap[tasklet_id + NR_TASKLETS], sizeof(bitmap[tasklet_id]));
                intersect_bitmap(bitmap[tasklet_id], bitmap[tasklet_id + NR_TASKLETS], bitmap[tasklet_id + (NR_TASKLETS << 1)], bitmap_size);
                for (node_t k = 0; k < bitmap_size; k++) {
                    uint32_t tmp2 = bitmap[tasklet_id + (NR_TASKLETS << 1)][k];
                    if (tmp2) for (node_t l = 0; l < 32; l++) {
                        if (tmp2 & (1 << l)) ans++;
                    }
                }
            }
        }
    }
    return ans;
}
#endif

static ans_t __imp_clique4_2(sysname_t tasklet_id, node_t root, node_t second_root) {
    node_t(*tasklet_buf)[BUF_SIZE] = buf[tasklet_id];

    edge_ptr root_begin = row_ptr[root];  // intended DMA
    edge_ptr root_end = row_ptr[root + 1];  // intended DMA  TODO: opt
    edge_ptr second_root_begin = row_ptr[second_root];  // intended DMA
    edge_ptr second_root_end = row_ptr[second_root + 1];  // intended DMA
    ans_t ans = 0;
    node_t common_size = intersect_seq_buf_thresh_withcbuf(tasklet_buf, &col_idx[root_begin], root_end - root_begin, &col_idx[second_root_begin], second_root_end - second_root_begin, mram_buf[tasklet_id], second_root);
    for (node_t j = 0; j < common_size; j++) {
        node_t third_root = mram_buf[tasklet_id][j];  // intended DMA
        edge_ptr third_root_begin = row_ptr[third_root];  // intended DMA
        edge_ptr third_root_end = row_ptr[third_root + 1];  // intended DMA
        node_t common_size2 = intersect_seq_buf_thresh_withcbuf(tasklet_buf, mram_buf[tasklet_id], common_size, &col_idx[third_root_begin], third_root_end - third_root_begin, mram_buf[tasklet_id + NR_TASKLETS], third_root);
        ans += common_size2;
    }
    return ans;
}

static ans_t __imp_clique4(sysname_t tasklet_id, node_t root) {
    edge_ptr root_begin = row_ptr[root];  // intended DMA
    edge_ptr root_end = row_ptr[root + 1];  // intended DMA
    ans_t ans = 0;
    for (edge_ptr i = root_begin; i < root_end; i++) {
        node_t second_root = col_idx[i];  // intended DMA
        if (second_root >= root) break;
        ans += __imp_clique4_2(tasklet_id, root, second_root);
    }
    return ans;
}

extern void clique4(sysname_t tasklet_id) {
    static ans_t partial_ans[NR_TASKLETS];
    static uint64_t partial_cycle[NR_TASKLETS];
    static perfcounter_cycles cycles[NR_TASKLETS];
    
for (node_t i = tasklet_id; i < root_num; i += NR_TASKLETS) {
#ifdef PERF
        timer_start(&cycles[tasklet_id]);
#endif
        uint64_t edge_task = roots[i];
        node_t u = edge_task >> 32;
        node_t v = edge_task & 0xFFFFFFFF;

        ans[i] = __imp_clique4_2(tasklet_id, u, v);

#ifdef PERF
        cycle_ct[i] = timer_stop(&cycles[tasklet_id]);
#endif
    }
}
