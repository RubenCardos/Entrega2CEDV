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
Ogre::AnimationState *_animGhost3;
//--------------------------

// Donde vengo y donde voy ---
Pacman::GraphVertex* _now;
Pacman::GraphVertex* _next;
std::vector<Pacman::GraphVertex*> _adjVer;
// ----------------------------

// Donde vengo y donde voy Ghost---
Pacman::GraphVertex* _nowGhost;
Pacman::GraphVertex* _nextGhost;
std::vector<Pacman::GraphVertex*> _adjVerGhost;
// --------------------------------

// Donde vengo y donde voy Ghost2---
Pacman::GraphVertex* _nowGhost2;
Pacman::GraphVertex* _nextGhost2;
std::vector<Pacman::GraphVertex*> _adjVerGhost2;
// ---------------------------------

// Donde vengo y donde voy Ghost3---
Pacman::GraphVertex* _nowGhost3;
Pacman::GraphVertex* _nextGhost3;
std::vector<Pacman::GraphVertex*> _adjVerGhost3;
// ---------------------------------

// Vector de Rayos --------------------
std::vector<Ogre::SceneNode*> _rayWareHouse;
std::vector<Ogre::SceneNode*> _rayVector;
std::vector<Ogre::SceneNode*> _superRayWareHouse;
std::vector<Ogre::SceneNode*> _superRayVector;
// --------------------------------------

// Blink del resapwn -------------------
int _cont=60;
bool _blink=true;
int _contState=0;
std::vector<Pacman::GraphVertex*> _respawnVertex;
//--------------------------------------

// Blink del super -------------------
//int _contSuper=1500;
int _contSuper=3000;
//--------------------------------------

//Records------------------------------
bool _inRecords=false;
std::string _name="";
//-------------------------------------

//Next Level-------------------------
int _contNextLevel=120;
bool _nextLevel=false;
//-----------------------------------

void
Pacman::PlayState::enter ()
{

  cout <<"\nEntro en el PlayState, metodo enter\n"<< endl;

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
  _sceneMgr->getRootSceneNode()->addChild(_snPj);
  _pj=new Pj;

  // -----------------------------

  //Ghost------------------------
  Entity* _entGhost = _sceneMgr->createEntity("entGhost","ghost.mesh");
  SceneNode* _snGhost = _sceneMgr->createSceneNode("GhostSceneNode");
  _snGhost->attachObject(_entGhost);
  _snGhost->setScale(0.5,0.5,0.5);
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
  _snGhost2->setScale(0.5,0.5,0.5);
  _snGhost2->attachObject(_entGhost2);
  _sceneMgr->getRootSceneNode()->addChild(_snGhost2);
  _ghost2=new Pj;

  _animGhost2 = _sceneMgr->getEntity("entGhost2")->getAnimationState("moveIntro");
  _animGhost2->setEnabled(true);
  _animGhost2->setLoop(true);
  _animGhost2->setTimePosition(0.0);
  //-------------------------------------------------
  

  //Ghost 3------------------------
  Entity* _entGhost3 = _sceneMgr->createEntity("entGhost3","ghost.mesh");
  SceneNode* _snGhost3 = _sceneMgr->createSceneNode("GhostSceneNode3");
  _snGhost3->setScale(0.5,0.5,0.5);
  _snGhost3->attachObject(_entGhost3);
  _sceneMgr->getRootSceneNode()->addChild(_snGhost3);
  _ghost3=new Pj;

  _animGhost3 = _sceneMgr->getEntity("entGhost3")->getAnimationState("moveIntro");
  _animGhost3->setEnabled(true);
  _animGhost3->setLoop(true);
  _animGhost3->setTimePosition(0.0);
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
  //------------------------

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

  //Prueba de cambio de posicion segun el vertice ---
  _snGhost3->setPosition(_scene->getGraph()->getVertex(28)->getData().getPosition());
  _nowGhost3=_scene->getGraph()->getVertex(28);
  _nextGhost3=NULL;
  //------------------------------------------------

  // Grafo, adyacentes ----
  _adjVer = _scene->getGraph()->adjacents(_now->getData().getIndex());
  //----------------------

  // Colision con fantasma ---
  _collisionWithGhostDetected=false;
  // ------------------------


  _exitGame = false;
  mKeyDirection = Vector3::ZERO;

  _pointsRay=0;
  _pointsGhost=0;
  _inRecords=false;

  
 
}

void 
Pacman::PlayState::createGUI()
{
	//CEGUI--------------------------------------------------------------
  
  SceneNode* _snPj =_sceneMgr->getSceneNode("PjSceneNode");

  // Interfaz Intro (Ocultar botones)--------------------
  CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  sheet->getChildAtIdx(0)->setVisible(false); //PLAY
  sheet->getChildAtIdx(1)->setVisible(false); //CREDITOS
  sheet->getChildAtIdx(2)->setVisible(false); //PUNTUACIONES
  sheet->getChildAtIdx(3)->setVisible(false); //LOGO
  sheet->getChildAtIdx(4)->setVisible(false); //LOGO
  sheet->getChildAtIdx(5)->setVisible(true);  //CAMARA



  //Interfaz Cegui Cabecera--------------------------------------
  
  CEGUI::Window* sheetBG =  CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticImage","background_wnd2");
  sheetBG->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f, 0.0f),CEGUI::UDim(0.0, 0)));
  sheetBG->setSize( CEGUI::USize(CEGUI::UDim(1.0, 0), CEGUI::UDim(0.10, 0)));
  sheetBG->setProperty("Image","Background");
  sheetBG->setProperty("FrameEnabled","False");
  sheetBG->setProperty("BackgroundEnabled", "False");

  CEGUI::Window* pauseButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","PauseButton");
  pauseButton->setText("[font='Carton_Six'] Pause ");
  pauseButton->setSize(CEGUI::USize(CEGUI::UDim(0.15,0),CEGUI::UDim(0.5,0)));
  pauseButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.6,0),CEGUI::UDim(0.3,0)));
  pauseButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&PlayState::pauseB,this));

  CEGUI::Window* quitButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","QuitButton");
  quitButton->setText("[font='Carton_Six'] Exit ");
  quitButton->setSize(CEGUI::USize(CEGUI::UDim(0.15,0),CEGUI::UDim(0.5,0)));
  quitButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.8,0),CEGUI::UDim(0.3,0)));
  quitButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&PlayState::quit,this));

  CEGUI::Window* textPoints = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText","textPoints");
  textPoints->setText("[font='Carton_Six'] 000");
  textPoints->setSize(CEGUI::USize(CEGUI::UDim(0.20,0),CEGUI::UDim(0.70,0)));
  textPoints->setXPosition(CEGUI::UDim(0.43f, 0.0f));
  textPoints->setYPosition(CEGUI::UDim(0.35f, 0.0f));
  textPoints->setProperty("FrameEnabled","False");
  textPoints->setProperty("BackgroundEnabled", "False");
  textPoints->setProperty("VertFormatting", "TopAligned");

  CEGUI::Window* textLives = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText","textLives");
  textLives->setText("[font='Carton_Six']");
  textLives->setSize(CEGUI::USize(CEGUI::UDim(0.20,0),CEGUI::UDim(0.70,0)));
  textLives->setXPosition(CEGUI::UDim(0.1f, 0.0f));
  textLives->setYPosition(CEGUI::UDim(0.35f, 0.0f));
  textLives->setProperty("FrameEnabled","False");
  textLives->setProperty("BackgroundEnabled", "False");
  textLives->setProperty("VertFormatting", "TopAligned");

  CEGUI::Window* textSuperCont = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText","textSuperCont");
  textSuperCont->setSize(CEGUI::USize(CEGUI::UDim(0.20,0),CEGUI::UDim(0.70,0)));
  textSuperCont->setXPosition(CEGUI::UDim(0.47f, 0.0f));
  textSuperCont->setYPosition(CEGUI::UDim(0.55f, 0.0f));
  textSuperCont->setProperty("FrameEnabled","False");
  textSuperCont->setProperty("BackgroundEnabled", "False");
  textSuperCont->setProperty("VertFormatting", "TopAligned");
  textSuperCont->setVisible(false);
  //-----------------------------------------------------------------------------------------------------


  //EditBox Name Records----------------------------------------------------------
  CEGUI::Window* sheetBGed = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticImage","background_editbox");
  sheetBGed->setPosition(CEGUI::UVector2(cegui_reldim(0),cegui_reldim(0)));
  sheetBGed->setSize(CEGUI::USize(cegui_reldim(1),cegui_reldim(1)));
  sheetBGed->setProperty("Image","BackgroundScore");
  sheetBGed->setProperty("FrameEnabled","False");
  sheetBGed->setProperty("BackgroundEnabled", "False");
    
  CEGUI::Editbox* eb = static_cast<CEGUI::Editbox*>(sheetBGed->createChild("OgreTray/Editbox","editbox"));
  eb->setPosition(CEGUI::UVector2(CEGUI::UDim(0.48f, 0.0f),CEGUI::UDim(0.25f, 0)));
  eb->setSize(CEGUI::USize(CEGUI::UDim(0.15,0),CEGUI::UDim(0.05,0)));
  eb->setFont("Carton_Six");
  
  CEGUI::Window* recordsPoints = static_cast<CEGUI::Window*>(sheetBGed->createChild("TaharezLook/StaticText","pointsrecords"));
  recordsPoints->setSize(CEGUI::USize(CEGUI::UDim(0.90,0),CEGUI::UDim(0.70,0)));
  recordsPoints->setXPosition(CEGUI::UDim(0.50f, 0.0f));
  recordsPoints->setYPosition(CEGUI::UDim(0.34f, 0.0f));
  recordsPoints->setProperty("FrameEnabled","False");
  recordsPoints->setProperty("BackgroundEnabled", "False");
  recordsPoints->setProperty("VertFormatting", "TopAligned");

  CEGUI::Window* butttonAcept = static_cast<CEGUI::Window*>(sheetBGed->createChild("OgreTray/Button"));
  butttonAcept->setPosition(CEGUI::UVector2(CEGUI::UDim(0.45f, 0.0f),CEGUI::UDim(0.67f, 0)));
  butttonAcept->setSize(CEGUI::USize(CEGUI::UDim(0.15,0),CEGUI::UDim(0.05,0)));
  butttonAcept->setText("[font='Carton_Six'] Accept");
  butttonAcept->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&PlayState::accept,this));
  sheetBGed->setVisible(false);
  //----------------------------------------------------------------------------------------------------

  //BackGround Next Level--------------------------------------------------------------------------------
  CEGUI::Window* sheetBGnl = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticImage","background_next");
  sheetBGnl->setPosition(CEGUI::UVector2(cegui_reldim(0),cegui_reldim(0)));
  sheetBGnl->setSize(CEGUI::USize(cegui_reldim(1),cegui_reldim(1)));
  sheetBGnl->setProperty("Image","BackgroundNextLevel");
  sheetBGnl->setProperty("FrameEnabled","False");
  sheetBGnl->setProperty("BackgroundEnabled", "False");
  sheetBGnl->setVisible(false);
  //--------------------------------------------------------------------------------------

  //Add Cegui-----------------
  sheetBG->addChild(textPoints);
  sheetBG->addChild(textLives);
  sheetBG->addChild(pauseButton);
  sheetBG->addChild(quitButton);
  sheet->addChild(textSuperCont);
  sheet->addChild(sheetBGnl);
  sheet->addChild(sheetBGed);
  sheet->addChild(sheetBG);
  //--------------------------------

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
        if(_aux.getIndex()!=32 && _aux.getIndex()!=33){
          // Ray -------------------------
          Entity* _entRay = _sceneMgr->createEntity("_entRay"+Ogre::StringConverter::toString(i),"ray.mesh");
          SceneNode* _snRay = _sceneMgr->createSceneNode("RaySceneNode"+Ogre::StringConverter::toString(i));
          _snRay->attachObject(_entRay);
          _snRay->setPosition(_aux.getPosition());
          _sceneMgr->getRootSceneNode()->addChild(_snRay);
          if(_aux.getIndex() == 8 || _aux.getIndex() == 15 || _aux.getIndex() == 22 || _aux.getIndex() == 28){
            _snRay->setScale(0.6,0.6,0.6);
            _superRayVector.push_back(_snRay);
            //Almaceno el rayo----------------------------------------
            _superRayWareHouse.push_back(_snRay);
            //--------------------------------------------------------
          }else{
            _snRay->setScale(0.3,0.3,0.3);
            _rayVector.push_back(_snRay);
            //Almaceno el rayo----------------------------------------
            _rayWareHouse.push_back(_snRay);
            //--------------------------------------------------------
          }
        //--------------------------------------------------------
        }
      
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

  //Compruebo los records ------------
  requestScore();
  delete _importer;
  //----------------------------------

  //Escondo la interfaz----------------------
  CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  sheet->getChild("background_editbox")->setVisible(false) ;
  //----------------------------------------

  //Limpio vectores--------------------
  _rayWareHouse.clear();
  _rayVector.clear();
  _superRayWareHouse.clear();
  _superRayVector.clear();
  //----------------------------------

  //Salgo del estado------------------------------
  _sceneMgr->clearScene();
  _sceneMgr->destroyCamera("PlayCamera");

  

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
  
  //Actualizamos si no estamos en los records----
  if(_inRecords==false){

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
    updateGhost3();
    //-----------------

    //Update Collisiones ---
    updateCollisions();
    updateRayCollisions();
    //----------------------
    
    // Update Pj ---------
    updatePj(_deltaT);
    //--------------------
    
    //Next Level-----------------
    if(_nextLevel==true){
      CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
      sheet->getChild("background_next")->setVisible(true);
      if(--_contNextLevel<=0){
        _nextLevel=false;
        sheet->getChild("background_next")->setVisible(false);
      }
    }
    //--------------------------

    //Actualizacion de la puntuacion en la interfaz----
    CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
    sheet->getChild("background_wnd2")->getChild("textPoints")->setText("[font='Carton_Six']"+Ogre::StringConverter::toString(_pointsRay));
    sheet->getChild("background_wnd2")->getChild("textLives")->setText("[font='Carton_Six']"+Ogre::StringConverter::toString(_pj->getLives()));
    sheet->getChild("background_editbox")->getChild("pointsrecords")->setText("[font='Carton_Six']"+Ogre::StringConverter::toString(_pointsRay)
      +"\n\n"+Ogre::StringConverter::toString(_pointsGhost)+"\n\n\n"+Ogre::StringConverter::toString(_pointsGhost+_pointsRay));


    //-------------------------------------------------
  }
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

  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown(static_cast<CEGUI::Key::Scan>(e.key));
  CEGUI::System::getSingleton().getDefaultGUIContext().injectChar(e.text);
  //-------------------------------
  
  // Tecla p --> PauseState.-------
  if (e.key == OIS::KC_P) {
    pushState(PauseState::getSingletonPtr());

  }
  //-----------------
  
  //Movimiento PJ----------------
 
  //Actualizamos si no estamos en los records----
  if(_inRecords==false){

    if (e.key == OIS::KC_S || e.key == OIS::KC_W || e.key == OIS::KC_D || e.key == OIS::KC_A) {
        mAnims[1]->setEnabled(false);
        mAnims[0]->setTimePosition(0);
        mAnims[0]->setEnabled(true);
        mAnims[0]->setLoop(true);
        mAnims[0]->setTimePosition(0);

      
        GameManager::getSingletonPtr()->_simpleEffect = GameManager::getSingletonPtr()->_pSoundFXManager->load("vaccum.wav");
        GameManager::getSingletonPtr()->_simpleEffect->play();
  }

}
  // Tecla g --> GameOverState.-------
  if (e.key == OIS::KC_G) {
    //Paso a los records-----------------------
    _inRecords=true;
    //-----------------------------------------
    GameManager::getSingletonPtr()->_simpleEffect = GameManager::getSingletonPtr()->_pSoundFXManager->load("game_over.wav");
    GameManager::getSingletonPtr()->_simpleEffect->play();
    
    //Muestro la interfaz----------------------
    CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
    sheet->getChild("background_editbox")->setVisible(true);
    sheet->getChild("background_editbox")->getChild("editbox")->activate();
    //----------------------------------------
  }
  //-----------------

  
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

bool
Pacman::PlayState::accept(const CEGUI::EventArgs &e)
{
  //Obtengo el nombre ----------------------------------------------------------------------
  CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  _name=sheet->getChild("background_editbox")->getChild("editbox")->getText().c_str() ;
  //-----------------------------------------------------------------------------------------

  //Paso al gameOver-------------------------------------------------------------------------
  sheet->getChild("background_editbox")->setVisible(false) ;
  changeState(GameOverState::getSingletonPtr());
  //-----------------------------------------------------------------------------------------
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
Pacman::PlayState::updateGhost3()
{
  
  SceneNode* _snGhost =_sceneMgr->getSceneNode("GhostSceneNode3");
  SceneNode* _snPj =_sceneMgr->getSceneNode("PjSceneNode");

  //Movimiento del ghost ------------
  //Si el next es nulo estoy en un vertice------
  if(_nextGhost3==NULL){
    //Calculo adyacentes-----------------
    _adjVerGhost3 = _scene->getGraph()->adjacents(_nowGhost3->getData().getIndex());
    //-----------------------------------

    //Algoritmo Voraz (IA)--------------------
    int res=0;
    double minimo=100.0;//Un valor muy alto o el valor maximo
    for (unsigned int i=0;i<_adjVerGhost3.size();i++){
      Vector3 _vectorDistance=_snPj->getPosition()-_adjVerGhost3[i]->getData().getPosition();
      double _distance=_vectorDistance.length();
      if(_distance<minimo){
        res=i;
        minimo=_distance;
      }
    }

    Vector3 _aux = _adjVerGhost3[res]->getData().getPosition()-_nowGhost3->getData().getPosition();
    _nextGhost3=_adjVerGhost3[res];

    //----------------------------------------

    //El auxiliar me da la direccion---------
    if(_aux.z>0){
      _ghost3->setDesp(Ogre::Vector3(0,0,0.03));
    }
    if(_aux.z<0){
      _ghost3->setDesp(Ogre::Vector3(0,0,-0.03));
    }
    if(_aux.x>0){
      _ghost3->setDesp(Ogre::Vector3(0.03,0,0));
    }
    if(_aux.x<0){
      _ghost3->setDesp(Ogre::Vector3(-0.03,0,0));
    }
    //------------------------------------
    _ghost3->setMoving(true);

  }
  //---------------------------------------------------------
  if(_ghost3->isMoving()){
    _snGhost->setPosition(_snGhost->getPosition()+_ghost3->getDesp());
    //Ver cuando he de parar---

    Vector3 _res=_nextGhost3->getData().getPosition()-_snGhost->getPosition();
    //cout << "Distancia " << _res.length() << endl;
    if(_res.length()<=0.035){
      _nowGhost3=_nextGhost3;
      _nextGhost3=NULL;
      _ghost3->setMoving(false);
      _adjVerGhost3.clear();
    }
  }

  if (_animGhost3 != NULL) {
    if (_animGhost3->hasEnded()) {
      _animGhost3->setTimePosition(0.0);
      _animGhost3->setEnabled(false);
    }
    else {
      _animGhost3->addTime(_deltaT);
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
      cout << "Estoy en: " << _now->getData().getIndex() << endl;
      
      if(_now->getData().getIndex()==32){
        cout << "Teletransporte "  << endl;
        _snPj->setPosition(_scene->getGraph()->getVertex(31)->getData().getPosition());
        _now=_scene->getGraph()->getVertex(31);
        _next=NULL;
      }

      if(_now->getData().getIndex()==33){
        cout << "Teletransporte "  << endl;
        _snPj->setPosition(_scene->getGraph()->getVertex(30)->getData().getPosition());
        _now=_scene->getGraph()->getVertex(30);
        _next=NULL;
      }

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
    _snPj->setVisible(true);


  }

  if(_pj->getState()=="super"){
    //Muestro el super contador---
    CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
    sheet->getChild("textSuperCont")->setText("[font='Carton_Six-30']"+Ogre::StringConverter::toString(_contSuper/100));
    sheet->getChild("textSuperCont")->setVisible(true);
    //----------------------------
    //Cambiar material Ghost------------------------------
    Ogre::MaterialPtr  Mat = static_cast<Ogre::MaterialPtr> (Ogre::MaterialManager::getSingleton().getByName("MatCuerpo"));
    Mat->getTechnique(0)->getPass(0)->setSpecular(Ogre::ColourValue::Blue);
    Mat->getTechnique(0)->getPass(0)->setDiffuse(5.0 ,1.0 ,1.0, 0.0);
    Mat->getTechnique(0)->getPass(0)->setAmbient(Ogre::ColourValue::Blue);
    //--------------------------------------------------------------

    if(--_contSuper<=0){
      //Cambiar material Ghost Blanco------------------------------
      Ogre::MaterialPtr  Mat = static_cast<Ogre::MaterialPtr> (Ogre::MaterialManager::getSingleton().getByName("MatCuerpo"));
      Mat->getTechnique(0)->getPass(0)->setSpecular(Ogre::ColourValue::White);
      Mat->getTechnique(0)->getPass(0)->setDiffuse(1.0 ,1.0 ,1.0, 1.0);
      Mat->getTechnique(0)->getPass(0)->setAmbient(Ogre::ColourValue::White);
      //--------------------------------------------------------------

      _contSuper=1500;
      _pj->changeState("normal");
      cout << "Vuelvo al estado normal" << endl;
      sheet->getChild("textSuperCont")->setVisible(false);
     
    }

    
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
    //Paso a los records-----------------------
    _inRecords=true;
    //-----------------------------------------
    GameManager::getSingletonPtr()->_simpleEffect = GameManager::getSingletonPtr()->_pSoundFXManager->load("game_over.wav");
    GameManager::getSingletonPtr()->_simpleEffect->play();
    //Muestrola interfaz----------------------
    CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
    sheet->getChild("background_editbox")->setVisible(true);
    sheet->getChild("background_editbox")->getChild("editbox")->activate();
    //----------------------------------------

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
  Ogre::AxisAlignedBox bbGhost3 = _sceneMgr->getSceneNode("GhostSceneNode3")->_getWorldAABB();
  //----------------------------------------------------------------------------------------

  //Colision Detectada ! -------------------------------------
  if(bbPj.intersects(bbGhost1) || bbPj.intersects(bbGhost2)|| bbPj.intersects(bbGhost3)){
  //----------------------------------------------------------   
   

    //Colision en caso de Pj normal ---  
    if(_pj->getState()=="normal"){
      cout << "Colision PJ - Ghost" << endl;
      _collisionWithGhostDetected=true;
      _pj->hit();
      _pj->changeState("respawn");
       GameManager::getSingletonPtr()->_simpleEffect = GameManager::getSingletonPtr()->_pSoundFXManager->load("shout.wav");
       GameManager::getSingletonPtr()->_simpleEffect->play();
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
        _pointsRay+=300;
        _pointsGhost+=1;
        //-----------------------------------------
        GameManager::getSingletonPtr()->_simpleEffect = GameManager::getSingletonPtr()->_pSoundFXManager->load("uncover.wav");
        GameManager::getSingletonPtr()->_simpleEffect->play();

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
        _pointsRay+=300;
        _pointsGhost+=1;
        //-----------------------------------------
        GameManager::getSingletonPtr()->_simpleEffect = GameManager::getSingletonPtr()->_pSoundFXManager->load("uncover.wav");
        GameManager::getSingletonPtr()->_simpleEffect->play();

        //----------------------------------------------------------------------
      }

      if(bbPj.intersects(bbGhost3)){
        // El fantasmas deberia cambiar de posicion y recomenzar su movimiento---

        //Nodo aleatorio que sea de tipo respawn---
        SceneNode* _auxGhost = _sceneMgr->getSceneNode("GhostSceneNode3");
        int _go=0+rand()%(_respawnVertex.size()-0);
        _auxGhost->setPosition(_respawnVertex[_go]->getData().getPosition());
        //-----------------------------------------

        //Reseteo variables -----------------------
        _nowGhost3=_respawnVertex[_go];
        _nextGhost3=NULL;
        _pointsRay+=300;
        _pointsGhost+=1;
        //-----------------------------------------
        GameManager::getSingletonPtr()->_simpleEffect = GameManager::getSingletonPtr()->_pSoundFXManager->load("uncover.wav");
        GameManager::getSingletonPtr()->_simpleEffect->play();

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
      bool bIsVisible =  _camera->isVisible(_sceneMgr->getSceneNode(_rayVector[i]->getName())->_getWorldAABB());
      if(bIsVisible==true){
        GameManager::getSingletonPtr()->_simpleEffect = GameManager::getSingletonPtr()->_pSoundFXManager->load("descorchar.wav");
        GameManager::getSingletonPtr()->_simpleEffect->play();
      }


      _aux->setVisible(false);
      _rayVector.erase(_rayVector.begin()+i);
      _pointsRay+=100;
      cout << "Puntuacion: " << _pointsRay << endl;
      cout << "Vidas: " << _pj->getLives() << endl;
      cout << "\nRayos restantes: " << (_rayVector.size()) << "\n" << endl;
    }
  }
  //--------------------------------------------------
  

  // Si no quedan rayos por coger seria el final del nivel ----------------
  if(_rayVector.size()==0){
    cout << "\n Nivel Acabado \n" << endl;
    _nextLevel=true;
    //Vacio el vector de Super Rayos------
    if(_superRayVector.size()>0){
      _superRayVector.clear();
    }
    //------------------------------------

    //Vuelvo a cargar los super rayos-----------
    for(unsigned int i=0;i<_superRayWareHouse.size();i++){
      _superRayWareHouse[i]->setVisible(true);
      _superRayVector.push_back(_superRayWareHouse[i]);
    }
    //------------------------------------------

    //Vuelvo a cargar los rayos-----------
    for(unsigned int i=0;i<_rayWareHouse.size();i++){
      _rayWareHouse[i]->setVisible(true);
      _rayVector.push_back(_rayWareHouse[i]);
    }
    //------------------------------------

  }
  //-----------------------------------------------------------------------

  // Colisiones SuperRayos ----------------------------------------------
  for (unsigned int i=0;i<_superRayVector.size();++i){
    Ogre::SceneNode* _aux=_superRayVector[i];
    Ogre::AxisAlignedBox bbRay = _aux->_getWorldAABB();
    if(bbPjSceneNode.intersects(bbRay)){
      _superRayVector.erase(_superRayVector.begin()+i);
      _pointsRay+=300;
      cout << "Puntuacion: " << _pointsRay << endl;
      cout << "Vidas: " << _pj->getLives() << endl;
      _aux->setVisible(false);
      _pj->changeState("super");
    }
  }
  //---------------------------------------------------------------------

}

void
Pacman::PlayState::requestScore()
{
    fstream fichero;//Fichero
    string frase;//Auxiliar
    bool reWrite=false;//Si debemos re-escribir el fichero o no
    String nombre;
    std::vector <String> puntuaciones;//vector para guardar puntuaciones

    fichero.open ( "Records.txt" , ios::in); //Suponemos que ya existe
    if (fichero.is_open()) {//se debe lanzar primero el metodo de lectura para comprobar que existe el fichero
      while (getline (fichero,frase)) {
        String nombre=Ogre::StringUtil::split (frase," ")[0];
        int punt = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase," ")[1]);
        cout << "\tNombre: "<< nombre << " Puntuacion " << punt << endl;
        if(_pointsRay>punt && reWrite == false){
          cout << "Nuevo Record!!!"<< endl;
          reWrite=true;

          cout << "\nNombre?\n" << endl;
          //cin >> nombre;
          nombre=_name;
          String aux=nombre+" "+Ogre::StringConverter::toString(_pointsRay+_pointsGhost);
          cout << aux << endl;
          puntuaciones.push_back(aux);
        }
        puntuaciones.push_back(frase);
      }
      fichero.close();    
    }else{

      cout << "Fichero inexistente o faltan permisos para abrirlo, creando archivo e records..." << endl;
      ofstream archivo;  // objeto de la clase ofstream
      archivo.open("Records.txt");
      for(int i=0;i<9;i++){
        archivo  << "AAA 000" << endl;
      }
      archivo.close();

      fichero.open ( "Records.txt" , ios::in);
      while (getline (fichero,frase)) {
        String nombre=Ogre::StringUtil::split (frase," ")[0];
        int punt = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase," ")[1]);
        cout << "\tNombre: "<< nombre << " Puntuacion " << punt << endl;
        if(_pointsRay>punt && reWrite == false){
          cout << "Nuevo Record!!!"<< endl;
          reWrite=true;

          nombre=_name;
          
          String aux=nombre+" "+Ogre::StringConverter::toString(_pointsRay+_pointsGhost);
          cout << aux << endl;
          puntuaciones.push_back(aux);
        }
        puntuaciones.push_back(frase);
      }
      fichero.close();
    }


    //Se debe re-escribir el fichero
    if(reWrite==true){
      puntuaciones.pop_back();//saco la ultima

      ofstream archivo;  // objeto de la clase ofstream
      archivo.open("Records.txt");
      for(unsigned int i=0;i<puntuaciones.size();i++){
          archivo  << puntuaciones[i] << endl;
      }
      archivo.close();
    }

}