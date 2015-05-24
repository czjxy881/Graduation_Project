#!/usr/bin/python
#coding:utf-8
'''
TODO:
1.中文输出
2.命令行支持
'''
from pylab import *
import sys
def draw(FileName,Title,XLabel,YLabel):
	#draw_lines=['k-','k--','k-.','k:','ks--','kp--','k*--','k+--','kx--','kD--','kd--'];#线条种类
	draw_lines=['ks-','kD-','kd-','k*-','kp-','ko-','t1-'];#线条种类
	draw_index=0; #线条的索引
	f=open(FileName);
	x=f.readline();
	xTick=x.split()[1:]; #横坐标
	x=range(1,size(xTick)+1) #等间距
	Max=0;
	for line in f:
		print line
		if draw_index>10:
			raise LengthError;
		line=line.split();
		Name=line[0];
		line=line[1:];
		Max=max(Max,double(line[-1])) #y最大值
		plot(x,line,draw_lines[draw_index],label=Name)
		draw_index=draw_index+1;
	axis([0.5,0.5+size(xTick),0,double(Max)+0.01])#坐标轴
	xticks(x,xTick)#刻度


	#font_file = "/Library/Fonts/Microsoft/SimSun.ttf" fontproperties=font_img_title
	#font_img_title = matplotlib.font_manager.FontProperties(fname=font_file, size=14)
	title(Title)
	xlabel(XLabel)
	ylabel(YLabel)
	legend(loc=2)#0自动 1 右上 2左上
	#picName=FileName.split('.')[0]+'.eps'
	#show()
	savefig(FileName.rsplit('.',1)[0]+'.eps')

if __name__=='__main__':
	FileName='dataResult/BuildResult_4_6_10-1.txt'
	if len(sys.argv)>1:
		FileName=sys.argv[1];
	
	draw(FileName,"DataExce Time","database size","time[s]");
	#draw("BuildResult_4_6_10-1.txt",u"建库时间曲线",u"时间[s]",u"数据库大小"); #输出有问题
