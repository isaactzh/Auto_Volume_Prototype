//import class to set up serial connection with wiring board
import processing.serial.*;
Serial port;
//button setup
color currentcolor;
RectButton rect1, rect2,rect3,rect4,rect5,rect6,rect7,rect8;
boolean locked = false;
 
void setup() {
   //set up window
   size(600, 400);
   color baseColor = color(102, 102, 102);
   currentcolor = baseColor;
   // List all the available serial ports in the output pane.
   // You will need to choose the port that the Wiring board is
   // connected to from this list. The first port in the list is
   // port #0 and the third port in the list is port #2.
   println(Serial.list());
   println(Serial.list()[1]);
   // Open the port that the Wiring board is connected to (in this case 1
   // which is the second open port in the array)
   // Make sure to open the port at the same speed Wiring is using (9600bps)
   port = new Serial(this, Serial.list()[1], 9600); 
   // Port number here may vary based on the connection to bluetooth module
   
   // Define and create rectangle button #1
   int x = 30;
   int y = 100;
   int size = 100;
   color buttoncolor = color(153, 102, 102);
   color highlight = color(102, 51, 51);
   rect1 = new RectButton(x, y, size, buttoncolor, highlight);
   // Define and create rectangle button #2
   x = 30;
   y = 220;
   size = 100;
   buttoncolor = color(153, 153, 153);
   highlight = color(102, 102, 102);
   rect2 = new RectButton(x, y, size, buttoncolor, highlight);
   x = 150;
   y = 100;
   size = 100;
   buttoncolor = color(204, 0, 204);
   highlight = color(102, 0, 102);
   rect3 = new RectButton(x, y, size, buttoncolor, highlight);
   x = 150;
   y = 220;
   size = 100;
   buttoncolor = color(204, 204, 0);
   highlight = color(102, 102, 0);
   rect4 = new RectButton(x, y, size, buttoncolor, highlight);
   x = 400;
   y = 100;
   size = 80;
   buttoncolor = color(0, 204, 204);
   highlight = color(0, 153, 153);
   rect5 = new RectButton(x, y, size, buttoncolor, highlight);
   x = 500;
   y = 100;
   size = 80;
   buttoncolor = color(0,204, 102);
   highlight = color(0,  153, 76);
   rect6 = new RectButton(x, y, size, buttoncolor, highlight);
   x = 270;
   y = 100;
   size = 100;
   buttoncolor = color(0, 204, 204);
   highlight = color(0, 153, 153);
   rect7 = new RectButton(x, y, size, buttoncolor, highlight);
   x = 270;
   y = 220;
   size = 100;
   buttoncolor = color(0,204, 102);
   highlight = color(0,  153, 76);
   rect8 = new RectButton(x, y, size, buttoncolor, highlight);
}
 
void draw() {
   background(currentcolor);
   stroke(255);
   update(mouseX, mouseY);
   rect1.display();
   rect2.display();
   rect3.display();
   rect4.display();
   rect5.display();
   rect6.display();
   rect7.display();
   rect8.display();
   fill(255);
   textSize(22);
   text("volup",48,160);
   text("voldown",35,280);
   textSize(20);
   text("toplimit",163,148);
   text("up",185,168);
   text("toplimit",163,268);
   text("down",175,288);
   text("botlimit",283,148);
   text("up",305,168);
   text("botlimit",283,268);
   text("down",295,288);
   textSize(18);
   text("auto",420,145);
   text("manual",509,145);
}
 
void update(int x, int y) {
   if(locked == false) {
      rect1.update();
      rect2.update();
      rect3.update();
      rect4.update();
      rect5.update();
      rect6.update();
      rect7.update();
      rect8.update();
   } else {
      locked = false;
   }
   //Turn LED on and off if buttons pressed where
   //H = on (high) and L = off (low)
   if(mousePressed) {
      if(rect1.pressed()) { //ON button
         currentcolor = rect1.basecolor;
         port.write("volUP");
         delay(200);
      } else if(rect2.pressed()) { //OFF button
         currentcolor = rect2.basecolor;
         port.write("volDOWN");
         delay(200);
      } else if(rect3.pressed()) { //ON button
         currentcolor = rect3.basecolor;
         port.write("toplimit_up");
         delay(200);
      } else if(rect4.pressed()) { //OFF button
         currentcolor = rect4.basecolor;
         port.write("toplimit_down");
         delay(200);
      } else if(rect5.pressed()) { //OFF button
         currentcolor = rect5.basecolor;
         port.write("auto");
         delay(200);
      } else if(rect6.pressed()) { //OFF button
         currentcolor = rect6.basecolor;
         port.write("manual");
         delay(200);
      } else if(rect7.pressed()) { //ON button
         currentcolor = rect7.basecolor;
         port.write("botlimit_up");
         delay(200);
      } else if(rect8.pressed()) { //OFF button
         currentcolor = rect8.basecolor;
         port.write("botlimit_down");
         delay(200);
      }
   }
}
 
class Button {
   int x, y;
   int size;
   color basecolor, highlightcolor;
   color currentcolor;
   boolean over = false;
   boolean pressed = false;
   void update() {
      if(over()) {
         currentcolor = highlightcolor;
      } else {
         currentcolor = basecolor;
      }
   }
   boolean pressed() {
      if(over) {
          locked = true;
          return true;
      } else {
          locked = false;
          return false;
      }
   }
   boolean over() {
      return true;
   }
   void display() {
   }
}
 
class RectButton extends Button {
   RectButton(int ix, int iy, int isize, color icolor, color ihighlight) {
      x = ix;
      y = iy;
      size = isize;
      basecolor = icolor;
      highlightcolor = ihighlight;
      currentcolor = basecolor;
   }
   boolean over() {
      if( overRect(x, y, size, size) ) {
         over = true;
         return true;
       } else {
         over = false;
         return false;
       }
    }
   void display() {
      stroke(255);
      fill(currentcolor);
      rect(x, y, size, size);
   }
}
 
boolean overRect(int x, int y, int width, int height) {
   if (mouseX >= x && mouseX <= x+width && mouseY >= y && mouseY <= y+height) {
      return true;
   } else {
      return false;
   }
}