clear all; close all; clc; clear global;

A = [[-148.46148681640625, -313.98635864257812, -356.88388061523437];...
     [-313.98635864257812, -663.79522705078125, -754.91699218750000];...
     [-356.88388061523437, -754.91699218750000, -857.56695556640625]];
 
[eigVectors, eigValues] = eig(A);

%%% Vectors are already unit length!

%%% From eig help page: Matrix V is the modal matrix — its columns are the eigenvectors of A.
vecA = eigVectors(:,1); 
% len = sqrt(vecA(1)*vecA(1) + vecA(2)*vecA(2) + vecA(3)*vecA(3));
% vecA = vecA ./ len;
vecB = eigVectors(:,2);
% len = sqrt(vecB(1)*vecB(1) + vecB(2)*vecB(2) + vecB(3)*vecB(3));
% vecB = vecB ./ len;
vecC = eigVectors(:,3);
% len = sqrt(vecC(1)*vecC(1) + vecC(2)*vecC(2) + vecC(3)*vecC(3));
% vecC = vecC ./ len;



%% Some random test code
temp2 = [0, 0, 1];
temp1 = vecC;
rotaxis = cross(temp2,temp1); len = sqrt(rotaxis(1)*rotaxis(1) + rotaxis(2)*rotaxis(2) + rotaxis(3)*rotaxis(3));
rotaxis = rotaxis ./ len
rotangle = acos(dot(temp2,temp1))

