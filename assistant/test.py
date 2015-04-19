# plot a sine wave from 0 to 4pi
from pylab import *
from matplotlib.ticker import MultipleLocator, FormatStrFormatter  
x_values = [1,2,3]
y_values = [2,1,5]
y=[3,3,3]
ax = subplot(111)
plot(x_values, y_values,'b+-')
plot(x_values, y,'bo-')
xlabel('x')
ylabel('sin(x)')
title('Simple plot')
grid(True)
savefig("sin.eps")
show()
