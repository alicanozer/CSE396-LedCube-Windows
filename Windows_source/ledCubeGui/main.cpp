
// MAIN_GUI / main.cpp

#include <irrlicht.h>
#include <math.h>
#include <list>
#include <vector>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include "rs232.h"
#include "SerialCom.h"
#include "Editor.h"

#define CUBE_MODE 1
#define BAUDRATE 115200

#define CUBE_X_SUM_VALUE 300
#define CUBE_Y_SUM_VALUE 130	//y merkez

   
#define CUBE1_X_CENTER -450

#define X_MIN -600
#define X_MAX 300

#define Y_MAX 200
#define Y_MIN 60

#define Z_MIN -150
#define Z_MAX 150

#define DIV_POS_X 2.0
#define  DIV_POS_Y 4.3
#define  DIV_POS_Z 2.0

#define LED_CUBE_REDUCE_X ((X_MAX - X_MIN) / 23.0)
#define LED_CUBE_REDUCE_Y ((Y_MAX - Y_MIN) / 7.0 )
#define LED_CUBE_REDUCE_Z ((Z_MAX - Z_MIN) / 7.0)

#define MOVE_VALUE 5
#define ENLARGE_SCALE_VALUE 1.03
#define REDUCTION_SCALE_VALUE 0.97
#define ROTATION_VALUE 10

#define SCALE_CHILD 2.0

#define EMU_ROT_SPEED 10


using namespace irr;
using namespace gui;
using namespace video;
using namespace scene;
using namespace core;
using namespace io;
using namespace std;

// function protptypes
void findExtremePoints(int cubeNo);
void calcCubeCoor(int CubeNo);
bool isPossibleMove(int cubeNo);
bool InitializeVideo();
void RenderAll();
void CreateGUI();
void moveNode(vector3df vel, int cubeNo);
void clearNodes(int cubeNo);
void rotateNode(irr::core::vector3df rot, int cubeNo, int rotNo);
void scaleMesh(ISceneNode* mesh, vector3df scaleVector, int cubeNo);
void sendCube(unsigned char cube[8][24][8], int portNo);
void createCubeSanal();
void rotationEmulators();

//******************
//IRRLICHT VARIABLES
//******************
IrrlichtDevice *mydevice;
IVideoDriver *mydriver;
ISceneManager *smgr;
IGUIEnvironment *mygui;
ITexture* imageTitle;
ITexture* imagedownTitle;
ISceneNode *sanalCube1, *sanalCube2, *sanalCube3;
int emuRotX = 0, emuRotY = 0, emuRotZ = 0; // Rotation Emulator.
ISceneNode* viewParentNode1, *pivotNode;
ISceneNode * viewChildNode1;
ISceneNode* viewParentNode2;
ISceneNode * viewChildNode2;
ISceneNode* viewParentNode3;
ISceneNode * viewChildNode3;

ISceneNode* extremeNode11[3];
ISceneNode* extremeNode12[3];
ISceneNode* extremeNode13[3];
ISceneNode* extremeNode14[3];
ISceneNode* extremeNode15[3];
ISceneNode* extremeNode16[3];

vector<ISceneNode*> nodes1(0);
vector<ISceneNode*> nodes2(0);
vector<ISceneNode*> nodes3(0);

IGUIComboBox *listCube;
IGUIComboBox *listChar;
IGUIComboBox *listObj;
IGUIComboBox *listAnimation;

IGUIScrollBar *scale1;
IGUIScrollBar *scale2;
IGUIScrollBar *scale3;
IGUIEditBox * editX, *editY, *editZ;
IGUICheckBox *checkPivot;

IGUIImage *img1;
IGUIImage *img2;

IGUIScrollBar *speed1;
IGUIScrollBar *speed2;
IGUIScrollBar *speed3;

// scale variables
vector3df currScaleCube1 = vector3df(1.0, 1.0, 1.0);
vector3df currScaleCube2 = vector3df(1.0, 1.0, 1.0);
vector3df currScaleCube3 = vector3df(1.0, 1.0, 1.0);

// single editor object
Editor *editor;
unsigned char current[8][24] = { 0 };
int mouseX, mouseY, oldMouseX, oldMouseY, mouseWheel;
unsigned char cubeCoor[8][24][8];
int moveNo;
int moveZ[3] = { 0, 0, 0 };
bool rotX = false, rotY = false;
bool noktaDegistimi=false;

vector3df centerPoint[3];			//center of the objects
vector3df pivotPoint;				//pivot points
ISceneNode* centerNodes[3];			// center nodes

//com port values
int comportNo;
unsigned char compArray[SIZE][SIZE][1];

// Cameras..
ICameraSceneNode *camera[6] = {0,0, 0, 0, 0, 0 };
ISceneNode		*sanalNode[8][8][8][3], *nodeCube;
//*********************
//MAIN PROGRAM FUNCTION
//*********************
int main()
{
	//start video
	if (!InitializeVideo())
		return 1;

	//get video driver and scene manager
	mydriver = mydevice->getVideoDriver();
	smgr = mydevice->getSceneManager();
	mygui = mydevice->getGUIEnvironment();


	//ICameraSceneNode *viewCamera = smgr->addCameraSceneNode(0, vector3df(0, 0, -640), vector3df(0, 0, 0));
	//smgr->setActiveCamera(viewCamera);

	viewParentNode1 = smgr->addEmptySceneNode();
	viewChildNode1 = smgr->addEmptySceneNode();

	viewParentNode2 = smgr->addEmptySceneNode();
	viewChildNode2 = smgr->addEmptySceneNode();

	viewParentNode3 = smgr->addEmptySceneNode();
	viewChildNode3 = smgr->addEmptySceneNode();

#ifdef CUBE_MODE
	/*Open comport*/
	cout << "Enter the port Number" << endl;
	cin >> comportNo;
	
	//comportNo = 3;

	if (OpenComport(comportNo, BAUDRATE) == 1){
		cerr << "Cannot open comport port no:  " << comportNo << endl;
		return -1;
	}
	SendByte(comportNo, 0XFF);
	SendByte(comportNo, 0XFF);
#if defined(_WIN32) || defined(_WIN64)
	Sleep(5);
#else
	usleep(5000);
#endif
#endif

	//create the GUI
	CreateGUI();

	//start updating the screen
	RenderAll();

	//do cleanup
	mydevice->drop();

	return 0;
}


//********************
//EVENT RECEIVER CLASS
//********************
class GuiEventReceiver : public IEventReceiver
{
protected:
	enum keyStatesENUM { UP, DOWN, PRESSED, RELEASED };
	keyStatesENUM keyState[KEY_KEY_CODES_COUNT];
public:

	GuiEventReceiver(){}

	virtual bool OnEvent(const SEvent& event)
	{

		//GUI EVENT
		if (event.EventType == EET_GUI_EVENT)
		{
			s32 id = event.GUIEvent.Caller->getID();

			switch (event.GUIEvent.EventType)		//tüm gui eventleri bu switchin içinde olacak
			{
			case EGET_BUTTON_CLICKED:
				if (id == 4)//küpleri resetler
				{
					clearNodes(0);
					clearNodes(1);
					clearNodes(2);

#ifdef CUBE_MODE
					SendByte(comportNo, 0XFF);
					SendByte(comportNo, 0XFF);
#if defined(_WIN32) || defined(_WIN64)
					Sleep(5);
#else
					usleep(5000);
#endif
#endif
				}

				if (id == 2)	//objects
				{
					int objeID = listObj->getSelected();
					int selectedCube = listCube->getSelected();
					clearNodes(selectedCube - 2);					
					if (objeID == 1){//3d plus object
						
					}else{
						std::string objs;
						if (objeID == 2){
#if defined(_WIN32) || defined(_WIN64)
							objs = "media/cube.txt";
#else
							objs = "../media/cube.txt";
#endif
						}else if (objeID == 3){
#if defined(_WIN32) || defined(_WIN64)
							objs = "media/silindir.txt";
#else
							objs = "../media.silindir.txt";
#endif
						}else if (objeID == 4){
#if defined(_WIN32) || defined(_WIN64)
							objs = "media/plus.txt";
#else
							objs = "../media/plus.txt";
#endif
						}else if (objeID == 5){
#if defined(_WIN32) || defined(_WIN64)
							objs = "media/cubeFull.txt";
#else
							objs = "../media/cubeFull.txt";
#endif
						}else if (objeID == 6){
#if defined(_WIN32) || defined(_WIN64)
							objs = "media/triangle.txt";
#else
							objs = "../media/triangle.txt";
#endif
						}else if (objeID == 7){
#if defined(_WIN32) || defined(_WIN64)
							objs = "media/reverseH.txt";
#else
							objs = "../media/reverseH.txt";
#endif
						}
						else if (objeID == 8){
#if defined(_WIN32) || defined(_WIN64)
							objs = "media/arrow.txt";
#else
							objs = "../media/arrow.txt";
#endif
						}
#if defined(_WIN32) || defined(_WIN64)
						ifstream file;
						file.open(objs, ios::in);
#else
						fstream file;
						file.open(objs.c_str(), ios::in);

#endif
						int _X, _Y, _Z;
						while (file >> _X){
							if (selectedCube == 2){
								file >> _Y;
								file >> _Z;
								viewChildNode1 = smgr->addSphereSceneNode();
								viewChildNode1->setScale(viewChildNode1->getScale() / SCALE_CHILD);
								viewParentNode1->addChild(viewChildNode1);
								viewChildNode1->setVisible(true);

								nodes1.push_back(viewChildNode1);
								viewChildNode1->setPosition(vector3df(CUBE1_X_CENTER + _X / DIV_POS_X, CUBE_Y_SUM_VALUE + _Y / DIV_POS_Y, _Z / DIV_POS_Z));
								viewChildNode1->updateAbsolutePosition();
							}else if (selectedCube == 3){
								file >> _Y;
								file >> _Z;
								viewChildNode2 = smgr->addSphereSceneNode();
								viewChildNode2->setScale(viewChildNode2->getScale() / SCALE_CHILD);
								viewParentNode2->addChild(viewChildNode2);
								viewChildNode2->setVisible(true);

								nodes2.push_back(viewChildNode2);
								viewChildNode2->setPosition(vector3df(CUBE_X_SUM_VALUE + CUBE1_X_CENTER + _X / DIV_POS_X, CUBE_Y_SUM_VALUE + _Y / DIV_POS_Y, _Z / DIV_POS_Z));
								viewChildNode2->updateAbsolutePosition();
							}else if (selectedCube == 4){
								file >> _Y;
								file >> _Z;
								viewChildNode3 = smgr->addSphereSceneNode();
								viewChildNode3->setScale(viewChildNode3->getScale() / SCALE_CHILD);
								viewParentNode3->addChild(viewChildNode3);
								viewChildNode3->setVisible(true);

								nodes3.push_back(viewChildNode3);
								viewChildNode3->setPosition(vector3df(2 * CUBE_X_SUM_VALUE + CUBE1_X_CENTER + _X / DIV_POS_X, CUBE_Y_SUM_VALUE + _Y / DIV_POS_Y, _Z / DIV_POS_Z));
								viewChildNode3->updateAbsolutePosition();
							}
						}
						file.close();
						if (listCube->getSelected() == 2){
							calcCubeCoor(0);
							findExtremePoints(0);
						}if (listCube->getSelected() == 3){
							calcCubeCoor(1);
							findExtremePoints(1);
						}if (listCube->getSelected() == 4){
							calcCubeCoor(2);
							findExtremePoints(2);
						}
						rotX = false;
						rotY = false;

					}
				}if (id == 1){		//buton ADD3 clicked
					int charID = listChar->getSelected();

					std::string chars;
					if (charID == 2){
#if defined(_WIN32) || defined(_WIN64)
						chars = "media/A.txt";

#else

						chars = "../media/A.txt";
#endif

					}
					else if (charID == 3){
#if defined(_WIN32) || defined(_WIN64)
						chars = "media/B.txt";

#else
						chars = "../media/B.txt";

#endif

					}
					else if (charID == 4){
#if defined(_WIN32) || defined(_WIN64)
						chars = "media/C.txt";

#else
						chars = "../media/C.txt";

#endif

					}
					else if (charID == 5){
#if defined(_WIN32) || defined(_WIN64)
						chars = "media/E.txt";

#else
						chars = "../media/E.txt";

#endif

					}
					else if (charID == 6){
#if defined(_WIN32) || defined(_WIN64)
						chars = "media/G.txt";

#else
						chars = "../media/G.txt";

#endif

					}
					else if (charID == 7){
#if defined(_WIN32) || defined(_WIN64)
						chars = "media/K.txt";

#else
						chars = "../media/K.txt";

#endif

					}
					else if (charID == 8){
#if defined(_WIN32) || defined(_WIN64)
						chars = "media/L.txt";

#else
						chars = "../media/L.txt";

#endif

					}
					else if (charID == 9){
#if defined(_WIN32) || defined(_WIN64)

						chars = "media/N.txt";
#else

						chars = "../media/N.txt";
#endif

					}
					else if (charID == 10){
#if defined(_WIN32) || defined(_WIN64)

						chars = "media/O.txt";
#else

						chars = "../media/O.txt";
#endif

					}
					else if (charID == 11){
#if defined(_WIN32) || defined(_WIN64)
						chars = "media/R.txt";

#else
						chars = "../media/R.txt";

#endif

					}
					else if (charID == 12){
#if defined(_WIN32) || defined(_WIN64)
						chars = "media/Z.txt";

#else

						chars = "../media/Z.txt";
#endif

					}
					else if (charID == 13){
#if defined(_WIN32) || defined(_WIN64)
						chars = "media/T.txt";
#else
						chars = "../media/T.txt";
#endif
					}
					else if (charID == 14){
#if defined(_WIN32) || defined(_WIN64)
						chars = "media/U_.txt";
#else
						chars = "../media/U_.txt";
#endif
					}
#if defined(_WIN32) || defined(_WIN64)
					ifstream file;
					file.open(chars, ios::in);
#else
					fstream file;
					file.open(chars.c_str(), ios::in);

#endif
					int _X, _Y;
					int selectedCube = listCube->getSelected();
					clearNodes(selectedCube - 2);
					while (file >> _X)
					{
						if (selectedCube == 2){
							file >> _Y;
							viewChildNode1 = smgr->addSphereSceneNode();
							viewChildNode1->setScale(viewChildNode1->getScale() / SCALE_CHILD);
							viewParentNode1->addChild(viewChildNode1);
							viewChildNode1->setVisible(true);

							nodes1.push_back(viewChildNode1);
							viewChildNode1->setPosition(vector3df(CUBE1_X_CENTER + _X / DIV_POS_X, CUBE_Y_SUM_VALUE + _Y / DIV_POS_Y, 0));
							viewChildNode1->updateAbsolutePosition();
						}
						else if (selectedCube == 3){
							file >> _Y;
							viewChildNode2 = smgr->addSphereSceneNode();
							viewChildNode2->setScale(viewChildNode2->getScale() / SCALE_CHILD);
							viewParentNode2->addChild(viewChildNode2);
							viewChildNode2->setVisible(true);

							nodes2.push_back(viewChildNode2);
							viewChildNode2->setPosition(vector3df(CUBE_X_SUM_VALUE + CUBE1_X_CENTER + _X / DIV_POS_X, CUBE_Y_SUM_VALUE + _Y / DIV_POS_Y,0));
							viewChildNode2->updateAbsolutePosition();
						}
						else if (selectedCube == 4){
							file >> _Y;
							viewChildNode3 = smgr->addSphereSceneNode();
							viewChildNode3->setScale(viewChildNode3->getScale() / SCALE_CHILD);
							viewParentNode3->addChild(viewChildNode3);
							viewChildNode3->setVisible(true);

							nodes3.push_back(viewChildNode3);
							viewChildNode3->setPosition(vector3df(2 * CUBE_X_SUM_VALUE + CUBE1_X_CENTER + _X / DIV_POS_X, CUBE_Y_SUM_VALUE + _Y / DIV_POS_Y, 0));
							viewChildNode3->updateAbsolutePosition();
						}
						// Process str					

					}
					file.close();
				

					if (listCube->getSelected() == 2){
						calcCubeCoor(0);
						findExtremePoints(0);
					}
					if (listCube->getSelected() == 3){
						calcCubeCoor(1);
						findExtremePoints(1);
					}
					if (listCube->getSelected() == 4){
						calcCubeCoor(2);
						findExtremePoints(2);
					}

					rotX = false;
					rotY = false;

				}

				if (id == 15){		//buton clear
					if (listCube->getSelected() == 2){
						clearNodes(0);
					}
					if (listCube->getSelected() == 3){
						clearNodes(1);
					}
					if (listCube->getSelected() == 4){
						clearNodes(2);
					}
				}
				if (id == 160){
					rotX = false;
					rotY = false;
					emuRotX = ( emuRotX + EMU_ROT_SPEED) % 360;
					rotationEmulators();

				}
				if (id == 161){
					rotX = false;
					rotY = false;
					emuRotY = (emuRotY + EMU_ROT_SPEED) % 360;
					rotationEmulators();
				}
				if (id == 162){
					rotX = false;
					rotY = false;
					emuRotZ = (emuRotZ + EMU_ROT_SPEED) % 360;
					rotationEmulators();
				}if (id == 163){
					rotX = false;
					rotY = false;
					emuRotX = 0;
					emuRotY = 0;
					emuRotZ = 0;
					sanalCube1->setRotation(vector3df(0, 0, 0));
					sanalCube2->setRotation(vector3df(0, 0, 0));
					sanalCube3->setRotation(vector3df(0, 0, 0));
				}

				if (id == 14){		//buton open editor

					editor = new Editor();
					vector<vector3df> positions = editor->startEditor();

					if (listCube->getSelected() == 2){
						clearNodes(0);

						for (int i = 0; i < positions.size(); i += 3){

							viewChildNode1 = smgr->addSphereSceneNode();
							viewChildNode1->setScale(viewChildNode1->getScale() / SCALE_CHILD);
							viewChildNode1->setPosition(vector3df((positions[i].X / DIV_POS_X) + CUBE1_X_CENTER, (positions[i].Y / DIV_POS_Y) + CUBE_Y_SUM_VALUE, positions[i].Z / DIV_POS_Z));
							viewChildNode1->updateAbsolutePosition();

							viewChildNode1->setVisible(true);
							viewParentNode1->addChild(viewChildNode1);
							nodes1.push_back(viewChildNode1);
						}


						calcCubeCoor(0);
						findExtremePoints(0);
					}

					if (listCube->getSelected() == 3){
						clearNodes(1);
						for (int i = 0; i < positions.size(); i += 3){

							viewChildNode2 = smgr->addSphereSceneNode();
							viewChildNode2->setScale(viewChildNode2->getScale() / SCALE_CHILD);
							viewChildNode2->setPosition(vector3df((positions[i].X / DIV_POS_X) + CUBE_X_SUM_VALUE + CUBE1_X_CENTER, (positions[i].Y / DIV_POS_Y) + CUBE_Y_SUM_VALUE, positions[i].Z / DIV_POS_Z));
							viewChildNode2->updateAbsolutePosition();


							viewChildNode2->setVisible(true);
							viewParentNode2->addChild(viewChildNode2);
							nodes2.push_back(viewChildNode2);
						}

						calcCubeCoor(1);
						findExtremePoints(1);
					}

					if (listCube->getSelected() == 4){
						clearNodes(2);
						for (int i = 0; i < positions.size(); i += 3){

							viewChildNode3 = smgr->addSphereSceneNode();
							viewChildNode3->setScale(viewChildNode3->getScale() / SCALE_CHILD);
							viewChildNode3->setPosition(vector3df((positions[i].X / DIV_POS_X) + 2 * CUBE_X_SUM_VALUE + CUBE1_X_CENTER, (positions[i].Y / DIV_POS_Y) + CUBE_Y_SUM_VALUE, (positions[i].Z / DIV_POS_Z)));
							viewChildNode3->updateAbsolutePosition();

							viewChildNode3->setVisible(true);
							viewParentNode3->addChild(viewChildNode3);
							nodes3.push_back(viewChildNode3);
						}

						calcCubeCoor(2);
						findExtremePoints(2);
					}

					rotX = false;
					rotY = false;

					/*if (editor != NULL)
					delete editor;*/
				}
				break;

			case EGET_COMBO_BOX_CHANGED:

				rotX = false;
				rotY = false;


				break;
			case EGET_CHECKBOX_CHANGED:
				if (!checkPivot->isChecked()){
					findExtremePoints(listCube->getSelected() - 2);
					pivotPoint = centerPoint[listCube->getSelected() - 2];
					
				}

				else{
#if defined(_WIN32) || defined(_WIN64)
					pivotPoint = vector3df(centerPoint[listCube->getSelected() - 2].X + _wtof(editX->getText()), centerPoint[listCube->getSelected() - 2].Y + _wtof(editY->getText()), centerPoint[listCube->getSelected() - 2].Z + _wtof(editZ->getText()));
#else
					char A[100],B[100],C[100];
					wcstombs(A,editX->getText(),100*sizeof(char));
					wcstombs(B,editY->getText(),100*sizeof(char));
					wcstombs(C,editZ->getText(),100*sizeof(char));

					pivotPoint = vector3df(centerPoint[listCube->getSelected() - 2].X + atof(A), 
					centerPoint[listCube->getSelected() - 2].Y + atof(B), 
					centerPoint[listCube->getSelected() - 2].Z + atof(C));
#endif				
				}
				pivotNode->setPosition(pivotPoint);
				pivotNode->setVisible(true);
					break;
			case EGET_EDITBOX_CHANGED:
				if (!checkPivot->isChecked()){
					findExtremePoints(listCube->getSelected() - 2);
					pivotPoint = centerPoint[listCube->getSelected() - 2];

				}
				else{
#if defined(_WIN32) || defined(_WIN64)
					pivotPoint = vector3df(centerPoint[listCube->getSelected() - 2].X + _wtof(editX->getText()), centerPoint[listCube->getSelected() - 2].Y + _wtof(editY->getText()), centerPoint[listCube->getSelected() - 2].Z + _wtof(editZ->getText()));

#else
					char A[100],B[100],C[100];
					wcstombs(A,editX->getText(),100*sizeof(char));
					wcstombs(B,editY->getText(),100*sizeof(char));
					wcstombs(C,editZ->getText(),100*sizeof(char));

					pivotPoint = vector3df(centerPoint[listCube->getSelected() - 2].X + atof(A), 
					centerPoint[listCube->getSelected() - 2].Y + atof(B), 
					centerPoint[listCube->getSelected() - 2].Z + atof(C));
#endif
				}
				pivotNode->setPosition(pivotPoint);
				pivotNode->setVisible(true);
				break;
			default:
				// We won't use the wheel
				break;
			}
		}

		if (event.EventType == EET_KEY_INPUT_EVENT)
		{
			// if key is Pressed Down
			if (event.KeyInput.PressedDown == true)
			{
				// If key was not down before
				if (keyState[event.KeyInput.Key] != DOWN)
				{
					keyState[event.KeyInput.Key] = PRESSED; // Set to Pressed
				}
				else
				{
					// if key was down before
					keyState[event.KeyInput.Key] = DOWN; // Set to Down
				}
			}
			else
			{
				// if the key is down
				if (keyState[event.KeyInput.Key] != UP)
				{
					keyState[event.KeyInput.Key] = RELEASED; // Set to Released
				}
			}
		}

		if (event.EventType == irr::EET_MOUSE_INPUT_EVENT)
		{
			

			 if (event.MouseInput.Event == irr::EMIE_LMOUSE_DOUBLE_CLICK){
				 if (rotX == false){
					 rotX = true;
					 pivotNode->setVisible(true);
				 }
				else if (rotX == true)
					rotX = false;

				
			}

			else if (event.MouseInput.Event == irr::EMIE_RMOUSE_DOUBLE_CLICK){
				if (rotY == false){
					rotY = true;
					pivotNode->setVisible(true);
				}
				else if (rotY == true){
					rotY = false;
					
				}
				

			}

			else if (event.MouseInput.Event == EMIE_MOUSE_MOVED){
				oldMouseX = mouseX;
				oldMouseY = mouseY;

				if (rotY == false && rotX == false)
					pivotNode->setVisible(false);

				int cubeNo = listCube->getSelected() - 2;
				mouseX = event.MouseInput.X;
				mouseY = event.MouseInput.Y;

				if (rotX && mouseY < oldMouseY){

					moveNo = 100;

					if (isPossibleMove(cubeNo)){
						
						rotateNode(vector3df(ROTATION_VALUE, 0, 0), cubeNo, 0);
						calcCubeCoor(cubeNo);
					}

				}

				else if (rotX && mouseY > oldMouseY){

					moveNo = 100;

					if (isPossibleMove(cubeNo)){
					
						rotateNode(vector3df(-ROTATION_VALUE, 0, 0), cubeNo, 0);
						calcCubeCoor(cubeNo);
					}
				}

				if (rotY && mouseX > oldMouseX){

					moveNo = 100;

					if (isPossibleMove(cubeNo)){
						
						rotateNode(vector3df(0, ROTATION_VALUE, 0), cubeNo, 1);
						calcCubeCoor(cubeNo);
					}
				}

				else if (rotY && mouseX < oldMouseX){

					moveNo = 100;

					if (isPossibleMove(cubeNo)){
						
						rotateNode(vector3df(0, -ROTATION_VALUE, 0), cubeNo, 1);
						calcCubeCoor(cubeNo);
					}
				}

			}

			if (event.MouseInput.Event == EMIE_MOUSE_WHEEL)
			{
				mouseWheel = event.MouseInput.Wheel;
				int cubeNo = listCube->getSelected() - 2;

				moveNo = 100;

				if (isPossibleMove(cubeNo)){
					pivotNode->setVisible(true);
					rotateNode(vector3df(0, 0, mouseWheel * 10), cubeNo, 2);
					calcCubeCoor(cubeNo);
				}

			}

		}

		return false;
	}

	
	inline bool keyDown(char keycode)
	{
		if (keyState[keycode] == DOWN || keyState[keycode] == PRESSED)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

};

void clearNodes(int cubeNo){

	int i, j, k;

	moveZ[cubeNo] = 0;
	pivotNode->setVisible(false);
	if (cubeNo == 0){
		nodes1.clear();
		viewParentNode1->removeAll();

		viewParentNode1 = smgr->addEmptySceneNode();
		viewChildNode1 = smgr->addEmptySceneNode();
		

		//printf("size: %d - parent: %f",nodes1.size(),viewParentNode->getAbsolutePosition().X);
	}
	if (cubeNo == 1){
		nodes2.clear();
		viewParentNode2->removeAll();

		viewParentNode2 = smgr->addEmptySceneNode();
		viewChildNode2 = smgr->addEmptySceneNode();

		//printf("size: %d - parent: %f",nodes1.size(),viewParentNode->getAbsolutePosition().X);
	}
	if (cubeNo == 2){
		nodes3.clear();
		viewParentNode3->removeAll();

		viewParentNode3 = smgr->addEmptySceneNode();
		viewChildNode3 = smgr->addEmptySceneNode();

		//printf("size: %d - parent: %f",nodes1.size(),viewParentNode->getAbsolutePosition().X);
	}

	rotX = false;
	rotY = false;

	calcCubeCoor(cubeNo);


#ifdef CUBE_MODE
	//new coordinants send to cube
	//CompressArray(compArray, cubeCoor);
	sendCube(cubeCoor, comportNo);
#endif
}

//Check for extreme points
inline void findExtremePoints(int cubeNo){

	double minX = 1500, minY = 1500, minZ = 1500, maxX = -1500, maxY = -1500, maxZ = -1500, i = 0;
	int nodeVectorSize;

	//printf("%d\n",viewParentNode->getChildren().size());
	//shapeNo=viewParentNode->getChildren().size();

	ISceneNode* it;
	if (cubeNo == 0)
		nodeVectorSize = nodes1.size();
	if (cubeNo == 1)
		nodeVectorSize = nodes2.size();
	if (cubeNo == 2)
		nodeVectorSize = nodes3.size();

	while (i < nodeVectorSize) {

		if (cubeNo == 0)
			it = nodes1.at(i);
		if (cubeNo == 1)
			it = nodes2.at(i);
		if (cubeNo == 2)
			it = nodes3.at(i);

		if ((double)it->getPosition().X > maxX){
			maxX = it->getPosition().X;
			extremeNode11[cubeNo] = it;
			//printf("maxX=%d  ",extremeNode11->getPosition().X);
		}
		if ((double)it->getPosition().X < minX){
			minX = it->getPosition().X;
			extremeNode12[cubeNo] = it;
			//printf("minX=%d  ",extremeNode12->getPosition().X);
		}
		if ((double)it->getPosition().Y > maxY){
			maxY = it->getPosition().Y;
			extremeNode13[cubeNo] = it;
			//printf("maxY=%d  ",extremeNode13->getPosition().Y);
		}
		if ((double)it->getPosition().Y < minY){
			minY = it->getPosition().Y;
			extremeNode14[cubeNo] = it;
			//printf("minY=%d  ",extremeNode14->getPosition().Y);
		}
		if ((double)it->getPosition().Z > maxZ){
			maxZ = it->getPosition().Z;
			extremeNode15[cubeNo] = it;
			//printf("maxZ=%d  ",extremeNode15->getPosition().Z);
		}
		if ((double)it->getPosition().Z < minZ){
			minZ = it->getPosition().Z;
			extremeNode16[cubeNo] = it;
			//printf("minZ=%d  ",extremeNode16->getPosition().Z);
		}

		i++;

		// cout<<nodes1.at(i)->getPosition().X;		
	}

	centerPoint[cubeNo].X = minX + (maxX - minX) / 2;
	centerPoint[cubeNo].Y = minY + (maxY - minY) / 2;
	centerPoint[cubeNo].Z = minZ + (maxZ - minZ) / 2;

	/*printf("en son %.2f\n", maxX);
	printf("en son %.2f\n", minX);
	printf("en son %.2f\n", maxY);
	printf("en son %.2f\n", minY);
	printf("en son %.2f\n", maxZ);
	printf("en son %.2f\n", minZ);*/
}



inline bool isPossibleMove(int cubeNo){
	/*daha önce obje eklenmediyse hareket ettirilemez*/
	if ((cubeNo == 0 && nodes1.size() == 0) ||
		(cubeNo == 1 && nodes2.size() == 0) ||
		(cubeNo == 2 && nodes3.size() == 0) ||
		(listCube->getSelected() == 0) ||
		(listCube->getSelected() == 1)){
		return false;
	}

	
	if (moveNo == 2)
		if (extremeNode11[cubeNo]->getAbsolutePosition().X < X_MAX &&
			extremeNode12[cubeNo]->getAbsolutePosition().X < X_MAX &&
			extremeNode13[cubeNo]->getAbsolutePosition().X < X_MAX &&
			extremeNode14[cubeNo]->getAbsolutePosition().X < X_MAX &&
			extremeNode15[cubeNo]->getAbsolutePosition().X < X_MAX &&
			extremeNode16[cubeNo]->getAbsolutePosition().X < X_MAX)
			return true;

	if (moveNo == 1)
		if (extremeNode11[cubeNo]->getAbsolutePosition().X > X_MIN &&
			extremeNode12[cubeNo]->getAbsolutePosition().X > X_MIN &&
			extremeNode13[cubeNo]->getAbsolutePosition().X > X_MIN &&
			extremeNode14[cubeNo]->getAbsolutePosition().X > X_MIN &&
			extremeNode15[cubeNo]->getAbsolutePosition().X > X_MIN &&
			extremeNode16[cubeNo]->getAbsolutePosition().X > X_MIN)
			return true;

	if (moveNo == 4)
		if (extremeNode11[cubeNo]->getAbsolutePosition().Y < Y_MAX	&&
			extremeNode12[cubeNo]->getAbsolutePosition().Y < Y_MAX &&
			extremeNode13[cubeNo]->getAbsolutePosition().Y < Y_MAX &&
			extremeNode14[cubeNo]->getAbsolutePosition().Y < Y_MAX &&
			extremeNode15[cubeNo]->getAbsolutePosition().Y < Y_MAX &&
			extremeNode16[cubeNo]->getAbsolutePosition().Y < Y_MAX)
			return true;

	if (moveNo == 3)
		if (extremeNode11[cubeNo]->getAbsolutePosition().Y > Y_MIN &&
			extremeNode12[cubeNo]->getAbsolutePosition().Y > Y_MIN &&
			extremeNode13[cubeNo]->getAbsolutePosition().Y > Y_MIN &&
			extremeNode14[cubeNo]->getAbsolutePosition().Y > Y_MIN &&
			extremeNode15[cubeNo]->getAbsolutePosition().Y > Y_MIN &&
			extremeNode16[cubeNo]->getAbsolutePosition().Y > Y_MIN)
			return true;

	if (moveNo == 6)
		if (extremeNode11[cubeNo]->getAbsolutePosition().Z + moveZ[cubeNo] < Z_MAX &&
			extremeNode12[cubeNo]->getAbsolutePosition().Z + moveZ[cubeNo] < Z_MAX &&
			extremeNode13[cubeNo]->getAbsolutePosition().Z + moveZ[cubeNo] < Z_MAX &&
			extremeNode14[cubeNo]->getAbsolutePosition().Z + moveZ[cubeNo]< Z_MAX &&
			extremeNode15[cubeNo]->getAbsolutePosition().Z + moveZ[cubeNo]< Z_MAX &&
			extremeNode16[cubeNo]->getAbsolutePosition().Z + moveZ[cubeNo] < Z_MAX)
			return true;

	if (moveNo == 5)
		if (extremeNode11[cubeNo]->getAbsolutePosition().Z + moveZ[cubeNo]> Z_MIN &&
			extremeNode12[cubeNo]->getAbsolutePosition().Z + moveZ[cubeNo]> Z_MIN &&
			extremeNode13[cubeNo]->getAbsolutePosition().Z + moveZ[cubeNo] > Z_MIN &&
			extremeNode14[cubeNo]->getAbsolutePosition().Z + moveZ[cubeNo]> Z_MIN &&
			extremeNode15[cubeNo]->getAbsolutePosition().Z + moveZ[cubeNo]> Z_MIN &&
			extremeNode16[cubeNo]->getAbsolutePosition().Z + moveZ[cubeNo] > Z_MIN)
			return true;

	// + scale for each x,y,z
	if (moveNo == 100){

		if ((extremeNode11[cubeNo]->getAbsolutePosition().X < X_MAX &&
			extremeNode12[cubeNo]->getAbsolutePosition().X < X_MAX  &&
			extremeNode13[cubeNo]->getAbsolutePosition().X < X_MAX  &&
			extremeNode14[cubeNo]->getAbsolutePosition().X < X_MAX  &&
			extremeNode15[cubeNo]->getAbsolutePosition().X < X_MAX  &&
			extremeNode16[cubeNo]->getAbsolutePosition().X < X_MAX)
			&& (extremeNode11[cubeNo]->getAbsolutePosition().Y < Y_MAX &&
			extremeNode12[cubeNo]->getAbsolutePosition().Y < Y_MAX &&
			extremeNode13[cubeNo]->getAbsolutePosition().Y < Y_MAX &&
			extremeNode14[cubeNo]->getAbsolutePosition().Y < Y_MAX &&
			extremeNode15[cubeNo]->getAbsolutePosition().Y < Y_MAX &&
			extremeNode16[cubeNo]->getAbsolutePosition().Y < Y_MAX)
			&& (extremeNode11[cubeNo]->getAbsolutePosition().Z + moveZ[cubeNo] < Z_MAX &&
			extremeNode12[cubeNo]->getAbsolutePosition().Z + moveZ[cubeNo]<Z_MAX &&
			extremeNode13[cubeNo]->getAbsolutePosition().Z + moveZ[cubeNo]< Z_MAX &&
			extremeNode14[cubeNo]->getAbsolutePosition().Z + moveZ[cubeNo] < Z_MAX &&
			extremeNode15[cubeNo]->getAbsolutePosition().Z + moveZ[cubeNo]< Z_MAX &&
			extremeNode16[cubeNo]->getAbsolutePosition().Z + moveZ[cubeNo]< Z_MAX)

			&& (extremeNode11[cubeNo]->getAbsolutePosition().X > X_MIN &&
			extremeNode12[cubeNo]->getAbsolutePosition().X > X_MIN &&
			extremeNode13[cubeNo]->getAbsolutePosition().X > X_MIN &&
			extremeNode14[cubeNo]->getAbsolutePosition().X >X_MIN &&
			extremeNode15[cubeNo]->getAbsolutePosition().X > X_MIN &&
			extremeNode16[cubeNo]->getAbsolutePosition().X > X_MIN)
			&& (extremeNode11[cubeNo]->getAbsolutePosition().Y > Y_MIN &&
			extremeNode12[cubeNo]->getAbsolutePosition().Y > Y_MIN &&
			extremeNode13[cubeNo]->getAbsolutePosition().Y > Y_MIN &&
			extremeNode14[cubeNo]->getAbsolutePosition().Y > Y_MIN &&
			extremeNode15[cubeNo]->getAbsolutePosition().Y > Y_MIN &&
			extremeNode16[cubeNo]->getAbsolutePosition().Y > Y_MIN)
			&& (extremeNode11[cubeNo]->getAbsolutePosition().Z + moveZ[cubeNo] > Z_MIN &&
			extremeNode12[cubeNo]->getAbsolutePosition().Z + moveZ[cubeNo]> Z_MIN &&
			extremeNode13[cubeNo]->getAbsolutePosition().Z + moveZ[cubeNo]> Z_MIN &&
			extremeNode14[cubeNo]->getAbsolutePosition().Z + moveZ[cubeNo]> Z_MIN &&
			extremeNode15[cubeNo]->getAbsolutePosition().Z + moveZ[cubeNo]> Z_MIN &&
			extremeNode16[cubeNo]->getAbsolutePosition().Z + moveZ[cubeNo]> Z_MIN)

			)

			return true;
	}

	if (moveNo == 101){

		return true;
	}

	return false;
}

GuiEventReceiver* receiver;

//move
inline void moveNode(vector3df vel, int cubeNo)
{
	

	/*node->setPosition(node->getAbsolutePosition() + vel);
	node->updateAbsolutePosition();*/
	if (cubeNo == 0){
		for (int i = 0; i < nodes1.size(); ++i){
			nodes1.at(i)->setPosition(nodes1.at(i)->getAbsolutePosition() + vel);
			nodes1.at(i)->updateAbsolutePosition();
		}
	}

	else if (cubeNo == 1){
		for (int i = 0; i < nodes2.size(); ++i){
			nodes2.at(i)->setPosition(nodes2.at(i)->getAbsolutePosition() + vel);
			nodes2.at(i)->updateAbsolutePosition();
		}
	}

	else if (cubeNo == 2){
		for (int i = 0; i < nodes3.size(); ++i){
			nodes3.at(i)->setPosition(nodes3.at(i)->getAbsolutePosition() + vel);
			nodes3.at(i)->updateAbsolutePosition();
		}
	}
}

inline void sendCube(unsigned char cube[8][24][8], int portNo){
	unsigned char c[8][24] = { 0 };

	char wr[512];
	unsigned char length = 0;
	int i, j, k;
	for (i = 0; i < 8; i++){
		for (j = 0; j < 24; j++){
			for (k = 0; k < 8; k++)
				c[i][j] = (c[i][j] << 1) + cube[i][j][k];
			if (current[i][j] != c[i][j]){
				if (j>=8&&j<16)
					wr[length++] = i * 32 + j - 8;
				else if (j >= 0 && j<8)
					wr[length++] = i * 32 + j + 8;
				else{
					wr[length++] = i * 32 + j ;
				}
				wr[length++] = c[i][j];
			
				current[i][j] = c[i][j];
			}
		}
	}

	SendBuf(portNo, wr, length);

}

/*
==========
rotateNode -- rotate a scene node locally
==========
*/

inline void rotateNode(irr::core::vector3df rot, int cubeNo, int rotNo)
{
	vector<ISceneNode*> nodes;

	if (checkPivot->isChecked()){
#if defined(_WIN32) || defined(_WIN64)
		pivotPoint = vector3df(centerPoint[listCube->getSelected() - 2].X + _wtof(editX->getText()), 
			centerPoint[listCube->getSelected() - 2].Y + _wtof(editY->getText()), 
			centerPoint[listCube->getSelected() - 2].Z + _wtof(editZ->getText()));
#else
		char A[100],B[100],C[100];
		wcstombs(A,editX->getText(),100*sizeof(char));
		wcstombs(B,editY->getText(),100*sizeof(char));
		wcstombs(C,editZ->getText(),100*sizeof(char));
		pivotPoint = vector3df(centerPoint[listCube->getSelected() - 2].X + atof(A),
		centerPoint[listCube->getSelected() - 2].Y + atof(B),
		centerPoint[listCube->getSelected() - 2].Z + atof(C));

#endif
		pivotNode->setPosition(pivotPoint);
	}
	else{
		pivotPoint = centerPoint[cubeNo];
		pivotNode->setPosition(pivotPoint);
	}
	moveNode(vector3df(0, 0, moveZ[cubeNo]), cubeNo);		//Q ve E de görüntü sabit iken
	moveNode(-pivotPoint, cubeNo);

	if (cubeNo == 0){
		
		nodes = nodes1;
	}
	else if (cubeNo == 1){
	
		nodes = nodes2;
	}
	else if (cubeNo == 2){

		nodes = nodes3;
	}

	f32 x, y, z;

	//cout << centerPoint.X<<" "<<centerPoint.Y<<" "<<centerPoint.Z<<endl;	

	for (int i = 0; i<nodes.size(); ++i){
		if (rotNo == 2){

			x = cos((double)rot.Z * PI / 180.0)*nodes.at(i)->getAbsolutePosition().X - sin((double)rot.Z *PI / 180.0)*nodes.at(i)->getAbsolutePosition().Y;	//- 0  * cos((double)rot.Z * PI / 180.0);
			y = sin((double)rot.Z * PI / 180.0)*nodes.at(i)->getAbsolutePosition().X + cos((double)rot.Z *PI / 180.0)*nodes.at(i)->getAbsolutePosition().Y;	//- 0 * sin((double)rot.Z * PI / 180.0);
			z = nodes.at(i)->getAbsolutePosition().Z;
			nodes.at(i)->setPosition(vector3df(x, y, z ) + pivotPoint);

			nodes.at(i)->updateAbsolutePosition();

		}

		else if (rotNo == 1){

			x = cos((double)rot.Y * PI / 180.0)*nodes.at(i)->getAbsolutePosition().X - sin((double)rot.Y*PI / 180.0)*nodes.at(i)->getAbsolutePosition().Z;	//- 0 * cos((double)rot.Y * PI / 180.0) + 0;
			y = nodes.at(i)->getAbsolutePosition().Y;
			z = sin((double)rot.Y * PI / 180.0)*nodes.at(i)->getAbsolutePosition().X + cos((double)rot.Y*PI / 180.0)*nodes.at(i)->getAbsolutePosition().Z;	//- 0 * sin((double)rot.Y * PI / 180.0);
			nodes.at(i)->setPosition(vector3df(x, y, z ) + pivotPoint);

			nodes.at(i)->updateAbsolutePosition();

		}

		else if (rotNo == 0){

			x = nodes.at(i)->getAbsolutePosition().X;
			y = cos((double)rot.X * PI / 180.0)*nodes.at(i)->getAbsolutePosition().Y - sin((double)rot.X*PI / 180.0)*nodes.at(i)->getAbsolutePosition().Z;	//+ 0 * cos((double)rot.X * PI / 180.0) - 0;
			z = sin((double)rot.X * PI / 180.0)*nodes.at(i)->getAbsolutePosition().Y + cos((double)rot.X*PI / 180.0)*nodes.at(i)->getAbsolutePosition().Z;	//+ 0 * sin((double)rot.X * PI / 180.0);
			nodes.at(i)->setPosition(vector3df(x, y, z)  + pivotPoint);

			nodes.at(i)->updateAbsolutePosition();

		}
	}
	moveNode(vector3df(0, 0, -moveZ[cubeNo]), cubeNo);		//Q ve E de görüntü sabit iken

}

inline void scaleMesh(vector3df scaleVector, int cubeNo)
{
	moveNode(vector3df(0, 0, moveZ[cubeNo]), cubeNo);		//Q ve E de görüntü sabit iken
	moveNode(-centerPoint[cubeNo], cubeNo);

	if (cubeNo == 0){

		for (int i = 0; i < nodes1.size(); ++i)
		{
			nodes1.at(i)->setPosition(nodes1.at(i)->getAbsolutePosition()*scaleVector + centerPoint[cubeNo]);
			nodes1.at(i)->updateAbsolutePosition();
		}
		
	}

	else if (cubeNo == 1){

		for (int i = 0; i < nodes2.size(); ++i)
		{
			nodes2.at(i)->setPosition(nodes2.at(i)->getAbsolutePosition()*scaleVector + centerPoint[cubeNo]);
			nodes2.at(i)->updateAbsolutePosition();
		}
		
	}

	else if (cubeNo == 2){

		for (int i = 0; i < nodes3.size(); ++i)
		{
			nodes3.at(i)->setPosition(nodes3.at(i)->getAbsolutePosition()*scaleVector + centerPoint[cubeNo]);
			nodes3.at(i)->updateAbsolutePosition();
		}
		
	}
	
	findExtremePoints(cubeNo);
	moveNode(vector3df(0, 0, -moveZ[cubeNo]), cubeNo);		//Q ve E de görüntü sabit iken
}

bool InitializeVideo()
{
	receiver = new GuiEventReceiver();
#if defined(_WIN32) || defined(_WIN64)
	//INITIALIZE VIDEO DEVICE
	mydevice = irr::createDevice(EDT_DIRECT3D9, dimension2d<u32>(1200, 680), 16, false, false, false, receiver);

#else
	//INITIALIZE VIDEO DEVICE
	mydevice = irr::createDevice(EDT_OPENGL, dimension2d<u32>(1200, 680), 16, false, false, false, receiver);

#endif


	if (mydevice == 0) {       //OpenGL didn't work, try software
		mydevice = irr::createDevice(EDT_SOFTWARE, dimension2d<u32>(1200, 680), 32, false, false, false);
		if (mydevice == 0)       //Software didn't work, the computer blows so quit
			return false;
	}

	mydevice->setResizable(false);

	return true;
}

inline void calcCubeCoor(int cubeNo){

	int i , j, k,l;

			

	for (l = 0; l < 3; ++l)
	for (i = 0; i < 8; ++i)
		for (j = 0; j < 24; ++j)
			for (k = 0; k < 8; ++k){
				cubeCoor[i][j][k] = 0;
				sanalNode[j % 8][i][k][l]->setVisible(false);
			}
	i = 0;
	

		while (i < nodes1.size()) {
			cubeCoor[((int)(((nodes1.at(i)->getAbsolutePosition().Y - Y_MIN) / LED_CUBE_REDUCE_Y) + 0.5)) % 8]
				[((int)(((nodes1.at(i)->getAbsolutePosition().X - X_MIN) / LED_CUBE_REDUCE_X) + 0.5)) % 24]
			[((int)(((nodes1.at(i)->getAbsolutePosition().Z - Z_MIN + moveZ[0]) / LED_CUBE_REDUCE_Z) + 0.5)) % 8]
			= 1;
			i++;
		}
		//cout << nodes1.at(0)->getAbsolutePosition().X << endl;
	
		i = 0;
		while (i < nodes2.size()) {
			cubeCoor[((int)(((nodes2.at(i)->getAbsolutePosition().Y - Y_MIN) / LED_CUBE_REDUCE_Y) + 0.5)) % 8]
				[((int)(((nodes2.at(i)->getAbsolutePosition().X - X_MIN) / LED_CUBE_REDUCE_X) + 0.5)) % 24]
			[((int)(((nodes2.at(i)->getAbsolutePosition().Z - Z_MIN + moveZ[1]) / LED_CUBE_REDUCE_Z) + 0.5)) % 8]
			 = 1;
			i++;
		}
	
		i = 0;
		while (i < nodes3.size()) {
			cubeCoor[((int)(((nodes3.at(i)->getAbsolutePosition().Y - Y_MIN) / LED_CUBE_REDUCE_Y) + 0.5)) % 8]
				[((int)(((nodes3.at(i)->getAbsolutePosition().X - X_MIN) / LED_CUBE_REDUCE_X) + 0.5)) % 24]
			[((int)(((nodes3.at(i)->getAbsolutePosition().Z - Z_MIN + moveZ[2]) / LED_CUBE_REDUCE_Z) + 0.5)) % 8]
			 = 1;
			i++;
		}
	
		
		
		for (i = 0; i < 8; ++i)
		for (j = 0; j < 24; ++j)
			for (k = 0; k < 8; ++k){
				if (cubeCoor[i][j][k] == 1){
					
						if ((j / 8) == 0)
							l = 0;
						if ((j / 8) == 1)
							l = 1;
						if ((j / 8) == 2)
							l = 2;
					
					sanalNode[j % 8][i][k][l]->setVisible(true);

				}
			}


#ifdef CUBE_MODE
	//new coordinants send to cube
	//CompressArray(compArray, cubeCoor);
	sendCube(cubeCoor, comportNo);
#endif

	//küpte yanacak koordinatlarý ekrana basar
	/*int numberOfWillLight = 1;

	for (int i = 0; i < 8; ++i)
	for (int j = 0; j < 24; ++j)
	for (int k = 0; k < 8; ++k)
	
	if (cubeCoor[i][j][k]){
		cout << numberOfWillLight << "  ------  ";
		printf("[%d][%d][%d] = %d \n", i, j, k, cubeCoor[i][j][k]);

	numberOfWillLight++;
	}*/
}

void RenderAll()
{
	createCubeSanal();
	while (mydevice->run()){

		mydriver->setViewPort(rect<s32>(0, 0, 1200, 680));
		mydriver->beginScene(true, true, SColor(200, 200, 200, 200));
		mygui->drawAll();
		int cubeNo = listCube->getSelected() - 2;

		// -y kub icin -x
		if (receiver->keyDown(KEY_KEY_A)){

			moveNo = 1;

			if (isPossibleMove(cubeNo)){
				pivotNode->setPosition(pivotNode->getPosition() + vector3df(-MOVE_VALUE, 0, 0));
				moveNode(vector3df(-MOVE_VALUE, 0, 0), cubeNo);
				centerPoint[cubeNo] += vector3df(-MOVE_VALUE, 0, 0);
				calcCubeCoor(cubeNo);

			}
		}

		// +y -> kupde +x
		else if (receiver->keyDown(KEY_KEY_D)){

			moveNo = 2;

			if (isPossibleMove(cubeNo)){
				pivotNode->setPosition(pivotNode->getPosition() + vector3df(MOVE_VALUE, 0, 0));
				moveNode(vector3df(MOVE_VALUE, 0, 0), cubeNo);
				centerPoint[cubeNo] += vector3df(MOVE_VALUE, 0, 0);
				calcCubeCoor(cubeNo);
			}
		}

		// +z kubte +y
		else if (receiver->keyDown(KEY_KEY_W)){

			moveNo = 4;

			if (isPossibleMove(cubeNo)){
				pivotNode->setPosition(pivotNode->getPosition() + vector3df(0, MOVE_VALUE, 0));
				moveNode(vector3df(0, MOVE_VALUE, 0), cubeNo);
				centerPoint[cubeNo] += vector3df(0, MOVE_VALUE, 0);
				calcCubeCoor(cubeNo);
			}
		}

		// -z , kubte -y
		else if (receiver->keyDown(KEY_KEY_S)){

			moveNo = 3;

			if (isPossibleMove(cubeNo)){
				pivotNode->setPosition(pivotNode->getPosition() + vector3df(0, -MOVE_VALUE, 0));
				moveNode(vector3df(0, -MOVE_VALUE, 0), cubeNo);
				centerPoint[cubeNo] += vector3df(0, -MOVE_VALUE, 0);
				calcCubeCoor(cubeNo);
			}
		}

		// +x , kubte z
		else if (receiver->keyDown(KEY_KEY_Q)){

			moveNo = 6;
			if (isPossibleMove(cubeNo)){

				moveZ[cubeNo] += MOVE_VALUE;
				centerPoint[cubeNo] += vector3df(0, 0, MOVE_VALUE);
				calcCubeCoor(cubeNo);
			}
		}

		// -x
		else if (receiver->keyDown(KEY_KEY_E)){

			moveNo = 5;
			if (isPossibleMove(cubeNo)){

				moveZ[cubeNo] += -MOVE_VALUE;
				centerPoint[cubeNo] += vector3df(0, 0, -MOVE_VALUE);
				calcCubeCoor(cubeNo);
			}
		}

		// scale +1.0
		else if (receiver->keyDown(KEY_KEY_G)){

			moveNo = 100;

			if (isPossibleMove(cubeNo)){

				scaleMesh(vector3df(ENLARGE_SCALE_VALUE, ENLARGE_SCALE_VALUE, ENLARGE_SCALE_VALUE), cubeNo);
				calcCubeCoor(cubeNo);
			}
		}

		// scale -1.0
		else if (receiver->keyDown(KEY_KEY_H)){

			moveNo = 101;

			if (isPossibleMove(cubeNo)){

				scaleMesh(vector3df(REDUCTION_SCALE_VALUE, REDUCTION_SCALE_VALUE, REDUCTION_SCALE_VALUE), cubeNo);
				calcCubeCoor(cubeNo);
			}
		}
		//Activate camera1
		smgr->setActiveCamera(camera[0]);
		//Set viewpoint to the first quarter (left top)
		mydriver->setViewPort(rect<s32>(10,60,310,360));
		//Draw scene
		smgr->drawAll();
		//Activate camera2
		smgr->setActiveCamera(camera[1]);
		//Set viewpoint to the second quarter (right top)
		mydriver->setViewPort(rect<s32>(310, 60, 610, 360));
		//Draw scene
		smgr->drawAll();
		//Activate camera3
		smgr->setActiveCamera(camera[2]);
		//Set viewpoint to the third quarter (left bottom)
		mydriver->setViewPort(rect<s32>(610, 60, 910, 360));
		//Draw scene
		smgr->drawAll();
		//Activate camera1
		smgr->setActiveCamera(camera[3]);
		//Set viewpoint to the first quarter (left top)
		mydriver->setViewPort(rect<s32>(10, 360, 310, 660));
		//Draw scene
		smgr->drawAll();
		//Activate camera1
		smgr->setActiveCamera(camera[4]);
		//Set viewpoint to the first quarter (left top)
		mydriver->setViewPort(rect<s32>(310, 360, 610, 660));
		//Draw scene
		smgr->drawAll();
		//Activate camera1
		smgr->setActiveCamera(camera[5]);
		//Set viewpoint to the first quarter (left top)
		mydriver->setViewPort(rect<s32>(610, 360, 910, 660));
		//Draw scene
		smgr->drawAll();
		mydriver->endScene();
	}
}

void CreateGUI()
{

#if defined(_WIN32) || defined(_WIN64)
	imageTitle = mydriver->getTexture("media\\title.png");
	imagedownTitle = mydriver->getTexture("media\\downTitle.png");

#else
	imageTitle = mydriver->getTexture("../media/title.png");
	imagedownTitle = mydriver->getTexture("../media/downTitle.png");

#endif
	mygui->addButton(rect<s32>(406, 8, 478, 32), 0, 1, L"ADD CHAR");
	mygui->addButton(rect<s32>(654, 8, 742, 32), 0, 2, L"ADD OBJECT");
	//mygui->addButton(rect<s32>(902, 8, 974, 32), 0, 3, L"ADD ANIMATION");
	mygui->addButton(rect<s32>(990, 8, 1050, 32), 0, 4, L"RESET");//küplere FF atar
	mygui->addStaticText(L"Pivot Point   X            Y                 Z      enable / disable ", rect<s32>(970, 290, 1190, 299), true, 204);
	mygui->addButton(rect<s32>(1070, 8, 1130, 32), 0, 15, L"CLEAR");
	editX = mygui->addEditBox(L"0", rect<s32>(1010, 300, 1040, 330), true, 0, 200);
	editY = mygui->addEditBox(L"0", rect<s32>(1045, 300, 1075, 330), true, 0, 201);
	editZ = mygui->addEditBox(L"0", rect<s32>(1080, 300, 1110, 330), true, 0, 202);
	checkPivot = mygui->addCheckBox(false,rect<s32>(1115, 300, 1135, 330), 0, 203, L"ROTATE  EMULATORS  X ");
	mygui->addButton(rect<s32>(1010, 360, 1130, 400), 0, 160, L"ROTATE  EMULATORS  X ");
	mygui->addButton(rect<s32>(1010, 405, 1130, 445), 0, 161, L"ROTATE  EMULATORS  Y ");
	mygui->addButton(rect<s32>(1010, 450, 1130, 490), 0, 162, L"ROTATE  EMULATORS  Z ");
	mygui->addButton(rect<s32>(1010, 495, 1130, 535), 0, 163, L"DEFAULT EMULATOR VIEW");
	mygui->addButton(rect<s32>(5, 8, 100, 32), 0, 14, L"OPEN EDITOR");

	listCube = mygui->addComboBox(rect<s32>(118, 8, 254, 30), 0, 4);
	listChar = mygui->addComboBox(rect<s32>(262, 8, 398, 30), 0, 5);
	listObj = mygui->addComboBox(rect<s32>(486, 8, 638, 30), 0, 6);
	//listAnimation = mygui->addComboBox(rect<s32>(750, 8, 894, 30), 0, 7);



	/*scale1 = mygui->addScrollBar(false, rect<s32>(400, 70, CUBE_X_SUM_VALUE, 470), 0, 8);
	scale2 = mygui->addScrollBar(false, rect<s32>(820, 70, 840, 470), 0, 9);*/
	//scale3 = mygui->addScrollBar(false, rect<s32>(1240, 70, 1260, 470), 0, 10);
#if defined(_WIN32) || defined(_WIN64)
    mygui->addImage(mydriver->getTexture("media/cube1.png"), position2d<int>(80, 35));
	mygui->addImage(mydriver->getTexture("media/cube2.png"), position2d<int>(380, 35));
	mygui->addImage(mydriver->getTexture("media/cube3.png"), position2d<int>(680, 35));
#else
	mygui->addImage(mydriver->getTexture("../media/cube1.png"), position2d<int>(80, 35));
	mygui->addImage(mydriver->getTexture("../media/cube2.png"), position2d<int>(380, 35));
	mygui->addImage(mydriver->getTexture("../media/cube3.png"), position2d<int>(680, 35));
#endif
	//img2 = mygui->addImage(imagedownTitle, position2d<int>(0, 480));

	/*speed1 = mygui->addScrollBar(true, rect<s32>(0, 505, 400, 525), 0, 11);
	speed2 = mygui->addScrollBar(true, rect<s32>(CUBE_X_SUM_VALUE, 505, 820, 525), 0, 12);
	speed3 = mygui->addScrollBar(true, rect<s32>(840, 505, 1240, 525), 0, 13);*/

	listObj->addItem(L"SHAPES");
	listObj->addItem(L"-----------------------");
	listObj->addItem(L"CUBE-3D");
	listObj->addItem(L"CYCLINDER-3D");
	listObj->addItem(L"PLUS-3D");
	listObj->addItem(L"CUBEFULL-3D");
	listObj->addItem(L"TRIANGLE-2D");
	listObj->addItem(L"REVERSE_H-3D");
	listObj->addItem(L"ARROW-2D");

	listCube->addItem(L"SELECT ACTIVE OBJECT");
	listCube->addItem(L"-----------------------");
	listCube->addItem(L"OBJECT 1");
	listCube->addItem(L"OBJECT 2");
	listCube->addItem(L"OBJECT 3");

	listChar->addItem(L"CHARS");
	listChar->addItem(L"-----------------------");
	listChar->addItem(L"A");//id 2
	listChar->addItem(L"B");
	listChar->addItem(L"C");
	listChar->addItem(L"E");
	listChar->addItem(L"G");
	listChar->addItem(L"K");
	listChar->addItem(L"L");
	listChar->addItem(L"N");
	listChar->addItem(L"O");
	listChar->addItem(L"R");
	listChar->addItem(L"Z");
	listChar->addItem(L"T");
	listChar->addItem(L"U+");

	/*listAnimation->addItem(L"ANIMATIONS");
	listAnimation->addItem(L"-----------------------");*/
	int camZ = 117, camZArka = 0,camCubeEmu = 300;
	float farVal = 300;
#if defined(_WIN32) || defined(_WIN64)
	mygui->addImage(mydriver->getTexture("media/kare.png"),position2d<int>(10,60));
	mygui->addImage(mydriver->getTexture("media/kare.png"), position2d<int>(310, 60));
	mygui->addImage(mydriver->getTexture("media/kare.png"), position2d<int>(610, 60));
	mygui->addImage(mydriver->getTexture("media/axis.png"), position2d<int>(950, 60));
	mygui->addImage(mydriver->getTexture("media/kutu.png"), position2d<int>(10, 360));
	mygui->addImage(mydriver->getTexture("media/kutu.png"), position2d<int>(310, 360));
	mygui->addImage(mydriver->getTexture("media/kutu.png"), position2d<int>(610, 360));
#else
	mygui->addImage(mydriver->getTexture("../media/kare.png"), position2d<int>(10, 60));
	mygui->addImage(mydriver->getTexture("../media/kare.png"), position2d<int>(310, 60));
	mygui->addImage(mydriver->getTexture("../media/kare.png"), position2d<int>(610, 60));
	mygui->addImage(mydriver->getTexture("../media/axis.png"), position2d<int>(950, 120));
	mygui->addImage(mydriver->getTexture("../media/kutu.png"), position2d<int>(10, 360));
	mygui->addImage(mydriver->getTexture("../media/kutu.png"), position2d<int>(310, 360));
	mygui->addImage(mydriver->getTexture("../media/kutu.png"), position2d<int>(610, 360));
#endif

	camera[0] = smgr->addCameraSceneNode(0, vector3df(-450, 130, -camZ), vector3df(-450, 130, camZArka));
	camera[1] = smgr->addCameraSceneNode(0, vector3df(-150, 130, -camZ), vector3df(-150, 130, camZArka));
	camera[2] = smgr->addCameraSceneNode(0, vector3df(150, 130, -camZ), vector3df(150, 130, camZArka));
	camera[3] = smgr->addCameraSceneNode(0, vector3df(-450, -170, -1100), vector3df(-450, -170, 0));
	camera[4] = smgr->addCameraSceneNode(0, vector3df(-150, -170, -2100), vector3df(-150, -170, -1000));
	camera[5] = smgr->addCameraSceneNode(0, vector3df(150, -170, -3100), vector3df(150, -170, -2000));
	
	camera[0]->setFarValue(farVal);
	camera[1]->setFarValue(farVal);
	camera[2]->setFarValue(farVal);
	camera[3]->setFarValue(900);
	camera[4]->setFarValue(900);
	camera[5]->setFarValue(900);
}

void createCubeSanal(){

	sanalCube1 = smgr->addEmptySceneNode();
	sanalCube2 = smgr->addEmptySceneNode();
	sanalCube3 = smgr->addEmptySceneNode();
	sanalCube1->setPosition(vector3df(-450,-170,-600));
	sanalCube1->setVisible(true);
	sanalCube2->setPosition(vector3df(-150, -170, -1600));
	sanalCube2->setVisible(true);
	sanalCube3->setPosition(vector3df(150, -170, -2600));
	sanalCube3->setVisible(true);
	/*sanalCube1->setRotation(vector3df(90, 0, 90));
	sanalCube2->setRotation(vector3df(90, 0, 90));
	sanalCube3->setRotation(vector3df(90, 0, 90)); */
	ISceneNode *scene, *temp;
	int aralik = 45;
	scene = smgr->addSphereSceneNode();
	scene->setVisible(false);
	scene->setMaterialFlag(EMF_LIGHTING, false);
	scene->setScale(scene->getScale()*3.25);
#if defined(_WIN32) || defined(_WIN64)
	scene->setMaterialTexture(0, mydriver->getTexture("media/blueTexture.png"));

#else
	scene->setMaterialTexture(0, mydriver->getTexture("../media/blueTexture.png"));
#endif

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			for (int k = 0; k < 8; k++)
			{

				scene->setPosition(vector3df(-144 + (i*aralik), -144 + (j * aralik), -144 + (k * aralik)));
				temp = scene->clone();
				sanalNode[i][j][k][0] = temp;
				sanalCube1->addChild(temp);
				temp = scene->clone();
				sanalCube2->addChild(temp);
				//temp->setVisible(true);
				sanalNode[i][j][k][1] = temp;
				temp = scene->clone();
				sanalCube3->addChild(temp);
				//temp->setVisible(true);
				sanalNode[i][j][k][2] = temp;
			}
		}
	}
#if defined(_WIN32) || defined(_WIN64)
	scene->setMaterialTexture(0, mydriver->getTexture("media/redTexture.png"));
#else
	scene->setMaterialTexture(0, mydriver->getTexture("../media/redTexture.png"));
#endif
	aralik = 30;
	pivotNode = scene->clone();
	pivotNode->setScale(pivotNode->getScale()*0.30);
	vector3df v31 = sanalNode[0][0][0][1]->getPosition();
	vector3df v32 = sanalNode[0][0][7][1]->getPosition();
	vector3df v33 = sanalNode[0][7][0][1]->getPosition();
	vector3df v34 = sanalNode[0][7][7][1]->getPosition();
	vector3df v35 = sanalNode[7][0][0][1]->getPosition();
	vector3df v36 = sanalNode[7][0][7][1]->getPosition();
	vector3df v37 = sanalNode[7][7][0][1]->getPosition();
	vector3df v38 = sanalNode[7][7][7][1]->getPosition();

	v31.X -= aralik;
	v31.Y -= aralik;
	v31.Z -= 2 * aralik;
	v32 = v31;
	v38.X += aralik;
	v38.Y += aralik;
	v38.Z += 2 * aralik;
	v37 = v38;
	aralik = 45;
	for (int j = 0; j < 12; ++j){
		v31 = v32;
		v38 = v37;
		for (int i = 0; i < 8; i++)
		{
			if (j == 0){
				v31.Z += aralik;
				v38.Z -= aralik;
			}
			else if (j == 1){
				v31.Y += aralik;
				v38.Y -= aralik;
			}
			else if (j == 2){
				v31.X += aralik;
				v38.X -= aralik;
			}
			temp = scene->clone();
			temp->setPosition(v31);
			sanalCube1->addChild(temp);
			temp->setVisible(true);
			temp = scene->clone();
			temp->setPosition(v38);
			sanalCube1->addChild(temp);
			temp->setVisible(true);
			temp = scene->clone();
			temp->setPosition(v31);
			sanalCube2->addChild(temp);
			temp->setVisible(true);
			temp = scene->clone();
			temp->setPosition(v38);
			sanalCube2->addChild(temp);
			temp->setVisible(true);
			temp = scene->clone();
			temp->setPosition(v31);
			sanalCube3->addChild(temp);
			temp->setVisible(true);
			temp = scene->clone();
			temp->setPosition(v38);
			sanalCube3->addChild(temp);
			temp->setVisible(true);


		}
	}

}

inline void rotationEmulators(){
	sanalCube1->setRotation(vector3df(emuRotX,emuRotY,emuRotZ));
	sanalCube2->setRotation(vector3df(emuRotX, emuRotY, emuRotZ));
	sanalCube3->setRotation(vector3df(emuRotX, emuRotY, emuRotZ));
}

