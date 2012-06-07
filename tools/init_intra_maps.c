#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "intra_fit_tools.h"

/*For validation purposes only.
  Copied from libvpx.*/
#if B_SZ==4
#define B_DC_PRED (OD_INTRA_DC)
#define B_TM_PRED (OD_INTRA_TM)
#define B_VE_PRED (OD_INTRA_VE)
#define B_HE_PRED (OD_INTRA_HE)
#define B_LD_PRED (OD_INTRA_LD)
#define B_RD_PRED (OD_INTRA_RD)
#define B_VR_PRED (OD_INTRA_VR)
#define B_VL_PRED (OD_INTRA_VL)
#define B_HU_PRED (OD_INTRA_HU)
#define B_HD_PRED (OD_INTRA_HD)

void vp8_intra4x4_predict_c(const unsigned char *src, int src_stride,
                            int b_mode,
                            unsigned char *dst, int dst_stride)
{
    int i, r, c;
    const unsigned char *Above = src - src_stride;
    unsigned char Left[4];
    unsigned char top_left = Above[-1];
    Left[0] = src[-1];
    Left[1] = src[-1 + src_stride];
    Left[2] = src[-1 + 2 * src_stride];
    Left[3] = src[-1 + 3 * src_stride];
    switch (b_mode)
    {
    case B_DC_PRED:
    {
        int expected_dc = 0;
        for (i = 0; i < 4; i++)
        {
            expected_dc += Above[i];
            expected_dc += Left[i];
        }
        expected_dc = (expected_dc + 4) >> 3;
        for (r = 0; r < 4; r++)
        {
            for (c = 0; c < 4; c++)
            {
                dst[c] = expected_dc;
            }
            dst += dst_stride;
        }
    }
    break;
    case B_TM_PRED:
    {
        /* prediction similar to true_motion prediction */
        for (r = 0; r < 4; r++)
        {
            for (c = 0; c < 4; c++)
            {
                int pred = Above[c] - top_left + Left[r];
                if (pred < 0)
                    pred = 0;
                if (pred > 255)
                    pred = 255;
                dst[c] = pred;
            }
            dst += dst_stride;
        }
    }
    break;
    case B_VE_PRED:
    {
        unsigned int ap[4];
        ap[0] = (top_left  + 2 * Above[0] + Above[1] + 2) >> 2;
        ap[1] = (Above[0] + 2 * Above[1] + Above[2] + 2) >> 2;
        ap[2] = (Above[1] + 2 * Above[2] + Above[3] + 2) >> 2;
        ap[3] = (Above[2] + 2 * Above[3] + Above[4] + 2) >> 2;
        for (r = 0; r < 4; r++)
        {
            for (c = 0; c < 4; c++)
            {
                dst[c] = ap[c];
            }
            dst += dst_stride;
        }
    }
    break;
    case B_HE_PRED:
    {
        unsigned int lp[4];
        lp[0] = (top_left + 2 * Left[0] + Left[1] + 2) >> 2;
        lp[1] = (Left[0] + 2 * Left[1] + Left[2] + 2) >> 2;
        lp[2] = (Left[1] + 2 * Left[2] + Left[3] + 2) >> 2;
        lp[3] = (Left[2] + 2 * Left[3] + Left[3] + 2) >> 2;
        for (r = 0; r < 4; r++)
        {
            for (c = 0; c < 4; c++)
            {
                dst[c] = lp[r];
            }
            dst += dst_stride;
        }
    }
    break;
    case B_LD_PRED:
    {
        const unsigned char *ptr = Above;
        dst[0 * dst_stride + 0] = (ptr[0] + ptr[1] * 2 + ptr[2] + 2) >> 2;
        dst[0 * dst_stride + 1] =
            dst[1 * dst_stride + 0] = (ptr[1] + ptr[2] * 2 + ptr[3] + 2) >> 2;
        dst[0 * dst_stride + 2] =
            dst[1 * dst_stride + 1] =
                dst[2 * dst_stride + 0] = (ptr[2] + ptr[3] * 2 + ptr[4] + 2) >> 2;
        dst[0 * dst_stride + 3] =
            dst[1 * dst_stride + 2] =
                dst[2 * dst_stride + 1] =
                    dst[3 * dst_stride + 0] = (ptr[3] + ptr[4] * 2 + ptr[5] + 2) >> 2;
        dst[1 * dst_stride + 3] =
            dst[2 * dst_stride + 2] =
                dst[3 * dst_stride + 1] = (ptr[4] + ptr[5] * 2 + ptr[6] + 2) >> 2;
        dst[2 * dst_stride + 3] =
            dst[3 * dst_stride + 2] = (ptr[5] + ptr[6] * 2 + ptr[7] + 2) >> 2;
        dst[3 * dst_stride + 3] = (ptr[6] + ptr[7] * 2 + ptr[7] + 2) >> 2;
    }
    break;
    case B_RD_PRED:
    {
        unsigned char pp[9];
        pp[0] = Left[3];
        pp[1] = Left[2];
        pp[2] = Left[1];
        pp[3] = Left[0];
        pp[4] = top_left;
        pp[5] = Above[0];
        pp[6] = Above[1];
        pp[7] = Above[2];
        pp[8] = Above[3];
        dst[3 * dst_stride + 0] = (pp[0] + pp[1] * 2 + pp[2] + 2) >> 2;
        dst[3 * dst_stride + 1] =
            dst[2 * dst_stride + 0] = (pp[1] + pp[2] * 2 + pp[3] + 2) >> 2;
        dst[3 * dst_stride + 2] =
            dst[2 * dst_stride + 1] =
                dst[1 * dst_stride + 0] = (pp[2] + pp[3] * 2 + pp[4] + 2) >> 2;
        dst[3 * dst_stride + 3] =
            dst[2 * dst_stride + 2] =
                dst[1 * dst_stride + 1] =
                    dst[0 * dst_stride + 0] = (pp[3] + pp[4] * 2 + pp[5] + 2) >> 2;
        dst[2 * dst_stride + 3] =
            dst[1 * dst_stride + 2] =
                dst[0 * dst_stride + 1] = (pp[4] + pp[5] * 2 + pp[6] + 2) >> 2;
        dst[1 * dst_stride + 3] =
            dst[0 * dst_stride + 2] = (pp[5] + pp[6] * 2 + pp[7] + 2) >> 2;
        dst[0 * dst_stride + 3] = (pp[6] + pp[7] * 2 + pp[8] + 2) >> 2;
    }
    break;
    case B_VR_PRED:
    {
        unsigned char pp[9];
        pp[0] = Left[3];
        pp[1] = Left[2];
        pp[2] = Left[1];
        pp[3] = Left[0];
        pp[4] = top_left;
        pp[5] = Above[0];
        pp[6] = Above[1];
        pp[7] = Above[2];
        pp[8] = Above[3];
        dst[3 * dst_stride + 0] = (pp[1] + pp[2] * 2 + pp[3] + 2) >> 2;
        dst[2 * dst_stride + 0] = (pp[2] + pp[3] * 2 + pp[4] + 2) >> 2;
        dst[3 * dst_stride + 1] =
            dst[1 * dst_stride + 0] = (pp[3] + pp[4] * 2 + pp[5] + 2) >> 2;
        dst[2 * dst_stride + 1] =
            dst[0 * dst_stride + 0] = (pp[4] + pp[5] + 1) >> 1;
        dst[3 * dst_stride + 2] =
            dst[1 * dst_stride + 1] = (pp[4] + pp[5] * 2 + pp[6] + 2) >> 2;
        dst[2 * dst_stride + 2] =
            dst[0 * dst_stride + 1] = (pp[5] + pp[6] + 1) >> 1;
        dst[3 * dst_stride + 3] =
            dst[1 * dst_stride + 2] = (pp[5] + pp[6] * 2 + pp[7] + 2) >> 2;
        dst[2 * dst_stride + 3] =
            dst[0 * dst_stride + 2] = (pp[6] + pp[7] + 1) >> 1;
        dst[1 * dst_stride + 3] = (pp[6] + pp[7] * 2 + pp[8] + 2) >> 2;
        dst[0 * dst_stride + 3] = (pp[7] + pp[8] + 1) >> 1;
    }
    break;
    case B_VL_PRED:
    {
        const unsigned char *pp = Above;
        dst[0 * dst_stride + 0] = (pp[0] + pp[1] + 1) >> 1;
        dst[1 * dst_stride + 0] = (pp[0] + pp[1] * 2 + pp[2] + 2) >> 2;
        dst[2 * dst_stride + 0] =
            dst[0 * dst_stride + 1] = (pp[1] + pp[2] + 1) >> 1;
        dst[1 * dst_stride + 1] =
            dst[3 * dst_stride + 0] = (pp[1] + pp[2] * 2 + pp[3] + 2) >> 2;
        dst[2 * dst_stride + 1] =
            dst[0 * dst_stride + 2] = (pp[2] + pp[3] + 1) >> 1;
        dst[3 * dst_stride + 1] =
            dst[1 * dst_stride + 2] = (pp[2] + pp[3] * 2 + pp[4] + 2) >> 2;
        dst[0 * dst_stride + 3] =
            dst[2 * dst_stride + 2] = (pp[3] + pp[4] + 1) >> 1;
        dst[1 * dst_stride + 3] =
            dst[3 * dst_stride + 2] = (pp[3] + pp[4] * 2 + pp[5] + 2) >> 2;
        dst[2 * dst_stride + 3] = (pp[4] + pp[5] + 1) >> 1;
        dst[3 * dst_stride + 3] = (pp[4] + pp[5] * 2 + pp[6] + 2) >> 2;
    }
    break;
    case B_HD_PRED:
    {
        unsigned char pp[9];
        pp[0] = Left[3];
        pp[1] = Left[2];
        pp[2] = Left[1];
        pp[3] = Left[0];
        pp[4] = top_left;
        pp[5] = Above[0];
        pp[6] = Above[1];
        pp[7] = Above[2];
        pp[8] = Above[3];
        dst[3 * dst_stride + 0] = (pp[0] + pp[1] + 1) >> 1;
        dst[3 * dst_stride + 1] = (pp[0] + pp[1] * 2 + pp[2] + 2) >> 2;
        dst[2 * dst_stride + 0] =
            dst[3 * dst_stride + 2] = (pp[1] + pp[2] + 1) >> 1;
        dst[2 * dst_stride + 1] =
            dst[3 * dst_stride + 3] = (pp[1] + pp[2] * 2 + pp[3] + 2) >> 2;
        dst[2 * dst_stride + 2] =
            dst[1 * dst_stride + 0] = (pp[2] + pp[3] + 1) >> 1;
        dst[2 * dst_stride + 3] =
            dst[1 * dst_stride + 1] = (pp[2] + pp[3] * 2 + pp[4] + 2) >> 2;
        dst[1 * dst_stride + 2] =
            dst[0 * dst_stride + 0] = (pp[3] + pp[4] + 1) >> 1;
        dst[1 * dst_stride + 3] =
            dst[0 * dst_stride + 1] = (pp[3] + pp[4] * 2 + pp[5] + 2) >> 2;
        dst[0 * dst_stride + 2] = (pp[4] + pp[5] * 2 + pp[6] + 2) >> 2;
        dst[0 * dst_stride + 3] = (pp[5] + pp[6] * 2 + pp[7] + 2) >> 2;
    }
    break;
    case B_HU_PRED:
    {
        unsigned char *pp = Left;
        dst[0 * dst_stride + 0] = (pp[0] + pp[1] + 1) >> 1;
        dst[0 * dst_stride + 1] = (pp[0] + pp[1] * 2 + pp[2] + 2) >> 2;
        dst[0 * dst_stride + 2] =
            dst[1 * dst_stride + 0] = (pp[1] + pp[2] + 1) >> 1;
        dst[0 * dst_stride + 3] =
            dst[1 * dst_stride + 1] = (pp[1] + pp[2] * 2 + pp[3] + 2) >> 2;
        dst[1 * dst_stride + 2] =
            dst[2 * dst_stride + 0] = (pp[2] + pp[3] + 1) >> 1;
        dst[1 * dst_stride + 3] =
            dst[2 * dst_stride + 1] = (pp[2] + pp[3] * 2 + pp[3] + 2) >> 2;
        dst[2 * dst_stride + 2] =
            dst[2 * dst_stride + 3] =
                dst[3 * dst_stride + 0] =
                    dst[3 * dst_stride + 1] =
                        dst[3 * dst_stride + 2] =
                            dst[3 * dst_stride + 3] = pp[3];
    }
    break;
    }
}
#endif

static void vp8_intra_predict(unsigned char *_dst,int _dst_stride,
 const unsigned char *_src,int _src_stride,int _mode){
  const unsigned char *above;
  unsigned char       *left;
  unsigned char        p[4*B_SZ];
  int                  x;
  int                  y;
  above=_src-_src_stride;
  p[2*B_SZ-1]=*(above-1);
  left=p+2*B_SZ;
  for(y=0;y<B_SZ;y++)left[y]=*(_src+_src_stride*y-1);
  switch(_mode){
    case OD_INTRA_DC:{
      int dc;
      dc=0;
      for(x=0;x<B_SZ;x++)dc+=above[x];
      for(y=0;y<B_SZ;y++)dc+=left[y];
      dc=dc+B_SZ>>B_SZ_LOG+1;
      for(y=0;y<B_SZ;y++){
        for(x=0;x<B_SZ;x++){
          *(_dst+y*_dst_stride+x)=(unsigned char)dc;
        }
      }
    }break;
    case OD_INTRA_TM:{
      for(y=0;y<B_SZ;y++){
        for(x=0;x<B_SZ;x++){
          *(_dst+y*_dst_stride+x)=OD_CLAMP255(above[x]+left[y]-*(above-1));
        }
      }
    }break;
    case OD_INTRA_HU:{
      for(y=B_SZ;y<B_SZ+(B_SZ>>1)+1;y++)left[y]=left[B_SZ-1];
      for(y=0;y<B_SZ;y++){
        for(x=0;x<(B_SZ>>1);x++){
          *(_dst+y*_dst_stride+2*x)=
           (unsigned char)(left[y+x]+left[y+x+1]+1>>1);
          *(_dst+y*_dst_stride+2*x+1)=
           (unsigned char)(left[y+x]+2*left[y+x+1]+left[x+y+2]+2>>2);
        }
      }
    }break;
    case OD_INTRA_HE:{
      unsigned char q[B_SZ];
      p[2*B_SZ-1]=*(above-1);
      left[B_SZ]=left[B_SZ-1];
      for(y=0;y<B_SZ;y++){
        q[y]=(unsigned char)(p[2*B_SZ+y-1]+2*p[2*B_SZ+y]+p[2*B_SZ+y+1]+2>>2);
      }
      for(y=0;y<B_SZ;y++)for(x=0;x<B_SZ;x++)*(_dst+y*_dst_stride+x)=q[y];
    }break;
    case OD_INTRA_HD:{
      for(x=0;x<B_SZ;x++)p[2*B_SZ-x-1]=*(above+x-1);
      for(y=0;y<B_SZ;y++){
        for(x=0;x<(B_SZ>>1);x++){
          if(y<x){
            *(_dst+y*_dst_stride+2*x)=(unsigned char)(
             p[2*B_SZ+y-2*x+1]+2*p[2*B_SZ+y-2*x]+p[2*B_SZ+y-2*x-1]+2>>2);
            *(_dst+y*_dst_stride+2*x+1)=(unsigned char)(
             p[2*B_SZ+y-2*x]+2*p[2*B_SZ+y-2*x-1]+p[2*B_SZ+y-2*x-2]+2>>2);
          }
          else{
            *(_dst+y*_dst_stride+2*x)=
             (unsigned char)(p[2*B_SZ+y-x-1]+p[2*B_SZ+y-x]+1>>1);
            *(_dst+y*_dst_stride+2*x+1)=(unsigned char)(
             p[2*B_SZ+y-x]+2*p[2*B_SZ+y-x-1]+p[2*B_SZ+y-x-2]+2>>2);
          }
        }
      }
    }break;
    case OD_INTRA_RD:{
      for(x=0;x<=B_SZ;x++)p[2*B_SZ-x-1]=*(above+x-1);
      for(y=0;y<B_SZ;y++){
        for(x=0;x<B_SZ;x++){
          *(_dst+y*_dst_stride+x)=(unsigned char)(
           p[2*B_SZ+y-x]+2*p[2*B_SZ+y-x-1]+p[2*B_SZ+y-x-2]+2>>2);
        }
      }
    }break;
    case OD_INTRA_VR:{
      for(x=0;x<=B_SZ;x++)p[2*B_SZ-x-1]=*(above+x-1);
      for(y=0;y<(B_SZ>>1);y++){
        for(x=0;x<B_SZ;x++){
          if(x<y){
            *(_dst+2*y*_dst_stride+x)=(unsigned char)(
             p[2*B_SZ+2*y-x-1]+2*p[2*B_SZ+2*y-x-2]+p[2*B_SZ+2*y-x-3]+2>>2);
            *(_dst+(2*y+1)*_dst_stride+x)=(unsigned char)(
             p[2*B_SZ+2*y-x]+2*p[2*B_SZ+2*y-x-1]+p[2*B_SZ+2*y-x-2]+2>>2);
          }
          else{
            *(_dst+2*y*_dst_stride+x)=
             (unsigned char)(p[2*B_SZ+y-x-1]+p[2*B_SZ+y-x-2]+1>>1);
            *(_dst+(2*y+1)*_dst_stride+x)=(unsigned char)(
             p[2*B_SZ+y-x]+2*p[2*B_SZ+y-x-1]+p[2*B_SZ+y-x-2]+2>>2);
          }
        }
      }
    }break;
    case OD_INTRA_VE:{
      unsigned char q[B_SZ];
      for(x=0;x<B_SZ;x++){
        q[x]=(unsigned char)(*(above+x-1)+2*above[x]+above[x+1]+2>>2);
      }
      for(y=0;y<B_SZ;y++)for(x=0;x<B_SZ;x++)*(_dst+y*_dst_stride+x)=q[x];
    }break;
    case OD_INTRA_VL:{
      for(y=0;y<(B_SZ>>1);y++){
        /*This ignores the pattern mis-match for the last few values of the
           VP8 predictor.*/
        for(x=0;x<B_SZ;x++){
          *(_dst+2*y*_dst_stride+x)=
           (unsigned char)(above[y+x]+above[y+x+1]+1>>1);
          *(_dst+(2*y+1)*_dst_stride+x)=
           (unsigned char)(above[y+x]+2*above[y+x+1]+above[y+x+2]+2>>2);
        }
      }
    }break;
    case OD_INTRA_LD:{
      for(y=0;y<B_SZ;y++){
        for(x=0;x<B_SZ;x++){
          *(_dst+y*_dst_stride+x)=(unsigned char)(above[y+x]+2*above[y+x+1]
           +above[OD_MINI(y+x+2,2*B_SZ-1)]+2>>2);
        }
      }
    }break;
  }
}

static void od_diff_hadamard(short _buf[B_SZ*B_SZ],
 const unsigned char *_src,int _src_stride,
 const unsigned char *_ref,int _ref_stride){
  int i;
  for(i=0;i<B_SZ;i++){
    int t[B_SZ];
    int r;
    int j;
    int k;
    int l;
    /*Hadamard stage 1:*/
    for(j=0;j<(B_SZ>>1);j++){
      t[j]=_src[j]-_ref[j]+_src[j+(B_SZ>>1)]-_ref[j+(B_SZ>>1)];
      t[j+(B_SZ>>1)]=_src[j]-_ref[j]-_src[j+(B_SZ>>1)]+_ref[j+(B_SZ>>1)];
    }
    /*Hadamard stages 2...N-1:*/
    for(l=B_SZ>>2;l>1;l>>=1){
      for(k=0;k<B_SZ;k+=2*l){
        for(j=0;j<l;j++){
          r=t[j+k];
          t[j+k]=r+t[j+k+l];
          t[j+k+l]=r-t[j+k+l];
        }
      }
    }
    /*Hadamard stage N:*/
    for(k=0;k<B_SZ;k+=2){
      _buf[k*B_SZ+i]=(short)(t[k]+t[k+1]);
      _buf[(k+1)*B_SZ+i]=(short)(t[k]-t[k+1]);
    }
    _src+=_src_stride;
    _ref+=_ref_stride;
  }
}

static unsigned od_hadamard_sad(const short _buf[B_SZ*B_SZ]){
  unsigned sad;
  int      i;
  sad=0;
  for(i=0;i<B_SZ;i++){
    int t[B_SZ];
    int j;
    int k;
    int l;
    int r;
    /*Hadamard stage 1:*/
    for(j=0;j<(B_SZ>>1);j++){
      t[j]=_buf[i*B_SZ+j]+_buf[i*B_SZ+j+(B_SZ>>1)];
      t[j+(B_SZ>>1)]=_buf[i*B_SZ+j]-_buf[i*B_SZ+j+(B_SZ>>1)];
    }
    /*Hadamard stages 2...N-1:*/
    for(l=B_SZ>>2;l>1;l>>=1){
      for(k=0;k<B_SZ;k+=2*l){
        for(j=0;j<l;j++){
          r=t[j+k];
          t[j+k]=r+t[j+k+l];
          t[j+k+l]=r-t[j+k+l];
        }
      }
    }
    /*Hadamard stage N:*/
    for(k=0;k<B_SZ;k+=2){
      sad+=abs(t[k]+t[k+1]);
      sad+=abs(t[k]-t[k+1]);
    }
  }
  return sad;
}

static unsigned od_satd(const unsigned char *_src,int _src_stride,
 const unsigned char *_ref,int _ref_stride){
  short buf[B_SZ*B_SZ];
  od_diff_hadamard(buf,_src,_src_stride,_ref,_ref_stride);
  return od_hadamard_sad(buf);
}


typedef struct init_intra_maps_ctx init_intra_maps_ctx;



struct init_intra_maps_ctx{
  FILE          *map_file;
  char          *map_filename;
  unsigned char *map;
  int            nxblocks;
  int            nyblocks;
};



static int init_intra_plane_start(void *_ctx,const char *_name,
 const th_info *_ti,int _pli,int _nxblocks,int _nyblocks){
  init_intra_maps_ctx *ctx;
  FILE                *map_file;
  char                *map_filename;
  ctx=(init_intra_maps_ctx *)_ctx;
  map_filename=get_map_filename(_name,_pli,_nxblocks,_nyblocks);
  map_file=fopen(map_filename,"wb");
  if(map_file==NULL){
    fprintf(stderr,"Error opening output file '%s'.\n",map_filename);
    return EXIT_FAILURE;
  }
  ctx->map_file=map_file;
  ctx->map_filename=map_filename;
  ctx->map=(unsigned char *)malloc(_nxblocks*(size_t)_nyblocks);
  ctx->nxblocks=_nxblocks;
  ctx->nyblocks=_nyblocks;
  return EXIT_SUCCESS;
}

static void init_intra_block(void *_ctx,const unsigned char *_data,int _stride,
 int _bi,int _bj){
  init_intra_maps_ctx *ctx;
  int                  mode;
  unsigned             best_satd;
  int                  best_mode;
  ctx=(init_intra_maps_ctx *)_ctx;
  best_mode=0;
  best_satd=UINT_MAX;
  for(mode=0;mode<10;mode++){
    unsigned char        block[B_SZ*B_SZ];
    unsigned             satd;
    memset(block,0,B_SZ*B_SZ);
    vp8_intra_predict(block,B_SZ,_data,_stride,mode);
#if B_SZ==4
    {
      unsigned char block2[B_SZ*B_SZ];
      vp8_intra4x4_predict_c(_data,_stride,mode,block2,B_SZ);
      if(memcmp(block,block2,sizeof(block2))!=0){
        fprintf(stderr,"Intra prediction mismatch.\n");
        exit(EXIT_FAILURE);
      }
    }
#endif
    satd=od_satd(block,B_SZ,_data,_stride);
    if(satd<best_satd){
      best_satd=satd;
      best_mode=mode;
    }
  }
  ctx->map[_bj*ctx->nxblocks+_bi]=(unsigned char)best_mode;
}

static int init_intra_plane_finish(void *_ctx){
  init_intra_maps_ctx *ctx;
  ctx=(init_intra_maps_ctx *)_ctx;
  if(fwrite(ctx->map,ctx->nxblocks*(size_t)ctx->nyblocks,1,ctx->map_file)<1){
    fprintf(stderr,"Error writing to output file '%s'.\n",ctx->map_filename);
    return EXIT_FAILURE;
  }
  fclose(ctx->map_file);
  free(ctx->map);
  free(ctx->map_filename);
  return EXIT_SUCCESS;
}

int main(int _argc,const char **_argv){
  init_intra_maps_ctx ctx;
  return apply_to_blocks(&ctx,init_intra_plane_start,init_intra_block,
   init_intra_plane_finish,_argc,_argv);
}