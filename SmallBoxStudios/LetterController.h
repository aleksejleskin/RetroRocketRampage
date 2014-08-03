#ifndef LETTER_CONTROLLER_H
#define LETTER_CONTROLLER_H

#include "Glyph.h"
#include <iostream>
#include <vector>
#include <iterator>
#include <sstream>
using namespace std;

 struct Word
 {
	 int wordId;
	 vector<Glyph*> m_glyph;
	 XMFLOAT3 m_wordPosition;
	 bool alive;
 };

class LetterController
{
public:
	LetterController();
	~LetterController();
	
	void LoadContent(DxGraphics *dx, ResourceManager& resource);
	bool ChangeColor(PhysicsObject* obj, float _speed, XMFLOAT4 _goalColour, float dt);
	void Update(float dt);
	void Render(DxGraphics* dx, Camera& cam, LightManager& lightManager);

	//Disables bulletphysics gravity for this object
	void WordGravitySwitch(int _wordID, bool _GravityState);
	//Disables buleltphysics collision detection with all the obejcts
	void WordCollisionSwitch(int _wordID, bool _collisionState);
	//set the new position of a word
	void SetWordPosition(int _wordID, XMFLOAT3 _wordPosition);
	//get current position of a word.
	XMFLOAT3 GetWordPosition(int _wordID);
	//set new roattion of the word
	void SetWordRotation(int _wordID, XMFLOAT3 _wordRotation);
	//set word colour.
	void SetWordColour(int _wordID, XMFLOAT4 _wordColour);
	//Enable disable word(collsion,draw)
	void WordEnableSwitch(int _wordID, bool _wordSwitch);

	//create a new word, set its scale space inbetween letters, position(center of the word position): returns an integer id, that can be used to alter the word.
	int CreateWord(XMFLOAT3 position, string word, float scale, float spaceing);
private:
	//pointer to the bulletphysics engine.
	btDynamicsWorld*	p_dynamicsWorld;
	//other resources.
	DxGraphics*			p_dx;
	ResourceManager*	p_resource;

	//Stores all the words
	//can also be a sentance.
	//allows easy use, easy to delete specific words, sentances, 
	vector<Word*>	m_wordGroups;

	//Pre load maximum (allocate space for vecotrs) not to create obejrcts at realtime
	//limit the maximum ammount of words in game.
	int				m_totalNumOfWords;
	//maximum ammount of letter per word
	int				m_numOfLettersInWords;
	int				m_currentWord;
};
	
#endif