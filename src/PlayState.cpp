#include "PlayState.h"
#include "PauseState.h"
#include "GameOverState.h"
#include "CEGUI.h"
#define CAM_HEIGHT 5 //Altura de la camara


template<> Pacman::PlayState* Ogre::Singleton<Pacman::PlayState>::msSingleton = 0;

// Prueba de animacion -----
Ogre::AnimationState *_animState;
Ogre::AnimationState *_animGhost;
Ogre::AnimationState *_animGhost2;
//--------------------------

// Donde vengo y donde voy ---
Pacman::GraphVertex* _now;
Pacman::GraphVertex* _next;
std::vector<Pacman::GraphVertex*> _adjVer;
//std::vector<Ogre::Vector3> _possibleMoves;
// ----------------------------

// Donde vengo y donde voy Ghost  ---
Pacman::GraphVertex* _nowGhost;
Pacman::GraphVertex* _nextGhost;
std::vector<Pacman::GraphVertex*> _adjVerGhost;
//std::vector<Ogre::Vector3> _possibleMovesGhost;
// ----------------------------

// Donde vengo y donde voy Ghost2  ---
Pacman::GraphVertex* _nowGhost2;
Pacman::GraphVertex* _nextGhost2;
std::vector<Pacman::GraphVertex*> _adjVerGhost2;
// ----------------------------

// Vector de Rayos --------------------
std::vector<Ogre::SceneNode*> _rayVector;
std::vector<Ogre::SceneNode*> _superRayVector;
// --------------------------------------

// Blink del resapwn -------------------
int _cont=60;
bool _blink=true;
int _contState=0;
std::vector<Pacman::GraphVertex*> _respawnVertex;
//--------------------------------------


void
Pacman::PlayState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();

  // Se recupera el gestor de escena y la cámara.----------------
  _sceneMgr = _root->getSceneManager("SceneManager");
  _camera = _sceneMgr->createCamera("PlayCamera");
  _viewport = _root->getAutoCreatedWindow()->addViewport(_camera);
  _viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 0.0));// Nuevo background colour.
  //Vista Ortho----------------------
  double width = _viewport->getActualWidth();
  double height = _viewport->getActualHeight();
  _camera->setAspectRatio(width / height);
  //-------------------------------------

  //Sonido--------------------------------------------
 	//GameManager::getSingletonPtr()->_mainTrack->unload(); //Quitamos el sonido que habia antes y abajo cargamos otro
  GameManager::getSingletonPtr()->_simpleEffect = GameManager::getSingletonPtr()->_pSoundFXManager->load("vaccum.wav");

  //-----------------------------------------------------------

  //Camara--------------------------------------------------------------
  //crear un pivote en más o menos por  el hombro del personaje
	mCameraPivot = _camera->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	//camara que girara alrededor del pivote
	mCameraGoal = mCameraPivot->createChildSceneNode(Vector3(0, 0, 15));
	mCameraNode = _camera->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	mCameraNode->setPosition(mCameraPivot->getPosition() + mCameraGoal->getPosition());

	mCameraPivot->setFixedYawAxis(true);
	mCameraGoal->setFixedYawAxis(true);
	mCameraNode->setFixedYawAxis(true);

	//nuestro modelo es bastante pequeña, para reducir los planos de recorte
	_camera->setNearClipDistance(5);
	_camera->setFarClipDistance(10000);
	mCameraNode->attachObject(_camera);
	mPivotPitch = 0;
	//-------------------------------------------------------------------------

  //Luz ambiente---------
  _sceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
  //-------------------------------------
  

  //Camara MiniMapa-------------------
  _cameraMiniMap = _sceneMgr->getCamera("MiniMapCamera");
  _cameraMiniMap->setPosition(Ogre::Vector3(0,50,50));//Pongo la camara en el mismo sitio, deberia ver lo mismo dos veces
  _cameraMiniMap->lookAt(Ogre::Vector3(0,0,0));//bajar el 60 un poco
  _cameraMiniMap->setNearClipDistance(5);
  _cameraMiniMap->setFarClipDistance(10000);
  //--------------------------------
  

  //Pj------------------------
  Entity* _entPj = _sceneMgr->createEntity("entPJ","personaje.mesh");
  SceneNode* _snPj = _sceneMgr->createSceneNode("PjSceneNode");
  _snPj->attachObject(_entPj);
  _snPj->setScale(0.5,0.5,0.5);
  _snPj->showBoundingBox(true);
  _sceneMgr->getRootSceneNode()->addChild(_snPj);
  _pj=new Pj;

  // -----------------------------

  //Ghost------------------------
  Entity* _entGhost = _sceneMgr->createEntity("entGhost","ghost.mesh");
  SceneNode* _snGhost = _sceneMgr->createSceneNode("GhostSceneNode");
  _snGhost->attachObject(_entGhost);
  _snGhost->setScale(0.5,0.5,0.5);
  _snGhost->showBoundingBox(true);
  _sceneMgr->getRootSceneNode()->addChild(_snGhost);
  _ghost=new Pj;

  _animGhost = _sceneMgr->getEntity("entGhost")->getAnimationState("moveIntro");
  _animGhost->setEnabled(true);
  _animGhost->setLoop(true);
  _animGhost->setTimePosition(0.0);
  //-------------------------------------------------

  //Ghost 2------------------------
  Entity* _entGhost2 = _sceneMgr->createEntity("entGhost2","ghost.mesh");
  SceneNode* _snGhost2 = _sceneMgr->createSceneNode("GhostSceneNode2");
  _snGhost2->showBoundingBox(true);
  _snGhost2->setScale(0.5,0.5,0.5);
  _snGhost2->attachObject(_entGhost2);
  _sceneMgr->getRootSceneNode()->addChild(_snGhost2);
  _ghost2=new Pj;

  _animGhost2 = _sceneMgr->getEntity("entGhost2")->getAnimationState("moveIntro");
  _animGhost2->setEnabled(true);
  _animGhost2->setLoop(true);
  _animGhost2->setTimePosition(0.0);
  //-------------------------------------------------

  // Mapa -------------------------
  //Pj------------------------
  Entity* _entMap = _sceneMgr->createEntity("entMap","tablero.mesh");
  SceneNode* _snMap = _sceneMgr->createSceneNode("MapSceneNode");
  _snMap->attachObject(_entMap);
  _snMap->setPosition(0,0,0); //x,y,z
  _snMap->setScale(25,25,25);
  _sceneMgr->getRootSceneNode()->addChild(_snMap);
  //---------------------------------------------------------

  //Skybox -----------------------------------------------
	_sceneMgr->setSkyBox(true, "MaterialSkybox");
	//-------------------------------------------------

	//Carga-----------------
	createGUI();
  loadGraph();
  loadAnimations(_entPj);

  //Luz Escena-------------------
  Ogre::Light* spotLight = _sceneMgr->createLight("SpotLight");
  //Colores difusos a azul
  spotLight->setDiffuseColour(0, 0, 1.0);
  spotLight->setSpecularColour(0, 0, 1.0);
	//Tipo de luz y sus repectivos direcciones y posiciones (Actua modo linterna)
	spotLight->setType(Ogre::Light::LT_SPOTLIGHT);
	spotLight->setDirection(-1, -1, 0);
	spotLight->setPosition(Ogre::Vector3(200, 200, 0));
	//Para desvanecer la luz, Grados de desvanecimiento...
	spotLight->setSpotlightRange(Ogre::Degree(35), Ogre::Degree(50));
	//---------------------------------------------------------------


  //Prueba de cambio de posicion segun el vertice ---
  _snPj->setPosition(_scene->getGraph()->getVertex(2)->getData().getPosition());
  _now=_scene->getGraph()->getVertex(2);
  //------------------------------------------------

  //Prueba de cambio de posicion segun el vertice ---
  _snGhost->setPosition(_scene->getGraph()->getVertex(11)->getData().getPosition());
  _nowGhost=_scene->getGraph()->getVertex(11);
  _nextGhost=NULL;
  //------------------------------------------------

  //Prueba de cambio de posicion segun el vertice ---
  _snGhost2->setPosition(_scene->getGraph()->getVertex(22)->getData().getPosition());
  _nowGhost2=_scene->getGraph()->getVertex(22);
  _nextGhost2=NULL;
  //------------------------------------------------


  // Grafo, adyacentes ----
  _adjVer = _scene->getGraph()->adjacents(_now->getData().getIndex());
  //----------------------

  // Colision con fantasma ---
  _collisionWithGhostDetected=false;
  // ------------------------


  _exitGame = false;
  mKeyDirection = Vector3::ZERO;

  _punt=0;
 
}

void 
Pacman::PlayState::createGUI()
{
	//CEGUI--------------------------------------------------------------
  // Interfaz Intro (Ocultar botones)--------------------
  CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  sheet->getChildAtIdx(0)->setVisible(false); //PLAY
  sheet->getChildAtIdx(1)->setVisible(false); //CREDITOS
  sheet->getChildAtIdx(2)->setVisible(false); //PUNTUACIONES
  sheet->getChildAtIdx(3)->setVisible(false); //LOGO
  sheet->getChildAtIdx(4)->setVisible(true);



  //Interfaz Cegui---------------------------------------
  CEGUI::ImageManager::getSingleton().addFromImageFile("Background","cabecera.jpg");
  CEGUI::Window* sheetBG =  CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticImage","background_wnd2");
  sheetBG->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f, 0.0f),CEGUI::UDim(0.0, 0)));
  sheetBG->setSize( CEGUI::USize(CEGUI::UDim(1.0, 0), CEGUI::UDim(0.10, 0)));
  sheetBG->setProperty("Image","Background");
  sheetBG->setProperty("FrameEnabled","False");
  sheetBG->setProperty("BackgroundEnabled", "False");

  CEGUI::Window* pauseButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","Ex1/PauseButton");
  pauseButton->setText("[font='Carton_Six'] Pausa ");
  pauseButton->setSize(CEGUI::USize(CEGUI::UDim(0.15,0),CEGUI::UDim(0.5,0)));
  pauseButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.6,0),CEGUI::UDim(0.3,0)));
  pauseButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&PlayState::pauseB,this));

  CEGUI::Window* quitButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","Ex1/QuitButton");
  quitButton->setText("[font='Carton_Six'] Salir ");
  quitButton->setSize(CEGUI::USize(CEGUI::UDim(0.15,0),CEGUI::UDim(0.5,0)));
  quitButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.8,0),CEGUI::UDim(0.3,0)));
  quitButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&PlayState::quit,this));

  sheetBG->addChild(pauseButton);
  sheetBG->addChild(quitButton);
  sheet->addChild(sheetBG);

}
void 
Pacman::PlayState::loadGraph()
{
	// Prueba Grafo ------------------------------
  try{
     _importer = new Importer;
     _scene = new Scene;

    // Parse data...
     _importer->parseScene("./data/output.xml", _scene);
 

      std::vector<GraphVertex*> _vertices=_scene->getGraph()->getVertexes();
      std::vector<GraphVertex*>::const_iterator it;
      int i =1;
      NUM_VERTEX=0;
      for (it = _vertices.begin();it != _vertices.end();++it){
        Node _aux=(*it)->getData();

        // Nodos Respawn -------------
        _respawnVertex.push_back(*it);
        //----------------------------

      // Ray -------------------------
		   Entity* _entRay = _sceneMgr->createEntity("_entRay"+Ogre::StringConverter::toString(i),"ray.mesh");
		   SceneNode* _snRay = _sceneMgr->createSceneNode("RaySceneNode"+Ogre::StringConverter::toString(i));
		  _snRay->attachObject(_entRay);
		  _snRay->setPosition(_aux.getPosition());
      _sceneMgr->getRootSceneNode()->addChild(_snRay);
		  if(_aux.getIndex() == 8 || _aux.getIndex() == 15 || _aux.getIndex() == 22 || _aux.getIndex() == 28){
        _snRay->setScale(0.6,0.6,0.6);
        _superRayVector.push_back(_snRay);
      }else{
        _snRay->setScale(0.3,0.3,0.3);
        _rayVector.push_back(_snRay);
      }
		  //--------------------------------------------------------
        i+=1;
        NUM_VERTEX+=1;
      }


  }
  catch (...){
      cerr << "Unexpected exception!" << endl;
  }

}

void 
Pacman::PlayState::loadAnimations(Entity* character)
{
	String animNames[] ={"walking","idle"};
	
		for (int i = 0; i < 2; i++)
		{
			mAnims[i] = character->getAnimationState(animNames[i]);
			
		}

		mAnims[1]->setEnabled(true);
		mAnims[1]->setLoop(true);
    mAnims[1]->setTimePosition(0);

}
void 
Pacman::PlayState::updateCamera(Real deltaTime)
{
		SceneNode* _snPj =_sceneMgr->getSceneNode("PjSceneNode");
		mCameraPivot->setPosition(_snPj->getPosition() + Ogre::Vector3::UNIT_Y * CAM_HEIGHT);
		//mover la cámara sin problemas a la meta
		Vector3 goalOffset = mCameraGoal->_getDerivedPosition() - mCameraNode->getPosition();
		mCameraNode->translate(goalOffset * deltaTime * 9.0f);
		//siempre ver el pivote
		mCameraNode->lookAt(mCameraPivot->_getDerivedPosition(), Ogre::Node::TS_WORLD);
}
void 
Pacman::PlayState::updateCameraGoal(Real deltaYaw, Real deltaPitch, Real deltaZoom)
{
		mCameraPivot->yaw(Degree(deltaYaw), Ogre::Node::TS_WORLD);

		// bound the pitch
		if (!(mPivotPitch + deltaPitch > 25 && deltaPitch > 0) &&
			!(mPivotPitch + deltaPitch < -60 && deltaPitch < 0))
		{
			mCameraPivot->pitch(Degree(deltaPitch), Ogre::Node::TS_LOCAL);
			mPivotPitch += deltaPitch;
		}
		
		Real dist = mCameraGoal->_getDerivedPosition().distance(mCameraPivot->_getDerivedPosition());
		Real distChange = deltaZoom * dist;

		// bound the zoom
		if (!(dist + distChange < 8 && distChange < 0) &&
			!(dist + distChange > 25 && distChange > 0))
		{
			mCameraGoal->translate(0, 0, distChange, Ogre::Node::TS_LOCAL);
		}
}

void
Pacman::PlayState::exit ()
{

  cout << "\nChange State llego a metodo exit\n" << endl;

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
  

}

bool
Pacman::PlayState::frameStarted
(const Ogre::FrameEvent& evt)
{
  //Actualizacion de Variables----------------
  _deltaT = evt.timeSinceLastFrame;
  SceneNode* _snPj =_sceneMgr->getSceneNode("PjSceneNode");
  //----------------------------------------------

  //Actualizamos Camara---------------------------
  updateCamera(_deltaT);
  //----------------------------------------------

  //Actualizamos la rotacion del personaje------
  updateCharacter(_deltaT);
  //--------------------------------------------

  //CEGUI --------------------------------------
  CEGUI::Window* rootWin = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
 	CEGUI::Window* ex1 = rootWin->getChild("CamWin");
  CEGUI::Window* RTTWindow = ex1->getChild("RTTWindow");
  RTTWindow->invalidate();
  //-------------------------------------------

  
  
  //Update Ghost ----
  updateGhost();
  updateGhost2();
  //-----------------
  
  //Update Collisiones ---
  updateCollisions();
  updateRayCollisions();
  //----------------------
  
  // Update Pj ---------
  updatePj(_deltaT);
  //--------------------
  



  //-------------------------------------
  return true;
}

void 
Pacman::PlayState::updateCharacter(Real deltaTime)
{
		mDirection = Ogre::Vector3::ZERO;   

		SceneNode* _snPj =_sceneMgr->getSceneNode("PjSceneNode");

		if (mKeyDirection != Ogre::Vector3::ZERO)
		{
			//Calcula la direccion basado en las direcciones del personaje
			mDirection += mKeyDirection.z * mCameraNode->getOrientation().zAxis();
			mDirection += mKeyDirection.x * mCameraNode->getOrientation().xAxis();
			mDirection.y = 0;
			mDirection.normalise(); //Normalizamos el vector

			Quaternion orientationPJ = _snPj->getOrientation().zAxis().getRotationTo(mDirection);

			Real yawPJ = orientationPJ.getYaw().valueDegrees();
			
			_snPj->yaw(Degree(yawPJ));
		}
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

  // Tecla g --> GameOverState.-------
  if (e.key == OIS::KC_G) {
    changeState(GameOverState::getSingletonPtr());
  }
  //-----------------

  if (e.key == OIS::KC_S || e.key == OIS::KC_W || e.key == OIS::KC_D || e.key == OIS::KC_A) {
        mAnims[1]->setEnabled(false);
        mAnims[0]->setTimePosition(0);
        mAnims[0]->setEnabled(true);
        mAnims[0]->setLoop(true);
        mAnims[0]->setTimePosition(0);


        GameManager::getSingletonPtr()->_simpleEffect->play();
  }


  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown(static_cast<CEGUI::Key::Scan>(e.key));
  CEGUI::System::getSingleton().getDefaultGUIContext().injectChar(e.text);
  //-------------------------------
  
  
  //Movimiento PJ----------------
 
  switch(e.key){
    case OIS::KC_S:{ 
    	mKeyDirection.z = 1;

      // Compruebo si esta direccion esta en las direciones posibles---
      std::vector<GraphVertex*>::const_iterator it;
      bool _check =true;
      int _go=0;
      for (it = _adjVer.begin();it != _adjVer.end() && _check;++it){
        Ogre::Vector3 _aux=(*it)->getData().getPosition()-_now->getData().getPosition();
        if(_aux.z>0){
          cout << "Voy!" << endl;
          _pj->setMoving(true);
          _pj->setDesp(Vector3(0,0,0.03));
          _next=_adjVer[_go];
          _check=false;

        }else{
          cout << "No se puede ir hacia alli" << endl;
          _go++;
        }

      }
      	
      
      //--------------------------------------------------------------
      
      
      break;}
    case OIS::KC_W:{
    	mKeyDirection.z = -1;
      // Compruebo si esta direccion esta en las direcionesposibles---
      std::vector<GraphVertex*>::const_iterator it;
      bool _check =true;
      int _go=0;
      for (it = _adjVer.begin();it != _adjVer.end() && _check;++it){
        Ogre::Vector3 _aux=(*it)->getData().getPosition()-_now->getData().getPosition();
        if(_aux.z<0){
          cout << "Voy!" << endl;
          _pj->setMoving(true);
          _pj->setDesp(Vector3(0,0,-0.03));
          _next=_adjVer[_go];
          _check=false;

        }else{
          cout << "No se puede ir hacia alli" << endl;
          _go++;
        }
      }

      //------------------------------------------------------
      break;}
    case OIS::KC_A:{ 
    	mKeyDirection.x = -1;
      // Compruebo si esta direccion esta en las direciones posibles---
      std::vector<GraphVertex*>::const_iterator it;
      bool _check =true;
      int _go=0;
      for (it = _adjVer.begin();it != _adjVer.end() && _check;++it){
        Ogre::Vector3 _aux=(*it)->getData().getPosition()-_now->getData().getPosition();
        if(_aux.x<0){
          cout << "Voy!" << endl;
          _pj->setMoving(true);
          _pj->setDesp(Vector3(-0.03,0,0));
          _next=_adjVer[_go];
          _check=false;

        }else{
          cout << "No se puede ir hacia alli" << endl;
          _go++;
        }
      }

      //---------------------------------------------

      break;}
    case OIS::KC_D:{
    	mKeyDirection.x = 1;
      // Compruebo si esta direccion esta en las direciones posibles---
      std::vector<GraphVertex*>::const_iterator it;
      bool _check =true;
      int _go=0;
      for (it = _adjVer.begin();it != _adjVer.end() && _check;++it){
        Ogre::Vector3 _aux=(*it)->getData().getPosition()-_now->getData().getPosition();
        if(_aux.x>0){
          cout << "Voy!" << endl;
          _pj->setMoving(true);
          _pj->setDesp(Vector3(0.03,0,0));
          _next=_adjVer[_go];
          _check=false;

        }else{
          cout << "No se puede ir hacia alli" << endl;
          _go++;
        }
      }
      }

      //-----------------------------------------------
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
  if (e.key == OIS::KC_W && mKeyDirection.z == -1) mKeyDirection.z = 0;
	else if (e.key == OIS::KC_A && mKeyDirection.x == -1) mKeyDirection.x = 0;
	else if (e.key == OIS::KC_S && mKeyDirection.z == 1) mKeyDirection.z = 0;
	else if (e.key == OIS::KC_D && mKeyDirection.x == 1) mKeyDirection.x = 0;

  
  //-------------------------------
  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp(static_cast<CEGUI::Key::Scan>(e.key));
  //-------------------------------
}

void
Pacman::PlayState::mouseMoved
(const OIS::MouseEvent &e)
{
	updateCameraGoal(-0.05f * e.state.X.rel,-0.05f * e.state.Y.rel,-0.0005f * e.state.Z.rel);
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
Pacman::PlayState::pauseB(const CEGUI::EventArgs &e)
{
  pushState(PauseState::getSingletonPtr());
  return true;
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
    //cout << "Distancia " << _res.length() << endl;
    if(_res.length()<=0.035){
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

void
Pacman::PlayState::updateGhost2()
{
  
  SceneNode* _snGhost =_sceneMgr->getSceneNode("GhostSceneNode2");

  //Movimiento del ghost ------------
  //Si el next es nulo estoy en un vertice------
  if(_nextGhost2==NULL){
    //Calculo adyacentes-----------------
    _adjVerGhost2 = _scene->getGraph()->adjacents(_nowGhost2->getData().getIndex());
    //-----------------------------------

    //Tiro Dados-----------------------------
    int _go=0+rand()%(_adjVerGhost2.size()-0);
    //----------------------------------------

    // Auxiliar para ver haia donde voy ------
    Vector3 _aux = _adjVerGhost2[_go]->getData().getPosition()-_nowGhost2->getData().getPosition();
    _nextGhost2=_adjVerGhost2[_go];
    //---------------------------------------

    //El auxiliar me da la direccion---------
    if(_aux.z>0){
      _ghost2->setDesp(Ogre::Vector3(0,0,0.03));
    }
    if(_aux.z<0){
      _ghost2->setDesp(Ogre::Vector3(0,0,-0.03));
    }
    if(_aux.x>0){
      _ghost2->setDesp(Ogre::Vector3(0.03,0,0));
    }
    if(_aux.x<0){
      _ghost2->setDesp(Ogre::Vector3(-0.03,0,0));
    }
    //------------------------------------
    _ghost2->setMoving(true);

  }
  //---------------------------------------------------------
  if(_ghost2->isMoving()){
    _snGhost->setPosition(_snGhost->getPosition()+_ghost2->getDesp());
    //Ver cuando he de parar---

    Vector3 _res=_nextGhost2->getData().getPosition()-_snGhost->getPosition();
    //cout << "Distancia " << _res.length() << endl;
    if(_res.length()<=0.035){
      _nowGhost2=_nextGhost2;
      _nextGhost2=NULL;
      _ghost2->setMoving(false);
      _adjVerGhost2.clear();
    }
  }

  if (_animGhost2 != NULL) {
    if (_animGhost2->hasEnded()) {
      _animGhost2->setTimePosition(0.0);
      _animGhost2->setEnabled(false);
    }
    else {
      _animGhost2->addTime(_deltaT);
    }
  }
  //------------------------
  // ----------------- 
}

void
Pacman::PlayState::updatePj(Real _deltaTime)
{
  
  SceneNode* _snPj =_sceneMgr->getSceneNode("PjSceneNode");

  if(_next==NULL){
    //Calculo adyacentes-----------------
    _adjVer = _scene->getGraph()->adjacents(_now->getData().getIndex());
    //-----------------------------------
  }


  if(_pj->isMoving()){
    //mientras me muevo los adyacentes solo son desp y -desp
    _adjVer.clear();
    _adjVer.push_back(_now); 
    _adjVer.push_back(_next); 
    //-------------------------------------

    _snPj->setPosition(_snPj->getPosition()+_pj->getDesp());
    //Ver cuando he de parar---

    Vector3 _res=_next->getData().getPosition()-_snPj->getPosition();
    //Me paro --------------------
    if(_res.length()<=0.05){
      _now=_next;
      _next=NULL;
      _pj->setMoving(false);
      _adjVer.clear();
     
      //Animacion de idle
      mAnims[0]->setEnabled(false);
      mAnims[1]->setEnabled(true);
      mAnims[1]->setLoop(true);
      mAnims[1]->setTimePosition(0.0);
    }
    //-------------------------------
  }

  //Animaciones ------------------
  for (int i = 0; i < 2; i++)
  {
    if (mAnims[i] != NULL) {
      if (mAnims[i]->hasEnded()) {
        mAnims[i]->setTimePosition(0.0);
        mAnims[i]->setEnabled(false);
      }
      else {
        mAnims[i]->addTime(_deltaT);
      }


    } 
  }
  //-------------------------------

  
  //Actualizacion grafica segun el estado
  if(_pj->getState()=="normal"){
    
  }

  if(_pj->getState()=="super"){
    cout << "Estado super " << endl;
  } 

  if(_pj->getState()=="respawn"){
    //Blink--------------------
    if(_blink){
      _snPj->setVisible(false);
      if(--_cont<1){
        _blink=false;
      }
    }
    else{
      _snPj->setVisible(true);
      if(++_cont>59){
        _blink=true;
      }
    }
    //---------------------------

    //Cuando pasa X tiempo vuelva a estado normal
    _contState+=1;
    if(_contState>=600){
      _pj->changeState("normal");
      _contState=0;
      _snPj->setVisible(true);
    }
    //------------------------------------------
  }
  //----------------------------------


  //Si no tenemos vidas pasamos a GameOver ----
  if(_pj->getLives()==0){
    _sceneMgr->clearScene();
    changeState(GameOverState::getSingletonPtr());
  }
  //-------------------------------------------

  
}

void
Pacman::PlayState::updateCollisions()
{
  
  //Variables para la colision--------------------------------------------------------
  Ogre::AxisAlignedBox bbPj = _sceneMgr->getSceneNode("PjSceneNode")->_getWorldAABB();
  Ogre::AxisAlignedBox bbGhost1 = _sceneMgr->getSceneNode("GhostSceneNode")->_getWorldAABB();
  Ogre::AxisAlignedBox bbGhost2 = _sceneMgr->getSceneNode("GhostSceneNode2")->_getWorldAABB();
  //----------------------------------------------------------------------------------------

  //Colision Detectada ! -------------------------------------
  if(bbPj.intersects(bbGhost1) || bbPj.intersects(bbGhost2)){
  //----------------------------------------------------------   

    //Colision en caso de Pj normal ---  
    if(_pj->getState()=="normal"){
      cout << "Colision PJ - Ghost" << endl;
      _collisionWithGhostDetected=true;
      _pj->hit();
      _pj->changeState("respawn");
    }
    //---------------------------------

    //Colision en caso de Pj super-----  
    if(_pj->getState()=="super"){
      
      if(bbPj.intersects(bbGhost1)){
        // El fantasmas deberia cambiar de posicion y recomenzar su movimiento---

        //Nodo aleatorio que sea de tipo respawn---
        SceneNode* _auxGhost = _sceneMgr->getSceneNode("GhostSceneNode");
        int _go=0+rand()%(_respawnVertex.size()-0);
        _auxGhost->setPosition(_respawnVertex[_go]->getData().getPosition());
        //-----------------------------------------

        //Reseteo variables -----------------------
        _nowGhost=_respawnVertex[_go];
        _nextGhost=NULL;
        _punt+=300;
        //-----------------------------------------
      

        //----------------------------------------------------------------------
      }

      if(bbPj.intersects(bbGhost2)){
        // El fantasmas deberia cambiar de posicion y recomenzar su movimiento---

        //Nodo aleatorio que sea de tipo respawn---
        SceneNode* _auxGhost = _sceneMgr->getSceneNode("GhostSceneNode2");
        int _go=0+rand()%(_respawnVertex.size()-0);
        _auxGhost->setPosition(_respawnVertex[_go]->getData().getPosition());
        //-----------------------------------------

        //Reseteo variables -----------------------
        _nowGhost2=_respawnVertex[_go];
        _nextGhost2=NULL;
        _punt+=300;
        //-----------------------------------------
      

        //----------------------------------------------------------------------
      }

      
    }
    //---------------------------------

  }


}

void
Pacman::PlayState::updateRayCollisions()
{
  
  
  //Colisiones Rayos--------------------------------------

  Ogre::AxisAlignedBox bbPjSceneNode = _sceneMgr->getSceneNode("PjSceneNode")->_getWorldAABB();

  for (unsigned int i=0;i<_rayVector.size();++i){
    Ogre::SceneNode* _aux=_rayVector[i];
    Ogre::AxisAlignedBox bbRay = _aux->_getWorldAABB();
    if(bbPjSceneNode.intersects(bbRay)){
      _rayVector.erase(_rayVector.begin()+i);
      _punt+=100;
      cout << "Puntuacion: " << _punt << endl;
      cout << "Vidas: " << _pj->getLives() << endl;
      _aux->setVisible(false);
      cout << "\nRayos restantes: " << (_rayVector.size()) << "\n" << endl;
    }
  }
  //--------------------------------------------------

  // Si no quedan rayos por coger seria el final del nivel ----------------
  if(_rayVector.size()==0){
    cout << "\n Nivel Acabado \n" << endl;
  }
  //-----------------------------------------------------------------------

  // Colisiones SuperRayos ----------------------------------------------
  for (unsigned int i=0;i<_superRayVector.size();++i){
    Ogre::SceneNode* _aux=_superRayVector[i];
    Ogre::AxisAlignedBox bbRay = _aux->_getWorldAABB();
    if(bbPjSceneNode.intersects(bbRay)){
      _superRayVector.erase(_superRayVector.begin()+i);
      _punt+=300;
      cout << "Puntuacion: " << _punt << endl;
      cout << "Vidas: " << _pj->getLives() << endl;
      _aux->setVisible(false);
      _pj->changeState("super");
    }
  }
  //---------------------------------------------------------------------
  


}