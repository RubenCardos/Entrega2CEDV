#include "Pj.h"
#include <iostream>

using namespace std;

 Pacman::Pj::Pj
 ()
 {
 	_moving=false;
 	_desp=Ogre::Vector3(0,0,0);
 	_speed=8;
 	_orientation=1;
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
