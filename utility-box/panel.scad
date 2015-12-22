include <nixie.scad>;

LENGTH = 200;
WIDTH = 80;
RAD = 2.5;

// Laser cutter beam kerf (diameter)
LBD = 0.23;
// Material thickness
MAT_Z = 3.15;

SMOUNT = 5;
SRAD = 2;

SHIFT = 7.5;

/* EXPORT = false;*/
EXPORT = true;

// Working
W = WIDTH + LBD - (RAD * 2);
L = LENGTH + LBD - (RAD * 2);
echo("L:",L);
echo("W:",W);

if (EXPORT) {
  projection() panel();
} else {
  panel();
  translate([-AL*3/2-SHIFT,0,0]) nixie();
  translate([-AL*1/2-SHIFT,0,0]) nixie();
}

module panel() {
  difference() {
    minkowski() {
          cube([L,W,MAT_Z], center=true);
          cylinder(r=RAD,h=1);
    }
    translate([-SHIFT,0,0]) {
      translate([4,AH/2-10,-MAT_Z]) linear_extrude(height = MAT_Z*3) text("x10",size=4,valign="center");
      translate([4,AH/2-24,-MAT_Z]) linear_extrude(height = MAT_Z*3) text("x1",size=4,valign="center");
      translate([14,AH/2-TI,-MAT_Z]) linear_extrude(height = MAT_Z*3) text("MKWh",size=20,valign="center");
      // button
      /* translate([80,-20,MAT_Z]) cylinder(r=6,h=MAT_Z*2,center=true);*/
      // holes
      translate([-AL/2,0,0]) {
        screw_holes();
        translate([-AL/2+TI,AH/2-TI,0]) cylinder(r=TUBER,h=MAT_Z*2,center=true);
        translate([-AL+TI,AH/2-TI,0]) cube([AL,18,MAT_Z*2],center=true);
      }
      translate([-AL*3/2,0,0]) {
        screw_holes();
        translate([-AL/2+TI,AH/2-TI,0]) cylinder(r=TUBER,h=MAT_Z*2,center=true);
      }
      // connect holes
      translate([-AL/2,AH/2-TI,0]) minkowski() {
        cube([AL-2,18-2,MAT_Z*2],center=true);
        cylinder(r=1,h=1);
      }
      /* translate([-AL*3/2,AH/2-TI,0]) minkowski() {*/
      /*   cube([AL-2,18-2,MAT_Z*2],center=true);*/
      /*   cylinder(r=1,h=1);*/
      /* }*/
    }
    screws();
  }
}

module screws() {
  translate([-L/2+SMOUNT,-W/2+SMOUNT,0]) cylinder(r=SRAD,h=MAT_Z*2,center=true);
  translate([-L/2+SMOUNT,+W/2-SMOUNT,0]) cylinder(r=SRAD,h=MAT_Z*2,center=true);
  translate([+L/2-SMOUNT,+W/2-SMOUNT,0]) cylinder(r=SRAD,h=MAT_Z*2,center=true);
  translate([+L/2-SMOUNT,-W/2+SMOUNT,0]) cylinder(r=SRAD,h=MAT_Z*2,center=true);
}
  

