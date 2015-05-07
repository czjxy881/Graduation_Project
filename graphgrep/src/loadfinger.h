#ifndef _LOADFINGER_H
#define _LOADFINGER_H

typedef struct LoadFinger_Struct *LoadFinger;

LoadFinger LoadFingerCreate(int ELM);
void  LoadFingerDestroy(LoadFinger q);
int  *LoadFingerReadFingerprints(LoadFinger q, int size);
int  *LoadFingerReadFingerprintspaths(LoadFinger q, int size);
int  *LoadFingerReadFormula(LoadFinger q, int size);

#endif /*_LOADFINGER_H */
