
// Editor.cpp

#include "Editor.h"

Editor::Editor()
{
	paintedNode = new vector<ISceneNode *>(0);
	initEditor();
	vFirst = vector3df(0, 0, 0);
	vLast = vector3df(0, 0, 0);
#ifdef EDITOR_VIEW
	initEditorView();
#endif
}

// DESTRUCTOR. 
Editor::~Editor()
{
	if (deviceEditor != NULL)
		if (!deviceEditor->run())
			deviceEditor->drop();
	if (myRecEditor != NULL)
		delete[] myRecEditor;
	if (paintedNode != NULL)
		delete paintedNode;
#ifdef EDITOR_VIEW

	if (deviceEditorView != NULL)
		if (!deviceEditorView->run())
			deviceEditorView->drop();

	if (myRecEditorView != NULL)
		delete[] myRecEditorView;
#endif
}

// PRIVATE METHODS ..
// initilazitions editor.
void Editor::initEditor(){

	// Receivers ..
	myRecEditor = new MastEventReceiver();
	// create Devices.
#if defined(_WIN32) || defined(_WIN64)
	deviceEditor = createDevice(video::EDT_OPENGL, dimension2d<u32>(EDITOR_SCREEN_WIDTH, EDITOR_SCREEN_HEIGHT), 16, false, false, false, myRecEditor);
	
#else

	deviceEditor = createDevice(video::EDT_OPENGL, dimension2d<u32>(EDITOR_SCREEN_WIDTH, EDITOR_SCREEN_HEIGHT), 16, false, false, false, myRecEditor);
#endif

	if (!deviceEditor) // Errors creating device.
		return;
	deviceEditor->setWindowCaption(L"Editor Platform  X - Y Ekseni ");// set Header of Window .
	// Video driver..
	driverEditor = deviceEditor->getVideoDriver();
	// Scene Manager.
	smgrEditor = deviceEditor->getSceneManager();
	//Gui environment.
	guienvEditor = deviceEditor->getGUIEnvironment();
	//Edit box
	eBoxEditor = guienvEditor->addEditBox(L"", rect<s32>(10, 10, 260, 22), true, 0, 101);
	positionNodes = vector < vector3df >(0);
	// Scene NODES ..
	editorParentNode = smgrEditor->addEmptySceneNode();
	editorParentNode->setVisible(true);
	editorChildNode = smgrEditor->addSphereSceneNode();
	nodeEditorCurrMouse = editorChildNode->clone();

	if (nodeEditorCurrMouse && editorChildNode){

		editorParentNode->setVisible(true);
		editorChildNode->setMaterialFlag(EMF_LIGHTING, false);
		editorChildNode->setScale(editorChildNode->getScale()*0.60f);
#if defined(_WIN32) || defined(_WIN64)
		editorChildNode->setMaterialTexture(0, driverEditor->getTexture("media/blueTexture.png"));

#else

		editorChildNode->setMaterialTexture(0, driverEditor->getTexture("../media/blueTexture.png"));
#endif

		editorChildNode->setVisible(false);

		nodeEditorCurrMouse->setMaterialFlag(EMF_LIGHTING, false);
#if defined(_WIN32) || defined(_WIN64)
		nodeEditorCurrMouse->setMaterialTexture(0, driverEditor->getTexture("media/redTexture.png"));

#else

		nodeEditorCurrMouse->setMaterialTexture(0, driverEditor->getTexture("../media/redTexture.png"));
#endif

		nodeEditorCurrMouse->setVisible(true);
		nodeEditorCurrMouse->setScale(nodeEditorCurrMouse->getScale()*1.3f);
	}


	// add camera
	editorCam = smgrEditor->addCameraSceneNode(0, vector3df(0, 0, CAM_POS_DEFAULT), vector3df(0, 0, -CAM_POS_DEFAULT));
	X = 0;
	Y = 0;
	Z = 0;

	// camera View ..
	currentEksen = EKSEN_Z;
	editorParentNode->addChild(nodeEditorCurrMouse);
	deviceEditor->getCursorControl()->setVisible(false); // Unvisible mouse cursor
	mouseWheelBefore = 0; // mouse depth value set 0.
	smgrEditor->setActiveCamera(editorCam);
}

void Editor::stopEditor(){

	deviceEditor->closeDevice();
}

// initilazitions editorView ..
void Editor::initEditorView(){

	// Receiver ..
	myRecEditorView = new MastEventReceiver();

#if defined(_WIN32) || defined(_WIN64)
	// create Device.
	deviceEditorView = createDevice(video::EDT_OPENGL, dimension2d<u32>(EDITORVIEW_SCREEN_WIDTH, EDITORVIEW_SCREEN_HEIGHT), 16, false, false, false, myRecEditorView);

#else

	// create Device.
	deviceEditorView = createDevice(video::EDT_OPENGL, dimension2d<u32>(EDITORVIEW_SCREEN_WIDTH, EDITORVIEW_SCREEN_HEIGHT), 16, false, false, false, myRecEditorView);
#endif

	if (!deviceEditorView) // Errors creating device.
		return;

	// set Header of Window .
	deviceEditorView->setWindowCaption(L"Editor View");
	// Video driver ..
	driverEditorView = deviceEditorView->getVideoDriver();
	// Scene Manager.
	smgrEditorView = deviceEditorView->getSceneManager();
	//gui Environment
	guienvEditorView = deviceEditorView->getGUIEnvironment();
	//Create Edit box
	eBoxEditorView = guienvEditorView->addEditBox(L"", rect<s32>(10, 10, 400, 22), true, 0, 102);
	// Scene NODES ..

	editorViewParentNode = smgrEditorView->addEmptySceneNode();
	editorViewParentNode->setVisible(true);
	editorViewChildNode = smgrEditorView->addSphereSceneNode();
	nodeEditorViewCurrMouse = editorViewChildNode->clone();

	if (editorViewChildNode && nodeEditorViewCurrMouse){ // Child Node sets .
		editorViewChildNode->setMaterialFlag(EMF_LIGHTING, false);
		editorViewChildNode->setScale(editorViewChildNode->getScale()*0.75f);
#if defined(_WIN32) || defined(_WIN64)
		editorViewChildNode->setMaterialTexture(0, driverEditorView->getTexture("media/blueTexture.png"));

#else

		editorViewChildNode->setMaterialTexture(0, driverEditorView->getTexture("../media/blueTexture.png"));
#endif

		editorViewChildNode->setVisible(false);
		nodeEditorViewCurrMouse->setMaterialFlag(EMF_LIGHTING, false);
		nodeEditorViewCurrMouse->setScale(nodeEditorViewCurrMouse->getScale()*1.3f);
#if defined(_WIN32) || defined(_WIN64)
		nodeEditorViewCurrMouse->setMaterialTexture(0, driverEditorView->getTexture("media/redTexture.png"));

#else

		nodeEditorViewCurrMouse->setMaterialTexture(0, driverEditorView->getTexture("../media/redTexture.png"));
#endif

		nodeEditorViewCurrMouse->setVisible(true);
	}
	// add camera
	editorViewCam = smgrEditorView->addCameraSceneNode(0, vector3df(0, 0, CAM_POS_DEFAULT), vector3df(0, 0, -CAM_POS_DEFAULT));
	// Editor view Camera ..
	CAMPOSX = 0;
	CAMPOSY = 0;
	CAMPOSZ = -105;
	ROTX = 0;
	ROTY = 0;
	ROTZ = 0;
	editorViewParentNode->addChild(nodeEditorViewCurrMouse);
	smgrEditorView->setActiveCamera(editorViewCam);
}

vector<vector3df> Editor::startEditor(){

	myRecEditor->startEventProcess();
#ifdef EDITOR_VIEW
	myRecEditorView->startEventProcess();
#endif
	bool leftPress = false;
	mouseWheelCurr = 0;
	double value = 0;
	CAMPOSX = 0;
	CAMPOSY = 0;
	CAMPOSZ = CAM_POS_DEFAULT;
	while (deviceEditor->run())
	{
		mouseWheelCurr = (int)(myRecEditor->mouseWheel()*1.29);

		if (currentEksen == EKSEN_X)
			X = mouseWheelCurr;
		else
			X = myRecEditor->mouseX() - EDITOR_HALF_SCREEN_SIZE;

		if (currentEksen == EKSEN_Y)
			Y = mouseWheelCurr;
		else{
			Y = (EDITOR_HALF_SCREEN_SIZE - myRecEditor->mouseY());
		}

		if (currentEksen == EKSEN_X)
			Z = (myRecEditor->mouseX() - EDITOR_HALF_SCREEN_SIZE);
		else if (currentEksen == EKSEN_Y)
			Z = EDITOR_HALF_SCREEN_SIZE - myRecEditor->mouseY();
		else
			Z = mouseWheelCurr;


		driverEditor->beginScene(true, true, SColor(204, 204, 204, 204));
#ifdef EDITOR_VIEW
		driverEditorView->beginScene(true, true, SColor(204, 204, 204, 204));
#endif
		cameraCalibration(myRecEditor, editorCam);
		if (myRecEditor->keyDown(KEY_KEY_W)){ // ....  EVENT HANDLERS  .... 
			currentEksen = EKSEN_Y;
			deviceEditor->setWindowCaption(L"Editor Platform  X - Z Ekseni ");
			editorParentNode->setRotation(vector3df(((int)(editorParentNode->getRotation().X + NODE_ROTATION_SPEED)) % ROTATION_LIMIT, 0, 0));
		}
		else if (myRecEditor->keyDown(KEY_KEY_S)){
			currentEksen = EKSEN_Z;
			deviceEditor->setWindowCaption(L"Editor Platform  X - Y Ekseni ");
			editorCam->setPosition(vector3df(0, 0, CAM_POS_DEFAULT + (mouseWheelCurr * CAM_POS_INC)));
			editorParentNode->setRotation(vector3df(0, 0, 0));
			CAMPOSX = 0;
			CAMPOSY = 0;
			CAMPOSZ = CAM_POS_DEFAULT;
		}
		else if (myRecEditor->keyDown(KEY_KEY_A)){
			currentEksen = EKSEN_X;
			deviceEditor->setWindowCaption(L"Editor Platform  Y - Z Ekseni ");
			editorParentNode->setRotation(vector3df(0, ((int)(editorParentNode->getRotation().Y + NODE_ROTATION_SPEED)) % ROTATION_LIMIT, 0));
		}
		else if (myRecEditor->keyDown(KEY_KEY_D)){
			currentEksen = EKSEN_Z;
			deviceEditor->setWindowCaption(L"Editor Platform  X - Y Ekseni ");
			editorParentNode->setRotation(vector3df(0, 0, ((int)(editorParentNode->getRotation().Z + NODE_ROTATION_SPEED)) % ROTATION_LIMIT));
		}
		else if (myRecEditor->keyDown(KEY_KEY_P))
		{
			stopEditor();
		}

		if (myRecEditor->leftMousePressed()){
			if (!leftPress)
				vFirst = vLast; // Clear Buffer.
			drawLine();
			leftPress = true;
		}
		if (myRecEditor->leftMouseReleased()){
			if (leftPress){
				vFirst = vLast; // Clear buffer.
				leftPress = false;
			}
		}
		if (myRecEditor->rightMouseDown()){

			drawLine();
		}
		if (myRecEditor->keyDown(KEY_KEY_C))
		{
			positionNodes.clear();
			smgrEditor->clear();
			editorParentNode = smgrEditor->addEmptySceneNode();
			editorParentNode->setVisible(true);
			editorChildNode = smgrEditor->addSphereSceneNode();
			nodeEditorCurrMouse = editorChildNode->clone();

			if (nodeEditorCurrMouse && editorChildNode){

				editorParentNode->setVisible(true);
				editorChildNode->setMaterialFlag(EMF_LIGHTING, false);
				editorChildNode->setScale(editorChildNode->getScale()*0.60f);
#if defined(_WIN32) || defined(_WIN64)
				editorChildNode->setMaterialTexture(0, driverEditor->getTexture("media/blueTexture.png"));

#else

				editorChildNode->setMaterialTexture(0, driverEditor->getTexture("../media/blueTexture.png"));
#endif

				editorChildNode->setVisible(false);

				nodeEditorCurrMouse->setMaterialFlag(EMF_LIGHTING, false);
#if defined(_WIN32) || defined(_WIN64)
				nodeEditorCurrMouse->setMaterialTexture(0, driverEditor->getTexture("media/redTexture.png"));

#else

				nodeEditorCurrMouse->setMaterialTexture(0, driverEditor->getTexture("../media/redTexture.png"));
#endif

				nodeEditorCurrMouse->setVisible(true);
				nodeEditorCurrMouse->setScale(nodeEditorCurrMouse->getScale()*1.3f);
			}
			// add camera
			editorCam = smgrEditor->addCameraSceneNode(0, vector3df(0, 0, CAM_POS_DEFAULT), vector3df(0, 0, -CAM_POS_DEFAULT));
			X = 0;
			Y = 0;
			Z = 0;

			// camera View ..
			currentEksen = EKSEN_Z;
			editorParentNode->addChild(nodeEditorCurrMouse);
			deviceEditor->getCursorControl()->setVisible(false); // Unvisible mouse cursor
			mouseWheelBefore = 0; // mouse depth value set 0.
			smgrEditor->setActiveCamera(editorCam);

		}

		drawEditorFrame(driverEditor);
		driverEditor->draw3DLine(vector3df(-EDITOR_SCREEN_WIDTH, 0, 0), vector3df(EDITOR_SCREEN_WIDTH, 0, 0), SColor(255, 0, 255, 0));
		driverEditor->draw3DLine(vector3df(0, -EDITOR_SCREEN_WIDTH, 0), vector3df(0, EDITOR_SCREEN_WIDTH, 0), SColor(255, 255, 0, 0));
		driverEditor->draw3DLine(vector3df(0, 0, -EDITOR_SCREEN_WIDTH), vector3df(0, 0, EDITOR_SCREEN_WIDTH), SColor(0, 0, 0, 0));

		/*  Current node will be painted on editor */
		nodeEditorCurrMouse->setPosition(vector3df(X, Y, Z));
#ifdef EDITOR_VIEW
		nodeEditorViewCurrMouse->setPosition(vector3df(X, Y, Z));
		handleCameraDevice2(myRecEditorView, editorViewCam, eBoxEditorView, editorViewParentNode);
#endif


		snprintf(stringEditorHelper, 100, "X: %d   Y: %d   Z: %d ", X, Y, Z);
		mbstowcs(stringEditor, stringEditorHelper, (size_t)100);
		eBoxEditor->setText(stringEditor);

		guienvEditor->drawAll();
		smgrEditor->drawAll();
		driverEditor->endScene();
#ifdef EDITOR_VIEW		
		smgrEditorView->drawAll();
		guienvEditorView->drawAll();
		driverEditorView->endScene();
#endif

	}
	return positionNodes;
}

void Editor::handleCameraDevice2(MastEventReceiver * receiver, ICameraSceneNode * cam, IGUIEditBox *editBox, ISceneNode * parentNode){

	if (receiver->keyDown(KEY_KEY_W)){
		CAMPOSY += CAM_SPEED;
		cam->setPosition(vector3df(CAMPOSX, CAMPOSY, CAMPOSZ));
	}
	else if (receiver->keyDown(KEY_KEY_S)){
		CAMPOSY -= CAM_SPEED;
		cam->setPosition(vector3df(CAMPOSX, CAMPOSY, CAMPOSZ));
	}
	else if (receiver->keyDown(KEY_KEY_A)){
		CAMPOSX += CAM_SPEED;
		cam->setPosition(vector3df(CAMPOSX, CAMPOSY, CAMPOSZ));
	}
	else if (receiver->keyDown(KEY_KEY_D)){
		CAMPOSX -= CAM_SPEED;
		cam->setPosition(vector3df(CAMPOSX, CAMPOSY, CAMPOSZ));
	}
	else if (receiver->keyDown(KEY_KEY_Q)){
		CAMPOSZ += CAM_SPEED;
		cam->setPosition(vector3df(CAMPOSX, CAMPOSY, CAMPOSZ));
	}
	else if (receiver->keyDown(KEY_KEY_E)){
		CAMPOSZ -= CAM_SPEED;
		cam->setPosition(vector3df(CAMPOSX, CAMPOSY, CAMPOSZ));
	}

	if (receiver->keyDown(KEY_KEY_R)){
		ROTX += CAM_ROT_SPEED;
		if (ROTX > 360)
			ROTX = 0;
		parentNode->setRotation(vector3df(ROTX, ROTY, ROTZ));
	}
	else if (receiver->keyDown(KEY_KEY_T)){
		ROTY += CAM_ROT_SPEED;
		if (ROTY > 360)
			ROTY = 0;
		parentNode->setRotation(vector3df(ROTX, ROTY, ROTZ));
	}
	else if (receiver->keyDown(KEY_KEY_Y)){
		ROTZ += CAM_ROT_SPEED;
		if (ROTZ > 360)
			ROTZ = 0;
		parentNode->setRotation(vector3df(ROTX, ROTY, ROTZ));
	}

	snprintf(strEditorViewHelper, 250, "CAMERA POS   X: %d   Y: %d   Z: %d  <<<< >>>  ROT   X: %d Y: %d Z: %d ", CAMPOSX, CAMPOSY, CAMPOSZ, ROTX, ROTY, ROTZ);
	mbstowcs(strEditorView, strEditorViewHelper, (size_t)250);
	editBox->setText(strEditorView);


}

inline void Editor::drawOneNode(const vector3df &v3){
	positionChildNode = v3;
	editorChildNode->setPosition(positionChildNode);
	editorChildNode->setVisible(true);
	tempNode = editorChildNode->clone();
	editorParentNode->addChild(tempNode);
	editorChildNode->setVisible(false);

	positionNodes.push_back(v3);
#ifdef EDITOR_VIEW
	editorViewChildNode->setPosition(positionChildNode);
	editorViewChildNode->setVisible(true);
	tempNode = editorViewChildNode->clone();
	editorViewParentNode->addChild(tempNode);
	editorViewChildNode->setVisible(false);
#endif 
	paintedNode->push_back(tempNode);
}

inline void Editor::drawLine(){
	if (vFirst == vLast){
		vFirst = vector3df(X, Y, Z);
		drawOneNode(vFirst);

	}
	else{
		vLast = vector3df(X, Y, Z);
		Eksen_t stdIncEksen = EKSEN_X;// Standart increment +1  eksen hangisi. default X ekseni
		double x1, x2, y1, y2, z1, z2, xFark, yFark, zFark, xInc, yInc, zInc;
		int loopSize;
		x1 = vFirst.X;
		x2 = vLast.X;
		y1 = vFirst.Y;
		y2 = vLast.Y;
		z1 = vFirst.Z;
		z2 = vLast.Z;
		xFark = x2 - x1;
		yFark = y2 - y1;
		zFark = z2 - z1;
		if (fabs(zFark) > fabs(xFark) && fabs(zFark) > fabs(yFark))
			stdIncEksen = EKSEN_Z;
		else if (fabs(yFark) > fabs(xFark) && fabs(zFark) < fabs(yFark))
			stdIncEksen = EKSEN_Y;
		if (stdIncEksen == EKSEN_X){
			xInc = xFark / fabs(xFark);
			yInc = yFark / fabs(xFark);
			zInc = zFark / fabs(xFark);
			loopSize = fabs(xFark);
		}
		else if (stdIncEksen == EKSEN_Z){
			zInc = zFark / fabs(zFark);
			xInc = xFark / fabs(zFark);
			yInc = yFark / fabs(zFark);
			loopSize = fabs(zFark);
		}
		else{// Y eksen.
			yInc = yFark / fabs(yFark);
			xInc = xFark / fabs(yFark);
			zInc = zFark / fabs(yFark);
			loopSize = fabs(yFark);
		}
		for (size_t i = 1; i <= loopSize; i++)
		{
			drawOneNode(vector3df(x1 + (xInc*i), y1 + (yInc*i), z1 + (zInc*i)));
		}
		vFirst = vLast; // UPDATE NODE;
		vLast = vector3df(0, 0, 0);

	}

}

inline void Editor::cameraCalibration(MastEventReceiver *receiver, ICameraSceneNode *cam){

	if (receiver->keyDown(KEY_UP)){

		CAMPOSY += CAM_SPEED;
		cam->setPosition(vector3df(CAMPOSX, CAMPOSY, CAMPOSZ));

	}
	else if (receiver->keyDown(KEY_DOWN)){

		CAMPOSY -= CAM_SPEED;
		cam->setPosition(vector3df(CAMPOSX, CAMPOSY, CAMPOSZ));

	}
	else if (receiver->keyDown(KEY_RIGHT)){
		CAMPOSX += CAM_SPEED;
		cam->setPosition(vector3df(CAMPOSX, CAMPOSY, CAMPOSZ));
	}
	else if (receiver->keyDown(KEY_LEFT)){
		CAMPOSX -= CAM_SPEED;
		cam->setPosition(vector3df(CAMPOSX, CAMPOSY, CAMPOSZ));
	}
	else if (receiver->keyDown(KEY_F1)){
		CAMPOSZ -= CAM_SPEED;
		cam->setPosition(vector3df(CAMPOSX, CAMPOSY, CAMPOSZ));
	}
	else if (receiver->keyDown(KEY_F2)){
		CAMPOSZ += CAM_SPEED;
		cam->setPosition(vector3df(CAMPOSX, CAMPOSY, CAMPOSZ));
	}
}

inline void Editor::drawEditorFrame(IVideoDriver *driver){
	/*for (int i = -2*EDITOR_HALF_SCREEN_SIZE; i < 2*EDITOR_HALF_SCREEN_SIZE; i += 25)
	{
	if (currentEksen == EKSEN_Z){
	driver->draw3DLine(vector3df(-EDITOR_SCREEN_WIDTH, i, 0), vector3df(EDITOR_SCREEN_WIDTH, i, 0), SColor(255, 184, 184, 184));
	driver->draw3DLine(vector3df(i, -EDITOR_SCREEN_WIDTH, 0), vector3df(i, EDITOR_SCREEN_WIDTH, 0), SColor(255, 184, 184, 184));
	} else if(currentEksen == EKSEN_X){
	driver->draw3DLine(vector3df(0, -EDITOR_SCREEN_WIDTH, i), vector3df(0, EDITOR_SCREEN_WIDTH, i), SColor(255, 184, 184, 184));
	driver->draw3DLine(vector3df(0, i, -EDITOR_SCREEN_WIDTH), vector3df(0, i, EDITOR_SCREEN_WIDTH), SColor(255, 184, 184, 184));
	}
	else if (currentEksen == EKSEN_Y){
	driver->draw3DLine(vector3df(i, 0, -EDITOR_SCREEN_WIDTH), vector3df(i, 0, EDITOR_SCREEN_WIDTH), SColor(255, 184, 184, 184));
	driver->draw3DLine(vector3df(-EDITOR_SCREEN_WIDTH, 0, i), vector3df(EDITOR_SCREEN_WIDTH, 0 ,i), SColor(255, 184, 184, 184));
	}
	}*/
	for (int i = -2 * EDITOR_HALF_SCREEN_SIZE; i < 2 * EDITOR_HALF_SCREEN_SIZE; i += 50)
	{
		driver->draw3DLine(vector3df(-EDITOR_SCREEN_WIDTH, i, EDITOR_SCREEN_WIDTH), vector3df(EDITOR_SCREEN_WIDTH, i, EDITOR_SCREEN_WIDTH), SColor(255, 184, 184, 184));
		driver->draw3DLine(vector3df(i, -EDITOR_SCREEN_WIDTH, EDITOR_SCREEN_WIDTH), vector3df(i, EDITOR_SCREEN_WIDTH, EDITOR_SCREEN_WIDTH), SColor(255, 184, 184, 184));
		driver->draw3DLine(vector3df(EDITOR_SCREEN_WIDTH, -EDITOR_SCREEN_WIDTH, i), vector3df(EDITOR_SCREEN_WIDTH, EDITOR_SCREEN_WIDTH, i), SColor(255, 184, 184, 184));
		driver->draw3DLine(vector3df(EDITOR_SCREEN_WIDTH, i, -EDITOR_SCREEN_WIDTH), vector3df(EDITOR_SCREEN_WIDTH, i, EDITOR_SCREEN_WIDTH), SColor(255, 184, 184, 184));
		driver->draw3DLine(vector3df(i, EDITOR_SCREEN_WIDTH, -EDITOR_SCREEN_WIDTH), vector3df(i, EDITOR_SCREEN_WIDTH, EDITOR_SCREEN_WIDTH), SColor(255, 184, 184, 184));
		driver->draw3DLine(vector3df(-EDITOR_SCREEN_WIDTH, EDITOR_SCREEN_WIDTH, i), vector3df(EDITOR_SCREEN_WIDTH, EDITOR_SCREEN_WIDTH, i), SColor(255, 184, 184, 184));

		driver->draw3DLine(vector3df(-EDITOR_SCREEN_WIDTH, i, -EDITOR_SCREEN_WIDTH), vector3df(EDITOR_SCREEN_WIDTH, i, -EDITOR_SCREEN_WIDTH), SColor(255, 184, 184, 184));
		driver->draw3DLine(vector3df(i, -EDITOR_SCREEN_WIDTH, -EDITOR_SCREEN_WIDTH), vector3df(i, EDITOR_SCREEN_WIDTH, -EDITOR_SCREEN_WIDTH), SColor(255, 184, 184, 184));
		driver->draw3DLine(vector3df(-EDITOR_SCREEN_WIDTH, -EDITOR_SCREEN_WIDTH, i), vector3df(-EDITOR_SCREEN_WIDTH, EDITOR_SCREEN_WIDTH, i), SColor(255, 184, 184, 184));
		driver->draw3DLine(vector3df(-EDITOR_SCREEN_WIDTH, i, -EDITOR_SCREEN_WIDTH), vector3df(-EDITOR_SCREEN_WIDTH, i, EDITOR_SCREEN_WIDTH), SColor(255, 184, 184, 184));
		driver->draw3DLine(vector3df(i, -EDITOR_SCREEN_WIDTH, -EDITOR_SCREEN_WIDTH), vector3df(i, -EDITOR_SCREEN_WIDTH, EDITOR_SCREEN_WIDTH), SColor(255, 184, 184, 184));
		driver->draw3DLine(vector3df(-EDITOR_SCREEN_WIDTH, -EDITOR_SCREEN_WIDTH, i), vector3df(EDITOR_SCREEN_WIDTH, -EDITOR_SCREEN_WIDTH, i), SColor(255, 184, 184, 184));

	}
}