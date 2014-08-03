#include "GameMenu.h"


GameMenu::GameMenu()
{
	p_letterC = NULL;
	m_currentWord = 0;
	m_currentPage = 0;

	m_sellectionColour = XMFLOAT4(3, 192, 60, 0);
	m_defaultColour = XMFLOAT4(205, 92, 92, 0);
}

GameMenu::~GameMenu()
{

}

void GameMenu::LoadHandlers(LetterController &_letterController, Input &_input, SimulationMode & _simMode, ArcadeMode & _arcademode, GameSystem & _gameSystem, ResourceManager& _resources)
{
	p_gameSystem = &_gameSystem;
	resources = &_resources;
	p_simMode = &_simMode;
	p_arcademode = &_arcademode;
	p_letterC = &_letterController;
	p_input = &_input;
}

void GameMenu::LoadContent()
{
	//45 0 50
	m_menuPages.push_back(new Page()); //MAIN MENU PAGE
	float scale = 0.5f;
	float space = 0.5f;
	m_menuPages[m_menuPages.size() - 1]->m_wordID.push_back(p_letterC->CreateWord(XMFLOAT3(35, 2, 54), "SIMULATION", scale, space));
	m_menuPages[m_menuPages.size() - 1]->m_wordID.push_back(p_letterC->CreateWord(XMFLOAT3(35, 1, 54), "ARCADE", scale, space));
	m_menuPages[m_menuPages.size() - 1]->m_wordID.push_back(p_letterC->CreateWord(XMFLOAT3(35, 0, 54), "DESIGN", scale, space));
	m_menuPages[m_menuPages.size() - 1]->m_wordID.push_back(p_letterC->CreateWord(XMFLOAT3(35, -1, 54), "SETTINGS", scale, space));
	m_menuPages[m_menuPages.size() - 1]->m_wordID.push_back(p_letterC->CreateWord(XMFLOAT3(35, -2, 54), "EXIT", scale, space));


	m_menuPages.push_back(new Page()); //SETTINGS PAGE
	m_menuPages[m_menuPages.size() - 1]->m_wordID.push_back(p_letterC->CreateWord(XMFLOAT3(35, 2, 54), "RESOLUTION", scale, space));
	m_menuPages[m_menuPages.size() - 1]->m_wordID.push_back(p_letterC->CreateWord(XMFLOAT3(35, 1, 54), "MUSIC", scale, space));
	m_menuPages[m_menuPages.size() - 1]->m_wordID.push_back(p_letterC->CreateWord(XMFLOAT3(35, 0, 54), "CONTROLS", scale, space));
	m_menuPages[m_menuPages.size() - 1]->m_wordID.push_back(p_letterC->CreateWord(XMFLOAT3(35, -1, 54), "VSYNC", scale, space));
	m_menuPages[m_menuPages.size() - 1]->m_wordID.push_back(p_letterC->CreateWord(XMFLOAT3(35, -2, 54), "BACK", scale, space));


	while (!FadeOut(1))
	{
		FadeOut(1);
	}

	//turn off gravity for all menu letters
	for (unsigned int page = 0; page < m_menuPages.size(); page++)
	{
		for (unsigned int word = 0; word < m_menuPages[page]->m_wordID.size(); word++)
		{
			p_letterC->WordGravitySwitch(m_menuPages[page]->m_wordID[word], false);
			p_letterC->WordCollisionSwitch(m_menuPages[page]->m_wordID[word], false);
			p_letterC->SetWordColour(m_menuPages[page]->m_wordID[word], m_defaultColour);
			p_letterC->SetWordRotation(m_menuPages[page]->m_wordID[word], XMFLOAT3(0, 0, 0));

			m_menuPages[page]->m_wordActive.push_back(false);
		}
	}
}

void GameMenu::Update(float dt)
{

	p_letterC->SetWordColour(m_menuPages[m_currentPage]->m_wordID[m_currentWord], m_sellectionColour);


	if ((p_input->GetDirectInput()->GetKeyboardState(DIK_UPARROW)) &&
		(!p_input->GetDirectInput()->GetKeyboardPrevState(DIK_UPARROW)))
	{
		p_letterC->SetWordColour(m_menuPages[m_currentPage]->m_wordID[m_currentWord], m_defaultColour);

		m_currentWord--;
		if (m_currentWord < 0)
		{
			m_currentWord = m_menuPages[m_currentPage]->m_wordID.size() - 1;
		}
	}

	if ((p_input->GetDirectInput()->GetKeyboardState(DIK_DOWNARROW)) &&
		(!p_input->GetDirectInput()->GetKeyboardPrevState(DIK_DOWNARROW)))
	{
		p_letterC->SetWordColour(m_menuPages[m_currentPage]->m_wordID[m_currentWord], m_defaultColour);
		m_currentWord++;
		if ((unsigned)m_currentWord > m_menuPages[m_currentPage]->m_wordID.size() - 1)
		{
			m_currentWord = 0;
		}
	}

	if ((p_input->GetDirectInput()->GetKeyboardState(DIK_RETURN)) &&
		(!p_input->GetDirectInput()->GetKeyboardPrevState(DIK_RETURN)))
	{
		p_letterC->SetWordColour(m_menuPages[m_currentPage]->m_wordID[m_currentWord], m_defaultColour);
		m_menuPages[m_currentPage]->m_wordActive[m_currentWord] = true;

	}

	switch (m_currentPage)
	{
	case MAIN_PAGE:
	{
			  switch (m_currentWord)
			  {
			  case 0: //SIMUALTION
			  {
						  if (m_menuPages[m_currentPage]->m_wordActive[m_currentWord])
						  {
							  p_simMode->LoadContent(p_gameSystem->GetDX(), *resources, p_input);
							  m_menuPages[m_currentPage]->m_wordActive[m_currentWord] = false;

							  for (unsigned int pageId = 0; pageId < m_menuPages.size(); pageId++)
							  {
								  for (unsigned int wordId = 0; wordId < m_menuPages[pageId]->m_wordID.size(); wordId++)
								  {
									  p_letterC->WordEnableSwitch(m_menuPages[pageId]->m_wordID[wordId], false);
								  }
							  }
						  }
						break;
			  }

			  case 1: //ARCADE
			  {
						  if (m_menuPages[m_currentPage]->m_wordActive[m_currentWord])
						  {
							  p_arcademode->LoadContent(p_gameSystem->GetDX(), *resources, p_input, *p_gameSystem, *p_letterC);
							  m_menuPages[m_currentPage]->m_wordActive[m_currentWord] = false;

							  for (unsigned int pageId = 0; pageId < m_menuPages.size(); pageId++)
							  {
								  for (unsigned int wordId = 0; wordId < m_menuPages[pageId]->m_wordID.size(); wordId++)
								  {
									  p_letterC->WordEnableSwitch(m_menuPages[pageId]->m_wordID[wordId], false);
								  }
							  }
						  }
						break;
			  }

			  case 2: //DESIGN
			  {

						break;
			  }

			  case 3: //SETTINGS
			  {
						  if (m_menuPages[m_currentPage]->m_wordActive[m_currentWord])
						  {
							  if (FadeOut(m_currentPage))
							  {  
								  p_letterC->SetWordColour(m_menuPages[m_currentPage]->m_wordID[m_currentWord], m_defaultColour);
								  if (FadeIn(1))
								  {
									  m_currentPage++;
									  m_menuPages[0]->m_wordActive[m_currentWord] = false;
									  m_currentWord = 0;  
								  }
							  }
						  }
						break;
			  }

			  case 4: //EXIT
			  {
						  if (m_menuPages[m_currentPage]->m_wordActive[m_currentWord] == true)
						  {
							  p_input->SetQuit(true);
						  }
						  break;
			  }
			  default:
				  break;
			  }

			  break;
	}

	case SETTINGS:
	{
			  switch (m_currentWord)
			  {
			  case 0: //SIMUALTION
			  {
						  //VOID POINTER TO FUNCTIOn
						  break;
			  }

			  case 1: //ARCADE
			  {

						  break;
			  }

			  case 2: //DESIGN
			  {

						  break;
			  }

			  case 3: //SETTINGS
			  {
						  if (m_menuPages[m_currentPage]->m_wordActive[m_currentWord])
						  {
							  if (FadeOut(m_currentPage))
							  {
								  if (FadeIn(1))
								  {
									  p_letterC->SetWordColour(m_menuPages[m_currentPage]->m_wordID[m_currentWord], m_defaultColour);
									  m_currentPage++;
									  m_menuPages[0]->m_wordActive[m_currentWord] = false;
									  m_currentWord = 0;
								  }
							  }
						  }
						  break;
			  }

			  case 4: //Back
			  {
						  if (m_menuPages[m_currentPage]->m_wordActive[m_currentWord])
						  {
							  if (FadeOut(m_currentPage))
							  {
								  if (FadeIn(0))
								  {
									  m_currentPage--;
									  m_menuPages[1]->m_wordActive[m_currentWord] = false;
									  m_currentWord = 0;
								  }
							  }
						  }
						  break;
			  }
			  default:
				  break;
			  }
			  break;
	}

	default:
		break;
	}
}

void GameMenu::Render(DxGraphics* dx, Camera &_cam)
{

}

bool GameMenu::FadeOut(int _pageID)
{
	XMVECTOR Vgoal;
	XMVECTOR Vcurrent;
	for (unsigned int word = 0; word < m_menuPages[_pageID]->m_wordID.size(); word++)
	{
		XMFLOAT3 currentPos = p_letterC->GetWordPosition(m_menuPages[_pageID]->m_wordID[word]);
		XMFLOAT3 goalPos = XMFLOAT3(35, currentPos.y, 85);

		Vgoal = XMLoadFloat3(&goalPos);
		Vcurrent = XMLoadFloat3(&currentPos);
		Vcurrent = XMVectorLerp(Vcurrent, Vgoal, 0.12f * (1 + word));

		XMStoreFloat3(&currentPos, Vcurrent);
		p_letterC->SetWordPosition(m_menuPages[_pageID]->m_wordID[word], currentPos);
	}

	XMFLOAT3 currentPos = p_letterC->GetWordPosition(m_menuPages[_pageID]->m_wordID[0]);
	XMFLOAT3 goalPos = XMFLOAT3(35, currentPos.y, 85);
	Vgoal = XMLoadFloat3(&goalPos);
	Vcurrent = XMLoadFloat3(&currentPos);
	if (XMVector4NearEqual(Vcurrent, Vgoal, XMLoadFloat3(&XMFLOAT3(0.1f, 0.1f, 0.1f))))
	{
		return true;
	}
	return false;
}

bool GameMenu::FadeIn(int _pageID)
{
	XMVECTOR Vgoal;
	XMVECTOR Vcurrent;
	for (unsigned int word = 0; word < m_menuPages[_pageID]->m_wordID.size(); word++)
	{
		XMFLOAT3 currentPos = p_letterC->GetWordPosition(m_menuPages[_pageID]->m_wordID[word]);
		XMFLOAT3 goalPos = XMFLOAT3(35, currentPos.y, 54);


		Vgoal = XMLoadFloat3(&goalPos);
		Vcurrent = XMLoadFloat3(&currentPos);
		Vcurrent = XMVectorLerp(Vcurrent, Vgoal, 0.12f * (1 + word));

		XMStoreFloat3(&currentPos, Vcurrent);
		p_letterC->SetWordPosition(m_menuPages[_pageID]->m_wordID[word], currentPos);
	}

	XMFLOAT3 currentPos = p_letterC->GetWordPosition(m_menuPages[_pageID]->m_wordID[0]);
	XMFLOAT3 goalPos = XMFLOAT3(35, currentPos.y, 54);
	Vgoal = XMLoadFloat3(&goalPos);
	Vcurrent = XMLoadFloat3(&currentPos);
	if (XMVector4NearEqual(Vcurrent, Vgoal, XMLoadFloat3(&XMFLOAT3(0.1f, 0.1f, 0.1f))))
	{
		return true;
	}

	return false;
}