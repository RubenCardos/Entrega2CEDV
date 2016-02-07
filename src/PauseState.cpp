#include "PauseState.h"
#include "CEGUI.h"

template<> Pacman::PauseState* Ogre::Singleton<Pacman::PauseState>::msSingleton = 0;

using namespace CEGUI;

void
Pacman::PauseState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();

  // Se recupera el gestor de escena y la cámara.
  _sceneMgr = _root->getSceneManager("SceneManager");
  _camera = _sceneMgr->getCamera("IntroCamera");
  _viewport = _root->getAutoCreatedWindow()->getViewport(0);
  // Nuevo background colour.
  _viewport->setBackgroundColour(Ogre::ColourValue(0.0, 1.0, 0.0));


  CEGUI::Window* sheet = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();

  CEGUI::Window* sheetBG =  CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticImage","background_pause");
  sheetBG->setPosition(UVector2(cegui_reldim(0),cegui_reldim(0)));
  sheetBG->setSize(CEGUI::USize(CEGUI::UDim(20,0),CEGUI::UDim(20,0)));
  sheetBG->setProperty("Image","BackgroundPause");
  sheetBG->setProperty("FrameEnabled","False");
  sheetBG->setProperty("BackgroundEnabled", "False");

  CEGUI::Window* backButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","back");
  backButton->setText("[font='Carton_Six'] Resume ");
  backButton->setSize(CEGUI::USize(CEGUI::UDim(0.23,0),CEGUI::UDim(0.07,0)));
  backButton->setXPosition(UDim(0.40f, 0.0f));
  backButton->setYPosition(UDim(0.50f, 0.0f));
  backButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&PauseState::resumeB,this));
  
  
  sheetBG->addChild(backButton);
  sheet->addChild(sheetBG);

  _exitGame = false;
}

void
Pacman::PauseState::exit ()
{
   CEGUI::Window* sheet = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
   sheet->getChild("background_pause")->destroyChild("back");
   sheet->destroyChild("background_pause");

}
bool 
Pacman::PauseState::resumeB(const CEGUI::EventArgs &e){
  popState();
  return true;
}
void
Pacman::PauseState::pause ()
{
}

void
Pacman::PauseState::resume ()
{
  
}

bool
Pacman::PauseState::frameStarted
(const Ogre::FrameEvent& evt)
{
  return true;
}

bool
Pacman::PauseState::frameEnded
(const Ogre::FrameEvent& evt)
{
  if (_exitGame)
    return false;
  
  return true;
}

void
Pacman::PauseState::keyPressed
(const OIS::KeyEvent &e) {
  // Tecla p --> Estado anterior.
  if (e.key == OIS::KC_P) {
    popState();
  }
}

void
Pacman::PauseState::keyReleased
(const OIS::KeyEvent &e)
{
}

void
Pacman::PauseState::mouseMoved
(const OIS::MouseEvent &e)
{
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(e.state.X.rel, e.state.Y.rel);  
  
}

void
Pacman::PauseState::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(convertMouseButton(id));
}

void
Pacman::PauseState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(convertMouseButton(id));
}


CEGUI::MouseButton Pacman::PauseState::convertMouseButton(OIS::MouseButtonID id)//METODOS DE CEGUI
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

Pacman::PauseState*
Pacman::PauseState::getSingletonPtr ()
{
return msSingleton;
}

Pacman::PauseState&
Pacman::PauseState::getSingleton ()
{ 
  assert(msSingleton);
  return *msSingleton;
}

