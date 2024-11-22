#ifndef _DXRender_
#define _DXRender_

#include "Constants.h"

#include "Camara.h"
#include "Light.h"
#include "MeshPrimitives.h"

#include "GameMath.h"
#include "GameUiC.h"
#include <colision_math.h>




float x= 1;
float z =1;
float v = 1;

float xm, zm;

float xv, zv;

float camx, camz;

class DXRender
{
private:
	//Global Declarations//|
	IDXGISwapChain* SwapChain;
	ID3D11Device* d3d11Device;
	ID3D11DeviceContext* d3d11DevCon;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11DepthStencilView* depthStencilView;

	IDirectInputDevice8* DIKeyboard;
	IDirectInputDevice8* DIMouse;
	DIMOUSESTATE mouseLastState;
	LPDIRECTINPUT8 DirectInput;

	D3DXMATRIX View;
	D3DXMATRIX Projection;


	MPrimitives *cubo;
	MPrimitives *esfera;
	MPrimitives *toroide;
	MPrimitives *cono;
	MPrimitives *cilindro;
	MPrimitives* edificio;
	MPrimitives* barrier;
	MPrimitives* Bike;
	MPrimitives* Edificio1;
	MPrimitives* leaves;
	MPrimitives* trunk;
	MPrimitives* racefence;
	MPrimitives* rubble;
	MPrimitives* sandbag;
	MPrimitives* startline;
	MPrimitives *billboard;
	MPrimitives *skydomo;
	MPrimitives *terreno;
	MPrimitives *Bitmap;
	MPrimitives *BoundingSphere1;
	MPrimitives *BoundingSphere2;

	//JUEGO MODELOS
	MPrimitives* gokart;

	Camara *camara;
	Camara *skyCamara;

	float rotPerFrame;
	float sphereSpeed;
	D3DXVECTOR2 BoundingSphereTranslation;
	D3DXVECTOR2 DummyBoundyEvents;
	D3DXVECTOR2 LastBoundingPosition;

	//Variables personales.
	RaceCoords racecoords;
	rectangle_colision colision1;
	rectangle_colision colision2;
	float PC_multiply = 100.0;

	float angleb;
	float velocity = 300 * PC_multiply;
	float camYaw = 0;

	float aceleration = 0.000005f*PC_multiply;
	float deceleration = 0.00001f * PC_multiply;
	float top_speed = 0.02f * PC_multiply;
	float velocity_car = 0.0f * PC_multiply;

	float speed = 0.02f * (PC_multiply/10);
	float wheeling = .002f * (PC_multiply / 10);


public:
	DXRender()
	{
		rotPerFrame = 0.0f;
	}

	bool InitDevice(HWND hwnd, int Width, int Height)
	{
		//Describe our Buffer
		DXGI_MODE_DESC bufferDesc;

		ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

		bufferDesc.Width = Width;
		bufferDesc.Height = Height;
		bufferDesc.RefreshRate.Numerator = 60;
		bufferDesc.RefreshRate.Denominator = 1;
		bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		//Describe our SwapChain
		DXGI_SWAP_CHAIN_DESC swapChainDesc;

		ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

		swapChainDesc.BufferDesc = bufferDesc;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.OutputWindow = hwnd;
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		//Create our SwapChain
		D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
			D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &d3d11Device, NULL, &d3d11DevCon);

		//Create our BackBuffer
		ID3D11Texture2D* BackBuffer;
		SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);

		//Create our Render Target
		d3d11Device->CreateRenderTargetView(BackBuffer, NULL, &renderTargetView);
		BackBuffer->Release();

		//Crea la descripcion del raster, el cual determina como se dibujaran los poligonos
		D3D11_RASTERIZER_DESC rasterDesc;
		ID3D11RasterizerState* rasterState;
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D11_CULL_NONE;//se puede manipular para transparencias, cambiar a D3D11_CULL_NONE;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0;

		d3d11Device->CreateRasterizerState(&rasterDesc, &rasterState);
		d3d11DevCon->RSSetState(rasterState);

		
		//Describe our Depth/Stencil Buffer
		D3D11_TEXTURE2D_DESC depthStencilDesc;
		ID3D11Texture2D* depthStencilBuffer;

		depthStencilDesc.Width     = Width;
		depthStencilDesc.Height    = Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count   = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0; 
		depthStencilDesc.MiscFlags      = 0;

		//Create the Depth/Stencil View
		d3d11Device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
		d3d11Device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);

		//Set our Render Target
		d3d11DevCon->OMSetRenderTargets( 1, &renderTargetView, depthStencilView );



		cubo = new MPrimitives(d3d11Device, d3d11DevCon, "Texturas/cube.txt", L"Texturas/Edificio2/edificio2.jpg", L"Texturas/Edificio2/edificio2.jpg", ShaderDiffuse);
		esfera = new MPrimitives(d3d11Device, d3d11DevCon, 32, 32, 1.0f, L"Texturas/escudo.jpg", L"Texturas/escudoNormal.jpg", ShaderDiffuse);
		toroide = new MPrimitives(d3d11Device, d3d11DevCon, 32, 32, 1.0f, 0.5f, L"Texturas/escudo.jpg", L"Texturas/escudoNormal.jpg", ShaderDiffuse);
		cilindro = new MPrimitives(d3d11Device, d3d11DevCon, 16, 1.0f, 1.0f, 2.0f, L"Texturas/escudo.jpg", L"Texturas/escudoNormal.jpg", ShaderDiffuse);
		cono = new MPrimitives(d3d11Device, d3d11DevCon, 16, 0.0f, 1.0f, 2.0f, L"Texturas/escudo.jpg", L"Texturas/escudoNormal.jpg", ShaderDiffuse);
		//edificio = new MPrimitives(d3d11Device, d3d11DevCon, "Texturas/Edificio2/Edificio2.txt", L"Texturas/Edificio2/edificio2.jpg", L"Texturas/Edificio2/edificio2.jpg", ShaderDiffuse);
		
		edificio = new MPrimitives(d3d11Device, d3d11DevCon, "Texturas/Edificio2/Edificio2.txt", L"Texturas/Edificio2/edificio2.jpg", L"Texturas/Edificio2/edificio2.jpg", ShaderDiffuse);
		barrier = new MPrimitives(d3d11Device, d3d11DevCon, "Texturas/Barrier/Barrier.txt", L"Texturas/Barrier/barrier2.png", L"Texturas/Barrier/barrier.png", ShaderDiffuse);
		Bike = new MPrimitives(d3d11Device, d3d11DevCon, "Texturas/AE2.txt", L"Texturas/Bike/Bike.bmp", L"Texturas/Bike/Bike.bmp", ShaderDiffuse);
		Edificio1 = new MPrimitives(d3d11Device, d3d11DevCon, "Texturas/Edificio1/Edificio1.txt", L"Texturas/Edificio1/edificio1.jpg", L"Texturas/Edificio1/edificio1.jpg", ShaderDiffuse);
		leaves = new MPrimitives(d3d11Device, d3d11DevCon, "Texturas/maple-tree/leaves.txt", L"Texturas/maple-tree/sugar_maple_leaf.png", L"Texturas/maple-tree/sugar_maple_leaf.png", ShaderDiffuse);
		
		trunk = new MPrimitives(d3d11Device, d3d11DevCon, "Texturas/maple-tree/trunk.txt", L"Texturas/Sandbag/sandbag.bmp", L"Texturas/Sandbag/sandbag.bmp", ShaderDiffuse);
		racefence = new MPrimitives(d3d11Device, d3d11DevCon, "Texturas/roads.txt", L"Texturas/Edificio2/edificio2.jpg", L"Texturas/Edificio1/edificio1.jpg", ShaderDiffuse);
		rubble = new MPrimitives(d3d11Device, d3d11DevCon, "Texturas/Rubble/Rubble.txt", L"Texturas/Rubble/rubble.bmp", L"Texturas/Rubble/rubble.bmp", ShaderDiffuse);
		sandbag = new MPrimitives(d3d11Device, d3d11DevCon, "Texturas/Sandbag/Sandbag.txt", L"Texturas/Sandbag/sandbag.bmp", L"Texturas/Sandbag/sandbag.bmp", ShaderDiffuse);
		startline = new MPrimitives(d3d11Device, d3d11DevCon, "Texturas/starting-line/startline.txt", L"Texturas/Bike/Bike.bmp", L"Texturas/Bike/Bike.bmp", ShaderDiffuse);
		gokart = new MPrimitives(d3d11Device, d3d11DevCon, "Texturas/gokart.txt", L"Texturas/Edificio2/edificio2.jpg", L"Texturas/Edificio1/edificio1.jpg", ShaderDiffuse);

		billboard = new MPrimitives(d3d11Device, d3d11DevCon, 2.0f, 2.0f, L"Texturas/ave.png");
		
		skydomo = new MPrimitives(d3d11Device, d3d11DevCon, 32, 32, 1000.0f, L"Texturas/sky2.jpg", L"Texturas/escudoNormal.jpg", ShaderDiffuse);
		terreno = new MPrimitives(d3d11Device, d3d11DevCon, 100.0f, L"Texturas/teterr.jpg", L"Texturas/texterr.jpg", "Texturas/heightmap2.bmp", ShaderMultitextura);

		BoundingSphere1 = new MPrimitives(d3d11Device, d3d11DevCon, 32, 32, 1.0f, L"Texturas/escudo.jpg", L"Texturas/escudoNormal.jpg", ShaderDiffuse);
		BoundingSphere2 = new MPrimitives(d3d11Device, d3d11DevCon, 32, 32, 1.0f, L"Texturas/escudo.jpg", L"Texturas/escudoNormal.jpg", ShaderDiffuse);
		BoundingSphereTranslation = D3DXVECTOR2(0, 0);
		
		//Inicializar variables creadas por usuario
		InitObj_Matrix();
		gameui.initialize(hwnd, d3d11Device, d3d11DevCon);
		

		return true;
	}

	bool InitDirectInput(HINSTANCE hInstance, HWND hwnd)
	{
		DirectInput8Create(hInstance,
			DIRECTINPUT_VERSION,
			IID_IDirectInput8,
			(void**)&DirectInput,
			NULL); 

		DirectInput->CreateDevice(GUID_SysKeyboard,
			&DIKeyboard,
			NULL);

		DirectInput->CreateDevice(GUID_SysMouse,
			&DIMouse,
			NULL);

		DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
		DIKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

		DIMouse->SetDataFormat(&c_dfDIMouse);
		DIMouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

		return true;
	}

	void DetectInput()
	{
		DIMOUSESTATE mouseCurrState;

		BYTE keyboardState[256];

		DIKeyboard->Acquire();
		DIMouse->Acquire();

		DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);

		DIKeyboard->GetDeviceState(sizeof(keyboardState),(LPVOID)&keyboardState);

		

		

		float moveLeftRight = 0;
		float moveBackForward = 0;

		camYaw = 0;
		float camPitch = 0;

		bool car_mode = true;
		

		if(keyboardState[DIK_A])
		{
			if (car_mode) {
				camYaw -= wheeling;
			}
			else {
				moveLeftRight -= speed;
			}
			
		}
		if(keyboardState[DIK_D])
		{	
			if (car_mode) {
				camYaw += wheeling;
			}
			else {
				moveLeftRight += speed;
			}

		}
		if(keyboardState[DIK_W])

		{
			if (car_mode) {
				
				velocity_car += aceleration;
				clamp(velocity_car, 0, top_speed);

				moveBackForward += velocity_car;
			}
			else {

				moveBackForward += speed;
			}
		}

		else {
			if (car_mode){
				velocity_car -= deceleration;
				clamp(velocity_car, 0, top_speed);
				if (velocity_car > 0) {
					moveBackForward += velocity_car;
				}
			}
		}

		if(keyboardState[DIK_S])
		{
			if (car_mode) {

			}
			else {

				moveBackForward -= speed;
			}
		}
		if (keyboardState[DIK_LEFT])
		{
			DummyBoundyEvents.x = -.01f;
		}
		if (keyboardState[DIK_RIGHT])
		{
			DummyBoundyEvents.x = .01f;
		}
		if (keyboardState[DIK_UP])
		{
			DummyBoundyEvents.y = .01f;
		}
		if (keyboardState[DIK_DOWN])
		{
			DummyBoundyEvents.y = -.01f;
		}
		if (keyboardState[DIK_SPACE])
		{
			agregar_coordenadas(camara->Position.x - 50, camara->Position.z - 50);
		}

		if((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY))
		{
			float smoothness = 0.001f;
			camYaw = mouseCurrState.lX * smoothness;

			camPitch = mouseCurrState.lY * smoothness;

			mouseLastState = mouseCurrState;
		}

		camara->UpdateCamera(moveBackForward, moveLeftRight, camYaw, camPitch); 
		skyCamara->setSkyCamara(camara);
	}

	void ReleaseObjects()
	{
		//Release the COM Objects we created
		SwapChain->Release();
		d3d11Device->Release();
		d3d11DevCon->Release();
		renderTargetView->Release();
		depthStencilView->Release();
	}


	//Inicializael lugar donde estaran los objetos
	//aqui le dices donde va a estar el objeto.
	void InitObj_Matrix() {
		
		edificio->set_obj_v(-49, 5, -49);
		barrier->set_obj_v(23, 3, -20);
		Bike->set_obj_v(25, 11, 1);
		Edificio1->set_obj_v( -3, 3, 30);
		leaves->set_obj_v( -9, 3, -10);
		trunk->set_obj_v( -9, 6, -10);
		racefence->set_obj_v(60, 8, 10);
		rubble->set_obj_v( 25, 3, 10);
		sandbag->set_obj_v( 15, 3, 15);
		startline->set_obj_v( -6, 6, 10);
		cubo->set_obj_v(0, 0, 0);
		gokart->set_obj_v(0, 0, 0);

	}


	void init_colision() {

		colision1.init_rectangle(5, 5, 5);
		colision2.init_rectangle(5, 5, 5);


		colision1.set_position(Bike->get_obj_v());

		colision2.set_position(camara->Position);
	}
	

	bool InitScene(int Width, int Height)
	{
		//Create the Viewport
		D3D11_VIEWPORT viewport;
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = Width;
		viewport.Height = Height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		//set light values
		light.direction = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		light.ambient = D3DXVECTOR4(0.3f, 0.3f, 0.3f, 1.0f);
		light.diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);

		//Set the Viewport
		d3d11DevCon->RSSetViewports(1, &viewport);

		//Create the camera
		D3DXVECTOR3 Position = D3DXVECTOR3(50, 2.0f, 40.0f);
		D3DXVECTOR3 Target = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		camara = new Camara(Position,Target,Up, Width, Height);
		skyCamara = new Camara(Position,Target,Up, Width, Height);
		//esta es la gui
		Bitmap = new MPrimitives(d3d11Device, d3d11DevCon, 1, 1, L"Texturas/Health.png", camara);

		

		racecoords.rellenar();
		D3DXVECTOR2 a;
		a= racecoords.get_punto(2);
		a = a;

		init_colision();

		return true;
	}

	void UpdateScene()
	{
		
		

		gameui.program();
		LastBoundingPosition = BoundingSphereTranslation;
		BoundingSphereTranslation += DummyBoundyEvents;

	
		rotPerFrame += 0.1f;

		D3DXMATRIX rotMatY, northMat, westMat, eastMat, southMat, scale, rotMatX,MatAcomodo;
		
		

		D3DXMATRIX Bounding1, Bounding2, BoundinMatTranslation;
		D3DXMATRIX edificiopos, barrierpos, bikepos, edificio1pos, leavespos, trunkpos, fencepos, rubblepos, sandbagpos, startlinepos;

		//Variables del juego
		D3DXVECTOR3 gokartV;
		D3DXVECTOR3 camCoord;
		D3DXVECTOR2 CarMoveVec;

		D3DXMATRIX rotMatBike;
		D3DXMATRIX gokartM;

		
		colision1.return_translated_position();

		if (colision1.check_colision(colision2)) {
			
			velocity = 0;

			velocity = velocity;
		}
		else {
			velocity = 50;
		}


		//las coordenadas del jugador o la camara si se van a usar
		camCoord = D3DXVECTOR3(camara->Position.x - 50,15, camara->Position.z - 50);
		
		//las coordenadas del lugar hacia donde se va a mover el carro.
		//calcula la distancia de dos puntos y genera un vector hacia ese punto.
		CarMoveVec = D3DXVECTOR2(racecoords.get_selector_punto().x - Bike->get_obj_v().x, racecoords.get_selector_punto().y - Bike->get_obj_v().z);
		CarMoveVec = nomralizeVec2(CarMoveVec);

		//Ccambia de lugar o hacia donde debe de ir en los puntos de la pista
		racecoords.get_checkpoint(D3DXVECTOR2(Bike->get_obj_v().x, Bike->get_obj_v().z));


		CarMoveVec.x = CarMoveVec.x / 1000;
		CarMoveVec.y = CarMoveVec.y / 1000;

		//calcula el angulo hacia donde se mueve el carro.
		angleb = AngleofVectorT(CarMoveVec);
		//mueve el objeto hacia cierta direccion
		Bike->sum_obj_v(CarMoveVec.x * velocity, 0, CarMoveVec.y * velocity);
		gokart->set_obj_v(camCoord.x,10,camCoord.z);
		
		//rotar objetos
		D3DXMatrixRotationY(&rotMatBike, D3DXToRadian(angleb));
		D3DXMatrixRotationY(&rotMatY, D3DXToRadian(rotPerFrame));

		D3DXMatrixRotationY(&gokartM, camara->Yaw);
		



		D3DXMatrixTranslation(&MatAcomodo, 50, 3, 50);

		D3DXMatrixTranslation(&northMat, 0, 3, 0);
		D3DXMatrixTranslation(&southMat, 0, -3, 0);
		D3DXMatrixTranslation(&eastMat, 3, 0, 0);
		D3DXMatrixTranslation(&westMat, -3, 0, 0);
		D3DXMatrixTranslation(&Bounding1, 3, 6, 0);
		D3DXMatrixTranslation(&Bounding2, -3, 6, 0);

		D3DXMatrixTranslation(&BoundinMatTranslation, BoundingSphereTranslation.x, BoundingSphereTranslation.y, 0);
		D3DXMatrixScaling(&scale, 0.03f, 0.03f, 0.03f);




		

		cubo->UpdateScene(rotMatY * MatAcomodo);
		esfera->UpdateScene(rotMatY * northMat * MatAcomodo);
		toroide->UpdateScene(rotMatY * eastMat* MatAcomodo);
		cilindro->UpdateScene(rotMatY * eastMat * northMat* MatAcomodo);
		cono->UpdateScene(rotMatY * westMat* MatAcomodo);

		edificio->UpdateScene_Obj(MatAcomodo);
		barrier->UpdateScene_Obj(MatAcomodo);

		//carro
		Bike->UpdateScene_Obj(rotMatBike*MatAcomodo);

		Edificio1->UpdateScene_Obj(MatAcomodo);
		leaves->UpdateScene_Obj(MatAcomodo);
		trunk->UpdateScene_Obj(MatAcomodo);
		racefence->UpdateScene_Obj(MatAcomodo);
		rubble->UpdateScene_Obj(MatAcomodo);
		sandbag->UpdateScene_Obj(MatAcomodo);
		startline->UpdateScene_Obj(MatAcomodo);
		gokart->UpdateScene_Obj(gokartM *MatAcomodo);

		

		
		billboard->UpdateScene(westMat * northMat* MatAcomodo);
		

		D3DXMATRIX skyRotMat, terrMat;
		D3DXMatrixRotationX(&skyRotMat, D3DXToRadian(-90));
		skydomo->UpdateScene(skyRotMat);


		//D3DXMatrixTranslation(&terrMat, -50, 0, -50);
		D3DXMatrixIdentity(&terrMat);
		terreno->UpdateScene(terrMat);

		//camara->Position.y = terreno->getTerrainCollision(camara->getXZ()) +3;
		camara->Position.y = 30;


		//colisioes
		colision1.set_position(Bike->get_obj_v());
		colision2.set_position(camCoord);


		BoundingSphere1->UpdateScene(Bounding1 * MatAcomodo * BoundinMatTranslation);
		BoundingSphere2->UpdateScene(Bounding2* MatAcomodo);

		


		//if (CheckBoundingSphereCollision(BoundingSphere1->PrimitiveBoundings->Radius,
		//	BoundingSphere1->PrimitiveBoundings->PositionM, BoundingSphere1->getWorldMatrix(),
		//	BoundingSphere2->PrimitiveBoundings->Radius,
		//	BoundingSphere2->PrimitiveBoundings->PositionM, BoundingSphere2->getWorldMatrix())){

		//	BoundingSphereTranslation = LastBoundingPosition;
		//}

		

		


		DummyBoundyEvents.x = 0;
		DummyBoundyEvents.y = 0;
	}

	void DrawScene()
	{
		//Clear our backbuffer to the updated color
		D3DXCOLOR bgColor(0.0f, 0.0f, 0.25f, 1.0f);
		d3d11DevCon->ClearRenderTargetView(renderTargetView, bgColor);
		//Refresh the Depth/Stencil view
		d3d11DevCon->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
		
		cubo->DrawScene(camara);
		esfera->DrawScene(camara);
		toroide->DrawScene(camara);
		cilindro->DrawScene(camara);
		
		cono->DrawScene(camara);
		edificio->DrawScene(camara);
		barrier->DrawScene(camara);
		Bike->DrawScene(camara);
		Edificio1->DrawScene(camara);
		leaves->DrawScene(camara);
		trunk->DrawScene(camara);
		racefence->DrawScene(camara);
		rubble->DrawScene(camara);
		sandbag->DrawScene(camara);
		startline->DrawScene(camara);
		gokart->DrawScene(camara);
		//billboard->DrawScene(camara);

		

		terreno->DrawScene(camara);
		skydomo->DrawScene(skyCamara);

		BoundingSphere1->DrawScene(camara);
		BoundingSphere2->DrawScene(camara);

		Bitmap->DrawBitmap(camara);


		//Present the backbuffer to the screen
		SwapChain->Present(0, 0);
	}

	
	bool CheckBoundingSphereCollision(float firstSphereRadius,
		D3DXVECTOR3 firstSpherePos,
		D3DXMATRIX firstObjWorldSpace,
		float secondSphereRadius,
		D3DXVECTOR3 secondSpherePos,
		D3DXMATRIX secondObjWorldSpace)
	{
		//Declare local variables
		D3DXVECTOR3 world_1 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 world_2 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		float objectsDistance = 0.0f;

		//Transform the objects world space to objects REAL center in world space
		D3DXVec3TransformCoord(&world_1, &firstSpherePos, &firstObjWorldSpace);
		D3DXVec3TransformCoord(&world_2, &secondSpherePos, &secondObjWorldSpace);

		// Create Vector to verify distance
		world_1 -= world_2;

		//Get the distance between the two objects
		objectsDistance = D3DXVec3Length(&world_1);
		//objectsDistance = XMVectorGetX(XMVector3Length(world_1 - world_2));

		//If the distance between the two objects is less than the sum of their bounding spheres...
		if (objectsDistance <= (firstSphereRadius + secondSphereRadius))
			return true;

		//If the bounding spheres are not colliding, return false
		return false;

		
	}
	
	
};

#endif