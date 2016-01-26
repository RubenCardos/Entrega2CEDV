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

  CEGUI::Vector2f mousePos = CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().getPosition();
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(-mousePos.d_x,-mousePos.d_y);


  CEGUI::Window* sheet = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();


  CEGUI::Window* menuButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","menu");
  menuButton->setText("[font='Carton_Six'] Menu ");
  menuButton->setSize(CEGUI::USize(CEGUI::UDim(0.23,0),CEGUI::UDim(0.07,0)));
  menuButton->setXPosition(UDim(0.20f, 0.0f));
  menuButton->setYPosition(UDim(0.50f, 0.0f));
  //menuButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&IntroState::play,this));

  CEGUI::Window* resetButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","reset");
  resetButton->setText("[font='Carton_Six'] Reset ");
  resetButton->setSize(CEGUI::USize(CEGUI::UDim(0.23,0),CEGUI::UDim(0.07,0)));
  resetButton->setXPosition(UDim(0.20f, 0.0f));
  resetButton->setYPosition(UDim(0.60f, 0.0f));
  //resetButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&IntroState::records,this));

  CEGUI::Window* backButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","back");
  backButton->setText("[font='Carton_Six'] Resume ");
  backButton->setSize(CEGUI::USize(CEGUI::UDim(0.23,0),CEGUI::UDim(0.07,0)));
  backButton->setXPosition(UDim(0.20f, 0.0f));
  backButton->setYPosition(UDim(0.70f, 0.0f));
  //backButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&IntroState::credito,this));
  

  sheet->addChild(menuButton);
  sheet->addChild(resetButton);
  sheet->addChild(backButton);

  _exitGame = false;
}

void
Pacman::PauseState::exit ()
{
   CEGUI::Window* sheet = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
   sheet->destroyChild("menu");
   sheet->destroyChild("reset");
   sheet->destroyChild("back");

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
}

void
Pacman::PauseState::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
}

void
Pacman::PauseState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
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
