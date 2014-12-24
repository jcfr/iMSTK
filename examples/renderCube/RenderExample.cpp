#include "smCore/smSDK.h"
#include "smCore/smTextureManager.h"
#include "RenderExample.h"

/// \brief A simple example of how to render and object using SimMedTK
/// 
/// \detail This is the default constructor, however, this is where the main 
/// program runs.  This program will create a cube with a texture pattern 
/// numbering each side of the cube, that's all it does.
RenderExample::RenderExample()
{
	//Create an instance of the SimMedTK framework/SDK
	simmedtkSDK = smSDK::createSDK();

	//Create a new scene to work in
	scene1 = simmedtkSDK->createScene();

	//Create a viewer to see the scene through
	viewer = simmedtkSDK->createViewer();

	//Initialize the texture manager
	smTextureManager::init(smSDK::getErrorLog());

	//Load in the texture for the cube model
	smTextureManager::loadTexture("textures/cube.png", "cubetex");

	//Load the cube model
	cube.mesh->loadMesh("models/cube.obj", SM_FILETYPE_OBJ);

	//Assign the previously loaded texture to the cube model
	cube.mesh->assignTexture("cubetex");
	//Tell SimMedTK to render the faces of the model, and the texture assigned
	cube.renderDetail.renderType = (SIMMEDTK_RENDER_FACES | SIMMEDTK_RENDER_TEXTURE);

	//Add the cube to the scene to be rendered
	scene1->addSceneObject(&cube);

	//Setup the window title in the window manager
	viewer->setWindowTitle("SimMedTK RENDER TEST");
	//Add the RenderExample object we are in to the viewer from the SimMedTK SDK
	viewer->addObject(this);
	//Set some camera parameters
	viewer->camera()->setZClippingCoefficient(1000);
	viewer->camera()->setZNearCoefficient(0.001);
	viewer->camera()->setFieldOfView(SM_DEGREES2RADIANS(60));

	//Link up the event system between the viewer and the SimMedTK SDK
	//Note: This allows some default behavior like mouse and keyboard control
	viewer->setEventDispatcher(simmedtkSDK->getEventDispatcher());

	simmedtkSDK->getEventDispatcher()->registerEventHandler(viewer,SIMMEDTK_EVENTTYPE_KEYBOARD);

	//Run the simulator framework
	simmedtkSDK->run();
}

void renderExample() {
	RenderExample *re = new RenderExample();
	delete re;
}