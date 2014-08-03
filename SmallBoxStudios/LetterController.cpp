#include "LetterController.h"

LetterController::LetterController()
{
	//max words and letters.
	m_totalNumOfWords = 40;
	m_numOfLettersInWords = 20;

	//beggining of the array
	m_currentWord = 0;
}

LetterController::~LetterController()
{

}

void LetterController::LoadContent(DxGraphics *dx, ResourceManager& resource)
{
	p_dynamicsWorld = resource.GetPhysicsManager()->GetWorld();
	p_dx = dx;
	p_resource = &resource;

	for (int x = 0; x < m_totalNumOfWords; x++)
	{
		m_wordGroups.push_back(new Word());
		m_wordGroups[m_wordGroups.size() - 1]->alive = false;
		for (int x = 0; x < m_numOfLettersInWords; x++)
		{
			m_wordGroups[m_wordGroups.size() - 1]->m_glyph.push_back(new Glyph());
			m_wordGroups[m_wordGroups.size() - 1]->alive = false;
		}
	}
}

void LetterController::Update(float dt)
{

	for each(Word* obj in m_wordGroups)
	{
		if (obj->alive)
		{
			for each(Glyph* glyph in obj->m_glyph)
			{
				if (glyph->GetLoadedGlyph())
				{
					glyph->Update(dt);
				}
			}
	
		}
	}
}

void LetterController::Render(DxGraphics* dx, Camera& cam, LightManager& lightManager)
{
	for each(Word* obj in m_wordGroups)
	{
		if (obj->alive)
		{

			for each(Glyph* glyph in obj->m_glyph)
			{
				if (glyph->GetLoadedGlyph())
				{
					glyph->Render(dx, cam, lightManager);
					}
			}
		}
	}

}

//create a new word, set its scale space inbetween letters, position(center of the word position): returns an integer id, that can be used to alter the word.
int LetterController::CreateWord(XMFLOAT3 pos, string word,float scale,float spaceing)
{
	//rotate the letters in radians.
	XMFLOAT3 rotation = XMFLOAT3(0, 0, 0);
	//spacing inbetween leters.
	float spacing = spaceing;
	//length ow words in worldspace, will be devided in order to determine the center.
	float totalLength = 0;
	//local variable will be pushed into main wordgroup vector.
	m_wordGroups[m_currentWord]->wordId = m_currentWord;
	m_wordGroups[m_currentWord]->alive = true;
	m_wordGroups[m_currentWord]->m_wordPosition = pos;


	//Number of symbols in the word / sentance
	int numOfSymbols = word.length();
	string symbolArray[50];

	//Convert word into separate letters, inorder to determine the letters to create.
	for (size_type l = 0; l < numOfSymbols; l++)
	{
		symbolArray[l] = word[l];
	}

	//no need for a aspace after the word.
	int numOfSpaces = numOfSymbols - 1;
	float spaceSize = spaceing;

	int totalLetters = numOfSymbols;
	float letterWidth = 0.85f * scale;
	
	float wordTotalLength = numOfSpaces * spaceSize + totalLetters * letterWidth;

	float wordStartPosition = wordTotalLength / 2 * 1;

	//counter counts up if there is a space in the sentance
	int spaces = 0;
	int currentLetter = 0;
	for (int x = 0; x < numOfSymbols; x++)
	{
		if (symbolArray[x] != " ")
		{
			m_wordGroups[m_currentWord]->m_glyph[currentLetter]->LoadContent(symbolArray[x] + ".sbs", 0, 0, 0, scale, true, p_dx, XMFLOAT3(0, 0, 0), *p_resource);
			m_wordGroups[m_currentWord]->m_glyph[currentLetter]->CreateRigidBody(p_resource->GetPhysicsManager()->GetWorld(), 10.1f);

			XMFLOAT3 localPos = XMFLOAT3(0, 0, wordStartPosition);

			m_wordGroups[m_currentWord]->m_glyph[currentLetter]->SetLoadedGlyph(true);

			m_wordGroups[m_currentWord]->m_glyph[currentLetter]->SetLetterOffset(localPos);

			wordStartPosition -= spaceSize;
			wordStartPosition -= letterWidth;

			currentLetter++;
	}
		else
		{
			wordStartPosition -= spaceSize *1.5f;
			//count up in order to be able to populate the vector properly.
			spaces++;
		}
	}
	// m_glyph[m_wordGroups[m_wordGroups.size() - 1]->m_glyph.size() - 1] = newGlyph;
	SetWordPosition(m_currentWord, pos);
	
	//m_currentWord++;
	return m_currentWord++;
}

//speed should be form [0..1],
bool LetterController::ChangeColor(PhysicsObject * obj, float _speed, XMFLOAT4 _goalColour, float dt)
{
	//final colour of the model.
	XMFLOAT4 goalColour;
	//how fast to transition to that colour.
	float speed = _speed;

	//values used to detemine when the colour has been changed, for the whole model
	//highest vertecie index
	int vhighest = 0;
	//highest vertecie Y
	float vHighestPos = 0;

	//convert for dx to understand the colour.
	goalColour = _goalColour;
	goalColour.x = goalColour.x / 255;
	goalColour.y = goalColour.y / 255;
	goalColour.z = goalColour.z / 255;
	//alpha channel not used.
	goalColour.w = 1;

	unsigned int modelVerticeCount = obj->GetModel().verticesPosNor.size();
	for (unsigned int x = 0; x < modelVerticeCount; x++)
	{
		//height used to slow down colour change
		float height = 0;
		//worldspace y coordinate for a vertecie
		height = obj->GetModel().verticesPosNor[x].position.y * obj->GetPosition().y;
		//height = 1;

		//exclude division by zero and negative numbers
		//still asuming model in worldspace is positive Y coordinate.
		if (height < 0)
		{
			height = 1;
		}


		//RED
		if (obj->GetModel().verticesPosNor[x].colour.x > goalColour.x)
		{
			obj->GetModel().verticesPosNor[x].colour.x -= speed / height *dt;
		}
		if (obj->GetModel().verticesPosNor[x].colour.x < goalColour.x)
		{
			obj->GetModel().verticesPosNor[x].colour.x += speed / height *dt;
		}

		//GREEN
		if (obj->GetModel().verticesPosNor[x].colour.y > goalColour.y)
		{
			obj->GetModel().verticesPosNor[x].colour.y -= speed / height *dt;
		}
		if (obj->GetModel().verticesPosNor[x].colour.y < goalColour.y)
		{
			obj->GetModel().verticesPosNor[x].colour.y += speed / height *dt;
		}

		//BLUE
		if (obj->GetModel().verticesPosNor[x].colour.z > goalColour.z)
		{
			obj->GetModel().verticesPosNor[x].colour.z -= speed / height *dt;
		}
		if (obj->GetModel().verticesPosNor[x].colour.z < goalColour.z)
		{
			obj->GetModel().verticesPosNor[x].colour.z += speed / height *dt;
		}
	}
	return false;
}

//Disables bulletphysics gravity for this object
void LetterController::WordGravitySwitch(int _wordID, bool _GravityState)
{
	if (_GravityState)
	{

		for each(Glyph* obj in m_wordGroups[_wordID]->m_glyph)
		{
			if (obj->GetLoadedGlyph())
			{
				obj->GetRigidBody()->setGravity(btVector3(0, -10, 0));
			}
		}
	}
	else
	{
		for each(Glyph* obj in m_wordGroups[_wordID]->m_glyph)
		{
			if (obj->GetLoadedGlyph())
			{
				obj->GetRigidBody()->setGravity(btVector3(0, 0, 0));
			}
		}
	}
}

//Disables buleltphysics collision detection with all the obejcts
void LetterController::WordCollisionSwitch(int _wordID, bool _collisionState)
{
	if (_collisionState)
	{

		for each(Glyph* obj in m_wordGroups[_wordID]->m_glyph)
		{
			if (obj->GetLoadedGlyph())
			{
				obj->GetRigidBody()->setCollisionFlags(obj->GetRigidBody()->getCollisionFlags()
					| btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
			}
		}
	}
	else
	{
		for each(Glyph* obj in m_wordGroups[_wordID]->m_glyph)
		{
			if (obj->GetLoadedGlyph())
			{
				obj->GetRigidBody()->setCollisionFlags(obj->GetRigidBody()->getCollisionFlags()
					| btCollisionObject::CF_NO_CONTACT_RESPONSE);
			}
		}
	}
}

//set the new position of a word
void LetterController::SetWordPosition(int _wordID, XMFLOAT3 _wordPosition)
{
	m_wordGroups[_wordID]->m_wordPosition = _wordPosition;
	for each(Glyph* obj in m_wordGroups[_wordID]->m_glyph)
	{
		if (obj->GetLoadedGlyph())
		{
			obj->SetPosition(obj->GetLetterOffset() + _wordPosition);
		}
	}
}

//set new roattion of the word
void LetterController::SetWordRotation(int _wordID, XMFLOAT3 _wordRotation)
{
	XMFLOAT3 worldPos = m_wordGroups[_wordID]->m_wordPosition;


	for each(Glyph* obj in m_wordGroups[_wordID]->m_glyph)
	{
		if (obj->GetLoadedGlyph())
		{
			XMFLOAT3 letterOffset = obj->GetLetterOffset();
			float letterScale = obj->GetScale();
			XMFLOAT3 letterPosition = obj->GetPosition();

			XMMATRIX offset = XMMatrixTranslation(letterOffset.x, letterOffset.y, letterOffset.z);
			XMMATRIX rotation = XMMatrixRotationRollPitchYaw(_wordRotation.z, _wordRotation.x, _wordRotation.y);
			XMMATRIX zeroPosition = XMMatrixTranslation(0, 0, 0);
			XMMATRIX worldPosition = XMMatrixTranslation(letterPosition.x - letterOffset.x, letterPosition.y - letterOffset.y, letterPosition.z - letterOffset.z);

			XMMATRIX worldmat = XMMatrixIdentity();
			worldmat = offset * rotation * worldPosition;

			XMVECTOR rot, pos, scale;
			XMMatrixDecompose(&scale, &rot, &pos, worldmat);
			XMFLOAT3 rotationx;
			XMFLOAT3 position;
			XMStoreFloat3(&rotationx, rot);
			XMStoreFloat3(&position, pos);
			obj->SetPosition(position);

			btTransform transform = obj->GetRigidBody()->getWorldTransform();

			obj->SetRotation(_wordRotation);
		}
	}
}

//set word colour.
void LetterController::SetWordColour(int _wordID, XMFLOAT4 _wordColour)
{
	for each(Glyph* obj in m_wordGroups[_wordID]->m_glyph)
	{
		if (obj->GetLoadedGlyph())
		{
			obj->ColourFadeSwitch(true);
			obj->SetColour(_wordColour);
		}
	}
}

//Enable disable word(collsion,draw)
void LetterController::WordEnableSwitch(int _wordID, bool _wordSwitch)
{
	if (_wordSwitch)
	{
		m_wordGroups[_wordID]->alive = true;
		WordGravitySwitch(_wordID, true);
		WordCollisionSwitch(_wordID, true);
	}
	else
	{
		m_wordGroups[_wordID]->alive = false;
		WordGravitySwitch(_wordID, false);
		WordCollisionSwitch(_wordID, false);
	}
}

//get current position of a word.
XMFLOAT3 LetterController::GetWordPosition(int _wordID)
{
	return m_wordGroups[_wordID]->m_wordPosition;
}