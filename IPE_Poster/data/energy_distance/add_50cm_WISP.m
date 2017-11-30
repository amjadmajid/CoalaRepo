#!/usr/bin/env octave
% Shift measured WISP values by loss level corresponding to 50cm free-air

% Reference: [1] TODO.
%
% Code development: Ivar in 't Veen (I.J.G.intVeen@student.tudelft.nl)
%
% Last update: November 1, 2015


% Copyright (c) 2015, Embedded Software Group,
% Delft University of Technology, The Netherlands. All rights reserved.
%
% Redistribution and use in source and binary forms, with or without
% modification, are permitted provided that the following conditions
% are met:
%
% 1. Redistributions of source code must retain the above copyright notice,
% this list of conditions and the following disclaimer.
%
% 2. Redistributions in binary form must reproduce the above copyright
% notice, this list of conditions and the following disclaimer in the
% documentation and/or other materials provided with the distribution.
%
% 3. Neither the name of the copyright holder nor the names of its
% contributors may be used to endorse or promote products derived from this
% software without specific prior written permission.
%
% THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
% "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
% LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
% PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
% HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
% SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
% TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
% PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
% LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
% NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
% SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

%%% TESTED with GNU OCTAVE 3.8.1 %%%

close all;
clear all;

freq = 900; % [MHz]
dist = .5; % [m]

wave = 299792458 / (freq*1000*1000); % [m]

loss = 20 * log10(4*pi*(dist/(wave))); % [dBm] %=25.59dB for 900MHz

function r = convert_csv(name, l)
	inputfile = dlmread([name ".csv"]);
	outputfile = fopen([name '_incl50cm.csv'], 'w');
	
	fprintf(outputfile, '1,2\n');

	outputfile

	for i = 3:size(inputfile,1)
		if isfinite(inputfile(i,2))
			fprintf(outputfile, '%e,%e\n', inputfile(i,1) + l, inputfile(i,2));
		endif;
	end;

	fclose(outputfile);

	r = 0;
end;

r = convert_csv('perbyte_WISP_120B-1s', loss);
r = convert_csv('perbyte_WISP-opti_120B-1s', loss);

