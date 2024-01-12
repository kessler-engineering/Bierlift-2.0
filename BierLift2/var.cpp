#include "CRMui3.h"


void CRMui3::var_auto_save(bool save) {
  if (save) {
    _AutoSaveStatus = true;
    _AutoSaveTimer = millis();
  }
}


String CRMui3::var(const String &name) {
  return cfg[name];
}

void CRMui3::var(const String &name, const String &value, bool save) {
  cfg[name] = value;
  var_auto_save(save);
}

void CRMui3::var(const String &name, float value, bool save) {
  cfg[name] = value;
  var_auto_save(save);
}

void CRMui3::var(const String &name, double value, bool save) {
  cfg[name] = value;
  var_auto_save(save);
}

void CRMui3::var(const String &name, int value, bool save) {
  cfg[name] = value;
  var_auto_save(save);
}

void CRMui3::var(const String &name, long value, bool save) {
  cfg[name] = value;
  var_auto_save(save);
}

void CRMui3::var(const String &name, uint32_t value, bool save) {
  cfg[name] = value;
  var_auto_save(save);
}

void CRMui3::var(const String &name, char value, bool save) {
  cfg[name] = value;
  var_auto_save(save);
}
