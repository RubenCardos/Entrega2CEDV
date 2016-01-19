#include "PlayState.h"
#include "PauseState.h"
#define CAM_HEIGHT 2 


template<> Pacman::PlayState* Ogre::Singleton<Pacman::PlayState>::msSingleton = 0;

// Prueba de animacion -----
Ogre::AnimationState *_animState;
Ogre::AnimationState *_animGhost;
//--------------------------

// Donde vengo y donde voy ---
Pacman::GraphVertex* _now;
bool _exitVertex = false;
std::vector<Pacman::GraphVertex*> _adjVer;
std::vector<Ogre::Vector3> _possibleMoves;
// ----------------------------

// Donde vengo y donde voy Ghost  ---
Pacman::GraphVertex* _nowGhost;
Pacman::GraphVertex* _nextGhost;
std::vector<Pacman::GraphVertex*> _adjVerGhost;
std::vector<Ogre::Vector3> _possibleMovesGhost;
// ----------------------------

void
Pacman::PlayState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();

  // Se recupera el gestor de escena y la cámara.----------------
  _sceneMgr = _root->getSceneManager("SceneManager");
  _camera = _sceneMgr->createCamera("PlayCamera");
  _viewport = _root->getAutoCreatedWindow()->addViewport(_camera);
  _viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 0.0));// Nuevo background colour.
  //-------------------------------------

  /// create a pivot at roughly the character's shoulder
		mCameraPivot = _camera->getSceneManager()->getRootSceneNode()->createChildSceneNode();
		// this is where the camera should be soon, and it spins around the pivot
		mCameraGoal = mCameraPivot->createChildSceneNode(Vector3(0, 0, 15));
		// this is where the camera actually is
		mCameraNode = _camera->getSceneManager()->getRootSceneNode()->createChildSceneNode();
		mCameraNode->setPosition(mCameraPivot->getPosition() + mCameraGoal->getPosition());

		mCameraPivot->setFixedYawAxis(true);
		mCameraGoal->setFixedYawAxis(true);
		mCameraNode->setFixedYawAxis(true);

		// our model is quite small, so reduce the clipping planes
		_camera->setNearClipDistance(5);
		_camera->setFarClipDistance(10000);
		mCameraNode->attachObject(_camera);
		mPivotPitch = 0;




  //Pruebo a crear algo y ponerlo---------
  _sceneMgr->setAmbientLight(Ogre::ColourValue(0.8, 0.8, 0.8));
  //-------------------------------------

  //Quitar niebla------------------------------------------------
  Ogre::ColourValue fadeColour(0.0, 0.0, 0.0);
  _viewport->setBackgroundColour(fadeColour);
  _sceneMgr->setFog(Ogre::FOG_LINEAR, fadeColour, 0.0, 0, 0);
  _sceneMgr->setFog(Ogre::FOG_EXP, fadeColour, 0.000);
  //-----------------------------------------------------


  //Camara MiniMapa-------------------
  _cameraMiniMap = _sceneMgr->getCamera("MiniMapCamera");
  _cameraMiniMap->setPosition(Ogre::Vector3(0,50,50));//Pongo la camara en el mismo sitio, deberia ver lo mismo dos veces
  _cameraMiniMap->lookAt(Ogre::Vector3(0,0,0));//bajar el 60 un poco
  _cameraMiniMap->setNearClipDistance(5);
  _cameraMiniMap->setFarClipDistance(10000);
  //--------------------------------
  
  //Pj------------------------
  Entity* _entPj = _sceneMgr->createEntity("entPJ","Circle.mesh");
  SceneNode* _snPj = _sceneMgr->createSceneNode("PjSceneNode");
  _snPj->attachObject(_entPj);
  _sceneMgr->getRootSceneNode()->addChild(_snPj);
  _pj=new Pj;

  // -----------------------------

  //Ghost------------------------
  Entity* _entGhost = _sceneMgr->createEntity("entGhost","ghost.mesh");
  SceneNode* _snGhost = _sceneMgr->createSceneNode("GhostSceneNode");
  _snGhost->attachObject(_entGhost);
  _sceneMgr->getRootSceneNode()->addChild(_snGhost);
  _ghost=new Pj;

  _animGhost = _sceneMgr->getEntity("entGhost")->getAnimationState("moveIntro");
  _animGhost->setEnabled(true);
  _animGhost->setLoop(true);
  _animGhost->setTimePosition(0.0);


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
 

  // Mapa -------------------------
  //Pj------------------------
  Entity* _entMap = _sceneMgr->createEntity("entMap","tablero.mesh");
  SceneNode* _snMap = _sceneMgr->createSceneNode("MapSceneNode");
  _snMap->attachObject(_entMap);
  _snMap->setPosition(0,0,0); //x,y,z
  _snMap->setScale(23,23,23);
  _sceneMgr->getRootSceneNode()->addChild(_snMap);


  // Interfaz --------------------
  CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  sheet->getChildAtIdx(0)->setVisible(false); //PLAY
  sheet->getChildAtIdx(1)->setVisible(false); //CREDITOS
  sheet->getChildAtIdx(2)->setVisible(false);
  sheet->getChildAtIdx(3)->setVisible(true);

  //Boton Salir Juego---------------------------------------
  CEGUI::Window* quitButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button","Ex1/QuitButton");
  quitButton->setText("Exit");
  quitButton->setSize(CEGUI::USize(CEGUI::UDim(0.15,0),CEGUI::UDim(0.05,0)));
  quitButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.05,0),CEGUI::UDim(0.8,0)));
  quitButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&PlayState::quit,this));
  //-------------------------------------------------------
  
  sheet->addChild(quitButton);

  // Prueba Grafo ------------------------------
  try{
    _importer = new Importer;
    _scene = new Scene;

    // Parse data...
    _importer->parseScene("./data/output.xml", _scene);

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
        _snPoint->setScale(1,1,1);
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

  //Prueba de cambio de posicion segun el vertice ---
  _snGhost->setPosition(_scene->getGraph()->getVertex(11)->getData().getPosition());
  _nowGhost=_scene->getGraph()->getVertex(11);
  _nextGhost=NULL;
  //------------------------------------------------


  // Grafo, adyacentes ----
  _adjVer = _scene->getGraph()->adjacents(_now->getData().getIndex());
  std::vector<GraphVertex*>::const_iterator it;
  for (it = _adjVer.begin();it != _adjVer.end();++it){
    Node _aux=(*it)->getData();
    Ogre::Vector3 _vecMove=_aux.getPosition()-_now->getData().getPosition();
    _possibleMoves.push_back(_vecMove);
  }
  //----------------------

  _exitGame = false;
}


void 
Pacman::PlayState::updateCamera(Real deltaTime)
{
		SceneNode* _snPj =_sceneMgr->getSceneNode("PjSceneNode");
		// place the camera pivot roughly at the character's shoulder
		mCameraPivot->setPosition(_snPj->getPosition() + Ogre::Vector3::UNIT_Y * CAM_HEIGHT);
		// move the camera smoothly to the goal
		Vector3 goalOffset = mCameraGoal->_getDerivedPosition() - mCameraNode->getPosition();
		mCameraNode->translate(goalOffset * deltaTime * 9.0f);
		// always look at the pivot
		mCameraNode->lookAt(mCameraPivot->_getDerivedPosition(), Ogre::Node::TS_WORLD);
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

  //Actualizamos Camara---------------------------------
  updateCamera(_deltaT);
  


  //CEGUI --------------------------------------
  CEGUI::Window* rootWin = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  CEGUI::Window* ex1 = rootWin->getChild("CamWin");
  CEGUI::Window* RTTWindow = ex1->getChild("RTTWindow");
  RTTWindow->invalidate();
  //-------------------------------------------

  // Movimiento -------------------------
  Vector3 _aux=_snPj->getPosition();
  _aux.x=round(_aux.x);
  _aux.z=round(_aux.z);
  if(_now->getData().getPosition()!=_aux){
    _exitVertex=true;
  }else{
    _exitVertex=false;
  }

  if(_pj->isMoving()){
    _snPj->setPosition(Vector3(_snPj->getPosition())+=_pj->getDesp());
    //_now=NULL;
  }
  if(_exitVertex){
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

    // Los movimientos posibles han de ser ------------
    // en el que me estoy moviendo y el opuesto -------
    _possibleMoves.clear();
    _possibleMoves.push_back(_pj->getDesp());
    _possibleMoves.push_back(Ogre::Vector3(0,0,0)-(_pj->getDesp()));
    //-------------------------------------------------
    //-------------------------------------------------
  }
  }

  

  //--------------------

  
  //Update Ghost ----
  updateGhost();
  //-----------------


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
  SceneNode* _snPj =_sceneMgr->getSceneNode("PjSceneNode");
  switch(e.key){
    case OIS::KC_S:{ //ANTES ERA A
      // Compruebo si esta direccion esta en las direciones posibles---
      std::vector<Ogre::Vector3>::const_iterator it;
      bool _check =true;
      for (it = _possibleMoves.begin();it != _possibleMoves.end() && _check;++it){
        Ogre::Vector3 _aux=*it;
        if(_aux.z>0){
          cout << "Voy!" << endl;
          _pj->setMoving(true);
          _pj->setDesp(Vector3(0,0,0.03));
          _pj->setOrientation(4);
          _check=false;

        }else{
          cout << "No se puede ir hacia alli" << endl;
        }
      }
      //--------------------------------------------------------------
      
      
      break;}
    case OIS::KC_W:{//ANTES ERA D
      // Compruebo si esta direccion esta en las direcionesposibles---
      std::vector<Ogre::Vector3>::const_iterator it;
      bool _check =true;
      for (it = _possibleMoves.begin();it != _possibleMoves.end() && _check;++it){
        Ogre::Vector3 _aux=*it;
        if(_aux.z<0){
          cout << "Voy!" << endl;
          _pj->setMoving(true);
          _pj->setDesp(Vector3(0,0,-0.03));
          _pj->setOrientation(2);
          _check=false;
        }else{
          cout << "No se puede ir hacia alli" << endl;
        }
      }
      //------------------------------------------------------
      break;}
    case OIS::KC_A:{ //ANTES ERA W
      // Compruebo si esta direccion esta en las direciones posibles---
      std::vector<Ogre::Vector3>::const_iterator it;
      bool _check =true;
      for (it = _possibleMoves.begin();it != _possibleMoves.end() && _check;++it){
        Ogre::Vector3 _aux=*it;
        if(_aux.x<0){
          cout << "Voy!" << endl;
          _pj->setMoving(true);
          _pj->setDesp(Vector3(-0.03,0,0));
          _pj->setOrientation(1);
          _check=false;
        }else{
          cout << "No se puede ir hacia alli" << endl;
        }
      }
      //---------------------------------------------
      break;}
    case OIS::KC_D:{//ANTES ERA S
      // Compruebo si esta direccion esta en las direciones posibles---
      std::vector<Ogre::Vector3>::const_iterator it;
      bool _check =true;
      for (it = _possibleMoves.begin();it != _possibleMoves.end() && _check;++it){
        Ogre::Vector3 _aux=*it;
        if(_aux.x>0){
          cout << "Voy!" << endl;
          _pj->setMoving(true);
          _pj->setDesp(Vector3(0.03,0,0));
          _pj->setOrientation(3);
          _check=false;
        }else{
          cout << "No se puede ir hacia alli" << endl;
        }
      }
      }
      //-----------------------------------------------
      break;
   	case OIS::KC_R:
      _snPj->roll(Ogre::Degree(-90));
      break;
    case OIS::KC_SPACE:
      _animState = _sceneMgr->getEntity("entPJ")->getAnimationState("walking");
      _animState->setEnabled(true);
      _animState->setLoop(true);
      _animState->setTimePosition(0.0);
      break;
  }
  //----------------------------

  // Actualizo la orientacion del Pj ---------

  cout << "Orientacion " << _pj->getOrientation() << endl;


  //-------------------------------------------
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
  
    std::vector<GraphVertex*> _vertices=_scene->getGraph()->getVertexes();
    std::vector<GraphVertex*>::const_iterator it;
    for (it = _vertices.begin();it != _vertices.end();++it){
      Node _aux=(*it)->getData();
      if(_aux.getPosition()==_pos){
        if(_now!=*it){
          res=true;
        }
        
      }
    }
  //}
  
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

void
Pacman::PlayState::updateGhost()
{
  
  SceneNode* _snGhost =_sceneMgr->getSceneNode("GhostSceneNode");

  //Movimiento del ghost ------------
  

  //Si el next es nulo estoy en un vertice------
  if(_nextGhost==NULL){
    //Calculo adyacentes-----------------
    _adjVerGhost = _scene->getGraph()->adjacents(_nowGhost->getData().getIndex());
    //-----------------------------------

    //Tiro Dados-----------------------------
    int _go=0+rand()%(_adjVerGhost.size()-0);
    //----------------------------------------

    // Auxiliar para ver haia donde voy ------
    Vector3 _aux = _adjVerGhost[_go]->getData().getPosition()-_nowGhost->getData().getPosition();
    _nextGhost=_adjVerGhost[_go];
    //---------------------------------------

    //El auxiliar me da la direccion---------
    if(_aux.z>0){
      _ghost->setDesp(Ogre::Vector3(0,0,0.03));
    }
    if(_aux.z<0){
      _ghost->setDesp(Ogre::Vector3(0,0,-0.03));
    }
    if(_aux.x>0){
      _ghost->setDesp(Ogre::Vector3(0.03,0,0));
    }
    if(_aux.x<0){
      _ghost->setDesp(Ogre::Vector3(-0.03,0,0));
    }
    //------------------------------------

    _ghost->setMoving(true);


  }
  //---------------------------------------------------------
  if(_ghost->isMoving()){
    _snGhost->setPosition(_snGhost->getPosition()+_ghost->getDesp());
    //Ver cuando he de parar---

    Vector3 _res=_nextGhost->getData().getPosition()-_snGhost->getPosition();
    cout << "Distancia " << _res.length() << endl;
    if(_res.length()<=0.05){
      _nowGhost=_nextGhost;
      _nextGhost=NULL;
      _ghost->setMoving(false);
      _adjVerGhost.clear();
    }
  }

  if (_animGhost != NULL) {
    if (_animGhost->hasEnded()) {
      _animGhost->setTimePosition(0.0);
      _animGhost->setEnabled(false);
    }
    else {
      _animGhost->addTime(_deltaT);
    }
  }
  

  

  //------------------------
  // ----------------- 
}