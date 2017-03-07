/*
 * This file is part of BeerFridgeController.
 *
 *    BeerFridgeController is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    BeerFridgeController is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with BeerFridgeController.  If not, see <http://www.gnu.org/licenses/>.
 *    
 *    BeerFridgeController  Copyright (C) 2016-2017  Gabriel Tremblay
 *    
*/
#include "temp_lm35.h"

float get_temp_celcius(){
   return (5.0 * analogRead(TEMP_ANALOG_PIN) * 100.0) / 1024;
}

