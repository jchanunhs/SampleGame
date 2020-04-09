#include "MyDirectX.h"
#include "DirectSound.h"


const string APPTITLE = "Saitama Game";
const int SCREENWIDTH = 640;
const int SCREENHEIGHT = 480;
LPDIRECT3DSURFACE9 back_img = NULL;
LPD3DXSPRITE sprite_obj;
int starttime = 0;

int saitamaloser = 0; 
int borosloser = 0;

struct SPRITE	// define structure SPRITE at the beginning of MyGame.cpp
{
	int x, y, movex, movey;
	int frame, columns;
	int width, height;
	int startframe, endframe;
	int starttime, delay;
	int direction;
	SPRITE()
	{
		x = y = 0; 			movex = movey = 0;
		frame = 0; columns = 1; 	width = height = 0;
		startframe = endframe = 0;	starttime = delay = 0;
		direction = 1;
	}
};


int Collision(SPRITE sprite1, SPRITE sprite2)
{
	RECT rect1;
	rect1.left = sprite1.x + 1;
	rect1.top = sprite1.y + 1;
	rect1.right = sprite1.x + sprite1.width - 1;
	rect1.bottom = sprite1.y + sprite1.height - 1;

	RECT rect2;
	rect2.left = sprite2.x + 1;
	rect2.top = sprite2.y + 1;
	rect2.right = sprite2.x + sprite2.width - 1;
	rect2.bottom = sprite2.y + sprite2.height - 1;

	RECT dest;
	return IntersectRect(&dest, &rect1, &rect2);  // if rect1 and rect2 has intersection 
}

//Powerup Section
LPDIRECT3DTEXTURE9 power_img;
SPRITE powerupcell[5];
int valid_powerup[5]; //If the powerup has been used
int powerup[5];
const int powerup_move = 3; //Speed when powerup is taken
const int default_move = 1; //Sprite normal speed

//Heart Section
LPDIRECT3DTEXTURE9 heart_img;
SPRITE heart;
int valid_heart = 1;
int heal = 100; //Adds 100 health

//Saitama Section
LPDIRECT3DTEXTURE9 saitama_img;
SPRITE saitama;
int saitamaLife = 200;
int saitama_die = 0;
int saitama_damage[25];
int saitama_powerdamage[5];
int saitama_move = default_move;
//Special Move
int seriousPunch;

//Boros Section
LPDIRECT3DTEXTURE9 boros_img;
SPRITE boros;
int borosLife = 200;
int boros_die = 0;
int boros_damage[25];
int boros_powerdamage[5];
int boros_move = default_move;
//Special move
int roaringCannon = 0;





//Saitama Punch Section
const  int  SAITAMAPUNCH_NUMBERS = 25;
LPDIRECT3DTEXTURE9 saitamapunch_img;
SPRITE saitamapunch[SAITAMAPUNCH_NUMBERS];
int valid_saitamapunch[SAITAMAPUNCH_NUMBERS];
int start_saitamapunch;
int last_saitamapunch = -1;

//Boros Punch Section
const  int  BOROSPUNCH_NUMBERS = 25;
LPDIRECT3DTEXTURE9 borospunch_img;
SPRITE borospunch[BOROSPUNCH_NUMBERS];
int valid_borospunch[BOROSPUNCH_NUMBERS];
int start_borospunch;
int last_borospunch = -1;

//Serious Punch Section
const  int  SERIOUSPUNCH_NUMBERS = 5;
LPDIRECT3DTEXTURE9 seriouspunch_img;
SPRITE seriouspunch[SERIOUSPUNCH_NUMBERS];
int valid_seriouspunch[SERIOUSPUNCH_NUMBERS];
int start_seriouspunch;
int last_seriouspunch = -1;

//Roaring Cannon Section
const  int  ROARINGCANNON_NUMBERS = 5;
LPDIRECT3DTEXTURE9 roaringcannon_img;
SPRITE roaringcannon[ROARINGCANNON_NUMBERS];
int valid_roaringcannon[ROARINGCANNON_NUMBERS];
int start_roaringcannon;
int last_roaringcannon = -1;

//If either boros or saitama die;
int valid_die;

//Stone Section
const int cellwidth = 24;
const int cellheight = 24;
const int cellrows = SCREENHEIGHT / cellheight;
const int cellcolumns = SCREENWIDTH / cellwidth;
int cell[cellrows][cellcolumns];
LPDIRECT3DTEXTURE9 stone_img;
SPRITE stonecell[cellrows][cellcolumns];

//the wav sound
CSound *sound_seriouspunch;
CSound *sound_roaringcannon;
CSound *sound_punch;




void UpdateLoser(HWND hwnd)
{
		if (valid_die && borosloser)
		{
			MessageBox(0, "Game Over! Boros Lost!", "Game Over", MB_OK);
			gameover = true;
		}
		else if (valid_die && saitamaloser)
		{
			MessageBox(0, "Game Over! Saitama Lost!", "Game Over", MB_OK);
			gameover = true;
		}
	}







bool LoadSaitama()
{
	D3DXIMAGE_INFO info;
	HRESULT result;

	//load the saitama sprite
	saitama_img = LoadTexture("saitama.bmp", D3DCOLOR_XRGB(255, 0, 255));
	if (saitama_img == NULL)
		return false;
	//set the saitama properties
	result = D3DXGetImageInfoFromFile("saitama.bmp", &info);
	if (result != D3D_OK)
		return false;

	saitama.width = info.Width;
	saitama.height = info.Height/4;
	saitama.x = (SCREENWIDTH - saitama.width)/14;
	saitama.y = (SCREENHEIGHT - saitama.height)/2;
	saitama.movex = 0; saitama.movey = -1;
	saitama.endframe = 0;
	return true;
}

void UpdateSaitama()
{
	//check for right arrow
	if (Key_Down(DIK_D))
	{
		saitama.movex = saitama_move;
		saitama.movey = 0;
		saitama.frame = 1;
		saitama.x += saitama.movex; // only move when key pressed
		saitama.y += saitama.movey;
	}
	//check for up arrow
	else if (Key_Down(DIK_W))
	{
		saitama.movex = 0;
		saitama.movey = -saitama_move;
		saitama.frame = 0;
		saitama.x += saitama.movex;
		saitama.y += saitama.movey;
	}
	//check for down arrow
	else if (Key_Down(DIK_S))
	{
		saitama.movex = 0;
		saitama.movey = saitama_move;
		saitama.frame = 3;
		saitama.x += saitama.movex;
		saitama.y += saitama.movey;
	}
	//check for left arrow
	else if (Key_Down(DIK_A))
	{
		saitama.movex = -saitama_move;
		saitama.movey = 0;
		saitama.frame = 2;
		saitama.x += saitama.movex;
		saitama.y += saitama.movey;
	}
	if (saitama.x < 0)
		saitama.x = 0;
	if (saitama.x > SCREENWIDTH - saitama.width)
		saitama.x = SCREENWIDTH - saitama.width;
	if (saitama.y < 0)
		saitama.y = 0;
	if (saitama.y > SCREENHEIGHT - saitama.height)
		saitama.y = SCREENHEIGHT - saitama.height;
	for (int i = 0; i < cellrows; i++)
		for (int j = 0; j < cellcolumns; j++)
		{
			// if hit stone, saitama backward to its previous position
			if (cell[i][j] == 1 && Collision(saitama, stonecell[i][j]))

			{
				saitama.x -= saitama.movex;
				saitama.y -= saitama.movey;
			}
}



	if (Collision(saitama, boros)) {
		saitama.x -= saitama.movex;
		saitama.y -= saitama.movey;
	}

	for (int i = 0; i < 5; i++) {
		if (Collision(saitama, powerupcell[i])) {
			valid_powerup[i] = 0;
			saitama_move = powerup_move;
			seriousPunch += powerup[i];
			powerup[i] = 0;
		}
	}
	if (Collision(saitama, heart)) {
		valid_heart = 0;
		saitamaLife += heal;
		heal = 0;
	}
	if (seriousPunch == 0) {
		saitama_move = default_move;
	}

}


void DrawSaitamaText(HWND hwnd)
{
	HDC hdc = GetDC(hwnd);

	RECT rt;
	GetClientRect(hwnd, &rt);

	char s[80];
	sprintf_s(s, "Saitama Life left = %d", saitamaLife);

	SetBkColor(hdc, RGB(0, 0, 0));
	SetTextColor(hdc, RGB(255, 255, 255));
	DrawText(hdc, s, strlen(s), &rt, DT_LEFT);

	ReleaseDC(hwnd, hdc);
}





bool LoadBoros()
{
	D3DXIMAGE_INFO info;
	HRESULT result;

	//load the boros sprite
	boros_img = LoadTexture("boros2.bmp", D3DCOLOR_XRGB(255, 0, 255));
	if (boros_img == NULL)
		return false;
	//set the boros's properties
	result = D3DXGetImageInfoFromFile("boros2.bmp", &info);
	if (result != D3D_OK)
		return false;

	boros.width = info.Width;
	boros.height = info.Height/4;
	boros.x = (SCREENWIDTH - boros.width);
	boros.y = (SCREENHEIGHT - boros.height) /2;
	boros.movex = 0; boros.movey = -1;
	boros.endframe = 0;

	return true;
}

void UpdateBoros()
{
	//check for right arrow
	if (Key_Down(DIK_RIGHT))
	{
		boros.movex = boros_move;
		boros.movey = 0;
		boros.frame = 1;
		boros.x += boros.movex; // only move when key pressed
		boros.y += boros.movey;
	}
	//check for up arrow
	else if (Key_Down(DIK_UP))
	{
		boros.movex = 0;
		boros.movey = -boros_move;
		boros.frame = 0;
		boros.x += boros.movex;
		boros.y += boros.movey;
	}
	//check for down arrow
	else if (Key_Down(DIK_DOWN))
	{
		boros.movex = 0;
		boros.movey = boros_move;
		boros.frame = 3;
		boros.x += boros.movex;
		boros.y += boros.movey;
	}
	//check for left arrow
	else if (Key_Down(DIK_LEFT))
	{
		boros.movex = -boros_move;
		boros.movey = 0;
		boros.frame = 2;
		boros.x += boros.movex;
		boros.y += boros.movey;
	}

	if (boros.x < 0)
		boros.x = 0;
	if (boros.x > SCREENWIDTH - boros.width)
		boros.x = SCREENWIDTH - boros.width;
	if (boros.y < 0)
		boros.y = 0;
	if (boros.y > SCREENHEIGHT - boros.height)
		boros.y = SCREENHEIGHT - boros.height;
	for (int i = 0; i < cellrows; i++)
		for (int j = 0; j < cellcolumns; j++)
		{
			// if hit stone, saitama backward to its previous position
			if (cell[i][j] == 1 && Collision(boros, stonecell[i][j]))

			{
				boros.x -= boros.movex;
				boros.y -= boros.movey;
			}
		}

	if (Collision(saitama, boros)) {
		boros.x -= boros.movex;
		boros.y -= boros.movey;
	}

	for (int i = 0; i < 5; i++) {
		if (Collision(boros, powerupcell[i])) {
			valid_powerup[i] = 0;
			boros_move = powerup_move;
			roaringCannon += powerup[i];
			powerup[i] = 0;
		}
	}
	if (Collision(boros, heart)) {
		valid_heart = 0;
		borosLife += heal;
		heal = 0;
	}
	if (roaringCannon == 0) {
		boros_move = default_move;
	}
}





void DrawBorosText(HWND hwnd)
{
	HDC hdc = GetDC(hwnd);

	RECT rt;
	GetClientRect(hwnd, &rt);

	char s[80];
	sprintf_s(s, "Boros Life left = %d", borosLife);

		SetBkColor(hdc, RGB(0, 0, 0));
	SetTextColor(hdc, RGB(255, 255, 255));
	DrawText(hdc, s, strlen(s), &rt, DT_RIGHT);

	ReleaseDC(hwnd, hdc);
}

int increment = 2;
bool LoadPowerUp()
{
	D3DXIMAGE_INFO info;
	HRESULT result;
	//load the powerup image
	power_img = LoadTexture("powerup.bmp", D3DCOLOR_XRGB(255, 0, 255));
	if (power_img == NULL)
		return false;
	result = D3DXGetImageInfoFromFile("powerup.bmp", &info);
	if (result != D3D_OK)   return false;
	//set the powerup's properties
	for (int i = 0; i < 5; i++) {
		powerupcell[i].width = info.Width;
		powerupcell[i].height = info.Height;
		powerupcell[i].x = (rand()*increment) % SCREENWIDTH;
		powerupcell[i].y = (rand()*increment) % SCREENHEIGHT;
		increment=increment+2;
		if (powerupcell[i].x > SCREENWIDTH || powerupcell[i].x < SCREENWIDTH) {
			powerupcell[i].x = powerupcell[i].x + 1;
		}
		if (powerupcell[i].y > SCREENHEIGHT || powerupcell[i].y < SCREENHEIGHT) {
			powerupcell[i].y = powerupcell[i].y + 1;
		}
		if (Collision(saitama, powerupcell[i]) || Collision(boros, powerupcell[i])) {
			powerupcell[i].x = powerupcell[i].x + 5;
			powerupcell[i].y = powerupcell[i].y + 5;
		}
		valid_powerup[i] = 1; powerupcell[i].movex = 0; powerupcell[i].movey = 0;
		powerup[i] = 1;
	}
	return true;
}




bool LoadHeart()
{
	D3DXIMAGE_INFO info;
	HRESULT result;

	//load the heart image
	heart_img = LoadTexture("heart.bmp", D3DCOLOR_XRGB(255, 0, 255));
	if (heart_img == NULL)
		return false;
	result = D3DXGetImageInfoFromFile("heart.bmp", &info);
	if (result != D3D_OK)   return false;

	//set the heart's properties
	heart.width = info.Width;
	heart.height = info.Height;
	heart.x = (SCREENWIDTH - heart.width)/2;
	heart.y = (SCREENHEIGHT - heart.height) / 2;
	heart.movex = 0; heart.movey = -1;
	heart.endframe = 0;
	return true;
}

bool LoadSaitamaPunch() {
	D3DXIMAGE_INFO info;
	HRESULT result;

	//load the punch sprite
	saitamapunch_img = LoadTexture("punch.bmp",
		D3DCOLOR_XRGB(255, 0, 255));
	if (saitamapunch_img == NULL)      return false;

	//set the punch's properties
	result = D3DXGetImageInfoFromFile("punch.bmp", &info);
	if (result != D3D_OK)        return false;;
	for (int i = 0; i < SAITAMAPUNCH_NUMBERS; i++)
	{
		saitamapunch[i].width = info.Width;
		saitamapunch[i].height = info.Height;
	}

	// initialize all punch status to be invalid
	for (int i = 0; i < SAITAMAPUNCH_NUMBERS; i++) {
		valid_saitamapunch[i] = 0;
		saitama_damage[i] = 10;
	}
	return true;

}

bool LoadBorosPunch() {
	D3DXIMAGE_INFO info;
	HRESULT result;

	//load the punch sprite
	borospunch_img = LoadTexture("punch.bmp",
		D3DCOLOR_XRGB(255, 0, 255));
	if (borospunch_img == NULL)      return false;

	//set the punch's properties
	result = D3DXGetImageInfoFromFile("punch.bmp", &info);
	if (result != D3D_OK)        return false;;
	for (int i = 0; i < BOROSPUNCH_NUMBERS; i++)
	{
		borospunch[i].width = info.Width;
		borospunch[i].height = info.Height;
	}

	// initialize all punch status to be invalid
	for (int i = 0; i < BOROSPUNCH_NUMBERS; i++) {
		valid_borospunch[i] = 0;
		boros_damage[i] = 10;
	}
	return true;

}


bool LoadSeriousPunch() {
	D3DXIMAGE_INFO info;
	HRESULT result;

	//load the seriouspunch sprite
	seriouspunch_img = LoadTexture("seriouspunch.bmp",
		D3DCOLOR_XRGB(255, 0, 255));
	if (seriouspunch_img == NULL)      return false;

	//set the seriouspunch's properties
	result = D3DXGetImageInfoFromFile("seriouspunch.bmp", &info);
	if (result != D3D_OK)        return false;;
	for (int i = 0; i < SERIOUSPUNCH_NUMBERS; i++)
	{
		seriouspunch[i].width = info.Width;
		seriouspunch[i].height = info.Height;
	}

	// initialize all seriouspunch status to be invalid
	for (int i = 0; i < SERIOUSPUNCH_NUMBERS; i++) {
		valid_seriouspunch[i] = 0;
		saitama_powerdamage[i] = 100;
	}
	return true;

}

bool LoadRoaringCannon() {
	D3DXIMAGE_INFO info;
	HRESULT result;

	//load the roaringcannon sprite
	roaringcannon_img = LoadTexture("roaringcannon2.bmp",
		D3DCOLOR_XRGB(255, 0, 255));
	if (roaringcannon_img == NULL)      return false;

	//set the roaringcannon's properties
	result = D3DXGetImageInfoFromFile("roaringcannon2.bmp", &info);
	if (result != D3D_OK)        return false;;
	for (int i = 0; i < ROARINGCANNON_NUMBERS; i++)
	{
		roaringcannon[i].width = info.Width;
		roaringcannon[i].height = info.Height;
	}

	// initialize all roaringcannon status to be invalid
	for (int i = 0; i < ROARINGCANNON_NUMBERS; i++) {
		valid_roaringcannon[i] = 0;
		boros_powerdamage[i] = 100;
	}
	return true;

}


void UpdatePlayerAttacks()
{
	for (int i = 0; i < SAITAMAPUNCH_NUMBERS; i++)
	{
		// update punch positions
		saitamapunch[i].x += saitamapunch[i].movex;
		saitamapunch[i].y += saitamapunch[i].movey;
		// if punch move out of screen, that punch turns to be invalid
		if (saitamapunch[i].x < -24 || saitamapunch[i].x > (SCREENWIDTH - saitamapunch[i].width)+24)
			valid_saitamapunch[i] = 0;
		if (saitamapunch[i].y < -24 || saitamapunch[i].y > (SCREENHEIGHT - saitamapunch[i].height)+24)
			valid_saitamapunch[i] = 0;
	}
	for (int i = 0; i < BOROSPUNCH_NUMBERS; i++)
	{
		// update boros punch positions
		borospunch[i].x += borospunch[i].movex;
		borospunch[i].y += borospunch[i].movey;
		// if punch move out of screen, that punch turns to be invalid
		if (borospunch[i].x < -24 || borospunch[i].x > (SCREENWIDTH - borospunch[i].width)+24)
			valid_borospunch[i] = 0;
		if (borospunch[i].y < -24 || borospunch[i].y > (SCREENHEIGHT - borospunch[i].height)+24)
			valid_borospunch[i] = 0;
	}

	for (int i = 0; i < SERIOUSPUNCH_NUMBERS; i++)
	{
		// update seriouspunch positions
		seriouspunch[i].x += seriouspunch[i].movex;
		seriouspunch[i].y += seriouspunch[i].movey;

		// if seriouspunch move out of screen, that seriouspunch turns to be invalid
		if (seriouspunch[i].x < -72 || seriouspunch[i].x > (SCREENWIDTH - seriouspunch[i].width)+72)
			valid_seriouspunch[i] = 0;

		if (seriouspunch[i].y < -72 || seriouspunch[i].y > (SCREENHEIGHT - seriouspunch[i].height)+72)
			valid_seriouspunch[i] = 0;
	}

	for (int i = 0; i < ROARINGCANNON_NUMBERS; i++)
	{
		// update roaringcannon positions
		roaringcannon[i].x += roaringcannon[i].movex;
		roaringcannon[i].y += roaringcannon[i].movey;

		// if roaringcannon move out of screen, that roaringcannon turns to be invalid
		if (roaringcannon[i].x < -72 || roaringcannon[i].x > (SCREENWIDTH - roaringcannon[i].width)+72)
			valid_roaringcannon[i] = 0;

		if (roaringcannon[i].y < -72 || roaringcannon[i].y > (SCREENHEIGHT - roaringcannon[i].height)+72)
			valid_roaringcannon[i] = 0;
	}

	if (Key_Down(DIK_SPACE) && GetTickCount() - start_saitamapunch > 500)
	{
		
		PlaySound(sound_punch);
		start_saitamapunch = GetTickCount();
		last_saitamapunch++;   // fire a new punch
		if (last_saitamapunch >= SAITAMAPUNCH_NUMBERS)
			last_saitamapunch = 0;   // use a circular array
		valid_saitamapunch[last_saitamapunch] = 1;
		saitama_damage[last_saitamapunch] = 10;
		
		saitamapunch[last_saitamapunch].x = saitama.x;
		saitamapunch[last_saitamapunch].y = saitama.y;
		saitamapunch[last_saitamapunch].movex = saitama.movex * 5;
		saitamapunch[last_saitamapunch].movey = saitama.movey * 5;
		saitamapunch[last_saitamapunch].x += saitama.movex;
		saitamapunch[last_saitamapunch].y += saitama.movey;

	}

	for (int k = 0; k < SAITAMAPUNCH_NUMBERS; k++)
	{
		// if valid punch and valid enemy tank hit together
		if (Collision(saitamapunch[k], boros)&&valid_saitamapunch[k]) {
			valid_saitamapunch[k] = 0;
			borosLife = borosLife - saitama_damage[k];
			saitama_damage[k] = 0;
			if (borosLife <= 0 && !valid_die) {
				valid_die = 1; borosloser = 1;		
			}
		}
	}

	for (int i = 0; i < cellrows; i++)
		for (int j = 0; j < cellcolumns; j++)
			for (int k = 0; k < SAITAMAPUNCH_NUMBERS; k++)
			{
				if (cell[i][j] == 1 && valid_saitamapunch[k] && Collision(stonecell[i][j], saitamapunch[k]))
				{
					// punch hits stone and both turns invalid
					cell[i][j] = 0;
					saitama_damage[k] = 0;
					valid_saitamapunch[k] = 0;
				}
			}


	if (Key_Down(DIK_Z) && GetTickCount() - start_seriouspunch > 500) {

		if (seriousPunch >= 1) {
			seriousPunch--;

			start_seriouspunch = GetTickCount();
			last_seriouspunch++;   // fire a new punch
			if (last_seriouspunch >= SERIOUSPUNCH_NUMBERS)
				last_seriouspunch = 0;   // use a circular array
			valid_seriouspunch[last_seriouspunch] = 1;
			saitama_powerdamage[last_seriouspunch] = 100;
			seriouspunch[last_seriouspunch].x = saitama.x-24;
			seriouspunch[last_seriouspunch].y = saitama.y-24;
			seriouspunch[last_seriouspunch].movex = saitama.movex * 5;
			seriouspunch[last_seriouspunch].movey = saitama.movey * 5;
			seriouspunch[last_seriouspunch].x += saitama.movex ;
			seriouspunch[last_seriouspunch].y += saitama.movey;

		}

	}

	for (int i = 0; i < SERIOUSPUNCH_NUMBERS; i++)
	{
		if (Collision(seriouspunch[i], boros)&&valid_seriouspunch[i]) {
			borosLife = borosLife - saitama_powerdamage[i];
			saitama_powerdamage[i] = 0;
			valid_seriouspunch[i] = 0;
			if (borosLife <= 0 && !valid_die ) {
				valid_die = 1; borosloser = 1;
			}
		}
		for (int i = 0; i < cellrows; i++)
			for (int j = 0; j < cellcolumns; j++)
				for (int k = 0; k < SERIOUSPUNCH_NUMBERS; k++)
				{
					if (cell[i][j] == 1 && valid_seriouspunch[k] && Collision(stonecell[i][j], seriouspunch[k]))
					{
						// serious punch hits stone and continues rampaging
						cell[i][j] = 0;
						PlaySound(sound_seriouspunch);
					}
				}
	}


	if (Key_Down(DIK_9) && GetTickCount() - start_borospunch > 500)
	{
		PlaySound(sound_punch);
		start_borospunch = GetTickCount();
		last_borospunch++;   // fire a new punch
		if (last_borospunch >= BOROSPUNCH_NUMBERS)
			last_borospunch = 0;   // use a circular array
		valid_borospunch[last_borospunch] = 1;
		boros_damage[last_borospunch] = 10;
		borospunch[last_borospunch].x = boros.x ;
		borospunch[last_borospunch].y = boros.y +4;
		borospunch[last_borospunch].movex = boros.movex * 5;
		borospunch[last_borospunch].movey = boros.movey * 5;
		borospunch[last_borospunch].x += boros.movex ;
		borospunch[last_borospunch].y += boros.movey ;

	}


	for (int i = 0; i < BOROSPUNCH_NUMBERS; i++)
	{
		// if valid punch and valid enemy tank hit together
		if (Collision(borospunch[i], saitama)&&valid_borospunch[i]) {
			saitamaLife = saitamaLife - boros_damage[i];
			boros_damage[i] = 0;
			valid_borospunch[i] = 0;
			if (saitamaLife <= 0 && !valid_die) {
				valid_die = 1;			saitamaloser = 1;	
			}
		}

		for (int i = 0; i < cellrows; i++)
			for (int j = 0; j < cellcolumns; j++)
				for (int k = 0; k < BOROSPUNCH_NUMBERS; k++)
				{
					if (cell[i][j] == 1 && valid_borospunch[k] && Collision(stonecell[i][j], borospunch[k]))
					{
						// boros punch hits stone and both turns invalid
						cell[i][j] = 0;
						valid_borospunch[k] = 0;
						boros_damage[k] = 0;
					
					}
				}
	}


	if (Key_Down(DIK_8) && GetTickCount() - start_roaringcannon > 500) {
		if (roaringCannon >= 1) {
			roaringCannon--;

			start_roaringcannon = GetTickCount();
			last_roaringcannon++;   // fire a new punch
			if (last_roaringcannon >= ROARINGCANNON_NUMBERS)
				last_roaringcannon = 0;   // use a circular array
			valid_roaringcannon[last_roaringcannon] = 1;
			boros_powerdamage[last_roaringcannon] = 100;
			roaringcannon[last_roaringcannon].x = boros.x-24;
			roaringcannon[last_roaringcannon].y = boros.y-24;
			roaringcannon[last_roaringcannon].movex = boros.movex * 5;
			roaringcannon[last_roaringcannon].movey = boros.movey * 5;
			roaringcannon[last_roaringcannon].x += boros.movex ;
			roaringcannon[last_roaringcannon].y += boros.movey ;
		}
	}




	for (int i = 0; i < ROARINGCANNON_NUMBERS; i++)
	{
		if (Collision(roaringcannon[i], saitama)&&valid_roaringcannon[i]) {
			saitamaLife = saitamaLife - boros_powerdamage[i];
			boros_powerdamage[i] = 0;
			valid_roaringcannon[i] = 0;
			if (saitamaLife <= 0 && !valid_die) {
				valid_die = 1;			saitamaloser = 1;
			
			}
		}

		for (int i = 0; i < cellrows; i++)
			for (int j = 0; j < cellcolumns; j++)
				for (int k = 0; k < ROARINGCANNON_NUMBERS; k++)
				{
					if (cell[i][j] == 1 && valid_roaringcannon[k] && Collision(stonecell[i][j], roaringcannon[k]))
					{
						// roaring cannon hits stone and continues rampaging
						cell[i][j] = 0;
						PlaySound(sound_roaringcannon);
					}
				}

	}


}//End of update attack


bool LoadStones()
{
	D3DXIMAGE_INFO info;
	HRESULT result;

	//load the stone image
	stone_img = LoadTexture("stone.bmp", D3DCOLOR_XRGB(255, 0, 255));
	if (stone_img == NULL)
		return false;
	//set the stone's properties
	for (int i = 0; i < cellrows; i++)
		for (int j = 0; j < cellcolumns; j++)
		{
			cell[i][j] = 0;
			stonecell[i][j].width = cellwidth;
			stonecell[i][j].height = cellheight;
			stonecell[i][j].x = j * cellwidth;
			stonecell[i][j].y = i * cellheight;
			stonecell[i][j].movex = 0;
			stonecell[i][j].movey = 0;
		}

	// randomly build 100 stone cells
	for (int i = 0; i < 100; i++) {
		cell[rand() % cellrows][rand() % cellcolumns] = 1;
	}

	for (int i = 0; i < cellrows; i++)
		for (int j = 0; j < cellcolumns; j++)
		{
			for (int k = 0; k < 5; k++) {
				if (Collision(stonecell[i][j], saitama) ||
					Collision(stonecell[i][j], boros) ||
					Collision(stonecell[i][j], heart)||
					Collision(stonecell[i][j], powerupcell[k])) {
					cell[i][j] = 0;
				}}}
	return true;
}




bool Game_Init(HWND hwnd)
{
	Direct3D_Init(hwnd, SCREENWIDTH, SCREENHEIGHT, false);
	DirectInput_Init(hwnd);
	DirectSound_Init(hwnd);
	back_img = LoadSurface("background.bmp");
	LoadSaitama();
	LoadBoros();
	LoadPowerUp();
	LoadHeart();
	LoadSaitamaPunch();
	LoadBorosPunch();
	LoadSeriousPunch();
	LoadRoaringCannon();
	LoadStones();

	//load sound files
	sound_seriouspunch = LoadSound("seriouspunchsound.wav");
	if (sound_seriouspunch == NULL)   return 0;
	sound_roaringcannon = LoadSound("roaringcannonsound.wav");
	if (sound_roaringcannon == NULL)   return 0;
	sound_punch = LoadSound("punch.wav");
	if (sound_punch == NULL)   return 0;
	return true;
}

void Game_Run(HWND hwnd)
{
	if (!d3ddev) return;
	DirectInput_Update();
	
	if (GetTickCount() - starttime >= 30)
	{
		//reset timing
		starttime = GetTickCount();
		UpdateSaitama();
		UpdateBoros();
		UpdatePlayerAttacks();
		UpdateLoser(hwnd);		
	}

	DrawSaitamaText(hwnd);
	DrawBorosText(hwnd);
	if (d3ddev->BeginScene())
	{
		DrawSurface(backbuffer, 0, 0, back_img);
		sprite_obj->Begin(D3DXSPRITE_ALPHABLEND);
		
		if (saitamaloser == 0) {
			Sprite_Draw_Frame(saitama_img, saitama.x, saitama.y, saitama.frame,
				saitama.width, saitama.height, saitama.columns);	
		}
		if (borosloser == 0) {
			Sprite_Draw_Frame(boros_img, boros.x, boros.y, boros.frame,
				boros.width, boros.height, boros.columns);
		}
		
		for (int i = 0; i < 5; i++) {
			if (valid_powerup[i]) {
				Sprite_Draw_Frame(power_img,
					powerupcell[i].x, powerupcell[i].y,
					powerupcell[i].frame, powerupcell[i].width,
					powerupcell[i].height, powerupcell[i].columns);
			}
		}
		if (valid_heart) {
			Sprite_Draw_Frame(heart_img,
				heart.x, heart.y,
				heart.frame, heart.width,
				heart.height, heart.columns);
		}

		for (int i = 0; i < SAITAMAPUNCH_NUMBERS; i++) {
			if (valid_saitamapunch[i]) {
				Sprite_Draw_Frame(saitamapunch_img, saitamapunch[i].x, saitamapunch[i].y,
					saitamapunch[i].frame, saitamapunch[i].width, saitamapunch[i].height,
					saitamapunch[i].columns);
			}
		}

		for (int i = 0; i < BOROSPUNCH_NUMBERS; i++) {
			if (valid_borospunch[i]) {
				Sprite_Draw_Frame(borospunch_img, borospunch[i].x, borospunch[i].y,
					borospunch[i].frame, borospunch[i].width, borospunch[i].height,
					borospunch[i].columns);
			}
		}

		for (int i = 0; i < SERIOUSPUNCH_NUMBERS; i++) {
			if (valid_seriouspunch[i]) {
				Sprite_Draw_Frame(seriouspunch_img, seriouspunch[i].x, seriouspunch[i].y,
					seriouspunch[i].frame, seriouspunch[i].width, seriouspunch[i].height,
					seriouspunch[i].columns);
			}

		}

		for (int i = 0; i < ROARINGCANNON_NUMBERS; i++) {
			if (valid_roaringcannon[i]) {
				Sprite_Draw_Frame(roaringcannon_img, roaringcannon[i].x, roaringcannon[i].y,
					roaringcannon[i].frame, roaringcannon[i].width, roaringcannon[i].height,
					roaringcannon[i].columns);
			}
		}

		for (int i = 0; i < cellrows; i++)
			for (int j = 0; j < cellcolumns; j++)
				if (cell[i][j] == 1)
					Sprite_Draw_Frame(stone_img,
						stonecell[i][j].x, stonecell[i][j].y,
						stonecell[i][j].frame, stonecell[i][j].width,
						stonecell[i][j].height, stonecell[i][j].columns);



		sprite_obj->End();
		d3ddev->EndScene();
	}

	d3ddev->Present(NULL, NULL, NULL, NULL);
}

void Game_End() {

	DirectSound_Shutdown();
	DirectInput_Shutdown();
	Direct3D_Shutdown();

}
