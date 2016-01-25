#include "GameOverState.h"

template<> Pacman::GameOverState* Ogre::Singleton<Pacman::GameOverState>::msSingleton = 0;

void
Pacman::GameOverState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();

  // Se recupera el gestor de escena y la cámara.
  _sceneMgr = _root->getSceneManager("SceneManager");
  _camera = _sceneMgr->getCamera("IntroCamera");
  _viewport = _root->getAutoCreatedWindow()->getViewport(0);
  // Nuevo background colour.
  _viewport->setBackgroundColour(Ogre::ColourValue(1.0, 1.0, 1.0));

  _exitGame = false;
}

void
Pacman::GameOverState::exit ()
{
}

void
Pacman::GameOverState::pause ()
{
}

void
Pacman::GameOverState::resume ()
{
}

bool
Pacman::GameOverState::frameStarted
(const Ogre::FrameEvent& evt)
{
  return true;
}

bool
Pacman::GameOverState::frameEnded
(const Ogre::FrameEvent& evt)
{
  if (_exitGame)
    return false;
  
  return true;
}

void
Pacman::GameOverState::keyPressed
(const OIS::KeyEvent &e) {
  
}

void
Pacman::GameOverState::keyReleased
(const OIS::KeyEvent &e)
{
}

void
Pacman::GameOverState::mouseMoved
(const OIS::MouseEvent &e)
{
}

void
Pacman::GameOverState::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}

void
Pacman::GameOverState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}

Pacman::GameOverState*
Pacman::GameOverState::getSingletonPtr ()
{
return msSingleton;
}

Pacman::GameOverState&
Pacman::GameOverState::getSingleton ()
{ 
  assert(msSingleton);
  return *msSingleton;
}
