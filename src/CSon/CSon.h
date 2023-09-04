void FT_Start(long*SizeFT,long*SizeWAV);
long FT_Decomp(char * BufferFTSrc,char * BufferWAVDst);
long FT_Comp(char * BufferWAVSrc,char * BufferFTDst);
void FT_Restart();
void FT_Free();

void FT_StereoToMono(short * BuffSrcSter, short * BuffDstMono, int NumSamples);
void FT_MonoToStereo(short * BuffSrcMono, short * BuffDstSter, int NumSamples);
long FT_ReSample(short * BuffSrcMono, short * BuffDstMono, short * BuffTmp, int NumSamples, int FreqSrc, int FreqDst);
void FT_CutAtRatio(short * BuffIn, short * BuffOut, int NumSamples, float ratio);
int FT_ReflectBound(int i, int Min, int Max);
int FT_SatureBound(int i, int Min, int Max);
