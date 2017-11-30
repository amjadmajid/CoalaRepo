#!/usr/bin/env octave
% Generate theoretical energy/byte values

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

function f = drawfunction(x,a,b,c,d,l,h)
		p = 1.0 .* erfc( 1./((x./a).^d) );

		L = l*8;
		H = h*8;

		E = b * (L+H);	% energy per bit
		C = c;		% constant energy

		G = L/(L+H) .* (1 .- p) .^ (L+H);

		f = E./G .+ C;
end

function r = generate_csv(name, x, a, b, c, d,l,h)
	y = drawfunction(x, a, b, c, d, l, h);

	outputfile = fopen([name '_theoretical.csv'], 'w');
	fprintf(outputfile, '1,2\n');

	for i = 1:size(x,2)
		fprintf(outputfile, '%f,%e\n', x(i), y(i)/1000000);

		if( y(i) > 100 )
			break;
		end;
	end;

	fclose(outputfile);

	r = 0;
end;

r = generate_csv('wisp', [0:.5:60], 60, .005, 0, 4, 12, 11);
r = generate_csv('ble',  [0:.5:87], 85, .021,  20, 2, 12, 24);

