#include "IntroState.h"
#include "PlayState.h"

using namespace CEGUI;

template<> IntroState* Ogre::Singleton<IntroState>::msSingleton = 0;

void
IntroState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();

  _sceneMgr = _root->createSceneManager(Ogre::ST_GENERIC, "SceneManager");
  _camera = _sceneMgr->createCamera("IntroCamera");
  _viewport = _root->getAutoCreatedWindow()->addViewport(_camera);
  _viewport->setBackgroundColour(Ogre::ColourValue(1.0, 1.0, 1.0));

  _exitGame = false;

  //Camra miniMapa-------------
  _camBack = _sceneMgr->createCamera("MiniMapCamera");
  //---------------------------
  
  //Creamos interfaces en CEGUI----
  createGUI();
  //-------------------------------
}

void
IntroState::exit()
{
  _sceneMgr->clearScene();
  _root->getAutoCreatedWindow()->removeAllViewports();
}

void
IntroState::pause ()
{
}

void
IntroState::resume ()
{
}

bool
IntroState::frameStarted
(const Ogre::FrameEvent& evt) 
{
  return true;
}

bool
IntroState::frameEnded
(const Ogre::FrameEvent& evt)
{
  if (_exitGame)
    return false;
  
  return true;
}

void
IntroState::keyPressed
(const OIS::KeyEvent &e)
{
  // Transición al siguiente estado.
  // Espacio --> PlayState
  if (e.key == OIS::KC_SPACE) {
    changeState(PlayState::getSingletonPtr());
  }

  CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown(static_cast<CEGUI::Key::Scan>(e.key));
  CEGUI::System::getSingleton().getDefaultGUIContext().injectChar(e.text);

}

void
IntroState::keyReleased
(const OIS::KeyEvent &e )
{
  if (e.key == OIS::KC_ESCAPE) {
    _exitGame = true;
  }

  CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp(static_cast<CEGUI::Key::Scan>(e.key));
}

void
IntroState::mouseMoved
(const OIS::MouseEvent &e)
{
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(e.state.X.rel, e.state.Y.rel);  
  
}

void
IntroState::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(convertMouseButton(id));
}

void
IntroState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(convertMouseButton(id));
}

IntroState*
IntroState::getSingletonPtr ()
{
return msSingleton;
}

IntroState&
IntroState::getSingleton ()
{ 
  assert(msSingleton);
  return *msSingleton;
}

void IntroState::createGUI()
{
  //CEGUI
  renderer = &CEGUI::OgreRenderer::bootstrapSystem();
  CEGUI::Scheme::setDefaultResourceGroup("Schemes");
  CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");
  CEGUI::Font::setDefaultResourceGroup("Fonts");
  CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
  CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");

  CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
  CEGUI::System::getSingleton().getDefaultGUIContext().setDefaultFont("DejaVuSans-12");

  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");

  CEGUI::Vector2f mousePos = CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().getPosition();
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(-mousePos.d_x,-mousePos.d_y);

  // load all the fonts (if they are not loaded yet)
  CEGUI::FontManager::getSingleton().createAll("*.font", "Fonts");
    
  //Sheet
  CEGUI::Window* sheet = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow","Sheet");
   
   //Quit button
  CEGUI::Window* quitButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button","Ex1/QuitButton");
  quitButton->setText("Play");
  quitButton->setSize(CEGUI::USize(CEGUI::UDim(0.15,0),CEGUI::UDim(0.05,0)));
  quitButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5-0.15/2,0),CEGUI::UDim(0.2,0)));
  quitButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&IntroState::play,this));
  //Attaching buttons
  sheet->addChild(quitButton);
  
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
  
  sheet->addChild(ex1);

  //-----------------------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(sheet);

  
}

CEGUI::MouseButton IntroState::convertMouseButton(OIS::MouseButtonID id)//METODOS DE CEGUI
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
IntroState::play(const CEGUI::EventArgs &e)
{
  changeState(PlayState::getSingletonPtr());
  return true;
}
