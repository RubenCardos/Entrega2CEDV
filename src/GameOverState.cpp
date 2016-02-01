#include "GameOverState.h"
#include "CEGUI.h"
#include "PlayState.h"

template<> Pacman::GameOverState* Ogre::Singleton<Pacman::GameOverState>::msSingleton = 0;

using namespace CEGUI;

void
Pacman::GameOverState::enter ()
{
  
  cout << "\nChange State llego a metodo enter\n" << endl;
  // Recupero las variables necesarias----------------
  _root = Ogre::Root::getSingletonPtr();
  _sceneMgr = _root->getSceneManager("SceneManager");
  _camera = _sceneMgr->createCamera("GameOverCamera");
  _viewport = _root->getAutoCreatedWindow()->addViewport(_camera);
  _viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 0.0));
  //---------------------------------------------------

  //Camara------------------------------------------------------------
  _camera->setPosition(Ogre::Vector3(15,5,15));
  _camera->lookAt(Ogre::Vector3(0,5,0));
  _camera->setNearClipDistance(5);
  _camera->setFarClipDistance(10000);


  double width = _viewport->getActualWidth();
  double height = _viewport->getActualHeight();
  _camera->setAspectRatio(width / height);
  //---------------------------------------------------------------------

  //Musica-----------------------------------------------------------
  GameManager::getSingletonPtr()->_mainTrack->unload(); //Quitamos el sonido que habia antes y abajo cargamos otro
  GameManager::getSingletonPtr()->_mainTrack = GameManager::getSingletonPtr()->_pTrackManager->load("gameover.mp3");
  GameManager::getSingletonPtr()->_mainTrack->play();
  //-------------------------------------------------------------


  //Luz Escena-------------------
  Ogre::Light* spotLight = _sceneMgr->createLight("SpotLight");
  //Colores difusos a azul
  spotLight->setDiffuseColour(0, 1.0, 1.0);
  spotLight->setSpecularColour(0, 1.0, 2.0);
  //Tipo de luz y sus repectivos direcciones y posiciones (Actua modo linterna)
  spotLight->setType(Ogre::Light::LT_SPOTLIGHT);
  spotLight->setDirection(-1, -1, 0);
  spotLight->setPosition(Ogre::Vector3(200, 200, 0));
  //Para desvanecer la luz, Grados de desvanecimiento...
  spotLight->setSpotlightRange(Ogre::Degree(35), Ogre::Degree(50));
  //---------------------------------------------------------------


  //Carga del personaje---------------------------------------------
  Entity* _entCharacter = _sceneMgr->createEntity("entCharacter","personaje.mesh");
  SceneNode* _sceneCharacter = _sceneMgr->createSceneNode("Personaje");
  _sceneCharacter->attachObject(_entCharacter);
  _sceneCharacter->yaw(Ogre::Degree(48));
  _sceneMgr->getRootSceneNode()->addChild(_sceneCharacter);
  //------------------------------------------------------------------

  //Carga del fantasmaa----------------------------------------------------------
  Entity* _entGhost = _sceneMgr->createEntity("entGhost1","ghost1.mesh");
  SceneNode* _sceneGhost = _sceneMgr->createSceneNode("ghost");
  _sceneGhost->attachObject(_entGhost);
  _sceneGhost->yaw(Ogre::Degree(48));
  _sceneMgr->getRootSceneNode()->addChild(_sceneGhost);

   Entity* _entGhost2 = _sceneMgr->createEntity("entGhost2","ghost2.mesh");
   SceneNode* _sceneGhost2 = _sceneMgr->createSceneNode("ghost1");
  _sceneGhost2->attachObject(_entGhost2);
  _sceneGhost2->yaw(Ogre::Degree(48));
  _sceneMgr->getRootSceneNode()->addChild(_sceneGhost2);
  //----------------------------------------------------------------------


  //Animaciones del Personaje y del Fantasma-------------------------------------
  _animPJ = _sceneMgr->getEntity("entCharacter")->getAnimationState("gameover");
  _animPJ->setEnabled(true);
  _animPJ->setLoop(true);
  _animPJ->setTimePosition(0.0);

  _animGhost1 = _sceneMgr->getEntity("entGhost1")->getAnimationState("gameover");
  _animGhost1->setEnabled(true);
  _animGhost1->setLoop(true);
  _animGhost1->setTimePosition(0.0);

  _animGhost2 = _sceneMgr->getEntity("entGhost2")->getAnimationState("gameover");
  _animGhost2->setEnabled(true);
  _animGhost2->setLoop(true);
  _animGhost2->setTimePosition(0.0);
  //----------------------------------------------------------


   //Background-------------------------------------------
  Ogre::Plane plane1(Ogre::Vector3::UNIT_Y, 0);
  Ogre::MeshManager::getSingleton().createPlane("plane1",
  Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane1,
  100,100,1,1,true,1,1,1,Ogre::Vector3::UNIT_Z);

  Ogre::SceneNode* nodeBG = _sceneMgr->createSceneNode("gameover");
  Ogre::Entity* groundEnt = _sceneMgr->createEntity("planeEnt", "plane1");
  groundEnt->setMaterialName("gameover");
  nodeBG->setScale(0.3,0.3,0.3);
  nodeBG->setPosition(0,3,0);
  nodeBG->attachObject(groundEnt);
  nodeBG->yaw(Ogre::Degree(135));
  nodeBG->roll(Ogre::Degree(90));
  _sceneMgr->getRootSceneNode()->addChild(nodeBG);
  //--------------------------------------------------------
  createGUI();


  _exitGame = false;

  

}

bool
Pacman::GameOverState::quitGM(const CEGUI::EventArgs &e)
{
  _exitGame=true;
  return true;
}

bool
Pacman::GameOverState::resetGM(const CEGUI::EventArgs &e)
{
  changeState(PlayState::getSingletonPtr());
  
  return true;
}

void
Pacman::GameOverState::exit ()
{
  cout <<"\nPaso por el exit de game over\n"<< endl;
  _sceneMgr->clearScene();
  _sceneMgr->destroyCamera("GameOverCamera");

  //Limpio la interfaz de CEGUI
  CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  //sheet->destroyChild(sheet->getChild("background_gameover"));
  //--------------------------------------------------------------------
  _root->getAutoCreatedWindow()->removeAllViewports();
}
void
Pacman::GameOverState::createGUI ()
{
  //INTERFAZ CEGUI-----------------------------------------------------
  CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();

  
  CEGUI::Window* sheetBG =  CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticImage","background_gameover");
  sheetBG->setPosition(UVector2(cegui_reldim(0),cegui_reldim(0)));
  sheetBG->setSize(CEGUI::USize(CEGUI::UDim(20,0),CEGUI::UDim(20,0)));
  sheetBG->setProperty("Image","BackgroundGameOver");
  sheetBG->setProperty("FrameEnabled","False");
  sheetBG->setProperty("BackgroundEnabled", "False");


  CEGUI::Window* resetButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","resetGameOver");
  resetButton->setText("[font='Carton_Six'] Reiniciar ");
  resetButton->setSize(CEGUI::USize(CEGUI::UDim(0.23,0),CEGUI::UDim(0.07,0)));
  resetButton->setXPosition(UDim(0.75f, 0.0f));
  resetButton->setYPosition(UDim(0.50f, 0.0f));
  resetButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&GameOverState::resetGM,this));

  CEGUI::Window* exitButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","exitGameOver");
  exitButton->setText("[font='Carton_Six'] Salir ");
  exitButton->setSize(CEGUI::USize(CEGUI::UDim(0.23,0),CEGUI::UDim(0.07,0)));
  exitButton->setXPosition(UDim(0.03f, 0.0f));
  exitButton->setYPosition(UDim(0.50f, 0.0f));
  exitButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&GameOverState::quitGM,this));

  sheetBG->addChild(resetButton);
  sheetBG->addChild(exitButton);
  sheet->addChild(sheetBG);
   //------------------------------------------------------------------------------------
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

  _deltaTGameOver = evt.timeSinceLastFrame;


  if (_animPJ != NULL) {
    if (_animPJ->hasEnded()) {
      _animPJ->setTimePosition(0.0);
      _animPJ->setEnabled(false);
    }
    else {
      _animPJ->addTime(_deltaTGameOver);
    }
  }

  if (_animGhost1 != NULL) {
    if (_animGhost1->hasEnded()) {
      _animGhost1->setTimePosition(0.0);
      _animGhost1->setEnabled(false);
    }
    else {
      _animGhost1->addTime(_deltaTGameOver);
    }
  }

  if(_animGhost2 != NULL) {
    if (_animGhost2->hasEnded()) {
      _animGhost2->setTimePosition(0.0);
      _animGhost2->setEnabled(false);
    }
    else {
      _animGhost2->addTime(_deltaTGameOver);
    }
  }

  //Escalo los nodos para parezca que se alejan----
  SceneNode* _snPj =_sceneMgr->getSceneNode("Personaje");
  SceneNode* _snGhost =_sceneMgr->getSceneNode("ghost");
  SceneNode* _snGhost2 =_sceneMgr->getSceneNode("ghost1");

  if(_snPj->getScale() > Ogre::Vector3(0.01,0.01,0.01) ){
  
    _snPj->setScale((_snPj->getScale())-(_deltaTGameOver/8));
    _snGhost->setScale((_snGhost->getScale())-(_deltaTGameOver/8));
    _snGhost2->setScale((_snGhost2->getScale())-(_deltaTGameOver/8));
  
    _snPj->setPosition(_snPj->getPosition()+Ogre::Vector3(0,_deltaTGameOver/4,0));
    _snGhost->setPosition(_snGhost->getPosition()+Ogre::Vector3(0,_deltaTGameOver/4,0));
    _snGhost2->setPosition(_snGhost2->getPosition()+Ogre::Vector3(0,_deltaTGameOver/4,0));
  
  }else{
  
      _snPj->setVisible(false);
      _snGhost->setVisible(false);
      _snGhost2->setVisible(false);

  }
  //-----------------------------------------------


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
    CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(e.state.X.rel, e.state.Y.rel);  
}

void
Pacman::GameOverState::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(convertMouseButton(id));
}

void
Pacman::GameOverState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(convertMouseButton(id));
}


CEGUI::MouseButton Pacman::GameOverState::convertMouseButton(OIS::MouseButtonID id)//METODOS DE CEGUI
{
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
