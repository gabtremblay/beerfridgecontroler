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
#ifndef FSR_H
#define FSR_H

#define FSR_ANALOG_PIN 0

struct FSR_READOUT {
  int raw_value = 0;
  int scaled_value = 0;
};

FSR_READOUT get_fsr_readout(void);

#endif

