#include <QMouseEvent>
#include <QGuiApplication>
#include <QTime>
#include "NGLScene.h"
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>




NGLScene::NGLScene()
{
  setTitle("Load Shaders from JSON file");
 
}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}



void NGLScene::resizeGL( int _w, int _h )
{
  m_project=ngl::perspective( 45.0f, static_cast<float>( _w ) / _h, 0.05f, 350.0f );
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
}

void NGLScene::initializeGL()
{
  // we must call this first before any other GL commands to load and link the
  // gl commands from the lib, if this is not done program will crash
  ngl::NGLInit::initialize();

  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);
   // now to load the shader and set the values
  // grab an instance of shader manager
  
  bool loaded=ngl::ShaderLib::loadFromJson("shaders/shaders.json");
  if(!loaded)
  {
    std::cerr<<"problem loading shaders\n";
    exit(EXIT_FAILURE);
  }
  ngl::ShaderLib::printRegisteredUniforms("NoiseShader");
  ngl::ShaderLib::use("NoiseShader");
  // Now we will create a basic Camera from the graphics library
  // This is a static camera so it only needs to be set once
  // First create Values for the camera position
  ngl::Vec3 from(0,1,2);
  ngl::Vec3 to(0,0,0);
  ngl::Vec3 up(0,1,0);
  // now load to our new camera
  m_view=ngl::lookAt(from,to,up);
  ngl::Vec4 lightPos(-2.0f,5.0f,2.0f,0.0f);
  ngl::ShaderLib::setUniform("light.position",lightPos);
  ngl::ShaderLib::setUniform("light.ambient",0.0f,0.0f,0.0f,1.0f);
  ngl::ShaderLib::setUniform("light.diffuse",1.0f,1.0f,1.0f,1.0f);
  ngl::ShaderLib::setUniform("light.specular",0.8f,0.8f,0.8f,1.0f);
  // gold like NoiseShader material
  ngl::ShaderLib::setUniform("material.ambient",0.274725f,0.1995f,0.0745f,0.0f);
  ngl::ShaderLib::setUniform("material.diffuse",0.75164f,0.60648f,0.22648f,0.0f);
  ngl::ShaderLib::setUniform("material.specular",0.628281f,0.555802f,0.3666065f,0.0f);
  ngl::ShaderLib::setUniform("material.shininess",51.2f);
  ngl::ShaderLib::setUniform("viewerPos",from);

  ngl::ShaderLib::setUniform("time",0.0f);
  ngl::ShaderLib::setUniform("repeat",0.01f);

  startTimer(20);
}


void NGLScene::loadMatricesToShader()
{
  

  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat3 normalMatrix;
  ngl::Mat4 M;
  M=m_mouseGlobalTX;
  MV=  m_view*M;
  MVP= m_project*MV;
  normalMatrix=MV;
  normalMatrix.inverse().transpose();
  ngl::ShaderLib::setUniform("MV",MV);
  ngl::ShaderLib::setUniform("MVP",MVP);
  ngl::ShaderLib::setUniform("normalMatrix",normalMatrix);
  ngl::ShaderLib::setUniform("M",M);
}

void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_win.width,m_win.height);
  // grab an instance of the shader manager
  ngl::ShaderLib::use("NoiseShader");

  // Rotation based on the mouse position for our global transform
  ngl::Mat4 rotX;
  ngl::Mat4 rotY;
  // create the rotation matrices
  rotX.rotateX(m_win.spinXFace);
  rotY.rotateY(m_win.spinYFace);
  // multiply the rotations
  m_mouseGlobalTX=rotY*rotX;
  // add the translations
  m_mouseGlobalTX.m_m[3][0] = m_modelPos.m_x;
  m_mouseGlobalTX.m_m[3][1] = m_modelPos.m_y;
  m_mouseGlobalTX.m_m[3][2] = m_modelPos.m_z;

   // get the VBO instance and draw the built in teapot
  // draw
  loadMatricesToShader();
  ngl::VAOPrimitives::draw("teapot");


}


//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  static float repeat=0.1f;
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quit
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
  // turn on wirframe rendering
  case Qt::Key_W : glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); break;
  // turn off wire frame
  case Qt::Key_S : glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); break;
  // show full screen
  case Qt::Key_F : showFullScreen(); break;
  // show windowed
  case Qt::Key_N : showNormal(); break;
  case Qt::Key_1 :
    repeat-=0.01f;
    ngl::ShaderLib::setUniform("repeat",repeat);
  break;
  case Qt::Key_2 :
    repeat+=0.01f;
    ngl::ShaderLib::setUniform("repeat",repeat);
  break;
  default : break;
  }
  // finally update the GLWindow and re-draw
    update();
}

void NGLScene::timerEvent(QTimerEvent *)
{
  static float t=0.0f;
  
  ngl::ShaderLib::setUniform("time",t);
  t+=0.01f;
  update();
}
