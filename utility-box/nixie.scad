$fn=100;
// Laser cutter beam kerf (diameter)
LBD = 0.23;
// Material thickness
MAT_Z = 2.5;
// Screw R
SR = 1.5;
SRIN = 3.5;
AL = 40;
AH = 50;
// Tube radius
TUBER = 15;
// Tube inset
TI = 17;

/* nixie();*/

module nixie() {
  acrylic();
  translate([-AL/2+TI,AH/2-TI,0]) tube();
  translate([0,0,-12]) acrylic();
  translate([-AL/2+SRIN,-AH/2+SRIN,-12]) standoff(r=SR,h=MAT_Z*2,center=true);
  translate([-AL/2+SRIN,+AH/2-SRIN,-12]) standoff(r=SR,h=MAT_Z*2,center=true);
  translate([+AL/2-SRIN,+AH/2-SRIN,-12]) standoff(r=SR,h=MAT_Z*2,center=true);
  translate([+AL/2-SRIN,-AH/2+SRIN,-12]) standoff(r=SR,h=MAT_Z*2,center=true);
}

module acrylic() {
  difference() {
    cube([AL,AH,MAT_Z], center=true);
    screw_holes();
  }
}

module screw_holes() {
  translate([-AL/2+SRIN,-AH/2+SRIN,0]) cylinder(r=SR,h=MAT_Z*2,center=true);
  translate([-AL/2+SRIN,+AH/2-SRIN,0]) cylinder(r=SR,h=MAT_Z*2,center=true);
  translate([+AL/2-SRIN,+AH/2-SRIN,0]) cylinder(r=SR,h=MAT_Z*2,center=true);
  translate([+AL/2-SRIN,-AH/2+SRIN,0]) cylinder(r=SR,h=MAT_Z*2,center=true);
}

module tube(){
  union() {
    cylinder(r=TUBER,h=26);
    /* rotate_extrude(convexity = 10)*/
    /* translate([TUBER/2, 0, 26])*/
    /* rotate([90,0,0])*/
    /* circle(r=TUBER);*/
  }
}

module standoff() {
  difference() {
    cylinder(r=2,h=12);
    cylinder(r=1,h=12);
  }
}
