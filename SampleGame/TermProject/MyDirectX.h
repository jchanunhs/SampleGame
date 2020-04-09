/*
    Beginning Game Programming, Third Edition
    MyDirectX.h
*/

#pragma once

//header files
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <dinput.h>
#include <fstream>
#include "DirectSound.h"

using namespace std;

//libraries
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dsound.lib")
#pragma comment(lib,"dxerr.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"legacy_stdio_definitions.lib")

//program values
extern const string APPTITLE;
extern const int SCREENWIDTH;
extern const int SCREENHEIGHT;
extern bool gameover;

//Direct3D objects
extern LPDIRECT3D9 d3d; 
extern LPDIRECT3DDEVICE9 d3ddev; 
extern LPDIRECT3DSURFACE9 backbuffer;
extern LPD3DXSPRITE sprite_obj;

//primary DirectSound object
extern CSoundManager *dsound;

//game functions
bool Game_Init(HWND hwnd);
void Game_Run(HWND hwnd);
void Game_End();

//Direct3D functions
bool Direct3D_Init(HWND hwnd, int width, int height, bool fullscreen);
void Direct3D_Shutdown();
LPDIRECT3DSURFACE9 LoadSurface(string filename);
void DrawSurface(LPDIRECT3DSURFACE9 dest, float x, float y, LPDIRECT3DSURFACE9 source);
LPDIRECT3DTEXTURE9 LoadTexture (string filename, D3DCOLOR transcolor);
void Sprite_Draw_Frame(LPDIRECT3DTEXTURE9 texture, int destx, int desty, int framenum, int framew, int frameh, int columns);
void Sprite_Animate(int &frame, int startframe, int endframe, int direction, int &starttime, int delay);


//DirectInput objects, devices, and states
extern LPDIRECTINPUT8 dinput;
extern LPDIRECTINPUTDEVICE8 dimouse;
extern LPDIRECTINPUTDEVICE8 dikeyboard;
extern DIMOUSESTATE mouse_state;

//DirectInput functions
bool DirectInput_Init(HWND);
void DirectInput_Update();
void DirectInput_Shutdown();
int Key_Down(int);
int Mouse_Button(int);
int Mouse_X();
int Mouse_Y();

//DirectSound functions
bool DirectSound_Init(HWND hwnd);
void DirectSound_Shutdown();
CSound *LoadSound(string filename);
void PlaySound(CSound *sound);
void LoopSound(CSound *sound);
void StopSound(CSound *sound);

//vertex and quad definitions

#define WHITE D3DXCOLOR(1.0,1.0,1.0,1.0)
#define BLACK D3DXCOLOR(0.0,0.0,0.0,0.0)

#define D3DFVF_MYVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

struct VERTEX
{
    float x, y, z;		// vertex location
	float nx, ny, nz;	// normal vector of triangle surface
    float tu, tv;		// texture vertex location
};


struct QUAD
{
	VERTEX vertices[4];
	LPDIRECT3DVERTEXBUFFER9 buffer;
	LPDIRECT3DTEXTURE9 texture;
	D3DMATERIAL9 material;
};


void SetCamera(float x, float y, float z, float lookx, float looky, float lookz);
void SetPerspective(float fieldOfView, float aspectRatio, float nearRange, float farRange);
VERTEX CreateVertex(float x, float y, float z, float tu, float tv);
QUAD *CreateQuad(string textureFilename);
void DeleteQuad(QUAD *quad);
void DrawQuad(QUAD *quad);


struct MODEL
{
    LPD3DXMESH mesh;		
    D3DMATERIAL9* materials;	
    LPDIRECT3DTEXTURE9* textures;	
    DWORD material_count;		
};

MODEL *LoadModel(string filename);
void DrawModel(MODEL *model);
void DeleteModel(MODEL *model);


struct TERRAIN
{
	ID3DXMesh*	terrainMesh;
	LPDIRECT3DTEXTURE9 tex;

	int numVertRows;
	int numVertCols;
};

TERRAIN* CreateTerrain(string textureFileName, string heightFileName, float mapScale = 1);
void DrawTerrain(TERRAIN* terrain);
void DeleteTerrain(TERRAIN* terrain);
float GetHeight(TERRAIN* terrainWorld, float x, float z);