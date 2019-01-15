/*
 * FFT.h
 *
 *  Created on: 2018-06-19
 *      Author: ChengZu  Email: 1351606745@qq.com
 */

#ifndef FFT_H_
#define FFT_H_

class FFTClass
{
public:
    FFTClass() {};
    void __fastcall FFT(double *br, double *bi, int n, int ity);
    void __fastcall cstab(double *st, double *ct, int il, int ity);
    void __fastcall brtab(int *lbr, int il);
    void __fastcall binrv(double *bc, int il, const int *lb);
    void __fastcall fft1(double *br, double *bi, int il, const double *st, const double *ct);
    virtual ~FFTClass() {};
private:

};


#endif /* FFT_H_ */

