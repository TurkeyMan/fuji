xvs.1.1

; transform and project position
dp4 oPos.x,vPos,c[localToScreen]
dp4 oPos.y,vPos,c[localToScreen+1]
dp4 oPos.z,vPos,c[localToScreen+2]
dp4 oPos.w,vPos,c[localToScreen+3]

; compute directional light
dp3 oD0,vNormal,c[4]

; copy texture co-ordinate
//mov oT0,vUV
dp4 oT0.x,vT0,c[textureMatrix]
dp4 oT0.y,vT0,c[textureMatrix+1]
