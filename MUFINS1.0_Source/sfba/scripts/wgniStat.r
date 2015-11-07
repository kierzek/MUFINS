wgniStat=function(infile, outfile){
#infile: wgni Table file
#outfile: output filename

TABLE=file(infile,'r');
OUT=file(outfile,'w');
table=read.table(file=TABLE,sep='\t',header=T,row.names=1)
stat=table;
Nnt=ncol(table); #number of nutrients
M=2^Nnt; #number of media
for(i in 1:nrow(table)){
  for(j in seq(1,Nnt,by=2)){
    stat[i,j]=1-phyper(table[i,j],M/2,M/2,table[i,j]+table[i,j+1]);
    stat[i,j+1]=1-phyper(table[i,j+1],M/2,M/2,table[i,j]+table[i,j+1]);
  }
}
stat=rbind(colnames(stat),stat);
rn=rownames(stat);
rn[1]='';
stat=cbind(rn,stat);
write("The p-value for weak GNIs\n",OUT);
write.table(stat,file=OUT,sep='\t',row.names=F,col.names=F,quote=F);
close(TABLE);
close(OUT);
}


