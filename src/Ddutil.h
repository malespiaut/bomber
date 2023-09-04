#ifndef _DDUTILS_H_
#define _DDUTILS_H_
#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif	/* __cplusplus */

#ifdef DX7
//extern IDirectDrawPalette  *DDLoadPalette(IDirectDraw7 *pdd, LPCSTR szBitmap);
extern IDirectDrawSurface7 *DDLoadBitmap(IDirectDraw7 *pdd, LPCSTR szBitmap, int dx, int dy);
extern HRESULT              DDReLoadBitmap(IDirectDrawSurface7 *pdds, LPCSTR szBitmap);
extern HRESULT              DDCopyBitmap(IDirectDrawSurface7 *pdds, HBITMAP hbm, int x, int y, int dx, int dy);
extern DWORD                DDColorMatch(IDirectDrawSurface7 *pdds, COLORREF rgb);
extern HRESULT              DDSetColorKey(IDirectDrawSurface7 *pdds, COLORREF rgb);
#elif defined DX5
//extern IDirectDrawPalette  *DDLoadPalette(IDirectDraw4 *pdd, LPCSTR szBitmap);
extern IDirectDrawSurface4 *DDLoadBitmap(IDirectDraw4 *pdd, LPCSTR szBitmap, int dx, int dy);
extern HRESULT              DDReLoadBitmap(IDirectDrawSurface4 *pdds, LPCSTR szBitmap);
extern HRESULT              DDCopyBitmap(IDirectDrawSurface4 *pdds, HBITMAP hbm, int x, int y, int dx, int dy);
extern DWORD                DDColorMatch(IDirectDrawSurface4 *pdds, COLORREF rgb);
extern HRESULT              DDSetColorKey(IDirectDrawSurface4 *pdds, COLORREF rgb);
#else
//extern IDirectDrawPalette  *DDLoadPalette(IDirectDraw2 *pdd, LPCSTR szBitmap);
extern IDirectDrawSurface  *DDLoadBitmap(IDirectDraw2 *pdd, LPCSTR szBitmap, int dx, int dy);
extern HRESULT              DDReLoadBitmap(IDirectDrawSurface *pdds, LPCSTR szBitmap);
extern HRESULT              DDCopyBitmap(IDirectDrawSurface *pdds, HBITMAP hbm, int x, int y, int dx, int dy);
extern DWORD                DDColorMatch(IDirectDrawSurface *pdds, COLORREF rgb);
extern HRESULT              DDSetColorKey(IDirectDrawSurface *pdds, COLORREF rgb);
#endif

#ifdef __cplusplus
}
#endif	/* __cplusplus */

#endif
