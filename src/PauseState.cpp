#include "PauseState.h"

template<> Pacman::PauseState* Ogre::Singleton<Pacman::PauseState>::msSingleton = 0;

void
Pacman::PauseState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();

  // Se recupera el gestor de escena y la cámara.
  _sceneMgr = _root->getSceneManager("SceneManager");
  _camera = _sceneMgr->getCamera("IntroCamera");
  _viewport = _root->getAutoCreatedWindow()->getViewport(0);
  // Nuevo background colour.
  _viewport->setBackgroundColour(Ogre::ColourValue(0.0, 1.0, 0.0));

  _exitGame = false;
}

void
Pacman::PauseState::exit ()
{
}

void
Pacman::PauseState::pause ()
{
}

void
Pacman::PauseState::resume ()
{
}

bool
Pacman::PauseState::frameStarted
(const Ogre::FrameEvent& evt)
{
  return true;
}

bool
Pacman::PauseState::frameEnded
(const Ogre::FrameEvent& evt)
{
  if (_exitGame)
    return false;
  
  return true;
}

void
Pacman::PauseState::keyPressed
(const OIS::KeyEvent &e) {
  // Tecla p --> Estado anterior.
  if (e.key == OIS::KC_P) {
    popState();
  }
}

void
Pacman::PauseState::keyReleased
(const OIS::KeyEvent &e)
{
}

void
Pacman::PauseState::mouseMoved
(const OIS::MouseEvent &e)
{
}

void
Pacman::PauseState::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}

void
Pacman::PauseState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}

Pacman::PauseState*
Pacman::PauseState::getSingletonPtr ()
{
return msSingleton;
}

Pacman::PauseState&
Pacman::PauseState::getSingleton ()
{ 
  assert(msSingleton);
  return *msSingleton;
}
