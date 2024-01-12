#ifndef CRMbutton_h
#define CRMbutton_h

#define _EB_DEB 50      // дебаунс кнопки
#define _EB_HOLD 1000   // таймаут удержания кнопки
#define _EB_STEP 500    // период срабатывания степ
#define _EB_CLICK 500	  // таймаут накликивания

#include <Arduino.h>
#define _setFlag(x) (flags |= 1 << x)
#define _clrFlag(x) (flags &= ~(1 << x))
#define _readFlag(x) ((flags >> x) & 1)

#ifndef EB_DEB
#define EB_DEB _EB_DEB
#endif
#ifndef EB_HOLD
#define EB_HOLD _EB_HOLD
#endif
#ifndef EB_STEP
#define EB_STEP _EB_STEP
#endif
#ifndef EB_CLICK
#define EB_CLICK _EB_CLICK
#endif

template <uint8_t S1>
class CRMbutton {
  public:
    void run() {
      uint32_t thisMls = millis();
      uint32_t debounce = thisMls - _debTimer;
      if (!digitalRead(S1)) {                                                	// кнопка нажата
        if (!_readFlag(3)) {                                          	// и не была нажата ранее
          if (debounce > EB_DEB) {                                   	// и прошел дебаунс
            _setFlag(3);                                            // флаг кнопка была нажата
            _debTimer = thisMls;                                    // сброс таймаутов
            CBState = 0;                                           	// сброс состояния
          }
          if (debounce > EB_CLICK) {									// кнопка нажата после EB_CLICK
            clicks = 0;												// сбросить счётчик и флаг кликов
            flags &= ~0b01100000;
          }
        } else {                                                      	// кнопка уже была нажата
          if (!_readFlag(4)) {                                        // и удержание ещё не зафиксировано
            if (debounce < EB_HOLD) {                              	// прошло меньше удержания
              if (CBState != 0) _setFlag(2);                     	// но энкодер повёрнут! Запомнили
            } else {                                                // прошло больше времени удержания
              if (!_readFlag(2)) {                                // и энкодер не повёрнут
                CBState = 6;                                   	// значит это удержание (сигнал)
                _setFlag(4);                                    // запомнили что удерживается
                _debTimer = thisMls;                            // сброс таймаута
              }
            }
          } else {                                                    // удержание зафиксировано
            if (debounce > EB_STEP) {                              	// таймер степа
              CBState = 7;                                       	// сигналим
              _debTimer = thisMls;                                // сброс таймаута
            }
          }
        }
      } else {                                                        	// кнопка не нажата
        if (_readFlag(3)) {                                           	// но была нажата
          if (debounce > EB_DEB && !_readFlag(4) && !_readFlag(2)) {	// энкодер не трогали и не удерживали - это клик
            CBState = 5;
            clicks++;
          }
          flags &= ~0b00011100;                                       // clear 2 3 4
          _debTimer = thisMls;                                        // сброс таймаута
        } else if (clicks > 0 && debounce > EB_CLICK && !_readFlag(5)) flags |= 0b01100000;	 // флаг на клики
      }
    }

    uint8_t getState() {
      return flags && CBState;
    }

    void resetState() {
      CBState = 0;
    }

    bool isFast() {
      return _readFlag(1);
    }

    bool isTurn() {
      return (CBState > 0 && CBState < 5);
    }

    bool isClick() {
      return checkState(5);
    }

    bool isHolded() {
      return checkState(6);
    }

    bool isHold() {
      return _readFlag(4);
    }

    bool isStep() {
      return checkState(7);
    }

    bool hasClicks(uint8_t numClicks) {
      if (clicks == numClicks && _readFlag(6)) {
        _clrFlag(6);
				CBState = 0;
        return true;
      }
      return false;
    }

    uint8_t hasClicks() {
      if (_readFlag(6)) {
        _clrFlag(6);
				CBState = 0;
        return clicks;
      }
      return false;
    }

    uint8_t clicks = 0;

  private:
    bool checkState(uint8_t val) {
      if (CBState == val) {
        CBState = 0;
        return true;
      }
      return false;
    }
    uint32_t _debTimer = 0;
    uint8_t CBState = 0;
    uint8_t flags = 0;
};

#endif
