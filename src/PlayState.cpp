#include "PlayState.h"
#include "PauseState.h"

template<> Pacman::PlayState* Ogre::Singleton<Pacman::PlayState>::msSingleton = 0;

// Prueba de animacion -----
Ogre::AnimationState *_animState;
//--------------------------

// Donde vengo y donde voy ---
Pacman::GraphVertex* _now;
Pacman::GraphVertex* _next;
std::vector<Pacman::GraphVertex*> _adjVer;
std::vector<Ogre::Vector3> _possibleMoves;
// ----------------------------

void
Pacman::PlayState::enter ()
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

  //Camara MiniMapa-------------------
  _cameraMiniMap = _sceneMgr->getCamera("MiniMapCamera");
  _cameraMiniMap->setPosition(Ogre::Vector3(200,200,0));//Pongo la camra en el mismo sitio, deberia ver lo mismo dos veces
  _cameraMiniMap->lookAt(Ogre::Vector3(0,0,0));//bajar el 60 un poco
  _cameraMiniMap->setNearClipDistance(5);
  _cameraMiniMap->setFarClipDistance(10000);
  //--------------------------------
  
  //Pj------------------------
  Entity* _entPj = _sceneMgr->createEntity("entPJ","Circle.mesh");
  SceneNode* _snPj = _sceneMgr->createSceneNode("PjSceneNode");
  _snPj->attachObject(_entPj);
  _snPj->setPosition(0,20,0); //x,y,z
  //_snPj->pitch(Ogre::Degree(-90));
  _snPj->setScale(5,5,5);
  _sceneMgr->getRootSceneNode()->addChild(_snPj);

  _pj=new Pj;

  // -----------------------------

  //Camara--------------------
  _camera->setPosition(_snPj->getPosition().x,_snPj->getPosition().y-200,_snPj->getPosition().z);
  _camera->setNearClipDistance(5);
  _camera->setFarClipDistance(10000);
  _camera->yaw(Ogre::Degree(90));
  //-----------------------------

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

  // Mapa -------------------------
  //Pj------------------------
  Entity* _entMap = _sceneMgr->createEntity("entMap","Plane.001.mesh");
  SceneNode* _snMap = _sceneMgr->createSceneNode("MapSceneNode");
  _snMap->attachObject(_entMap);
  _snMap->setPosition(0,-15,0); //x,y,z
  _snMap->setScale(100,100,100);
  _sceneMgr->getRootSceneNode()->addChild(_snMap);

  // -----------------------------

  // ------------------

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

  // Prueba Grafo ------------------------------
  try{
    _importer = new Importer;
    _scene = new Scene;

    // Parse data...
    _importer->parseScene("./data/sample.xml", _scene);

    // Print some data...
      cout << "\nGraph info..." << endl;
      cout << "\t#Vertexes: " << _scene->getGraph()->getVertexes().size() << endl;
      cout << "\t#Edges: " << _scene->getGraph()->getEdges().size() << endl;

      cout << "Vertices" << endl; 

      std::vector<GraphVertex*> _vertices=_scene->getGraph()->getVertexes();
      std::vector<GraphVertex*>::const_iterator it;
      int i =1;
      for (it = _vertices.begin();it != _vertices.end();++it){
        Node _aux=(*it)->getData();
        cout << "\t" << static_cast<std::string>(_aux)  << endl;
        Entity* _entPoint = _sceneMgr->createEntity("entPoint"+Ogre::StringConverter::toString(i),"cube.mesh");
        SceneNode* _snPoint = _sceneMgr->createSceneNode("PointSceneNode"+Ogre::StringConverter::toString(i));
        _snPoint->attachObject(_entPoint);
        _snPoint->setPosition(_aux.getPosition()); //x,y,z
        _snPoint->setScale(5,5,5);
        _sceneMgr->getRootSceneNode()->addChild(_snPoint);
        i+=1;
      }
  }
  catch (...){
      cerr << "Unexpected exception!" << endl;
  }

  // ------------------------------------------
  
  //Prueba de cambio de posicion segun el vertice ---
  _snPj->setPosition(_scene->getGraph()->getVertex(2)->getData().getPosition());
  _now=_scene->getGraph()->getVertex(2);
  //------------------------------------------------

  // Grafo, adyacentes ----

  _adjVer = _scene->getGraph()->adjacents(_now->getData().getIndex());
  std::vector<GraphVertex*>::const_iterator it;
  for (it = _adjVer.begin();it != _adjVer.end();++it){
    Node _aux=(*it)->getData();
    Ogre::Vector3 _vecMove=_aux.getPosition()-_now->getData().getPosition();
    _possibleMoves.push_back(_vecMove);
  }
  //_possibleMoves.clear();
  //----------------------

  _exitGame = false;
}

void
Pacman::PlayState::exit ()
{
  //Salgo del estado------------------------------
  _sceneMgr->clearScene();
  _root->getAutoCreatedWindow()->removeAllViewports();
  //--------------------------------------------
}

void
Pacman::PlayState::pause()
{
}

void
Pacman::PlayState::resume()
{
  //Se restaura el background colour.------------------------------
  _viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 0.0));
  //---------------------------------------------------------------
}

bool
Pacman::PlayState::frameStarted
(const Ogre::FrameEvent& evt)
{
  //Actualizacion de Variables----------------
  _deltaT = evt.timeSinceLastFrame;
  SceneNode* _snPj =_sceneMgr->getSceneNode("PjSceneNode");
  //------------------------------
  
  // La camara sigue al personaje ------
  _camera->setPosition(_snPj->getPosition().x+200,_snPj->getPosition().y+75,_snPj->getPosition().z);
  //----------------------------------

  //CEGUI --------------------------------------
  CEGUI::Window* rootWin = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  CEGUI::Window* ex1 = rootWin->getChild("CamWin");
  CEGUI::Window* RTTWindow = ex1->getChild("RTTWindow");
  RTTWindow->invalidate();
  //-------------------------------------------

  // Movimiento (Prueba)----------

  if(_pj->isMoving()){
    _snPj->setPosition(Vector3(_snPj->getPosition())+=_pj->getDesp());
  }

  if(isPositionInAVertex(_snPj->getPosition())){
    _pj->setMoving(false);

    //Cuando llego a un vertice nuevo cambio _now ----
    _now=whereIAm(_snPj->getPosition());

    //-------------------------------------------------

    // Calculo los nuevos adyacentes -------------------
    _possibleMoves.clear();
    _adjVer.clear();
    _adjVer = _scene->getGraph()->adjacents(_now->getData().getIndex());
    std::vector<GraphVertex*>::const_iterator it;
    for (it = _adjVer.begin();it != _adjVer.end();++it){
      Node _aux=(*it)->getData();
      Ogre::Vector3 _vecMove=_aux.getPosition()-_now->getData().getPosition();
      _possibleMoves.push_back(_vecMove);
    }

    //-----------------------------------------------


  }else{
    _pj->setMoving(true);
  }

  //--------------------

  



  //Animacion (Prueba)----------------
  if (_animState != NULL) {
    if (_animState->hasEnded()) {
      _animState->setTimePosition(0.0);
      _animState->setEnabled(false);
    }
    else {
      _animState->addTime(_deltaT);
    }
  }
  //-------------------------------------
  return true;
}

bool
Pacman::PlayState::frameEnded
(const Ogre::FrameEvent& evt)
{
  //Salir del juego--
  if (_exitGame)
    return false;
  //-----------------
  return true;
}

void
Pacman::PlayState::keyPressed
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
    case OIS::KC_A:{
      // Compruebo si esta direccion esta en las direciones posibles---
      std::vector<Ogre::Vector3>::const_iterator it;
      bool _check =true;
      for (it = _possibleMoves.begin();it != _possibleMoves.end() && _check;++it){
        Ogre::Vector3 _aux=*it;
        if(_aux.z>0){
          cout << "Voy!" << endl;
          _pj->setMoving(true);
          _pj->setDesp(Vector3(0,0,_deltaT));
          _check=false;
        }else{
          cout << "No se puede ir hacia alli" << endl;
        }
      }
      //--------------------------------------------------------------
      
      
      break;}
    case OIS::KC_D:{
      // Compruebo si esta direccion esta en las direcionesposibles---
      std::vector<Ogre::Vector3>::const_iterator it;
      bool _check =true;
      for (it = _possibleMoves.begin();it != _possibleMoves.end() && _check;++it){
        Ogre::Vector3 _aux=*it;
        if(_aux.z<0){
          cout << "Voy!" << endl;
          _pj->setMoving(true);
          _pj->setDesp(Vector3(0,0,-_deltaT));;
          _check=false;
        }else{
          cout << "No se puede ir hacia alli" << endl;
        }
      }
      //------------------------------------------------------
      break;}
    case OIS::KC_W:{
      // Compruebo si esta direccion esta en las direciones posibles---
      std::vector<Ogre::Vector3>::const_iterator it;
      bool _check =true;
      for (it = _possibleMoves.begin();it != _possibleMoves.end() && _check;++it){
        Ogre::Vector3 _aux=*it;
        if(_aux.x<0){
          cout << "Voy!" << endl;
          _pj->setMoving(true);
          _pj->setDesp(Vector3(-_deltaT,0,0));
          _check=false;
        }else{
          cout << "No se puede ir hacia alli" << endl;
        }
      }
      //---------------------------------------------
      break;}
    case OIS::KC_S:{
      // Compruebo si esta direccion esta en las direciones posibles---
      std::vector<Ogre::Vector3>::const_iterator it;
      bool _check =true;
      for (it = _possibleMoves.begin();it != _possibleMoves.end() && _check;++it){
        Ogre::Vector3 _aux=*it;
        if(_aux.x>0){
          cout << "Voy!" << endl;
          _pj->setMoving(true);
          _pj->setDesp(Vector3(_deltaT,0,0));
          _check=false;
        }else{
          cout << "No se puede ir hacia alli" << endl;
        }
      }
      }
      //-----------------------------------------------
      break;
   	case OIS::KC_R:
      _aux->roll(Ogre::Degree(-90));
      break;
    case OIS::KC_SPACE:
      _animState = _sceneMgr->getEntity("entPJ")->getAnimationState("walking");
      _animState->setEnabled(true);
      _animState->setLoop(true);
      _animState->setTimePosition(0.0);
      break;
  }
  //----------------------------
}

void
Pacman::PlayState::keyReleased
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
Pacman::PlayState::mouseMoved
(const OIS::MouseEvent &e)
{
  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(e.state.X.rel, e.state.Y.rel);
  //-------------------------------
}

void
Pacman::PlayState::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(convertMouseButton(id));
  //-------------------------------
}

void
Pacman::PlayState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(convertMouseButton(id));
  //-------------------------------
}

Pacman::PlayState*
Pacman::PlayState::getSingletonPtr ()
{
return msSingleton;
}

Pacman::PlayState&
Pacman::PlayState::getSingleton ()
{ 
  assert(msSingleton);
  return *msSingleton;
}

CEGUI::MouseButton Pacman::PlayState::convertMouseButton(OIS::MouseButtonID id)//METODOS DE CEGUI
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
Pacman::PlayState::quit(const CEGUI::EventArgs &e)
{
  _exitGame=true;
  return true;
}

bool
Pacman::PlayState::isPositionInAVertex(Ogre::Vector3 _pos)
{
  
  bool res=false;

  // Busco si la posicion coincide con alguna de los nodos ---

  //Ajusto _pos para que sea numeros enteros---------------
  _pos.x=round(_pos.x);
  _pos.z=round(_pos.z);
  //cout << "Pos: " << _pos << endl;
  //--------------------------------------------------------

  std::vector<GraphVertex*> _vertices=_scene->getGraph()->getVertexes();
  std::vector<GraphVertex*>::const_iterator it;
  for (it = _vertices.begin();it != _vertices.end();++it){
    Node _aux=(*it)->getData();
    if(_aux.getPosition()==_pos){
      res=true;
    }
  }
  //----------------------------------------------------------


  return res;
}

Pacman::GraphVertex*
Pacman::PlayState::whereIAm(Ogre::Vector3 _pos)
{


  // Busco si la posicion coincide con alguna de los nodos ---

  //Ajusto _pos para que sea numeros enteros---------------
  _pos.x=round(_pos.x);
  _pos.z=round(_pos.z);
  //--------------------------------------------------------

  std::vector<GraphVertex*> _vertices=_scene->getGraph()->getVertexes();
  std::vector<GraphVertex*>::const_iterator it;
  for (it = _vertices.begin();it != _vertices.end();++it){
    Node _aux=(*it)->getData();
    if(_aux.getPosition()==_pos){
      return (*it);
    }
  }
  //----------------------------------------------------------
}
