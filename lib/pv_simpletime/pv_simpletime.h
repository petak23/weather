#ifndef PV_SIMPLETIME_H
#define PV_SIMPLETIME_H

/**
 * Knižnica pre formátovanie času
 * 
 * Posledná zmena(last change): 28.12.2021
 * @author Ing. Peter VOJTECH ml. <petak23@gmail.com>
 * @copyright  Copyright (c) 2016 - 2021 Ing. Peter VOJTECH ml.
 * @license
 * @link       http://petak23.echo-msz.eu
 * @version 1.0.0
 * 
 * 
 */
#include <SimpleTime.h>

class pvSimpletime
{
  public:
    pvSimpletime();

    void setTime( time_t t );
    String getFormDT();
    String getFormD();
    String getFormT();
    
  private:
    time_t t;
};

#endif