//Box
box_x = 150;
box_y = 140;
box_front_thick = 3;
box_side_thick = 2;
box_side_tall = 50;
//LCD
lcd_open_x = 80;
lcd_open_y = 27.5;
lcd_peg_x = 93;
lcd_peg_y = 55;
lcd_peg_tall = 9.3;
lcd_peg_dia = 8;
lcd_hole = 2.8;
lcd_y_offset = 10;
//ARDUINO
p1_x = 49;
p2_x = 34;
p3_x = 6;
p23_y = 72.5;
ard_rot = 180;
ard_x_offset = 12;
ard_y_offset = 8;
ard_peg_dia = 8;
ard_peg_tall = 21;
ard_hole = 2.5;
//BUTTONS
butt_d = 12;
//top buttons
top_butt_x_offset = -25;
top_butt_y_offset = 50;
top_butt_cnt = 3;
top_butt_dist = 25;
//side buttons
side_butt_dist = 25;
side_butt_cnt = 2;
ls_butt_x_offset = -60;
ls_butt_y_offset = 0;
rs_butt_x_offset = 60;
rs_butt_y_offset = 0;
//bottom menu buttons
bott_men_butt_x_offset = -60;
bott_men_butt_y_offset = -40;
bott_men_butt_cnt = 2;
bott_men_butt_dist = 25;
//bottom button
b_butt_x_offset = 20;
b_butt_y_offset = -40;
//rotary encoder
rot_d = 42;
rot_bolt_cir = 26;
rot_bolt_cnt = 3;
rot_bolt_dia = 4;
rot_ang = 90;
rot_x_offset = -35;
rot_y_offset = -40;
//toggle switch
tog_dia = 12;
tog_slot_wide = 1.8;
tog_slot_deep = 1;
tog_x_offset = -7.5;
tog_y_offset = -40;
//direction lever
dir_d = 22;
dir_flat = 20.6; 
dir_x_offset = 55;
dir_y_offset = -40;
//bread board
bb_dist = 40.6;
bb_x_offset = -68;
bb_y_offset = -28;
bb_peg_tall = 25;
bb_hole = 2;
bb_peg_dia = 10;
bb_rot = 0;

//SETTINGS
$fn=120;

difference(){
    union(){
        //Main Box
        difference(){
            translate([-box_x/2,-box_y/2,-box_side_tall])
            cube([box_x,box_y,box_front_thick+box_side_tall]);
            translate([-(box_x-box_side_thick*2)/2,-(box_y-box_side_thick*2)/2,-box_side_tall-1])
            cube([(box_x-box_side_thick*2),(box_y-box_side_thick*2),box_side_tall+1]);
        }
        //lcd pegs
        translate([lcd_peg_x/2,lcd_peg_y/2+lcd_y_offset,-lcd_peg_tall])
        cyl_hole(lcd_peg_dia,lcd_peg_tall,lcd_hole);
        translate([lcd_peg_x/2,-lcd_peg_y/2+lcd_y_offset,-lcd_peg_tall])
        cyl_hole(lcd_peg_dia,lcd_peg_tall,lcd_hole);
        translate([-lcd_peg_x/2,-lcd_peg_y/2+lcd_y_offset,-lcd_peg_tall])
        cyl_hole(lcd_peg_dia,lcd_peg_tall,lcd_hole);
        translate([-lcd_peg_x/2,lcd_peg_y/2+lcd_y_offset,-lcd_peg_tall])
        cyl_hole(lcd_peg_dia,lcd_peg_tall,lcd_hole);
        //arduino pegs
        translate([ard_x_offset,ard_y_offset,-ard_peg_tall])
        rotate(ard_rot)
        translate([-p1_x/2,-p23_y/2,0])
        union(){
            cyl_hole(ard_peg_dia,ard_peg_tall,ard_hole);
            translate([p1_x,0,0])
            cyl_hole(ard_peg_dia,ard_peg_tall,ard_hole);
            translate([p2_x,p23_y,0])
            cyl_hole(ard_peg_dia,ard_peg_tall,ard_hole);
            translate([p3_x,p23_y,0])
            cyl_hole(ard_peg_dia,ard_peg_tall,ard_hole);
        }
        //breadboard pegs
        translate([bb_x_offset,bb_y_offset,-bb_peg_tall])
        rotate(bb_rot)
        union(){
            cyl_hole(bb_peg_dia,bb_peg_tall,bb_hole);
            translate([bb_dist,0,0])
            cyl_hole(bb_peg_dia,bb_peg_tall,bb_hole);
        }
    }
    union(){
        //LCD
        translate([-lcd_open_x/2,-lcd_open_y/2+lcd_y_offset,-1])
        cube([lcd_open_x,lcd_open_y,box_front_thick+2]);
        //top buttons
        translate([top_butt_x_offset,top_butt_y_offset,-1])
        for(i=[0:top_butt_cnt-1]){
            translate([i*top_butt_dist,0,0])
            cylinder(d=butt_d,h=box_front_thick+2);
        }

        //left buttons
        translate([ls_butt_x_offset,ls_butt_y_offset,-1])
        for(i=[0:side_butt_cnt-1]){
            translate([0,i*side_butt_dist,0])
            cylinder(d=butt_d,h=box_front_thick+2);
        }
        //right buttons
        translate([rs_butt_x_offset,rs_butt_y_offset,-1])
        for(i=[0:side_butt_cnt-1]){
            translate([0,i*side_butt_dist,0])
            cylinder(d=butt_d,h=box_front_thick+2);
        }
        //bottom menu buttons
        translate([bott_men_butt_x_offset,bott_men_butt_y_offset,-1])
        for(i=[0:bott_men_butt_cnt-1]){
            translate([i*bott_men_butt_dist,0,0])
            cylinder(d=butt_d,h=box_front_thick+2);
        }
        //bottom button
        translate([b_butt_x_offset,b_butt_y_offset,-1])
        cylinder(d=butt_d,h=box_front_thick+2);
        //rotary encoder
        //translate([rot_x_offset, rot_y_offset,-1])
        //union(){
        //    cylinder(d=rot_d,h=box_front_thick+2);
        //    for(i=[0:rot_bolt_cnt-1]){
        //        rotate(i*(360/rot_bolt_cnt)+rot_ang)
        //        translate([rot_bolt_cir,0,0])
        //        cylinder(d=rot_bolt_dia,h=box_front_thick+2);
        //    }
        //}
        //toggle switch
        translate([tog_x_offset,tog_y_offset,-1])
        toggle(tog_dia,box_front_thick+2,tog_slot_wide,tog_slot_deep);
        //direction switch
        translate([dir_x_offset,dir_y_offset,-1])
        cylinder(d=dir_d,h=box_front_thick+2);
    }
}

//direction switch flats
translate([dir_x_offset,dir_y_offset,0])
union(){
    translate([dir_flat/2,-dir_d/2,0])
    cube([dir_d-dir_flat,dir_d,box_front_thick]);
    rotate(90)
    translate([dir_flat/2,-dir_d/2,0])
    cube([dir_d-dir_flat,dir_d,box_front_thick]);
    rotate(180)
    translate([dir_flat/2,-dir_d/2,0])
    cube([dir_d-dir_flat,dir_d,box_front_thick]);
    rotate(-90)
    translate([dir_flat/2,-dir_d/2,0])
    cube([dir_d-dir_flat,dir_d,box_front_thick]);
}

module cyl_hole(cyl_dia, cyl_tall, hole_dia){
    difference(){
        cylinder(d=cyl_dia,h=cyl_tall);
        translate([0,0,-1])
        cylinder(d=hole_dia,h=cyl_tall+2);
    }
}

module toggle(cyl_dia, cyl_tall, slot_gap, slot_deep){
    difference(){
        cylinder(d=cyl_dia,h=cyl_tall);
        translate([-cyl_dia/2-slot_deep,-slot_gap/2,-1])
        cube([slot_deep*2,slot_gap,cyl_tall+2]);
    }
}
