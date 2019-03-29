
#include "se.h"

dctx *ddpfs_ctx;
struct ib_device *d_dev; //创设备
int ib_port = 1;

static void ddpfs_ib_add_one(struct ib_device *device)
{
    ddpfs_ctx= (dctx *)kmalloc(sizeof(dctx), GFP_KERNEL);
    d_dev = device;

    ddpfs_ctx->ctx_pd = ib_alloc_pd(device);
    if(!ddpfs_ctx->ctx_pd){
        p_err("could not allocate pd\n", 0);
    }
    return 0;
}

static void ddpfs_ib_remove_one(struct ib_device *device, void *client_data){
    return;
}

struct ib_client ddpfs_ib_client = {
    .name = "ddpfs_ib_client",
    .add = ddpfs_ib_add_one,
    .remove = ddpfs_ib_remove_one
};

int d_ib_query_port(struct ib_device *device, u8 port_num, struct ib_port_attr *port_attr)
{
    int c;
    c = ib_query_port(device, port_num, port_attr);
    if(c<0)
    {
        p_err("Fail to do query_port");
    }
    return c;
}

int ddpfs_setup_mr(dctx *ctx)
{
    int ret;
    struct scatterlist sg = {0};
    struct ib_send_wr *bad_wr;
    u8 key = 0;

    ib_update_fast_reg_key(ctx->m->rdma_mr, ++key);
    ctx->m->reg_wr.key = ctx->m->rdma_mr->rkey;

    ctx->m->rdma_buf = kzalloc(D_LOG_RDMA_REGION, GFP_KERNEL);
    ctx->m->rdma_dma_addr = dma_map_single(ctx->pd->device->dma_device, ctx->m->rdma_buf,D_LOG_RDMA_REGION, DMA_BIDIRECTIONAL);

    sg_dma_address(&sg) = ctx->m->rdma_dma_addr;
    sg_dma_len(&sg) = D_LOG_RDMA_REGION;

    p_pri("MAP %llx .. %llx", ctx->m-rdma_dma_addr, ctx->m->rdma_dma_addr + D_LOG_RDMA_REGION);
    ret = ib_map_mr_sg(ctx->m->rdma_mr, &sg, 1, NULL, PAGE_SIZE);
    if(ret != 1){
        p_err("ERROR map sg ret %d, sg.address %llx sg.len %u",ret,sg.dma_address, sg.dma_length);
        return -EINVAL;
    }
    p_pri("map sg ret %d, sg.address %llx sg.len %u\n", ret, sg.dma_address,sg.dma_length);
    p_pri("reg MR sg%d rkey 0x%x va %llx len %u page_size %u", ctx->m->reg_wr.wr.num_sge, ctx->m->reg_wr.key,
    ctx->m->rdma_mr->iova, ctx->m->rdma_mr->length, ctx->mi->rdma_mr->page_size);

    return ret;
}

int ddpfs_setup_local_memory(dctx *ctx){
    int ret;
    int i;
    struct ddpfs_mem_info *m;

    ctx->m = kzalloc(sizeof(struct ddpfs_mem_info), GFP_KERNEL);
    if(!ctx->m){
        return -ENOMEM;
    }
    return 0;
}

int setup_mem(dctx *ctx){
    int ret,i;
    struct ddpfa_mem_info *m;

    ctx->m = kzalloc(sizeof(struct ddpfs_mem_info), GFP_KERNEL)
    if(!ctx->m){
        return -ENOMEM;
    }
    m->recv_pages = alloc_pages(GFP_KERNEL,9);
    if(!m->recv_pages){
        p_err("Fail to allocate pages for recv buf",0);
        return -ENOMEM;
    }
    m->recv_buf = page_address(m->recv_pages);

    m->recv_dma_addr = dma_map_single(ctx->pd->device->dma_device,m->recv_buf, 
    )
}
