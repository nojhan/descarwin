function plottimeseries()
data=dlmread('timeseries.txt',',',0,1);

for i=1:size(data,1)
    for j=1:size(data,2) 
        datan(i,j)=data(i,j)/data(i,1);
        if datan(i,j) == 0 datan(i,j)=NaN;
    end
end

set(0,'DefaultAxesLineStyleOrder',{'*',':','o','-','--'});

set(0,'DefaultAxesColorOrder',[1 0 0;0 1 0;0 0 1;0 1 1;1 1 0; 1 0 1]);


warning off;


plot(datan.');

c = cell( 30, 1 );
for i1=1:30, c{i1}=num2str(i1); end
legend(c)

end