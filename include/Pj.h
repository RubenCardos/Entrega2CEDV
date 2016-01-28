#ifndef PJ_H
#define PJ_H

#include <Ogre.h>

 
namespace Pacman {

class Pj {
 public:
  Pj ();
  ~Pj ();
 
  // Funciones de acceso.
  bool isMoving ();
  void setMoving(bool n_moving);
  Ogre::Vector3 getDesp();
  void setDesp(Ogre::Vector3 n_desp);
  int getOrientation();
  void setOrientation(int n_orientation);
  int getLives();
  void hit();
  std::string getState();
  void changeState(std::string state);

  
  
 
 private:
  bool _moving;
  Ogre::Vector3 _desp;
  int _speed;
  int _orientation; //1,2,3,4 En sentido horario
  int _lives;
  std::string _state;//Estado en el que nos encontramos


};
 
}

#endif
