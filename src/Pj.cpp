#include "Pj.h"
#include <iostream>

#define LIVES 5;


using namespace std;

 Pacman::Pj::Pj
 ()
 {
 	_moving=false;
 	_desp=Ogre::Vector3(0,0,0);
 	_speed=1;
 	_orientation=1;
 	_lives=LIVES;
 	_state="normal";
 }

 Pacman::Pj::~Pj ()
 {
 	
 }

 bool
 Pacman::Pj::isMoving () 
 {
 	return _moving;
 }

 void Pacman::Pj::setMoving(bool n_moving)
 {
 	_moving=n_moving;
 }

 Ogre::Vector3 Pacman::Pj::getDesp() 
 {
 	return _desp;
 }

 void Pacman::Pj::setDesp(Ogre::Vector3 n_desp)
 {
 	_desp.x=n_desp.x*_speed;
 	_desp.y=n_desp.y*_speed;
 	_desp.z=n_desp.z*_speed;
 }

 int Pacman::Pj::getOrientation() 
 {
 	return _orientation;
 }

 void Pacman::Pj::setOrientation(int n_orientation)
 {
 	_orientation=n_orientation;
 }

  int Pacman::Pj::getLives() 
 {
 	return _lives;
 }

 void Pacman::Pj::hit()
 {
 	_lives-=1;
 }

 std::string Pacman::Pj::getState()
 {
 	return _state;
 }

 void Pacman::Pj::changeState(std::string state)
 {
 	
 	if(state=="super"){
 		_state="super";
 	}
 	else{
 		if(state=="respawn"){
 			_state="respawn";
 		}
 		else
 		{
 			_state="normal";
 		}

 	}
 	
 }

 int Pacman::Pj::getSpeed() 
 {
 	return _speed;
 }

 void Pacman::Pj::setSpeed(int n_speed)
 {
 	_speed=n_speed;
 }

