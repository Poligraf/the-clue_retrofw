/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa
  Portions copyright (c) 2005 Jens Granseuer

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include "inphdl/inphdl.h"

#include "SDL/SDL.h"

struct IHandler {
    S32 ul_XSensitivity;
    S32 ul_YSensitivity;
    U32 ul_WaitTicks;

    /* us_KeyCode; */

    bool MouseExists;
    bool EscStatus;
    bool FunctionKeyStatus;
    bool MouseStatus;

    bool JoyExists;
    SDL_Joystick *Joystick;
};

struct IHandler IHandler;


void gfxWaitTOF(void);


static void inpDoPseudoMultiTasking(void)
{
    animator();
}

void inpOpenAllInputDevs(void)
{
    inpSetKeyRepeat((1 << 5) | 10);

    IHandler.EscStatus = true;
    IHandler.FunctionKeyStatus = true;

    IHandler.MouseExists = true;
    IHandler.MouseStatus = true;

    IHandler.JoyExists = false;
/*
    if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) == 0) {
        if (SDL_NumJoysticks() > 0) {
            IHandler.Joystick = SDL_JoystickOpen(0);

            if (IHandler.Joystick) {
                IHandler.JoyExists = true;
            } else {
                DebugMsg(ERR_DEBUG, ERROR_MODULE_INPUT,
                         "Failed to open Joystick 0");
            }
        }
    } else {
        DebugMsg(ERR_DEBUG, ERROR_MODULE_INPUT,
                 "SDL_InitSubSystem: %s", SDL_GetError());
    }
*/
    inpClearKbBuffer();
}

void inpCloseAllInputDevs(void)
{
    /*
    if (SDL_JoystickOpened(0)) {
        SDL_JoystickClose(IHandler.Joystick);
    }

    if (SDL_WasInit(SDL_INIT_JOYSTICK) != 0) {
        SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
    }
    */
}

void inpMousePtrOn(void)
{
      SDL_ShowCursor(SDL_DISABLE);
}

void inpMousePtrOff(void)
{
   SDL_ShowCursor(SDL_DISABLE);
}

S32 inpWaitFor(S32 l_Mask)
{
    SDL_Event ev;
    S32 action;
    U32 WaitTime = 0;

    if ((IHandler.EscStatus) && (!(l_Mask & INP_NO_ESC))) {
	l_Mask |= INP_ESC;
    }

    if ((IHandler.FunctionKeyStatus) && (!(l_Mask & INP_FUNCTION_KEY))) {
	l_Mask |= INP_FUNCTION_KEY;
    }

    action = 0;

    /* Nun wird auf den Event gewartet... */
    WaitTime = 0;


    SDLKey sym = (SDLKey)0;

    if (IHandler.EscStatus && !(l_Mask & INP_NO_ESC)) l_Mask |= INP_ESC;
    if (IHandler.FunctionKeyStatus)	l_Mask |= INP_FUNCTION_KEY;

    while (action == 0) {
	gfxWaitTOF();

	WaitTime++;
	/* Abfrage des Zeit-Flags */
	if ((l_Mask & INP_TIME) && WaitTime >= IHandler.ul_WaitTicks) {
	    action |= INP_TIME;
        }

	while (SDL_PollEvent(&ev)) {
    switch (ev.type)
    {
      case SDL_KEYDOWN:
        sym = ev.key.keysym.sym;
        if ((l_Mask & INP_LEFT) && (sym == SDLK_LEFT)) {
          action |= INP_KEYBOARD|INP_LEFT;
        } else if ((l_Mask & INP_RIGHT) && (sym == SDLK_RIGHT)) {
          action |= INP_KEYBOARD|INP_RIGHT;
        }
        if ((l_Mask & INP_UP) && (sym == SDLK_UP)) {
          action |= INP_KEYBOARD|INP_UP;
        } else if ((l_Mask & INP_DOWN) && (sym == SDLK_DOWN)) {
          action |= INP_KEYBOARD|INP_DOWN;
        }
      break;
      case SDL_KEYUP:
        sym = ev.key.keysym.sym;
        if ((l_Mask & (INP_LBUTTONP | INP_LBUTTONR)) && ((sym == SDLK_SPACE) || (sym == SDLK_RETURN))) {
          action |= INP_KEYBOARD|INP_LBUTTONP;
        }
        if (IHandler.EscStatus && (l_Mask & INP_ESC) && (sym == SDLK_LALT)) {
          action |= INP_KEYBOARD|INP_ESC;
        }
        if (IHandler.FunctionKeyStatus && (l_Mask & INP_FUNCTION_KEY) && ((sym == SDLK_LSHIFT))) {
          action |= INP_KEYBOARD|INP_FUNCTION_KEY;
        }

      break;







    }
  }

        inpClearKbBuffer();
	inpDoPseudoMultiTasking();
    }

    return action;
}

void inpSetWaitTicks(U32 l_Ticks)
{
    IHandler.ul_WaitTicks = l_Ticks;
}

void inpTurnESC(bool us_NewStatus)
{
    IHandler.EscStatus = us_NewStatus;
}

void inpTurnFunctionKey(bool us_NewStatus)
{
    IHandler.FunctionKeyStatus = us_NewStatus;
}

void inpTurnMouse(bool us_NewStatus)
{
    IHandler.MouseStatus = us_NewStatus;
}

void inpDelay(S32 l_Ticks)
{
    S32 i;

    for (i = 0; i < l_Ticks; i++) {
	gfxWaitTOF();
	inpDoPseudoMultiTasking();
    }
}

void inpSetKeyRepeat(unsigned char rate)
{
	/* needs some experimenting... */

	int x = (((rate & 0xe0) >> 5) + 1) << 8;
	int y = ((rate & 0x1f) + 1) << 4;
	SDL_EnableKeyRepeat(x, y);
}

void inpClearKbBuffer(void)
{
}
