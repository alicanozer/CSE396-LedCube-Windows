
// MastEventReceiver.h

#include "irrlicht.h"

using namespace std;
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class MastEventReceiver : public IEventReceiver
{
protected:

	// Enumeration for UP, DOWN, PRESSED and RELEASED key states. Also used for mouse button states.
	enum keyStatesENUM { UP, DOWN, PRESSED, RELEASED };
	// Enumeration for Event Handling State.
	enum processStateENUM { STARTED, ENDED };
	// Mouse button states.
	keyStatesENUM mouseButtonState[2]; //Left(0), Middle(1) and Right(2) Buttons.
	// Keyboard key states.
	keyStatesENUM keyState[KEY_KEY_CODES_COUNT];
	// Mouse X/Y coordinates and Wheel data.
	struct mouseData
	{
		int X;
		int Y;
		float wheel; //wheel is how far the wheel has moved
	};

	struct mouseData mouse;
	processStateENUM processState; // STARTED = handling events, ENDED = not handling events
	virtual bool OnEvent(const SEvent & event);

public:

	MastEventReceiver();
	float mouseWheel();
	int mouseX();
	int mouseY();
	bool leftMouseReleased();
	bool leftMouseUp();
	bool leftMousePressed();
	bool leftMouseDown();
	bool middleMouseReleased();
	bool middleMouseUp();
	bool middleMousePressed();
	bool middleMouseDown();
	bool rightMouseReleased();
	bool rightMouseUp();
	bool rightMousePressed();
	bool rightMouseDown();
	bool keyPressed(char keycode);
	bool keyDown(char keycode);
	bool keyUp(char keycode);
	bool keyReleased(char keycode);
	void endEventProcess();
	void startEventProcess();
	void init();

private:

};
