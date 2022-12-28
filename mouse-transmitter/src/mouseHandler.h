#ifndef MOUSE_HANDLER
#define MOUSE_HANDLER

#include <Arduino.h>
#include <TouchScreen.h>
#include "CONFIG_VARS.h"
#include "Functions.h"

#define MINPRESSURE 10
#define MAXPRESSURE 4000

#define TS_MINX 0
#define TS_MINY 0
#define TS_MAXX SW
#define TS_MAXY SH

#define TS_MINX_IN 120
#define TS_MINY_IN 100
#define TS_MAXX_IN 800
#define TS_MAXY_IN 920

// multiplies translation to this value
// #define X_MUL 2.5
// #define Y_MUL 2.5

// variance in touches should be less than this value in order to mark the touch as a click
// the higher, the less sensitive. varies in different processor clock rates
#define CLICK_SENS 1.0

// amount that a finger should be resting on screen to recognize it as a touch initiative
#define CLICK_DURATION 500000

// the differential in each position with the previous one that should be ignored as a movement
// the higher, the more sensitive. set to -1 to ignore
#define X_MOVEMENT_DIFFERENTIAL_IGNORANCE 1
#define Y_MOVEMENT_DIFFERENTIAL_IGNORANCE 1

#define TIME_MEASURE micros

typedef TSPoint TDCoord;

class MouseHandler
{

#define t_interval 1000
#define touchReset_interval 100000
#define touchDown_interval 1000
#define touchDirection_interval 1000
#define historyBufSize 30

  unsigned long t_currentMillis = t_interval;
  unsigned long t_previousMillis = 0;

  unsigned long touchReset_currentMillis = touchReset_interval;
  unsigned long touchReset_previousMillis = 0;

  unsigned long lastTouchOrigin_time = 0;

  unsigned long touchDown_currentMillis = touchDown_interval;
  unsigned long touchDown_previousMillis = 0;

  unsigned long touchDirection_currentMillis = touchDirection_interval;
  unsigned long touchDirection_previousMillis = 0;

  unsigned long firstTouchDown_Millis = 0;
  unsigned long firstTouchUp_Millis = 0;

  bool hasTouch = false;
  bool resetTouchDown = false;

  unsigned short historyIdx = 0;
  unsigned int touchCycleCount = 0;

  TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
  TDCoord lastTouchOrigin = TDCoord(-1, -1, -1);
  TDCoord lastTouchDirection = TDCoord(-1, -1, -1);
  TDCoord lastTouchPosition_global = TDCoord(-1, -1, -1);

  unsigned short history[historyBufSize][3];

  TDCoord getRelativePoint(TDCoord);
  TDCoord touchTranslation(TDCoord);
  TDCoord touchVelocity(TDCoord);

  void clearLastTouchIfNeeded(TDCoord, TDCoord, bool);
  void resetHistory();
  void compareWithLastTouch(TDCoord);

  bool isTouching(TDCoord);
  bool checkIfItWasClick();

public:
  MouseHandler()
  {
    resetHistory();
  }

  void print(TDCoord);
  bool isTouchDown();
  TDCoord handleMouse(int = 2.5, int = 2.5);
  TDCoord loop(int = 2.5, int = 2.5);
};

bool MouseHandler::isTouchDown()
{
  TDCoord p = ts.getPoint();
  return isTouching(p);
}

void MouseHandler::resetHistory()
{
  for (unsigned short i = 0; i < historyBufSize; i++)
  {
    history[i][0] = 0;
    history[i][1] = 0;
    history[i][2] = 0;
  }
  historyIdx = 0;
}

bool MouseHandler::isTouching(TDCoord p) { return p.z < MAXPRESSURE && p.z > MINPRESSURE; }

void MouseHandler::compareWithLastTouch(TDCoord p)
{

  bool flag = false;
  _kResetTimer(touchDirection_currentMillis,
               touchDirection_previousMillis,
               touchDirection_interval,
               flag = true;);

  if (!flag)
    return;

  int x = ((lastTouchOrigin.x < p.x) && (p.x < lastTouchDirection.x)) || ((lastTouchDirection.x < p.x) && (p.x < lastTouchOrigin.x));
  int y = ((lastTouchOrigin.y < p.y) && (p.y < lastTouchDirection.y)) || ((lastTouchDirection.y < p.y) && (p.y < lastTouchOrigin.y));

  if (lastTouchDirection.z == -1)
  {
    lastTouchDirection = p;
    return;
  }

  if (x || y)
  {
    hasTouch = false;
  }
  lastTouchDirection = p;
}

void MouseHandler::clearLastTouchIfNeeded(TDCoord t, TDCoord p, bool force)
{

  _kResetTimer(
      touchReset_currentMillis, touchReset_previousMillis, touchReset_interval,
      // if (!isTouching(p) || force)
      hasTouch = false;
      // if (hasTouch) {
      // hasTouch = false;

      // lastTouchOrigin = TDCoord(
      //     (p.x - t.x) / 2,
      //     (p.y - t.y) / 2,
      // t.z);
      // lastTouchOrigin = TDCoord(
      //     (p.x - t.x) / 2000,
      //     (p.y - t.y) / 2000,
      //     t.z);
      // }

  );
}

void MouseHandler::print(TDCoord p = TDCoord(-1, -1, -1))
{

  if (p.z == -1)
    p = ts.getPoint();

  Serial.print("X: ");
  Serial.print(p.x);
  Serial.print("    Y: ");
  Serial.print(p.y);
  Serial.println();
}

TDCoord MouseHandler::getRelativePoint(TDCoord p)
{
  if (!hasTouch)
  {
    lastTouchOrigin = p;
    hasTouch = true;
    lastTouchOrigin_time = TIME_MEASURE();
  }

  if (resetTouchDown)
  {
    firstTouchDown_Millis = TIME_MEASURE();
    resetTouchDown = false;
    touchCycleCount = 0;
  }

  return p;
}

TDCoord MouseHandler::touchVelocity(TDCoord c)
{

  unsigned long t = touchReset_currentMillis - lastTouchOrigin_time;
  t = t == 0 ? 1 : t / 1000;
  TDCoord velocity = TDCoord(
      (c.x - lastTouchOrigin.x) / t,
      (c.y - lastTouchOrigin.y) / t,
      (c.z - lastTouchOrigin.z) / t);
  return velocity;
}

TDCoord MouseHandler::touchTranslation(TDCoord c)
{
  TDCoord translation = TDCoord(
      c.x - lastTouchOrigin.x,
      c.y - lastTouchOrigin.y,
      c.z - lastTouchOrigin.z);
  return translation;
}

bool MouseHandler::checkIfItWasClick()
{
  double avg_x = 0;
  double avg_y = 0;
  double variance_x = 0;
  double variance_y = 0;

  bool flag = false;

  unsigned short invalid_count_x = 0;

  if (TIME_MEASURE() - firstTouchDown_Millis < CLICK_DURATION)
  {

    for (unsigned short i = 0; i < historyBufSize; i++)
    {

      invalid_count_x += history[i][0] == 0 ? 1 : 0;
      if (history[i][0] == 0)
        continue;
      avg_x += history[i][0];
      avg_y += history[i][1];
    }

    if (abs(invalid_count_x - touchCycleCount) / 3 > abs(historyBufSize - touchCycleCount))
      return false;

    avg_x /= (historyBufSize - invalid_count_x);
    avg_y /= (historyBufSize - invalid_count_x);

    for (unsigned short i = 0; i < historyBufSize; i++)
    {
      if (history[i][0] == 0)
        continue;

      variance_x += pow(history[i][0] - avg_x, 2);
      variance_y += pow(history[i][1] - avg_y, 2);
    }

    int div_x = (historyBufSize - invalid_count_x);
    int div_y = (historyBufSize - invalid_count_x);

    if (div_x == 0 || div_y == 0)
      return false;

    variance_x /= div_x;
    variance_y /= div_y;

    flag = true;

    // Serial.print("x: ");
    // Serial.print(variance_x);

    // Serial.print("    y: ");
    // Serial.println(variance_y);
  }

  resetTouchDown = true;
  resetHistory();

  return flag && (variance_x <= CLICK_SENS && variance_y <= CLICK_SENS);
}

TDCoord MouseHandler::handleMouse(int x_mul, int y_mul)
{

  TDCoord p = ts.getPoint();
  TDCoord out = TDCoord(0, 0, 0);

  if (isTouching(p))
  {
    if (lastTouchPosition_global.z == -1)
    {
      lastTouchPosition_global = p;
      return out;
    }
    TDCoord r = getRelativePoint(p);
    TDCoord t = touchTranslation(r);
    // compareWithLastTouch(p);
    touchCycleCount++;

    // out.x = t.x * x_mul;
    // out.y = t.y * y_mul;
    
    // out.x = p.x - lastTouchPosition_global.x) * x_mul;
    // out.y = p.y - lastTouchPosition_global.y) * y_mul;

    // Serial.print("c: "); print(p);
    // Serial.print("o: "); print(lastTouchPosition_global);

    // Serial.print(" ");
    // Serial.println(lastTouchPosition_global.x);
    short int dif_x = p.x - lastTouchPosition_global.x;
    short int dif_y = p.y - lastTouchPosition_global.y;

    short int fin_x = abs(dif_x) <= X_MOVEMENT_DIFFERENTIAL_IGNORANCE ? 0 : dif_x;
    short int fin_y = abs(dif_y) <= Y_MOVEMENT_DIFFERENTIAL_IGNORANCE ? 0 : dif_y;
    // Serial.println(fin_x);
    // Serial.print(" ");
    // Serial.println(fin_y);

    // out.x = (p.x - lastTouchPosition_global.x) * X_MUL;
    // out.y = (p.y - lastTouchPosition_global.y) * Y_MUL;

    out.x = fin_x * x_mul;
    out.y = fin_y * y_mul;


    // if (fin_x != 0 && fin_y != 0)// && out.x != 0 && out.y != 0)
    lastTouchPosition_global = p;

    clearLastTouchIfNeeded(t, p, false);
    // clearLastTouchIfNeeded(t, TDCoord(fin_x, fin_y, 0), false);

    if (TIME_MEASURE() % 4 == 0)
    {
      if (historyIdx >= historyBufSize)
        historyIdx = 0;
      history[historyIdx][0] = r.x;
      history[historyIdx][1] = r.y;

      // history[historyIdx][0] = fin_x + 1;
      // history[historyIdx][1] = fin_y + 1;

      history[historyIdx++][2] = r.z;
    }
    // out = t;
  }
  else
  {
    historyIdx = 0;
    lastTouchPosition_global = TDCoord(-1, -1, -1);
    clearLastTouchIfNeeded(TDCoord(), TDCoord(), true);
    firstTouchUp_Millis = TIME_MEASURE();
    bool isClick = checkIfItWasClick();

    out.z = isClick ? 1 : 0;
    // if (isClick)
    // {
    // Serial.println("Clicked");
    // Mouse.press();
    // Mouse.release();
    // while (isTouching(p))
    //   ;
    // delay(10);
    // }
  }
  return out;
}

TDCoord MouseHandler::loop(int x_mul, int y_mul)
{
  bool flag = false;
  _kResetTimer(t_currentMillis, t_previousMillis, t_interval, flag = true;);
  if (!flag)
    return TDCoord(0, 0, 0);

  return handleMouse (x_mul, y_mul);
}

#endif