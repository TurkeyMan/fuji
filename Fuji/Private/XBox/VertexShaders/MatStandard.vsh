xvs.1.1

#include "Registers.h"

mova a0, v4

// dp4 oPos.y,vPos,c[localToScreen+1]
dp4 r0.x, v0, c20[a0.y]  // t1<0>
dp4 r0.y, v0, c21[a0.y]  // t1<1>
dp4 r0.z, v0, c22[a0.y]  // t1<2>

mul r1.xyz, r0, v3.y

dp4 r0.x, v0, c20[a0.x]  // t0<0>
dp4 r0.y, v0, c21[a0.x]  // t0<1>
dp4 r0.z, v0, c22[a0.x]  // t0<2>

mad r1.xyz, r0, v3.x, r1

dp4 r0.x, v0, c20[a0.z]  // t2<0>
dp4 r0.y, v0, c21[a0.z]  // t2<1>
dp4 r0.z, v0, c22[a0.z]  // t2<2>

mad r1.xyz, r0, v3.z, r1

dp4 r0.x, v0, c20[a0.w]  // t3<0>
dp4 r0.y, v0, c21[a0.w]  // t3<1>
dp4 r0.z, v0, c22[a0.w]  // t3<2>
mad r1.xyz, r0, v3.w, r1

mul r0, v0, c11  // pos<0,1,2,3>

mad r0.xyz, r1, c12, r0  // pos<0,1,2>

dp4 oPos.x, r0, c0  // output<0>
dp4 oPos.y, r0, c1  // output<1>
dp4 oPos.z, r0, c2  // output<2>
dp4 oPos.w, r0, c3  // output<3>

dp4 r0.x, v1, c8  // output<8>
dp4 r0.y, v1, c9  // output<9>

mad oD0, v2, c10.xxxz, c10.yyyw  // output<4,5,6,7>

mov oT0, r0.xyyy  // ::main<8,9,10,11>
