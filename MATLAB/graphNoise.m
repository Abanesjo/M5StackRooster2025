%Graphing to find the threshold value

time = load ('time.txt');
low = load ('silent.txt');
med = load ('occassional.txt');
high = load ('heavy.txt');
usual = load ('scattered.txt');


subplot(2, 2, 1);
plot(time, low);
title('Still Hand');
xlabel('Time in seconds');
ylabel('Movement in Degrees');
xlim([0 15]);
ylim([0 1]);
yline(0.15,'-');


subplot(2, 2, 2);
plot (time, med);
title('Deliberate Slight Movements');
xlabel('Time in seconds');
ylabel('Movement in Degrees');
xlim([0 15]);
ylim([0 20]);
yline(4.1,'-');


subplot(2, 2, 3);
plot (time, high);
title('Deliberate Rapid Movements');
xlabel('Time in seconds');
ylabel('Movement in Degrees');  
xlim([0 15]);
ylim([0 20]);
yline(5.2,'-');


subplot(2, 2, 4);
plot (time, usual);
title('Finding a right Position to sleep, and still henceforth');
xlabel('Time in seconds');
ylabel('Movement in Degrees');
xlim([0 15]);
ylim([0 20]);
yline(1.1,'-');
