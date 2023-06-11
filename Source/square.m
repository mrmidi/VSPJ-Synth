% create a square wave. frequency is 3 hz, amplitude is 1, and the phase is 0.
% length of signal - 1 seconds

Fs = 1000; % sampling frequency
t = 0:1/Fs:1-1/Fs; % time vector
f = 3; % frequency of signal
A = 1; % amplitude of signal
N = Fs * 1; % number of samples