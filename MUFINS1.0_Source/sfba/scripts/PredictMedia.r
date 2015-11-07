predictMedia=function(infile, outfile, th){
#infile: wgni Table file
#outfile: output filename
#th: threshold for weak GNI

TABLE=file(infile,'r');
OUT=file(outfile,'w');
table=read.table(file=TABLE,sep='\t',header=T,row.names=1);
stat=table;
Ng=nrow(table); #number of essential genes for this model
Nnt=ncol(table)/2; #number of nutrients
M=2^Nnt; #number of media
ind=seq(1,ncol(table),by=2);
for(i in 1:Ng){
  for(j in ind){
    stat[i,j]=1-phyper(table[i,j],M/2,M/2,table[i,j]+table[i,j+1]);
    stat[i,j+1]=1-phyper(table[i,j+1],M/2,M/2,table[i,j]+table[i,j+1]);
  }
}
ind=seq(1,ncol(table),by=2);
media=matrix(0,Nnt,5);
for(i in 1:Nnt){
   media[i,1]=length(which(stat[,ind[i]]<th));
   media[i,2]=length(which(stat[,ind[i]+1]<th));
   media[i,3]=media[i,1]+ media[i,2];
   media[i,4]=1-phyper(media[i,1]-1,Ng,Ng,media[i,3]);
   media[i,5]=1-phyper(media[i,2]-1,Ng,Ng,media[i,3]);
}

nts=gsub("(\\..+)",'',colnames(table)[ind]);
write("\tPresence\tAbsence\tSum\tPval_Pre\tPval_Abs",OUT);
media=cbind(nts,media);
write.table(media,file=OUT,sep='\t',row.names=F,col.names=F,quote=F);
close(TABLE);
close(OUT);
}


