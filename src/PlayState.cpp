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
  _camera->setPosition(Ogre::Vector3(150,150,150));//el tercer parametro hace que se aleje mas la camara, el segundo para que rote hacia arriba o hacia abajo
  _camera->lookAt(Ogre::Vector3(0,0,0));//bajar el 60 un poco
  _camera->setNearClipDistance(5);
  _camera->setFarClipDistance(10000);
  //-----------------------------

  //Camara MiniMapa-------------------
  _cameraMiniMap = _sceneMgr->getCamera("MiniMapCamera");
  _cameraMiniMap->setPosition(Ogre::Vector3(200,200,0));//Pongo la camra en el mismo sitio, deberia ver lo mismo dos veces
  _cameraMiniMap->lookAt(Ogre::Vector3(0,0,0));//bajar el 60 un poco
  _cameraMiniMap->setNearClipDistance(5);
  _cameraMiniMap->setFarClipDistance(10000);
  //--------------------------------
  
  //Nodo------------------------
  Entity* _entPj = _sceneMgr->createEntity("Plane.mesh");
  SceneNode* _snPj = _sceneMgr->createSceneNode("PjSceneNode");
  _snPj->attachObject(_entPj);
  _snPj->setPosition(0,20,0); //x,y,z
  _snPj->setScale(25,5,5);
  _sceneMgr->getRootSceneNode()->addChild(_snPj);
  // -----------------------------

  //Plano ------------------------
  Ogre::Plane plane1(Ogre::Vector3::UNIT_Y, -5);
  Ogre::MeshManager::getSingleton().createPlane("plane1", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane1,
  200,200,1,1,true,1,1,1,Ogre::Vector3::UNIT_Z);

  SceneNode* _nodePlane1 = _sceneMgr->createSceneNode("Tablero");
  Entity* _groundEnt1 = _sceneMgr->createEntity("planeEnt1", "plane1");
  _groundEnt1->setMaterialName("Ground"); 
  _nodePlane1->attachObject(_groundEnt1);
  _nodePlane1->setPosition(0,0,0);
  _sceneMgr->getRootSceneNode()->addChild(_nodePlane1);
  //-----------------------------

  // Interfaz --------------------
  CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  sheet->getChildAtIdx(0)->setVisible(false);
  sheet->getChildAtIdx(1)->setVisible(true);
  //Boton Salir Juego---------------------------------------
  CEGUI::Window* quitButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button","Ex1/QuitButton");
  quitButton->setText("Exit");
  quitButton->setSize(CEGUI::USize(CEGUI::UDim(0.15,0),CEGUI::UDim(0.05,0)));
  quitButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.05,0),CEGUI::UDim(0.8,0)));
  quitButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&PlayState::quit,this));
  //-------------------------------------------------------
  
  sheet->addChild(quitButton);
  //------------------------------

  //Mapa Colisiones --------------

  //Nodo Extra-------------------------------------------------------
  Ogre::SceneNode *nodecol = _sceneMgr->createSceneNode("Nodo_Suelo");
  _sceneMgr->getRootSceneNode()->addChild(nodecol);
  //----------------------------------------------------------------

  Ogre::SceneNode *nodefloor = _sceneMgr->createSceneNode("Col_Suelo");
  Ogre::Entity *entfloor = _sceneMgr->createEntity("Col_Suelo", "Col_Suelo.mesh");
  //entcol->setQueryFlags(STAGE);   // Usamos flags propios! para Escenario
  nodefloor->setScale(15,15,15);
  nodefloor->attachObject(entfloor);
  nodefloor->setVisible(false);
  nodecol->addChild(nodefloor);

  // Cajas del escenario (baja poligonalizacion)------------------------------- 
  std::stringstream sauxnode, sauxmesh;
  std::string s = "Col_Box";
  for (int i=1; i<27; i++) {
    sauxnode << s << i; sauxmesh << s << i << ".mesh";
    cout << "\nsauxnode:" << sauxnode.str() << endl;
    cout << "sauxmesh:" << sauxmesh.str() << "\n" << endl;
    Ogre::SceneNode *nodebox = _sceneMgr->createSceneNode(sauxnode.str());
    Ogre::Entity *entboxcol = _sceneMgr->createEntity(sauxnode.str(), sauxmesh.str());
    //entboxcol->setQueryFlags(STAGE);    // Escenario
    nodebox->setScale(15,15,15);
    nodebox->attachObject(entboxcol);
    //nodebox->setVisible(false);
    nodecol->addChild(nodebox);
    sauxnode.str(""); sauxmesh.str(""); // Limpiamos el stream
  }

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
  
  //CEGUI --------------------------------------
  CEGUI::Window* rootWin = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  CEGUI::Window* ex1 = rootWin->getChild("CamWin");
  CEGUI::Window* RTTWindow = ex1->getChild("RTTWindow");
  RTTWindow->invalidate();
  //-------------------------------------------

  //Nodo de prueba----------------
 
  
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

  //Movimiento PJ----------------
  SceneNode* _aux =_sceneMgr->getSceneNode("PjSceneNode");
  switch(e.key){
    case OIS::KC_A:
      _aux->setPosition(Vector3(_aux->getPosition())+=Vector3(0,0,1));
      _camera->setPosition(Vector3(_camera->getPosition())+=Vector3(0,0,1));
      break;
    case OIS::KC_D:
      _aux->setPosition(Vector3(_aux->getPosition())+=Vector3(0,0,-1));
      _camera->setPosition(Vector3(_camera->getPosition())+=Vector3(0,0,-1));
      break;
    case OIS::KC_W:
      _aux->setPosition(Vector3(_aux->getPosition())+=Vector3(-1,0,0));
      _camera->setPosition(Vector3(_camera->getPosition())+=Vector3(-1,0,0));
      break;
    case OIS::KC_S:
      _aux->setPosition(Vector3(_aux->getPosition())+=Vector3(1,0,0));
      _camera->setPosition(Vector3(_camera->getPosition())+=Vector3(1,0,0));
      break;
   	case OIS::KC_R:
      _aux->yaw(Ogre::Degree(-90));
      break;
  }
  //----------------------------
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
