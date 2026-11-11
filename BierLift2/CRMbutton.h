#ifndef CRMbutton_h
#define CRMbutton_h

#define _EB_DEB 50      // Entprellzeit
#define _EB_HOLD 1000   // Zeit bis "gehalten"
#define _EB_STEP 500    // Intervall der Step-Signale bei gehaltener Taste
#define _EB_CLICK 500	  // Zeitfenster für Mehrfachklicks

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
      if (!digitalRead(S1)) {                                                	// Taste gedrückt
        if (!_readFlag(3)) {                                          	// und vorher nicht gedrückt
          if (debounce > EB_DEB) {                                   	// und Entprellzeit abgelaufen
            _setFlag(3);                                            // Flag: Taste wurde gedrückt
            _debTimer = thisMls;                                    // Timer zurücksetzen
            CBState = 0;                                           	// Zustand zurücksetzen
          }
          if (debounce > EB_CLICK) {									// Taste erst nach EB_CLICK erneut gedrückt
            clicks = 0;												// Klickzähler und Klick-Flag zurücksetzen
            flags &= ~0b01100000;
          }
        } else {                                                      	// Taste war schon gedrückt
          if (!_readFlag(4)) {                                        // und Halten noch nicht erkannt
            if (debounce < EB_HOLD) {                              	// Haltezeit noch nicht erreicht
              if (CBState != 0) _setFlag(2);                     	// aber Encoder gedreht – merken (für Taster ohne Bedeutung)
            } else {                                                // Haltezeit überschritten
              if (!_readFlag(2)) {                                // und Encoder nicht gedreht
                CBState = 6;                                   	// also: gehalten (Signal)
                _setFlag(4);                                    // merken, dass gehalten wird
                _debTimer = thisMls;                            // Timer zurücksetzen
              }
            }
          } else {                                                    // Halten erkannt
            if (debounce > EB_STEP) {                              	// Step-Timer
              CBState = 7;                                       	// Signal ausgeben
              _debTimer = thisMls;                                // Timer zurücksetzen
            }
          }
        }
      } else {                                                        	// Taste nicht gedrückt
        if (_readFlag(3)) {                                           	// war aber gedrückt
          if (debounce > EB_DEB && !_readFlag(4) && !_readFlag(2)) {	// nicht gedreht und nicht gehalten – also ein Klick
            CBState = 5;
            clicks++;
          }
          flags &= ~0b00011100;                                       // Flags 2, 3, 4 löschen
          _debTimer = thisMls;                                        // Timer zurücksetzen
        } else if (clicks > 0 && debounce > EB_CLICK && !_readFlag(5)) flags |= 0b01100000;	 // Flag: Klickserie abgeschlossen
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
