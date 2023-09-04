#include "General.h"
#include <ddraw.h>
#include "ddutil.h"

#ifdef DX7
extern "C" IDirectDrawSurface7*
DDLoadBitmap(IDirectDraw7* pdd, LPCSTR szBitmap, int dx, int dy)
#elif defined DX5
extern "C" IDirectDrawSurface4*
DDLoadBitmap(IDirectDraw4* pdd, LPCSTR szBitmap, int dx, int dy)
#else
extern "C" IDirectDrawSurface*
DDLoadBitmap(IDirectDraw2* pdd, LPCSTR szBitmap, int dx, int dy)
#endif
{
    HBITMAP                 hbm;
    BITMAP                  bm;
#ifdef DX7
    DDSURFACEDESC2          ddsd;
    IDirectDrawSurface7    *pdds;
#elif defined DX5
    DDSURFACEDESC2          ddsd;
    IDirectDrawSurface4    *pdds;
#else
    DDSURFACEDESC	        ddsd;
    IDirectDrawSurface     *pdds;
#endif

    hbm = (HBITMAP) LoadImage(GetModuleHandle(NULL), szBitmap, IMAGE_BITMAP, dx,
                              dy, LR_CREATEDIBSECTION);
    if (hbm == NULL)
        hbm = (HBITMAP) LoadImage(NULL, szBitmap, IMAGE_BITMAP, dx, dy,
                                  LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (hbm == NULL)
        return NULL;
    GetObject(hbm, sizeof(bm), &bm);

    memset(&ddsd,0,sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    ddsd.dwWidth = bm.bmWidth;
    ddsd.dwHeight = bm.bmHeight;
    if (pdd->CreateSurface(&ddsd, &pdds, NULL) != DD_OK)
        return NULL;
    DDCopyBitmap(pdds, hbm, 0, 0, 0, 0);
    DeleteObject(hbm);
    return pdds;
}

#ifdef DX7
HRESULT
DDReLoadBitmap(IDirectDrawSurface7* pdds, LPCSTR szBitmap)
#elif defined DX5
HRESULT
DDReLoadBitmap(IDirectDrawSurface4* pdds, LPCSTR szBitmap)
#else
HRESULT
DDReLoadBitmap(IDirectDrawSurface* pdds, LPCSTR szBitmap)
#endif
{
    HBITMAP                 hbm;
    HRESULT                 hr;

    hbm = (HBITMAP) LoadImage(GetModuleHandle(NULL), szBitmap, IMAGE_BITMAP, 0,
                              0, LR_CREATEDIBSECTION);
    if (hbm == NULL)
        hbm = (HBITMAP) LoadImage(NULL, szBitmap, IMAGE_BITMAP, 0, 0,
                                  LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (hbm == NULL)
        return E_FAIL;

    hr = DDCopyBitmap(pdds, hbm, 0, 0, 0, 0);
    if (hr != DD_OK)
    DeleteObject(hbm);
    return hr;
}

#ifdef DX7
extern "C" HRESULT
DDCopyBitmap(IDirectDrawSurface7* pdds, HBITMAP hbm, int x, int y,
             int dx, int dy)
#elif defined DX5
extern "C" HRESULT
DDCopyBitmap(IDirectDrawSurface4* pdds, HBITMAP hbm, int x, int y,
             int dx, int dy)
#else
extern "C" HRESULT
DDCopyBitmap(IDirectDrawSurface* pdds, HBITMAP hbm, int x, int y,
             int dx, int dy)
#endif
{
    HDC                     hdcImage;
    HDC                     hdc;
    BITMAP                  bm;
#ifndef DX3
    DDSURFACEDESC2          ddsd;
#else
    DDSURFACEDESC           ddsd;
#endif
    HRESULT                 hr;

    if (hbm == NULL || pdds == NULL)
        return E_FAIL;

    pdds->Restore();
    hdcImage = CreateCompatibleDC(NULL);
    if (!hdcImage)
		return E_FAIL;
    SelectObject(hdcImage, hbm);

    GetObject(hbm, sizeof(bm), &bm);
    dx = dx == 0 ? bm.bmWidth : dx;
    dy = dy == 0 ? bm.bmHeight : dy;
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;
    pdds->GetSurfaceDesc(&ddsd);

    if ((hr = pdds->GetDC(&hdc)) == DD_OK)
    {
        StretchBlt(hdc, 0, 0, ddsd.dwWidth, ddsd.dwHeight, hdcImage, x, y,
                   dx, dy, SRCCOPY);
        pdds->ReleaseDC(hdc);
    }
    DeleteDC(hdcImage);
    return hr;
}

#ifdef DX7
extern "C" DWORD
DDColorMatch(IDirectDrawSurface7* pdds, COLORREF rgb)
#elif defined DX5
extern "C" DWORD
DDColorMatch(IDirectDrawSurface4* pdds, COLORREF rgb)
#else
extern "C" DWORD
DDColorMatch(IDirectDrawSurface* pdds, COLORREF rgb)
#endif
{
    COLORREF                rgbT;
    HDC                     hdc;
    DWORD                   dw=CLR_INVALID;
#ifndef DX3
    DDSURFACEDESC2          ddsd;
#else
    DDSURFACEDESC           ddsd;
#endif
    HRESULT                 hres;

    if ((rgb!=CLR_INVALID)&&(pdds->GetDC(&hdc)==DD_OK))
    {
        rgbT=GetPixel(hdc,0,0);
        SetPixel(hdc,0,0,rgb);
        pdds->ReleaseDC(hdc);
    }

    ddsd.dwSize = sizeof(ddsd);
	hres=pdds->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT,NULL);
    if (hres == DD_OK)
    {
        dw =*(DWORD*)ddsd.lpSurface;
        if(ddsd.ddpfPixelFormat.dwRGBBitCount<32)
            dw&=(1<<ddsd.ddpfPixelFormat.dwRGBBitCount)-1;
        pdds->Unlock(NULL);
    }

    if ((rgb!=CLR_INVALID)&&(pdds->GetDC(&hdc)==DD_OK))
    {
        SetPixel(hdc,0,0,rgbT);
        pdds->ReleaseDC(hdc);
    }
    return dw;
}

#ifdef DX7
extern "C" HRESULT
DDSetColorKey(IDirectDrawSurface7* pdds, COLORREF rgb)
#elif defined DX5
extern "C" HRESULT
DDSetColorKey(IDirectDrawSurface4* pdds, COLORREF rgb)
#else
extern "C" HRESULT
DDSetColorKey(IDirectDrawSurface* pdds, COLORREF rgb)
#endif
{
    DDCOLORKEY ddck;

    ddck.dwColorSpaceLowValue = DDColorMatch(pdds, rgb);
    ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue;
    return pdds->SetColorKey(DDCKEY_SRCBLT, &ddck);
}
