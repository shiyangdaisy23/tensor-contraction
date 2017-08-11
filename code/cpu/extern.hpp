#pragma once

/* CBLAS forward extern declarations
 *
 * Link with the CBLAS implementation of your choice
 */

#if defined (__cplusplus)
extern "C" {
#endif

enum CBLAS_ORDER        {CblasRowMajor=101, CblasColMajor=102};
enum CBLAS_TRANSPOSE    {CblasNoTrans=111, CblasTrans=112,
                         CblasConjTrans=113, CblasConjNoTrans=114};

#define CBLAS_INT int

void
cblas_sgemm(enum CBLAS_ORDER order,
            enum CBLAS_TRANSPOSE transA, enum CBLAS_TRANSPOSE transB,
            CBLAS_INT m, CBLAS_INT n, CBLAS_INT k,
            float alpha,
            const float *A, CBLAS_INT ldA,
            const float *B, CBLAS_INT ldB,
            float beta,
            float *C, CBLAS_INT ldC);

void
cblas_dgemm(enum CBLAS_ORDER order,
            enum CBLAS_TRANSPOSE transA, enum CBLAS_TRANSPOSE transB,
            CBLAS_INT m, CBLAS_INT n, CBLAS_INT k,
            double alpha,
            const double *A, CBLAS_INT ldA,
            const double *B, CBLAS_INT ldB,
            double beta,
            double *C, CBLAS_INT ldC);

void
cblas_cgemm(enum CBLAS_ORDER order,
            enum CBLAS_TRANSPOSE transA, enum CBLAS_TRANSPOSE transB,
            CBLAS_INT m, CBLAS_INT n, CBLAS_INT k,
            const float *alpha,
            const float *A, CBLAS_INT ldA,
            const float *B, CBLAS_INT ldB,
            const float *beta,
            float *C, CBLAS_INT ldC);

void
cblas_zgemm(enum CBLAS_ORDER order,
            enum CBLAS_TRANSPOSE transA, enum CBLAS_TRANSPOSE transB,
            CBLAS_INT m, CBLAS_INT n, CBLAS_INT k,
            const double *alpha,
            const double *A, CBLAS_INT ldA,
            const double *B, CBLAS_INT ldB,
            const double *beta,
            double *C, CBLAS_INT ldC);

#if defined (__cplusplus)
} // extern "C"
#endif
