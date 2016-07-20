/*    
    simpleMultiTask.ino
    Copyright (c) 2016 ItKindaWorks All right reserved.
    github.com/ItKindaWorks

    This file is part of simpleMultiTask

    simpleMultiTask is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    simpleMultiTask is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with simpleMultiTask.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Metro.h> 

const int blinkPin1 = 13;
const int blinkPin2 = 12;

boolean blink1State = false;
boolean blink2State = false;

Metro blink1Metro = Metro(1000);
Metro blink2Metro = Metro(500);


void setup(){
  pinMode(blinkPin1 , OUTPUT);
  pinMode(blinkPin2 , OUTPUT);
}


void loop(){

  if(blink1Metro.check()){
    blink1State = !blink1State;
    digitalWrite(blinkPin1, blink1State);
  }


  if(blink2Metro.check()){
    blink2State = !blink2State;
    digitalWrite(blinkPin2, blink2State);
  }
  
}















