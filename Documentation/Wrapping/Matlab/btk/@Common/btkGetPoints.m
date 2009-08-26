function [points pointsInfo] = btkGet3DPoints(h) %#ok
%BTKGETPOINTS Extract points from a biomechanical acquisition
% 
%  POINTS = BTKGETPOINTS(H) returns a structure of points.  Each 
%  fieldname in POINTS corresponds to a point's label.  The biomechanical 
%  acquisition handle H is represented as a 32-bit integer and can be only
%  used with the btk* functions.
%
%  [POINTS POINTSINFO] = BTKGETPOINTS(H) returns points' informations  
%  in POINTSINFO.  The structure POINTSINFO has the followig fields:
%    - POINTSINFO.FIRSTFRAME
%    - POINTSINFO.FREQUENCY
%    - POINTSINFO.UNITS
%  POINTSINFO.FIRSTFRAME contains the index of the first frame of the 
%  acquisition. POINTSINFO.FREQUENCY contains the points' frequency.  
%  POINTSINFO.UNITS contains the unit of each point.  Each fieldname in 
%  POINTSINFO.UNITS corresponds to point's label. 

%  Author: A. Barré
%  Copyright 2009- Biomechanical ToolKit (BTK).

% The following comment, MATLAB compiler pragma, is necessary to avoid 
% compiling this M-file instead of linking against the MEX-file.  Don't remove.
%# mex

error(generatemsgid('NotSupported'),'MEX file for BTKGETPOINTS not found');

% [EOF] btkGet3DPoints.m

