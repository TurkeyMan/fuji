xvs.1.1

; transform and project position
dp4 oPos.x,vPos,localToScreen[0]
dp4 oPos.y,vPos,localToScreen[1]
dp4 oPos.z,vPos,localToScreen[2]
dp4 oPos.w,vPos,localToScreen[3]

; compute directional light
dp3 oD0,vNormal,c[4]

; copy texture co-ordinate
//mov oT0,vUV
dp4 oT0.x,vUV,textureMatrix[0]
dp4 oT0.y,vUV,textureMatrix[1]
