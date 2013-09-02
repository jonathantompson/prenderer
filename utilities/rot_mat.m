clear all; clc;
syms c s;
syms m0 m1 m2 m3 m4 m5 m6 m7 m8 m9 m10 m11 m12 m13 m14 m15;

% Rotate X axis row major
disp('X Axis row major');
A = [1, 0,  0, 0; ...
     0, c, -s, 0; ...
     0, s,  c, 0; ...
     0, 0,  0, 1];
M = [ m0,  m1,  m2,  m3; ...
      m4,  m5,  m6,  m7; ...
      m8,  m9, m10, m11; ...
     m12, m13, m14, m15];
   
M * A

% Rotate X axis column major
disp('X Axis column major');
A = [1, 0,  0, 0; ...
     0, c, -s, 0; ...
     0, s,  c, 0; ...
     0, 0,  0, 1];
M = [m0, m4,  m8, m12; ...
     m1, m5,  m9, m13; ...
     m2, m6, m10, m14; ...
     m3, m7, m11, m15];
   
M * A

% Rotate Y axis row major
disp('Y Axis row major');
A = [ c, 0, s, 0; ...
      0, 1, 0, 0; ...
     -s, 0, c, 0; ...
      0, 0, 0, 1];
M = [ m0,  m1,  m2,  m3; ...
      m4,  m5,  m6,  m7; ...
      m8,  m9, m10, m11; ...
     m12, m13, m14, m15];
M * A

% Rotate Y axis column major
disp('Y Axis column major');
A = [ c, 0, s, 0; ...
      0, 1, 0, 0; ...
     -s, 0, c, 0; ...
      0, 0, 0, 1];
M = [m0, m4,  m8, m12; ...
     m1, m5,  m9, m13; ...
     m2, m6, m10, m14; ...
     m3, m7, m11, m15];
   
M * A

% Rotate Z axis row major
disp('Z Axis row major');
A = [ c, -s, 0, 0; ...
      s,  c, 0, 0; ...
      0,  0, 1, 0; ...
      0,  0, 0, 1];
M = [ m0,  m1,  m2,  m3; ...
      m4,  m5,  m6,  m7; ...
      m8,  m9, m10, m11; ...
     m12, m13, m14, m15];
   
M * A

% Rotate Z axis column major
disp('Z Axis column major');
A = [ c, -s, 0, 0; ...
      s,  c, 0, 0; ...
      0,  0, 1, 0; ...
      0,  0, 0, 1];
M = [m0, m4,  m8, m12; ...
     m1, m5,  m9, m13; ...
     m2, m6, m10, m14; ...
     m3, m7, m11, m15];
   
M * A