#include <Arduino.h>
#include <SimpleTime.h>
#include "pv_simpletime.h"

pvSimpletime::pvSimpletime() {

}

void pvSimpletime::setTime( time_t t ) {
  this->t = t;
};

String pvSimpletime::getFormDT() {
  return getFormD() + " " + getFormT();
};

String pvSimpletime::getFormD() {
  String tmp;
  tmp = (day(this->t) < 10 ? "0" : "") + String(day(this->t)) + ".";
  tmp += (month(this->t) < 10 ? "0" : "") + String(month(this->t)) + ".";
  tmp += (year(this->t) < 10 ? "0" : "") + String(year(this->t));
  return tmp;
};

String pvSimpletime::getFormT() {
  String tmp;
  tmp = (hour(this->t) < 10 ? "0" : "") + String(hour(this->t)) + ":";
  tmp += (minute(this->t) < 10 ? "0" : "") + String(minute(this->t)) + ":";
  tmp += (second(this->t) < 10 ? "0" : "") + String(second(this->t));
  return tmp;
};