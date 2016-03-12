$fn=100;
/* ==================================*/
// --- OBJECT DIA AND SETTINGS ---

SCALE = 1;
// DIAMENSIONS ALL INNER BOX SIZE
HEIGHT = 110/SCALE;
WIDTH = 440/SCALE;
LENGTH = 720/SCALE;

spacing = 2; // spacing between dxf export for print

my_pos = 515;
mx_pos = 110;
mx = 145; // -5 for rounding
my = 145;
fy_pos = 270;
fx_pos = 100;
fx = 215; // -5 for rounding
fy = 115;

// Slot
// Tweak these till it looks right
XSlots = 24; // number of slots in base
YSlots = 12; // number of slots on sides
ZSlots = 4; // number of slots on sides

// Laser cutter beam kerf (diameter)
LBD = 0.23;
// Material thickness
MAT_Z = 3.15;

// --- COMPILE SETTING ----
// make export true if you want to create DXF print sheet, otherwise 3D render is created to visualise
export = true;

// --- SETTINGS END ---
/* ========================*/

// --- WORKING ----
H = HEIGHT + MAT_Z*2 + LBD;
W = WIDTH + MAT_Z*2 + LBD;
L = LENGTH + MAT_Z*2 + LBD;

// dent dia in each axis
DLX = (L / XSlots);
DLY = (W / YSlots);
DLZ = (H / ZSlots);
endSpace = W+spacing+MAT_Z;

// --- BUILD ---
// EXPORT
if (export) {
  projection() base();
  projection() translate([MAT_Z,endSpace]) end();
  projection() translate([H+MAT_Z*2+spacing,W+spacing+MAT_Z]) end();
  projection() translate([0,(W+spacing+MAT_Z/2)*2]) side(0);
  projection() translate([0,(W+spacing)*2+H+MAT_Z*3]) side(1);
  echo("L:",L);
  echo("W:",W);
  echo("H:",H);
} else {
// PREVIEW
  base();
  translate([MAT_Z,MAT_Z,MAT_Z]) rotate([0,270,0]) end();
  translate([L,MAT_Z,MAT_Z]) rotate([0,270,0]) end(1);
  translate([0,MAT_Z,MAT_Z]) rotate([90,0,0]) side(1);
  translate([0,W,MAT_Z]) rotate([90,0,0]) side(0);
}

// --- END BUILD ---

// Base Piece
module base() {
  difference() {
    cube([L,W,MAT_Z]);
      for (x = [DLX:DLX+DLX:L-DLX+LBD*XSlots*2]) {
        translate([x,MAT_Z/2-LBD/2,+MAT_Z/2]) dent(DLX,1);
        translate([x,W-MAT_Z/2+LBD/2,MAT_Z/2]) dent(DLX,1);
      }
      for (x = [DLY:DLY+DLY:W-DLY+LBD*YSlots*2]) {
        translate([-LBD/2,x,MAT_Z/2]) dent(DLY,2);
        translate([L+LBD/2,x,MAT_Z/2]) dent(DLY,2);
      }
      translate([fy_pos,fx/2+15,MAT_Z/2]) fuse_box();
      translate([my_pos,mx_pos,MAT_Z/2]) meter_box();
  }
}

// End piece
module end() {
  WEnd = W-MAT_Z*2;
    difference() {
      union() {
        cube([H,WEnd,MAT_Z]);
        for (x = [DLY-MAT_Z:DLY+DLY:WEnd-DLY+LBD*YSlots*2]) {
          translate([LBD/2,x,MAT_Z/2]) tooth(DLY,2);
        }
        for (x = [DLZ:DLZ+DLZ:H-DLZ+LBD*ZSlots*2]) {
          translate([x,-MAT_Z/2+LBD,MAT_Z/2]) tooth(DLZ,1);
          translate([x,WEnd+MAT_Z/2-LBD/2,MAT_Z/2]) tooth(DLZ,1);
        }
      }
    }
}

// Sides
module side() {
  WEnd = W-MAT_Z*2;
  difference() {
    union() {
      cube([L,H,MAT_Z]);
        for (x = [DLX:DLX+DLX:L-DLX+LBD*XSlots*2]) {
          translate([x,-MAT_Z/2+LBD/2,MAT_Z/2]) tooth(DLX,1);
        }
    }
    for (x = [DLZ:DLZ+DLZ:H-DLZ+LBD*ZSlots*2]) {
      translate([-LBD/2,x,MAT_Z/2]) dent(DLZ,2);
      translate([L+LBD/2,x,MAT_Z/2]) dent(DLZ,2);
    }
  }
}

module top() {
  minkowski() {
    cube([L,W,MAT_Z]);
    cylinder(r=12.5,h=MAT_Z,$fn=100);
  }
}

// Creates dents
module dent(DL,direction) {
  if (direction == 1) {
    cube([DL-LBD,MAT_Z-LBD/2,MAT_Z*2], center=true);
  } else if (direction == 2) {
    cube([MAT_Z*2-LBD/2,DL-LBD,MAT_Z*2], center=true);
  } else if (direction == 3) {
    cube([MAT_Z*2-LBD/2,W/4-LBD,MAT_Z*2], center=true);
  } else {
    cube([MAT_Z-LBD/2,W/4-LBD,MAT_Z*2], center=true);
  }
}

// Creates teeth
module tooth(DL,direction) {
  if (direction == 1) {
    cube([DL+LBD,MAT_Z+LBD/2,MAT_Z], center=true);
  } else if (direction == 2) {
    cube([MAT_Z*2+LBD/2,DL+LBD,MAT_Z], center=true);
  } else {
    cube([MAT_Z+LBD/2,W/4+LBD,MAT_Z], center=true);
  }
}

module fuse_box() {
  minkowski() {
    cube([fy,fx,MAT_Z*2], center=true);
    cylinder(r=5,h=1);
  }
}

module meter_box() {
  minkowski() {
    cube([my,mx,MAT_Z*2], center=true);
    cylinder(r=5,h=1);
  }
}
