function btkClearAnalysis(h) %#ok
%BTKCLEARANALYSIS Clear the content of the metadata ANALYSIS.
% 
%  BTKCLEARANALYSIS(H) clears the following children of the metadata ANALYSIS:
%    - NAMES;
%    - CONTEXTS;
%    - SUBJECTS;
%    - UNITS;
%    - VALUES;
%    - USED.
%  The analysis is then empty. Use MEX functions BTKAPPENDANALYSISPAREMETER 
%  and BTKREMOVEANALYSISPAREMETER to set analysis' parameters.

%  Author: A. Barré
%  Copyright 2009- Biomechanical ToolKit (BTK).

% The following comment, MATLAB compiler pragma, is necessary to avoid 
% compiling this M-file instead of linking against the MEX-file.  Don't remove.
%# mex

error(generatemsgid('NotSupported'),'MEX file for BTKCLEARANALYSIS not found');

% [EOF] btkClearAnalysis.m