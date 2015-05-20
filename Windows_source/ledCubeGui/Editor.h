
// Editor.h

#include "irrlicht.h"
#include "mouseReceiver.h"
#include <math.h>
#include "MastEventReceiver.h"
#include <vector>
#include <iostream>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
using namespace std;


// #define EDITOR_VIEW 1 // enable EditorView  

#define	EDITOR_SCREEN_WIDTH 600
#define EDITOR_SCREEN_HEIGHT 600
#define EDITOR_HALF_SCREEN_SIZE 300

#define	EDITORVIEW_SCREEN_WIDTH 640
#define EDITORVIEW_SCREEN_HEIGHT 480

#define CAM_SPEED 3
#define CAM_ROT_SPEED 10
#define CAM_POS_INC 1
#define CAM_POS_DEFAULT -420
#define NODE_ROTATION_SPEED 2
#define ROTATION_LIMIT 360

// Eksens.
typedef enum{ EKSEN_X, EKSEN_Y, EKSEN_Z }Eksen_t;

class Editor
{

public:

	Editor();
	~Editor();
	vector<vector3df> startEditor(); // start editor & editorView .
	void stopEditor(); // editor stop devices.

private:

	int CAMPOSX;//  Camera positions.
	int CAMPOSY;
	int CAMPOSZ;
	int ROTX; // Camera Rotations.
	int ROTY;
	int	ROTZ;
	int X, Y, Z; // mouse X Y Z .
	int mouseWheelCurr, mouseWheelBefore; //  mouse Wheel Old and next value.
	wchar_t strEditorView[250];  // String  X Y Z positions of Mouse
	char strEditorViewHelper[250];
	wchar_t stringEditor[100]; // String Camera informations positions  & rotations
	char stringEditorHelper[100];
	Eksen_t currentEksen; // Current Eksen.
	vector<ISceneNode *> *paintedNode = NULL;// painted nodes in editor.
	vector<vector3df> positionNodes;
	size_t size; // size that String convert wchar_t
	MastEventReceiver *myRecEditor = NULL; // Event Receivers for devices.
	MastEventReceiver *myRecEditorView = NULL;
	vector3df positionChildNode; // positions child node that will be added.
	ISceneNode *editorViewParentNode = NULL; // Scene Nodes...
	ISceneNode *editorViewChildNode = NULL;
	ISceneNode *editorParentNode = NULL;
	ISceneNode *editorChildNode = NULL;
	ISceneNode *nodeEditorCurrMouse = NULL;
	ISceneNode *nodeEditorViewCurrMouse = NULL;
	ISceneNode *tempNode = NULL;
	IrrlichtDevice *deviceEditor = NULL; // Devices.
	IrrlichtDevice *deviceEditorView = NULL;
	IVideoDriver *driverEditor = NULL; // drivers.
	IVideoDriver *driverEditorView = NULL;
	ISceneManager *smgrEditor = NULL; // Scene Managers.
	ISceneManager *smgrEditorView = NULL;
	IGUIEnvironment *guienvEditor = NULL; // Gui Environments.
	IGUIEnvironment *guienvEditorView = NULL;
	IGUIEditBox *eBoxEditor = NULL; // Edit Boxes.
	IGUIEditBox *eBoxEditorView = NULL;
	ICameraSceneNode * editorViewCam = NULL;
	ICameraSceneNode * editorCam = NULL;

	// MEthods.. Editor View Handle camera poositions & rotations. 
	// HANDLE CAMERA EditorView CONTROLS::
	/*
	ROT X   = KEY_R
	ROT Y   = KEY_T
	ROT Z   = KEY_Y

	CAMPOS X +  = KEY_A
	CAMPOS X -  = KEY_D
	CAMPOS Y +  = KEY_W
	CAMPOS Y -  = KEY_S
	CAMPOS Z +  = KEY_Q
	CAMPOS Z -  = KEY_E
	*/

	void handleCameraDevice2(MastEventReceiver * receiver, ICameraSceneNode * cam, IGUIEditBox *editBox, ISceneNode * parentNode);
	void initEditor(); // Initilazitons.
	void initEditorView(); // Initilazitons.
	void drawOneNode(const vector3df &v3);
	void drawLine();
	vector3df vFirst, vLast; // for draw line .
	// CAMERA CONTROLS 	
	/*
	CAMPOS X + = KEY_RIGHT
	CAMPOS X - = KEY_LEFT
	CAMPOS Y + = KEY_UP
	CAMPOS Y - = KEY_DOWN
	CAMPOS Z + = KEY_F2
	CAMPOS Z - = KEY_F1
	*/
	void cameraCalibration(MastEventReceiver *receiver, ICameraSceneNode *cam); // camera calibration method.
	void drawEditorFrame(IVideoDriver *driver); // draw editor frame lines.
};
