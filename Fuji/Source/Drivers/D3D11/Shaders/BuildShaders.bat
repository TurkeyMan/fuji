echo Vertex shaders
..\..\..\..\Bin\fxc.exe /FhMatStandard_a.h MatStandard.hlsl /Evs_main /Vng_vs_main_a /Tvs_4_0_level_9_0 /Zi /nologo /D_ANIMATION
..\..\..\..\Bin\fxc.exe /FhMatStandard_s.h MatStandard.hlsl /Evs_main /Vng_vs_main_s /Tvs_4_0_level_9_0 /Zi /nologo

echo Pixel shader
..\..\..\..\Bin\fxc.exe /FhMatStandard_ps.h MatStandard.hlsl /Eps_main /Vng_ps_main /Tps_4_0_level_9_0 /Zi /nologo

pause

