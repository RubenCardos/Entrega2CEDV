#include "PlayState.h"
#include "PauseState.h"

template<> PlayState* Ogre::Singleton<PlayState>::msSingleton = 0;

void
PlayState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();

  // Se recupera el gestor de escena y la cámara.
  _sceneMgr = _root->getSceneManager("SceneManager");
  _camera = _sceneMgr->getCamera("IntroCamera");
  _viewport = _root->getAutoCreatedWindow()->addViewport(_camera);
  // Nuevo background colour.
  _viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 0.0));


  //Pruebo a crear algo y ponerlo
  _sceneMgr->setAmbientLight(Ogre::ColourValue(0.8, 0.8, 0.8)); 
  //Camara
  _camera->setPosition(Ogre::Vector3(50,50,50));//el tercer parametro hace que se aleje mas la camara, el segundo para que rote hacia arriba o hacia abajo
  _camera->lookAt(Ogre::Vector3(0,0,0));//bajar el 60 un poco
  _camera->setNearClipDistance(5);
  _camera->setFarClipDistance(10000);
  
  //Nodo
  SceneNode* node1 = _sceneMgr->createSceneNode("Node1");
  Entity *ent1 =_sceneMgr->createEntity("Cubo1", "cube.mesh");
  node1->attachObject(ent1);
  _sceneMgr->getRootSceneNode()->addChild(node1);
  node1->setPosition(0,0,0);
  node1->setScale(20,20,20);
  // -----------------------------

  _exitGame = false;
}

void
PlayState::exit ()
{
  _sceneMgr->clearScene();
  _root->getAutoCreatedWindow()->removeAllViewports();
}

void
PlayState::pause()
{
}

void
PlayState::resume()
{
  // Se restaura el background colour.
  _viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 0.0));
}

bool
PlayState::frameStarted
(const Ogre::FrameEvent& evt)
{
  return true;
}

bool
PlayState::frameEnded
(const Ogre::FrameEvent& evt)
{
  if (_exitGame)
    return false;
  
  return true;
}

void
PlayState::keyPressed
(const OIS::KeyEvent &e)
{
  // Tecla p --> PauseState.
  if (e.key == OIS::KC_P) {
    pushState(PauseState::getSingletonPtr());
  }
}

void
PlayState::keyReleased
(const OIS::KeyEvent &e)
{
  if (e.key == OIS::KC_ESCAPE) {
    _exitGame = true;
  }
}

void
PlayState::mouseMoved
(const OIS::MouseEvent &e)
{
}

void
PlayState::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}

void
PlayState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}

PlayState*
PlayState::getSingletonPtr ()
{
return msSingleton;
}

PlayState&
PlayState::getSingleton ()
{ 
  assert(msSingleton);
  return *msSingleton;
}
