/*
    Beginning Game Programming, Third Edition
    MyDirectX.cpp
*/

#include "MyDirectX.h"

//Direct3D variables
LPDIRECT3D9 d3d = NULL; 
LPDIRECT3DDEVICE9 d3ddev = NULL; 
LPDIRECT3DSURFACE9 backbuffer = NULL;

//DirectInput variables
LPDIRECTINPUT8 dinput = NULL;
LPDIRECTINPUTDEVICE8 dimouse = NULL;
LPDIRECTINPUTDEVICE8 dikeyboard = NULL;
DIMOUSESTATE mouse_state;
char keys[256];

//DirectSound variables
CSoundManager *dsound = NULL;

// Direct3D initialization 
bool Direct3D_Init(HWND hwnd, int width, int height, bool fullscreen)
{
    //initialize Direct3D
    d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (!d3d) return false;

    //set Direct3D presentation parameters
    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = (!fullscreen);
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount = 1;
    d3dpp.BackBufferWidth = width;
    d3dpp.BackBufferHeight = height;
    d3dpp.hDeviceWindow = hwnd;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    //create Direct3D device
    d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);

    if (!d3ddev) return false;

    //get a pointer to the back buffer surface
    d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
	
	//create sprite handler object
	HRESULT result = D3DXCreateSprite ( d3ddev,	&sprite_obj);
	if (result != D3D_OK)
		return false;

    return true;
}


// Direct3D shutdown
void Direct3D_Shutdown()
{
	if (sprite_obj) sprite_obj->Release();
    if (d3ddev) d3ddev->Release();
    if (d3d) d3d->Release();
}


// Draws a surface to the screen using StretchRect
void DrawSurface(LPDIRECT3DSURFACE9 dest, float x, float y, LPDIRECT3DSURFACE9 source)
{
    //get width/height from source surface
    D3DSURFACE_DESC desc;
    source->GetDesc(&desc);

    //create rects for drawing
    RECT source_rect = {0, 0, (long)desc.Width, (long)desc.Height };
    RECT dest_rect = { (long)x, (long)y, (long)x+desc.Width, (long)y+desc.Height};
    
    //draw the source surface onto the dest
    d3ddev->StretchRect(source, &source_rect, dest, &dest_rect, D3DTEXF_NONE);

}


// Loads a bitmap file into a surface
LPDIRECT3DSURFACE9 LoadSurface(string filename)
{
    LPDIRECT3DSURFACE9 image = NULL;
    
    //get width and height from bitmap file
    D3DXIMAGE_INFO info;
    HRESULT result = D3DXGetImageInfoFromFile(filename.c_str(), &info);
    if (result != D3D_OK)
        return NULL;

    //create surface
    result = d3ddev->CreateOffscreenPlainSurface(
        info.Width,         //width of the surface
        info.Height,        //height of the surface
        D3DFMT_X8R8G8B8,    //surface format
        D3DPOOL_DEFAULT,    //memory pool to use
        &image,             //pointer to the surface
        NULL);              //reserved (always NULL)

    if (result != D3D_OK) return NULL;

    //load surface from file into newly created surface
    result = D3DXLoadSurfaceFromFile(
        image,                  //destination surface
        NULL,                   //destination palette
        NULL,                   //destination rectangle
        filename.c_str(),       //source filename
        NULL,                   //source rectangle
        D3DX_DEFAULT,           //controls how image is filtered
        D3DCOLOR_XRGB(0,0,0),   //for transparency (0 for none)
        NULL);                  //source image info (usually NULL)

    //make sure file was loaded okay
    if (result != D3D_OK) return NULL;

    return image;
}

LPDIRECT3DTEXTURE9 LoadTexture (string filename, D3DCOLOR transcolor)
// transcolor defines the background color and 
// should be transparent when rendering on screen
{
    LPDIRECT3DTEXTURE9 texture = NULL;
    D3DXIMAGE_INFO info;
    HRESULT result;

    //get width and height from bitmap file
    result = D3DXGetImageInfoFromFile(filename.c_str(), &info);

    if (result != D3D_OK)
        return NULL;
    result = D3DXCreateTextureFromFileEx (d3ddev,
		filename.c_str(), info.Width, info.Height,         
		1, D3DPOOL_DEFAULT, D3DFMT_UNKNOWN, 
		D3DPOOL_DEFAULT, D3DX_DEFAULT, 
		D3DX_DEFAULT, transcolor, &info, 
		NULL, &texture ); 

    //make sure the texture was loaded correctly
    if (result != D3D_OK)
        return NULL;

    return texture;
}


void Sprite_Draw_Frame(LPDIRECT3DTEXTURE9 texture, int destx, int desty, int framenum, int framew, int frameh, int columns)
{
	D3DXVECTOR3 position( (float)destx, (float)desty, 0 );
	D3DCOLOR white = D3DCOLOR_XRGB(255,255,255);

	RECT rect;
 	rect.left = (framenum % columns) * framew;
	rect.top = (framenum / columns) * frameh;
	rect.right = rect.left + framew;
	rect.bottom = rect.top + frameh;

	sprite_obj->Draw( texture, &rect, NULL, &position, white);
}

void Sprite_Animate(int &frame, int startframe, int endframe, int direction, int &starttime, int delay)
{
	if ((int)GetTickCount() > starttime + delay)
	{
		starttime = GetTickCount();

		frame += direction;
		if (frame > endframe) frame = startframe;
		if (frame < startframe) frame = endframe;
	}	
}

/**
 ** DirectInput initialization
 **/
bool DirectInput_Init(HWND hwnd)
{
    //initialize DirectInput object
    HRESULT result = DirectInput8Create(
        GetModuleHandle(NULL), 
        DIRECTINPUT_VERSION, 
        IID_IDirectInput8,
        (void**)&dinput,
        NULL);

    //initialize the keyboard
    dinput->CreateDevice(GUID_SysKeyboard, &dikeyboard, NULL);
    dikeyboard->SetDataFormat(&c_dfDIKeyboard);
    dikeyboard->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
    dikeyboard->Acquire();

    //initialize the mouse
    dinput->CreateDevice(GUID_SysMouse, &dimouse, NULL);
    dimouse->SetDataFormat(&c_dfDIMouse);
    dimouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
    dimouse->Acquire();
    d3ddev->ShowCursor(false);

    return true;
}

/**
 ** DirectInput update
 **/
void DirectInput_Update()
{
    //update mouse
	dimouse->Acquire();
    dimouse->GetDeviceState(sizeof(mouse_state), (LPVOID)&mouse_state);

    //update keyboard
	dikeyboard->Acquire();
    dikeyboard->GetDeviceState(sizeof(keys), (LPVOID)&keys);
}

/**
 ** Return mouse x movement
 **/
int Mouse_X()
{
    return mouse_state.lX;
}

/**
 ** Return mouse y movement
 **/
int Mouse_Y()
{
    return mouse_state.lY;
}

/**
 ** Return mouse button state
 **/
int Mouse_Button(int button)
{
    return mouse_state.rgbButtons[button] & 0x80;
}

/**
 ** Return key press state
 **/
int Key_Down(int key)
{
    return (keys[key] & 0x80);
}

/**
 ** DirectInput shutdown
 **/
void DirectInput_Shutdown()
{
    if (dikeyboard) 
    {
        dikeyboard->Unacquire();
        dikeyboard->Release();
        dikeyboard = NULL;
    }
    if (dimouse) 
    {
        dimouse->Unacquire();
        dimouse->Release();
        dimouse = NULL;
    }
}

//DirectSound Initialization
bool DirectSound_Init(HWND hwnd)
{
    //create DirectSound manager object
    dsound = new CSoundManager();

    //initialize DirectSound
    HRESULT result;
    result = dsound->Initialize(hwnd, DSSCL_PRIORITY);
    if (result != DS_OK) return false;

    //set the primary buffer format
    result = dsound->SetPrimaryBufferFormat(2, 22050, 16);
    if (result != DS_OK) return false;

    //return success
    return true;
}

void DirectSound_Shutdown()
{
    if (dsound) delete dsound;
}

CSound *LoadSound(string filename)
{
    HRESULT result;

    //create local reference to wave data
    CSound *wave = NULL;

    //attempt to load the wave file
    char s[255];
    sprintf_s(s, "%s", filename.c_str());
    result = dsound->Create(&wave, s);
    if (result != DS_OK) wave = NULL;

    //return the wave
    return wave;
}

void PlaySound(CSound *sound)
{
    sound->Play();
}

void LoopSound(CSound *sound)
{
    sound->Play(0, DSBPLAY_LOOPING);
}

void StopSound(CSound *sound)
{
    sound->Stop();
}


void SetCamera(float x, float y, float z, float lookx, float looky, float lookz)
{
	D3DXMATRIX matView;
	D3DXVECTOR3 updir(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 cameraSource;
	D3DXVECTOR3 cameraTarget;

	//move the camera
	cameraSource.x = x;
	cameraSource.y = y;
	cameraSource.z = z;

	//point the camera
	cameraTarget.x = lookx;
	cameraTarget.y = looky;
	cameraTarget.z = lookz;

	//set up the camera view matrix
	D3DXMatrixLookAtLH(&matView, &cameraSource, &cameraTarget, &updir);
	d3ddev->SetTransform(D3DTS_VIEW, &matView);
}

void SetPerspective(float fieldOfView, float aspectRatio, float nearRange, float farRange)
{
	//set the perspective so things in the distance will look smaller
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, fieldOfView, aspectRatio, nearRange, farRange);
	d3ddev->SetTransform(D3DTS_PROJECTION, &matProj);
}

VERTEX CreateVertex(float x, float y, float z, float tu, float tv)
{
	VERTEX vertex;
	vertex.x = x;
	vertex.y = y;
	vertex.z = z;
	vertex.tu = tu;
	vertex.tv = tv;
	return vertex;
}


QUAD *CreateQuad(string textureFilename)
{
	QUAD *quad = (QUAD*)malloc(sizeof(QUAD));

	//load the texture
	D3DXCreateTextureFromFile(d3ddev, textureFilename.c_str(), &quad->texture);

	//create the vertex buffer for this quad
	d3ddev->CreateVertexBuffer(
		4 * sizeof(VERTEX),
		0,
		D3DFVF_MYVERTEX, D3DPOOL_DEFAULT,
		&quad->buffer,
		NULL);

	//create the four corners of this dual triangle strip
	//each vertex is X,Y,Z and the texture coordinates U,V
	quad->vertices[0] = CreateVertex(-1.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	quad->vertices[1] = CreateVertex(1.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	quad->vertices[2] = CreateVertex(-1.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	quad->vertices[3] = CreateVertex(1.0f, -1.0f, 0.0f, 1.0f, 1.0f);

	D3DMATERIAL9 WHITE_MTRL = {WHITE, WHITE, WHITE, BLACK, 2.0f};
	quad->material = WHITE_MTRL;
	
	return quad;
}

void DeleteQuad(QUAD *quad)
{
	if (quad == NULL)
		return;

	//free the vertex buffer
	if (quad->buffer != NULL)
		quad->buffer->Release();

	//free the texture
	if (quad->texture != NULL)
		quad->texture->Release();

	//free the quad
	free(quad);
}

void DrawQuad(QUAD *quad)
{
	//fill vertex buffer with this quad's vertices
	void *temp = NULL;
	quad->buffer->Lock(0, sizeof(quad->vertices), (void**)&temp, 0);
	memcpy(temp, quad->vertices, sizeof(quad->vertices));
	quad->buffer->Unlock();

	d3ddev->SetFVF(D3DFVF_MYVERTEX);

	//draw the textured dual triangle strip
	d3ddev->SetTexture(0, quad->texture);
	d3ddev->SetMaterial(&quad->material);
	d3ddev->SetStreamSource(0, quad->buffer, 0, sizeof(VERTEX));
	d3ddev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}



MODEL *LoadModel(string filename)
{
    MODEL *model = (MODEL*)malloc(sizeof(MODEL));
    LPD3DXBUFFER matbuffer;
    HRESULT result;

    //load mesh from the specified file
    result = D3DXLoadMeshFromX(
        filename.c_str(),               //filename
        D3DXMESH_SYSTEMMEM,     //mesh options
        d3ddev,                 //Direct3D device
        NULL,                   //adjacency buffer
        &matbuffer,             //material buffer
        NULL,                   //special effects
        &model->material_count, //number of materials
        &model->mesh);          //resulting mesh

    if (result != D3D_OK)
    {
        MessageBox(0, "Error loading model file", APPTITLE.c_str(), 0);
        return NULL;
    }

    //extract material properties and texture names from material buffer
    LPD3DXMATERIAL d3dxMaterials = (LPD3DXMATERIAL)matbuffer->GetBufferPointer();
    model->materials = new D3DMATERIAL9[model->material_count];
    model->textures  = new LPDIRECT3DTEXTURE9[model->material_count];

    //create the materials and textures
    for(DWORD i=0; i<model->material_count; i++)
    {
        //grab the material
        model->materials[i] = d3dxMaterials[i].MatD3D;

        //set ambient color for material 
        model->materials[i].Ambient = model->materials[i].Diffuse;

        model->textures[i] = NULL;
		if (d3dxMaterials[i].pTextureFilename != NULL)
		{
			string filename = d3dxMaterials[i].pTextureFilename;
			ifstream f;
			f.open(filename);
			if (f)
			{
				result = D3DXCreateTextureFromFile(d3ddev, filename.c_str(), &model->textures[i]);
				if (result != D3D_OK)
				{
					MessageBox(NULL, filename.c_str(), "Error", MB_OK);
					return false;
				}
				f.close();
			}
		}
    }

    //done using material buffer
    matbuffer->Release();

    return model;
}

void DeleteModel(MODEL *model)
{
    //remove materials from memory
    if( model->materials != NULL ) 
        delete[] model->materials;

    //remove textures from memory
    if (model->textures != NULL)
    {
        for( DWORD i = 0; i < model->material_count; i++)
        {
            if (model->textures[i] != NULL)
                model->textures[i]->Release();
        }
        delete[] model->textures;
    }
    
    //remove mesh from memory
    if (model->mesh != NULL)
        model->mesh->Release();

    //remove model struct from memory
    if (model != NULL)
        free(model);
    
}

void DrawModel(MODEL *model)
{
	for( DWORD i=0; i < model->material_count; i++ )
	{
		d3ddev->SetMaterial (&model->materials[i]);
		d3ddev->SetTexture (0, model->textures[i]);
		model->mesh->DrawSubset(i);
	}
        
}
