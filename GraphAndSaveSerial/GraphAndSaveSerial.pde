/*
 graphPoints tab delimited data from serial port and writes the data to a file.
 ENTER will dump the screen to biggles.png in the sketches subdirectory
 any other key press will start/stop saving to data.txt in the sketches sudirectory, data.txt must exist prior 
 A line beginning with * will be interpreted as column titles and written each time you start saving data  
 Doesn't like \t at end of line
 
 */

final int port = 10;         // run program to see list then change this to match the array index of COM: port 
final int baud = 57600;

import processing.serial.*;
Serial myPort;                

int maxColumns = 10;
int[] columnDataIn = new int[maxColumns];
String[] columnTitleIn = new String[maxColumns];
color[] columnColor = new color[maxColumns];

String stringIn;
String rowStore;
int fileSaveYes = 0;
int writeHeader = 0;

int currentX = 0;
int width = 800;
int hieght = 600;

void setup () {
  size(width,hieght);

  println(Serial.list());
  myPort = new Serial(this, Serial.list()[port], baud);   // out of bounds? pluged in your serial cable? 
  myPort.clear();
  myPort.bufferUntil('\n');

  // println(PFont.list());
  PFont myFont = createFont(PFont.list()[277], 18 );
  textFont(myFont);

  columnColor[0] = color(#ff0000);
  columnColor[1] = color(#00ff00);
  columnColor[2] = color(#0000ff);
  columnColor[3] = color(#A123D8);
  columnColor[4] = color(#00ffff);
  columnColor[5] = color(#E87A05);
  columnColor[6] = color(#007C0D);
  columnColor[7] = color(#000000);
  columnColor[8] = color(#989805);
  columnColor[9] = color(#8B6D98);
  smooth();
  background(255);
  strokeWeight(1);
  stroke(#B9B6B6,100);
  for (int i = 0; i < height/100; i++) {
    line(currentX, (height - (i*100)) + 1, width-1, height - (i*100));  //crude grid
  }
}

void draw () {
}

void serialEvent(Serial myPort) {
  stringIn = myPort.readStringUntil('\n');
  if (stringIn != null) {
    String m =stringIn.substring(0,1);
    if (m.equals("!")) {
      println(stringIn.substring(1));
      fill(128);
      text(stringIn.substring(1), 10, 30);
    } 
    else {  
      if (m.equals("*")) {
        columnTitleIn = split(stringIn.substring(1), "\t");

        rowStore = stringIn.substring(1);
        print(rowStore);
      }
      else {
        print(stringIn);
        String[] columnValues = split(stringIn, "\t");
        for (int i = 0; i < columnValues.length; i++) {
          if (columnValues.length <= maxColumns) {                // can be less but not more
            columnValues[i] = trim(columnValues[i]);
            columnDataIn[i] =  int(columnValues[i]);
          }
          if (columnDataIn[i]  >= 0 ) {          // lazy
            stroke(columnColor[i]);
            fill(columnColor[i]);
            text(columnTitleIn[i], 10+(100*i), 15);
            line(currentX, (height - columnDataIn[i]) + 1, currentX, height - columnDataIn[i]);
          }
        }
      }
      stroke(#B9B6B6,100);
      for (int i = 0; i < height/100; i++) {
        line(currentX, (height - (i*100)) + 1, currentX, height - (i*100));  //crude grid
      }
      if (currentX < width-1) {
        currentX++;
      } 
      else {
        copy(1, 40, width-1, height - 40, 0, 40, width-1, height-40);
        stroke(255);
        line(currentX, 0, currentX, height);
      }
    }
    if (fileSaveYes != 0) {
      if (writeHeader != 0) {
        writeFile(rowStore);
        writeHeader = 0;
      }  
      writeFile(stringIn);
    }
  }
}

void keyPressed() {
  if (key == ENTER || key == RETURN) {
    save("biggles.png");
  } 
  else {
    if (fileSaveYes == 0) {
      fileSaveYes=1;
      writeHeader = 1;
      fill(#FF0000);
      text("** Saving **", 650, 15);
    } 
    else {
      fileSaveYes=0;
      fill(255,255);
      stroke(255);
      rect(650, 0, 150, 20);
      // text("** Saving **", 10, 30);
    }
  }
}


void writeFile(String rowIn) {
  String[] newRow = new String[1];
  newRow[0] = hour()+ ":" +  minute()+ ":" + second()+ " " + month() + "/"  + day() + "/" + year();
  newRow[0] += "\t";
  newRow[0] += trim(rowIn);
  String[] oldRows = loadStrings("data.txt");
  if (oldRows != null) {
    String[] newRows = new String[oldRows.length + newRow.length];
    for (int s = 0; s < oldRows.length; s++) {
      newRows[s] = oldRows[s];
    }
    for (int s = oldRows.length; s < newRows.length; s++) {
      newRows[s] = newRow[s - oldRows.length];
    }
    saveStrings("data.txt", newRows);
  }
  else {
    String[] newRows = new String[0];
    newRows[0] = "Yippee";
    saveStrings("data.txt", newRows);
  }
}

String timeStamp() {
  String now = hour()+ ":" +  minute()+ ":" + second()+ " " + month() + "/"  + day() + "/" + year();
  return now;
}

