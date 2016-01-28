#include "IntroState.h"
#include "PlayState.h"
#include "CEGUI.h"

using namespace CEGUI;

template<> Pacman::IntroState* Ogre::Singleton<Pacman::IntroState>::msSingleton = 0;


void
Pacman::IntroState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();

  _sceneMgr = _root->createSceneManager(Ogre::ST_GENERIC, "SceneManager");
  _camera = _sceneMgr->createCamera("IntroCamera");
  _viewport = _root->getAutoCreatedWindow()->addViewport(_camera);
  _sceneMgr->setAmbientLight(Ogre::ColourValue(0.5,0.5,0.5));


  GameManager::getSingletonPtr()->_mainTrack = GameManager::getSingletonPtr()->_pTrackManager->load("Intro.mp3");
  GameManager::getSingletonPtr()->_mainTrack->play();
  
  

  _exitGame = false;

  //Camara miniMapa-------------
  _camBack = _sceneMgr->createCamera("MiniMapCamera");
  //---------------------------
  
  _camera->setPosition(Ogre::Vector3(20,5,20));
  _camera->lookAt(Ogre::Vector3(0,1,0));
  _camera->setNearClipDistance(5);
  _camera->setFarClipDistance(10000);


  double width = _viewport->getActualWidth();
  double height = _viewport->getActualHeight();
  _camera->setAspectRatio(width / height);

  //Background-------------------------------------------
  Ogre::Plane plane1(Ogre::Vector3::UNIT_Y, 0);
  Ogre::MeshManager::getSingleton().createPlane("plane1",
  Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane1,
  35,55,1,1,true,1,1,1,Ogre::Vector3::UNIT_Z);

  Ogre::SceneNode* nodeBG = _sceneMgr->createSceneNode("wallpaper");
  Ogre::Entity* groundEnt = _sceneMgr->createEntity("planeEnt", "plane1");
  groundEnt->setMaterialName("wallpaper");
  nodeBG->attachObject(groundEnt);
  nodeBG->setPosition(-5,0,-5);
  nodeBG->yaw(Ogre::Degree(135));
  nodeBG->roll(Ogre::Degree(90));
  _sceneMgr->getRootSceneNode()->addChild(nodeBG);
  //--------------------------------------------------------

  //Carga del personaje---------------------------------------------
  Entity* _entCharacter = _sceneMgr->createEntity("entCharacter","personaje.mesh");
  SceneNode* _sceneCharacter = _sceneMgr->createSceneNode("Personaje");
  _sceneCharacter->setPosition(-5,-20,0);
  _sceneCharacter->setScale(4,4,4);
  _sceneCharacter->attachObject(_entCharacter);
  _sceneCharacter->yaw(Ogre::Degree(60));
  _sceneMgr->getRootSceneNode()->addChild(_sceneCharacter);
  //------------------------------------------------------------------

  //Carga del fantasmaa----------------------------------------------------------
  Entity* _entGhost = _sceneMgr->createEntity("entGhost","ghost.mesh");
  SceneNode* _sceneGhost = _sceneMgr->createSceneNode("ghost");
  _sceneGhost->setPosition(-15,-13,2);  //x->profundidad y->arriba o abajo z-> derecha o izquierda
  _sceneGhost->setScale(3,3,3);
  _sceneGhost->attachObject(_entGhost);
  _sceneGhost->yaw(Ogre::Degree(70));
  _sceneMgr->getRootSceneNode()->addChild(_sceneGhost);
  //----------------------------------------------------------------------

  //Luz Escena-------------------
  Ogre::Light* spotLight = _sceneMgr->createLight("SpotLight");
  //Colores difusos a azul
  spotLight->setDiffuseColour(0, 0, 0.5);
  spotLight->setSpecularColour(0, 0, 1.0);
	//Tipo de luz y sus repectivos direcciones y posiciones (Actua modo linterna)
	spotLight->setType(Ogre::Light::LT_SPOTLIGHT);
	spotLight->setDirection(-1, -1, 0);
	spotLight->setPosition(Ogre::Vector3(50, 50, 0));
	//Para desvanecer la luz, Grados de desvanecimiento...
	spotLight->setSpotlightRange(Ogre::Degree(35), Ogre::Degree(50));
	//---------------------------------------------------------------
  
  //Animaciones del Personaje y del Fantasma-------------------------------------
  _animPJ = _sceneMgr->getEntity("entCharacter")->getAnimationState("idle");
  _animPJ->setEnabled(true);
  _animPJ->setLoop(true);
  _animPJ->setTimePosition(0.0);

  _animGhost = _sceneMgr->getEntity("entGhost")->getAnimationState("moveIntro");
  _animGhost->setEnabled(true);
  _animGhost->setLoop(true);
  _animGhost->setTimePosition(0.0);
  //----------------------------------------------------------

  //Creamos la interfaz ------------------------  
  createGUI();
  //----------------------------

  //ImageManager de los fondos de las ventanas creditos y puntuaciones----------------------
  ImageManager::getSingleton().addFromImageFile("BackgroundCredito","CREDITOS.jpg");
  ImageManager::getSingleton().addFromImageFile("BackgroundRecords","PUNTUACIONES.jpg");

  //--------------------------------------------------------------------------------------------
}


void
Pacman::IntroState::exit()
{
  _sceneMgr->clearScene();
  _root->getAutoCreatedWindow()->removeAllViewports();


}

void
Pacman::IntroState::pause ()
{
}

void
Pacman::IntroState::resume ()
{
}

bool
Pacman::IntroState::frameStarted
(const Ogre::FrameEvent& evt) 
{
  _deltaTIntro = evt.timeSinceLastFrame;

  if (_animPJ != NULL) {
    if (_animPJ->hasEnded()) {
      _animPJ->setTimePosition(0.0);
      _animPJ->setEnabled(false);
    }
    else {
      _animPJ->addTime(_deltaTIntro);
    }
  }

  if (_animGhost != NULL) {
    if (_animGhost->hasEnded()) {
      _animGhost->setTimePosition(0.0);
      _animGhost->setEnabled(false);
    }
    else {
      _animGhost->addTime(_deltaTIntro);
    }
  }


  return true;
}

bool
Pacman::IntroState::frameEnded
(const Ogre::FrameEvent& evt)
{
  if (_exitGame)
    return false;
  
  return true;
}

void
Pacman::IntroState::keyPressed
(const OIS::KeyEvent &e)
{
  // Transición al siguiente estado.
  // Espacio --> PlayState
  if (e.key == OIS::KC_SPACE) {
   pushState(PlayState::getSingletonPtr());
  }

  CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown(static_cast<CEGUI::Key::Scan>(e.key));
  CEGUI::System::getSingleton().getDefaultGUIContext().injectChar(e.text);

}

void
Pacman::IntroState::keyReleased
(const OIS::KeyEvent &e )
{
  if (e.key == OIS::KC_ESCAPE) {
    _exitGame = true;
  }

  CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp(static_cast<CEGUI::Key::Scan>(e.key));
}

void
Pacman::IntroState::mouseMoved
(const OIS::MouseEvent &e)
{
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(e.state.X.rel, e.state.Y.rel);  
  
}

void
Pacman::IntroState::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(convertMouseButton(id));
}

void
Pacman::IntroState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(convertMouseButton(id));
}

Pacman::IntroState*
Pacman::IntroState::getSingletonPtr ()
{
return msSingleton;
}

Pacman::IntroState&
Pacman::IntroState::getSingleton ()
{ 
  assert(msSingleton);
  return *msSingleton;
}

void Pacman::IntroState::createGUI()
{
  //CEGUI
  renderer = &CEGUI::OgreRenderer::bootstrapSystem();
  CEGUI::Scheme::setDefaultResourceGroup("Schemes");
  CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");
  CEGUI::Font::setDefaultResourceGroup("Fonts");
  CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
  CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
  CEGUI::SchemeManager::getSingleton().createFromFile("OgreTray.scheme");
  CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
  CEGUI::System::getSingleton().getDefaultGUIContext().setDefaultFont("DejaVuSans-12");
  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");
  CEGUI::Vector2f mousePos = CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().getPosition();
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(-mousePos.d_x,-mousePos.d_y);
  CEGUI::FontManager::getSingleton().createAll("*.font", "Fonts");
    
  //Sheet
  CEGUI::Window* sheet = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow","Sheet");

  //Cargo la imagen
  ImageManager::getSingleton().addFromImageFile("BackgroundImage","logo.png");

  //Sheet
  Window* sheetBG =  WindowManager::getSingleton().createWindow("TaharezLook/StaticImage","background_wnd");
  sheetBG->setPosition(CEGUI::UVector2(CEGUI::UDim(0.58f, 0.0f),CEGUI::UDim(0.05, 0)));
  sheetBG->setSize( CEGUI::USize(CEGUI::UDim(0.40, 0), CEGUI::UDim(0.40, 0)));
  sheetBG->setProperty("Image","BackgroundImage");
  sheetBG->setProperty("FrameEnabled","False");
  sheetBG->setProperty("BackgroundEnabled", "False");

  CEGUI::Window* playButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","play");
  playButton->setText("[font='Carton_Six'] Play ");
  playButton->setSize(CEGUI::USize(CEGUI::UDim(0.25,0),CEGUI::UDim(0.07,0)));
  playButton->setXPosition(UDim(0.65f, 0.0f));
  playButton->setYPosition(UDim(0.50f, 0.0f));
  playButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&IntroState::play,this));

  CEGUI::Window* recordsButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","records");
  recordsButton->setText("[font='Carton_Six'] Puntuaciones ");
  recordsButton->setSize(CEGUI::USize(CEGUI::UDim(0.25,0),CEGUI::UDim(0.07,0)));
  recordsButton->setXPosition(UDim(0.65f, 0.0f));
  recordsButton->setYPosition(UDim(0.60f, 0.0f));
  recordsButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&IntroState::records,this));

  CEGUI::Window* creditoButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","credito");
  creditoButton->setText("[font='Carton_Six'] Creditos ");
  creditoButton->setSize(CEGUI::USize(CEGUI::UDim(0.25,0),CEGUI::UDim(0.07,0)));
  creditoButton->setXPosition(UDim(0.65f, 0.0f));
  creditoButton->setYPosition(UDim(0.70f, 0.0f));
  creditoButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&IntroState::credito,this));


  //Attaching buttons
  sheet->addChild(recordsButton);
  sheet->addChild(playButton);
  sheet->addChild(creditoButton);
  sheet->addChild(sheetBG);

  
  //Render a Textura(Minimapa)-----------------
 TexturePtr rtt_texture = TextureManager::getSingleton().createManual(
     "RttTex", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
     TEX_TYPE_2D, 512, 512, 0, PF_R8G8B8, TU_RENDERTARGET);

  RenderTexture *rtex = rtt_texture->getBuffer()->getRenderTarget();

  rtex->addViewport(_camBack);
  rtex->getViewport(0)->setClearEveryFrame(true);
  rtex->getViewport(0)->setBackgroundColour(ColourValue::Black);
  rtex->getViewport(0)->setOverlaysEnabled(false);
  rtex->setAutoUpdated(true);

  CEGUI::Texture& guiTex = renderer->createTexture("CEGUITex", rtt_texture);
  CEGUI::OgreTexture& rendererTex = static_cast<CEGUI::OgreTexture&>(guiTex);

  rendererTex.setOgreTexture(rtt_texture, false);

  CEGUI::BasicImage* image = static_cast<CEGUI::BasicImage*>(&CEGUI::ImageManager::getSingleton().create("BasicImage", "RTTImage"));

  CEGUI::Rectf  imageArea= CEGUI::Rectf(0.0f, 0.0f, guiTex.getSize().d_width, guiTex.getSize().d_height);
  
  image->setArea(imageArea);
  image->setAutoScaled(CEGUI::ASM_Disabled);
  image->setTexture(&rendererTex);
  
  CEGUI::Window* ex1 = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("render.layout");
  CEGUI::Window* RTTWindow = ex1->getChild("RTTWindow");

  ex1->setVisible(false);

  RTTWindow->setProperty("Image","RTTImage");
  
  ex1->setSize(CEGUI::USize(CEGUI::UDim(0.25,0),CEGUI::UDim(0.25,0)));
  ex1->setPosition(CEGUI::UVector2(CEGUI::UDim(0.03,0),CEGUI::UDim(0.12,0)));

  sheet->addChild(ex1);

  //-----------------------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(sheet);

  
}

CEGUI::MouseButton Pacman::IntroState::convertMouseButton(OIS::MouseButtonID id)//METODOS DE CEGUI
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

bool
Pacman::IntroState::play(const CEGUI::EventArgs &e)
{
  changeState(PlayState::getSingletonPtr());
  
  return true;
}

bool
Pacman::IntroState::back(const CEGUI::EventArgs &e)
{
  
	CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
	
  if(creditos==true){
  	sheet->destroyChild("background_credito");
  }else{
  	sheet->destroyChild("background_records");
  }


  return true;
}

bool
Pacman::IntroState::credito(const CEGUI::EventArgs &e)
{

	CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  creditos=true;

  //Sheet Creditos
  Window* sheetBG =  WindowManager::getSingleton().createWindow("TaharezLook/StaticImage","background_credito");
  sheetBG->setPosition( UVector2(cegui_reldim(0),cegui_reldim(0)));
  sheetBG->setSize( USize(cegui_reldim(1),cegui_reldim(1)));
  sheetBG->setProperty("Image","BackgroundCredito");
  sheetBG->setProperty("FrameEnabled","False");
  sheetBG->setProperty("BackgroundEnabled", "False");

  CEGUI::Window* backButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","back");
  backButton->setText("[font='Carton_Six'] Volver ");
  backButton->setSize(CEGUI::USize(CEGUI::UDim(0.23,0),CEGUI::UDim(0.07,0)));
  backButton->setXPosition(UDim(0.66f, 0.0f));
  backButton->setYPosition(UDim(0.80f, 0.0f));
  backButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&IntroState::back,this));
  
  sheetBG->addChild(backButton);
  sheet->addChild(sheetBG);
  return true;
}

bool
Pacman::IntroState::records(const CEGUI::EventArgs &e)
{
  CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  creditos=false;

  //Sheet Records
  Window* sheetBG =  WindowManager::getSingleton().createWindow("TaharezLook/StaticImage","background_records");
  sheetBG->setPosition( UVector2(cegui_reldim(0),cegui_reldim(0)));
  sheetBG->setSize( USize(cegui_reldim(1),cegui_reldim(1)));
  sheetBG->setProperty("Image","BackgroundRecords");
  sheetBG->setProperty("FrameEnabled","False");
  sheetBG->setProperty("BackgroundEnabled", "False");

  CEGUI::Window* backButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","back");
  backButton->setText("[font='Carton_Six'] Volver ");
  backButton->setSize(CEGUI::USize(CEGUI::UDim(0.23,0),CEGUI::UDim(0.07,0)));
  backButton->setXPosition(UDim(0.66f, 0.0f));
  backButton->setYPosition(UDim(0.80f, 0.0f));
  backButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&IntroState::back,this));
  
  CEGUI::Window* text = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText","text");
  text->setText("[font='Carton_Six']"+readRecords());
  text->setSize(CEGUI::USize(CEGUI::UDim(0.90,0),CEGUI::UDim(0.90,0)));
  text->setXPosition(UDim(0.0f, 0.0f));
  text->setYPosition(UDim(0.0f, 0.0f));
  //text->setProperty("FrameEnabled","False");
  text->setProperty("BackgroundEnabled", "False");


  sheetBG->addChild(backButton);
  sheetBG->addChild(text);
  sheet->addChild(sheetBG);
  
  return true;
}

void 
Pacman::IntroState::createRecordsFile(){
  cout << "Fichero inexistente o faltan permisos para abrirlo, creando archivo e records..." << endl;
  ofstream archivo;  // objeto de la clase ofstream
  archivo.open("Records.txt");
  for(int i=0;i<9;i++){
    archivo  << "AAA 000" << endl;
  }
  archivo.close();
}

std::string 
Pacman::IntroState::readRecords(){
    
    std::string res="";

    fstream fichero;//Fichero
    std::string frase;//Auxiliar

    fichero.open ( "Records.txt" , ios::in);
    if (fichero.is_open()) {
      while (getline (fichero,frase)) {
        std::string nombre=Ogre::StringUtil::split (frase," ")[0];
        int punt = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase," ")[1]);
        cout << "\tNombre: "<< nombre << " Puntuacion " << punt << endl;
        res+= nombre+"     "+Ogre::StringUtil::split (frase," ")[1]+"\n";
      }
      fichero.close();    
    }else{
        createRecordsFile();
        fichero.open ( "Records.txt" , ios::in);
        if (fichero.is_open()) {
          while (getline (fichero,frase)) {
            std::string nombre=Ogre::StringUtil::split (frase," ")[0];
            int punt = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase," ")[1]);
            cout << "\tNombre: "<< nombre << " Puntuacion " << punt << endl;
            res+= nombre+"     "+Ogre::StringUtil::split (frase," ")[1]+"\n";
          }
          fichero.close();   
        }
    }
  return res;
}
