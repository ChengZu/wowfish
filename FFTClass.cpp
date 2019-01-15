#include <cmath>
#include <stdlib.h>
#include "FFTClass.h"
const double    PI = 3.14159265;
/* */


void __fastcall FFTClass::fft1(double *br, double *bi, int il, const double *st, const double *ct)
{
    //the size of br,bi should be equal to il.

    //and size of st and ct no less than il.
    int is = il >> 1;
    int mpx = 0;
    int ic = 1;
    int ia;

    while(is)
    {
        is >>= 1;
        mpx++;
    }

    is = il;

    for(ia = 1; ia <= mpx; ia++)
    {
        int ib;
        int ka = 0;
        is >>= 1;
        for(ib = 1; ib <= ic; ib++)
        {
            int in = 1;
            int k;
            for(k = 1; k <= is; k++)
            {
                int     j1 = ka + k;
                int     j2 = j1 + is;
                double  xr = br[j1];
                double  xi = bi[j1];
                double  yr = br[j2];
                double  yi = bi[j2];
                br[j1] = xr + yr;
                bi[j1] = xi + yi;
                xr -= yr;
                xi -= yi;
                br[j2] = xr * ct[in] - xi * st[in];
                bi[j2] = xr * st[in] + xi * ct[in];
                in += ic;
            }

            ka += is << 1;
        }

        ic <<= 1;
    }
}

/* */
void __fastcall FFTClass::binrv(double *bc, int il, const int *lb)
{
    int is = il - 1;
    int i;
    for(i = 2; i <= is; i++)
    {
        int ig = lb[i];
        if(ig <= i) continue;

        double  xx = bc[i];
        bc[i] = bc[ig];
        bc[ig] = xx;
    }
}

/* */
void __fastcall FFTClass::brtab(int *lbr, int il)
{
    int is = il >> 1;
    int mpx = 0;
    while(is)
    {
        is >>= 1;
        mpx++;
    }

    int ln;
    for(ln = 1; ln <= il; ln++)
    {
        int j1 = ln - 1;
        int ibord = 0;
        for(int k = 1; k <= mpx; k++)
        {
            int j2 = j1 >> 1;
            ibord = ibord * 2 + (j1 - 2 * j2);
            j1 = j2;
        }

        lbr[ln] = ibord + 1;
    }
}

/* */
void __fastcall FFTClass::cstab(double *st, double *ct, int il, int ity)
{
    double  yy;
    yy = -PI * 2.0 / il;
    if(ity < 0) yy = -yy;

    int     l;
    double  ang = 0.0;
    for(l = 1; l <= il; l++)
    {
        st[l] = sin(ang);
        ct[l] = cos(ang);
        ang += yy;
    }
}
void __fastcall FFTClass::FFT(double *br, double *bi, int n, int ity)
//ity>0 forward ,ity<0 backward

//n=pow(2,m);

//Here br and bi start from 0.
{
    double  *ct, *st;
    int     *il;
    if(n <= 0) return;

    double  invsqrtn = 1.0 / sqrt(n);
    ct = (double *)malloc(sizeof(double) * n) - 1;
    st = (double *)malloc(sizeof(double) * n) - 1;
    cstab(st, ct, n, ity);
    il = (int *)malloc(sizeof(int) * n) - 1;
    brtab(il, n);
    fft1(br - 1, bi - 1, n, st, ct);
    binrv(br - 1, n, il);
    binrv(bi - 1, n, il);
    for(int i = 0; i < n; i++)
    {
        br[i] *= invsqrtn;
        bi[i] *= invsqrtn;
    }

    free(il + 1);
    free(ct + 1);
    free(st + 1);
}
