#include "PlayState.h"
#include "PauseState.h"

template<> PlayState* Ogre::Singleton<PlayState>::msSingleton = 0;

void
PlayState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();

  // Se recupera el gestor de escena y la cámara.----------------
  _sceneMgr = _root->getSceneManager("SceneManager");
  _camera = _sceneMgr->getCamera("IntroCamera");
  _viewport = _root->getAutoCreatedWindow()->addViewport(_camera);
  _viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 0.0));// Nuevo background colour.
  //-------------------------------------

  //Pruebo a crear algo y ponerlo---------
  _sceneMgr->setAmbientLight(Ogre::ColourValue(0.8, 0.8, 0.8));
  //-------------------------------------
   
  //Camara--------------------
  _camera->setPosition(Ogre::Vector3(50,50,50));//el tercer parametro hace que se aleje mas la camara, el segundo para que rote hacia arriba o hacia abajo
  _camera->lookAt(Ogre::Vector3(0,0,0));//bajar el 60 un poco
  _camera->setNearClipDistance(5);
  _camera->setFarClipDistance(10000);
  //-----------------------------

  //Camara MiniMapa-------------------
  _cameraMiniMap = _sceneMgr->getCamera("MiniMapCamera");
  _cameraMiniMap->setPosition(Ogre::Vector3(50,50,50));//Pongo la camra en el mismo sitio, deberia ver lo mismo dos veces
  _cameraMiniMap->lookAt(Ogre::Vector3(0,0,0));//bajar el 60 un poco
  _cameraMiniMap->setNearClipDistance(5);
  _cameraMiniMap->setFarClipDistance(10000);
  //--------------------------------
  
  //Nodo------------------------
  SceneNode* node1 = _sceneMgr->createSceneNode("Node1");
  Entity *ent1 =_sceneMgr->createEntity("Cubo1", "cube.mesh");
  node1->attachObject(ent1);
  _sceneMgr->getRootSceneNode()->addChild(node1);
  node1->setPosition(0,0,0);
  node1->setScale(20,20,20);
  // -----------------------------

  // Interfaz --------------------
  CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  sheet->getChildAtIdx(0)->setVisible(false);
  
  //Boton Salir Juego---------------------------------------
  CEGUI::Window* quitButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button","Ex1/QuitButton");
  quitButton->setText("Exit");
  quitButton->setSize(CEGUI::USize(CEGUI::UDim(0.15,0),CEGUI::UDim(0.05,0)));
  quitButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.05,0),CEGUI::UDim(0.8,0)));
  quitButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&PlayState::quit,this));
  //-------------------------------------------------------
  
  sheet->addChild(quitButton);
  //------------------------------

  _exitGame = false;
}

void
PlayState::exit ()
{
  //Sago del estado------------------------------
  _sceneMgr->clearScene();
  _root->getAutoCreatedWindow()->removeAllViewports();
  //--------------------------------------------
}

void
PlayState::pause()
{
}

void
PlayState::resume()
{
  //Se restaura el background colour.------------------------------
  _viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 0.0));
  //---------------------------------------------------------------
}

bool
PlayState::frameStarted
(const Ogre::FrameEvent& evt)
{
  //Actualizacion de Variables----------------
  Real _deltaT = evt.timeSinceLastFrame;
  //------------------------------
  CEGUI::Window* rootWin = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  CEGUI::Window* ex1 = rootWin->getChild("CamWin");
  CEGUI::Window* RTTWindow = ex1->getChild("RTTWindow");
  RTTWindow->invalidate();

  //Nodo de prueba----------------
  SceneNode* aux =static_cast<SceneNode*>(_sceneMgr->getRootSceneNode()->getChild("Node1"));
  aux->yaw(Degree(_deltaT*10));
  //-------------------------------------
  return true;
}

bool
PlayState::frameEnded
(const Ogre::FrameEvent& evt)
{
  //Salir del juego--
  if (_exitGame)
    return false;
  //-----------------
  return true;
}

void
PlayState::keyPressed
(const OIS::KeyEvent &e)
{
  // Tecla p --> PauseState.-------
  if (e.key == OIS::KC_P) {
    pushState(PauseState::getSingletonPtr());
  }
  //-----------------

  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown(static_cast<CEGUI::Key::Scan>(e.key));
  CEGUI::System::getSingleton().getDefaultGUIContext().injectChar(e.text);
  //-------------------------------
}

void
PlayState::keyReleased
(const OIS::KeyEvent &e)
{
  //Salgo del juego---------------
  if (e.key == OIS::KC_ESCAPE) {
    _exitGame = true;
  }
  //-------------------------------
  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp(static_cast<CEGUI::Key::Scan>(e.key));
  //-------------------------------
}

void
PlayState::mouseMoved
(const OIS::MouseEvent &e)
{
  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(e.state.X.rel, e.state.Y.rel);
  //-------------------------------
}

void
PlayState::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(convertMouseButton(id));
  //-------------------------------
}

void
PlayState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(convertMouseButton(id));
  //-------------------------------
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

CEGUI::MouseButton PlayState::convertMouseButton(OIS::MouseButtonID id)//METODOS DE CEGUI
{
  //CEGUI--------------------------
  CEGUI::MouseButton ceguiId;
  switch(id)
    {
    case OIS::MB_Left:
      ceguiId = CEGUI::LeftButton;
      break;
    case OIS::MB_Right:
      ceguiId = CEGUI::RightButton;
      break;
    case OIS::MB_Middle:
      ceguiId = CEGUI::MiddleButton;
      break;
    default:
      ceguiId = CEGUI::LeftButton;
    }
  return ceguiId;
  //------------------------------
}

bool
PlayState::quit(const CEGUI::EventArgs &e)
{
  _exitGame=true;
  return true;
}
