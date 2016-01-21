/*********************************************************************
 * Módulo 1. Curso de Experto en Desarrollo de Videojuegos
 * Autor: David Vallejo Fernández    David.Vallejo@uclm.es
 *
 * Código modificado a partir de Managing Game States with OGRE
 * http://www.ogre3d.org/tikiwiki/Managing+Game+States+with+OGRE
 * Inspirado en Managing Game States in C++
 * http://gamedevgeek.com/tutorials/managing-game-states-in-c/
 *
 * You can redistribute and/or modify this file under the terms of the
 * GNU General Public License ad published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * and later version. See <http://www.gnu.org/licenses/>.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.  
 *********************************************************************/

#ifndef PlayState_H
#define PlayState_H

#include <Ogre.h>
#include <OIS/OIS.h>

#include "GameState.h"

 #include <CEGUI.h>
 #include <RendererModules/Ogre/Renderer.h>
 #include <RendererModules/Ogre/Texture.h>

 #include <Importer.h>
#include <Scene.h>
#include <Camera.h>

#include "Pj.h"

namespace Pacman {

class PlayState : public Ogre::Singleton<PlayState>, public GameState
{
 public:
  PlayState () {}

  void enter ();
  void exit ();
  void pause ();
  void resume ();
  void updateCamera(Ogre::Real deltaTime);
  void loadGraph();
  void createGUI();

  void keyPressed (const OIS::KeyEvent &e);
  void keyReleased (const OIS::KeyEvent &e);

  void mouseMoved (const OIS::MouseEvent &e);
  void mousePressed (const OIS::MouseEvent &e, OIS::MouseButtonID id);
  void mouseReleased (const OIS::MouseEvent &e, OIS::MouseButtonID id);

  bool frameStarted (const Ogre::FrameEvent& evt);
  bool frameEnded (const Ogre::FrameEvent& evt);

  // Heredados de Ogre::Singleton.
  static PlayState& getSingleton ();
  static PlayState* getSingletonPtr ();

  CEGUI::MouseButton convertMouseButton(OIS::MouseButtonID id);

 protected:
  SceneNode* mBodyNode;
  SceneNode* mCameraPivot;
  SceneNode* mCameraGoal;
  SceneNode* mCameraNode;
  Real mPivotPitch;
  Ogre::Root* _root;
  Ogre::SceneManager* _sceneMgr;
  Ogre::Viewport* _viewport;
  Ogre::Camera* _camera;
  Ogre::Camera* _cameraMiniMap;
  Importer* _importer;
  Scene* _scene;
  Real _deltaT;
  Pj* _pj;
  Pj* _ghost;
  Pj* _ghost2;

  bool quit(const CEGUI::EventArgs &e);
  bool pauseB(const CEGUI::EventArgs &e);
  

  bool _exitGame;

  void updateGhost();
  void updateGhost2();
  void updatePj();
};

}

#endif
