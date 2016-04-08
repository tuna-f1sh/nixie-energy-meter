/// *==================================*/
// --- OBJECT DIA AND SETTINGS ---
$fn=100;

SCALE = 1;
// DIAMENSIONS ALL INNER BOX SIZE
HEIGHT = 35/SCALE;
WIDTH = 80/SCALE;
LENGTH = 80/SCALE;

spacing = 2; // spacing between dxf export for print

// Slot
// Tweak these till it looks right
XSlots = 8; // number of slots in base
YSlots = 8; // number of slots on sides
ZSlots = 4; // number of slots on sides

// Laser cutter beam kerf (diameter)
LBD = 0.23;
// Material thickness
MAT_Z = 3.15;

// --- COMPILE SETTING ----
// make export true if you want to create DXF print sheet, otherwise 3D render is created to visualise
export = true;
closed = true;

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
endSpace = W*2+H*2+spacing*4+MAT_Z*5;

// --- BUILD ---
// EXPORT
if (export) {
  projection() base();
  if (closed) {
    projection() translate([0,W+spacing]) base(0);
  }
  projection() translate([0,(W*2+spacing*2+MAT_Z)]) side(0);
  projection() translate([0,(W*2+spacing*3+MAT_Z*2+H+spacing)]) side(1);
  projection() translate([MAT_Z,endSpace]) end();
  projection() translate([H+MAT_Z*4+spacing,endSpace]) end(1);
  echo("L:",L);
  echo("W:",W);
  echo("H:",H);
} else {
// PREVIEW
  base();
  if (closed) {
    translate([0,0,H+MAT_Z]) base(0);
  }
  translate([MAT_Z,MAT_Z,MAT_Z]) rotate([0,270,0]) end();
  translate([L,MAT_Z,MAT_Z]) rotate([0,270,0]) end(1);
  translate([0,MAT_Z,MAT_Z]) rotate([90,0,0]) side(1);
  translate([0,W,MAT_Z]) rotate([90,0,0]) side(0);
}

// --- END BUILD ---

// Base Piece
module base(lid) {
  difference() {
    cube([L,W,MAT_Z]);
      for (x = [DLX:DLX+DLX:L-DLX+LBD*XSlots*2]) {
        if (lid) {
          translate([x,MAT_Z/2-LBD/2,+MAT_Z/2]) dent(DLX,1,-1);
          translate([x,W-MAT_Z/2+LBD/2,MAT_Z/2]) dent(DLX,1,-1);
        } else {
          translate([x,MAT_Z/2-LBD/2,+MAT_Z/2]) dent(DLX,1,LBD);
          translate([x,W-MAT_Z/2+LBD/2,MAT_Z/2]) dent(DLX,1,LBD);
        }
      }
      for (x = [DLY:DLY+DLY:W-DLY+LBD*YSlots*2]) {
        translate([-LBD/2,x,MAT_Z/2]) dent(DLY,2,LBD);
        translate([L+LBD/2,x,MAT_Z/2]) dent(DLY,2,LBD);
      }
      if (lid) {
        translate([L/4,H/2,0]) cylinder(r=2.5-LBD,h=MAT_Z);
      }
  }
}

// End piece
module end(face) {
  WEnd = W-MAT_Z*2;
    difference() {
      union() {
        cube([H,WEnd,MAT_Z]);
        for (x = [DLY-MAT_Z:DLY+DLY:WEnd-DLY+LBD*YSlots*2]) {
          translate([LBD/2,x,MAT_Z/2]) tooth(DLY,2,LBD);
          if (closed) {
            translate([H-LBD/2,x,MAT_Z/2]) tooth(DLY,2,LBD);
          }
        }
        for (x = [DLZ:DLZ+DLZ:H-DLZ+LBD*ZSlots*2]) {
          translate([x,-MAT_Z/2+LBD,MAT_Z/2]) tooth(DLZ,1,LBD);
          translate([x,WEnd+MAT_Z/2-LBD/2,MAT_Z/2]) tooth(DLZ,1,LBD);
        }
      }
      /* if (face) {*/
      /*   translate([H/2,WEnd/2,0]) {*/
      /*     rotate(90) linear_extrude(height = MAT_Z*3) text("1",size=40,halign = "center",valign="center");*/
      /*   }*/
      /* }*/
    }
}

// Sides
module side(connectors) {
  WEnd = W-MAT_Z*2;
  difference() {
    union() {
      cube([L,H,MAT_Z]);
        for (x = [DLX:DLX+DLX:L-DLX+LBD*XSlots*2]) {
          translate([x,-MAT_Z/2+LBD/2,MAT_Z/2]) tooth(DLX,1,LBD);
          if (closed) {
            translate([x,H+MAT_Z/2-LBD/2,MAT_Z/2]) tooth(DLX,1,LBD);
          }
        }
    }
    for (x = [DLZ:DLZ+DLZ:H-DLZ+LBD*ZSlots*2]) {
      translate([-LBD/2,x,MAT_Z/2]) dent(DLZ,2,LBD);
      translate([L+LBD/2,x,MAT_Z/2]) dent(DLZ,2,LBD);
    }
    if (connectors) {
      translate([L/4, H/2,0]) cylinder(r=4-LBD,h=MAT_Z);
      translate([L/4+20, H/2,0]) cylinder(r=3-LBD,h=MAT_Z);
    }
  }
}

// Creates dents
module dent(DL,direction,lbd) {
  if (direction == 1) {
    cube([DL-lbd,MAT_Z-lbd/2,MAT_Z*2], center=true);
  } else if (direction == 2) {
    cube([MAT_Z*2-lbd/2,DL-lbd,MAT_Z*2], center=true);
  } else if (direction == 3) {
    cube([MAT_Z*2-lbd/2,W/4-lbd,MAT_Z*2], center=true);
  } else {
    cube([MAT_Z-lbd/2,W/4-lbd,MAT_Z*2], center=true);
  }
}

// Creates teeth
module tooth(DL,direction,lbd) {
  if (direction == 1) {
    cube([DL+lbd,MAT_Z+lbd/2,MAT_Z], center=true);
  } else if (direction == 2) {
    cube([MAT_Z*2+lbd/2,DL+lbd,MAT_Z], center=true);
  } else {
    cube([MAT_Z+lbd/2,W/4+lbd,MAT_Z], center=true);
  }
}
