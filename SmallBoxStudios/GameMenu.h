#ifndef GAME_MENU_H
#define GAME_MENU_H

#include "Camera.h"
#include "DxGraphics.h"
#include "LetterController.h"
#include "Input.h"
#include "ArcadeMode.h"
#include "SimulationMode.h"

//Holds Words that will represent options
struct Page
{
	vector<int> m_wordID;
	vector<bool> m_wordActive;

	XMFLOAT3 defaultPosition;
	bool current;
};

enum PAGENUM
{
	MAIN_PAGE,
	SETTINGS
};

class GameMenu
{
public:
	GameMenu();
	~GameMenu();

	//Load pointers to controllers
	void LoadHandlers(LetterController & _letterController, Input &_input, SimulationMode & _simMode, ArcadeMode & _arcademode, GameSystem & _gameSystem, ResourceManager& _resources);

	//Create pages 
	void LoadContent();

	void Update(float dt);
	void Render(DxGraphics* dx, Camera &_cam);

	bool FadeOut(int _pageID);
	bool FadeIn(int _pageID);
	vector<Page*> m_menuPages;

private:
	LetterController *p_letterC;
	Input			 *p_input;
	//Menu Pages
	
	//
	int m_currentWord;
	int m_currentPage;

	XMFLOAT4 m_sellectionColour;
	XMFLOAT4 m_defaultColour;
	SimulationMode * p_simMode;
	ArcadeMode * p_arcademode;
	GameSystem * p_gameSystem;
	ResourceManager* resources;
};

#endif