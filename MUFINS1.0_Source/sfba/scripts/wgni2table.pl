#!/usr/bin/perl -w
use strict;
use warnings;

my $Narg=@ARGV;
if($Narg==2){
	my $infile1=$ARGV[0];
	my $outfile=$ARGV[1];
	open (WGNI, "$infile1") or die "\nERROR: Could not open $infile1\n";
	open (OUT, ">$outfile") or die "\nERROR: Could not open $outfile\n";
	my $i=0;
	while(<WGNI>){
		chomp;
		if(/Gene/){		
			my @tt=split /\t/;
			my $gn=$tt[1];
			my @Npn=(); #number of positive, number of negative for a gene
			my @nts=(); #nutrient names
			LABLE:
			while(<WGNI>){
				if(/Nutrients/){
					while(<WGNI>){
						chomp;
						if(/^$/){last LABLE;}
						my @tt=split /\t/;
						my @wp=split(/\s/,$tt[1]);
						my @wn=split(/\s/,$tt[2]);
						if($i==0){push @nts,$tt[0];}
						push @Npn, ($wp[3],$wn[3]);
					}
				}				
			}
			if($i==0){
				foreach my $nt (@nts){
					print OUT "\t$nt.WP";
					print OUT "\t$nt.WN";
				}
				print OUT "\n";
			}
			print OUT join("\t",($gn,@Npn)),"\n";	
			$i++;				
		}
	}
	close (WGNI) or die "\nERROR: Could not close $infile1\n";
	close (OUT) or die "\nERROR: Could not close $outfile\n";
}
elsif($Narg==3){
	my $infile1=$ARGV[0];
	my $infile2=$ARGV[1];
	my $outfile=$ARGV[2];
	open (WGNI, "$infile1") or die "\nERROR: Could not open $infile1\n";
	open (GESS, "$infile2") or die "\nERROR: Could not open $infile2\n";
	open (OUT, ">$outfile") or die "\nERROR: Could not open $outfile\n";
	my %essg=(); #essential gene name hash
	while(<GESS>){
		chomp;
		my @tt=split /\s/;
		$essg{$tt[0]}=1;
	}
	my $i=0;
	while(<WGNI>){
		chomp;
		if(/Gene/){		
			my @tt=split /\t/;
			if(exists $essg{$tt[1]}){
				my @Npn=(); #number of positive, number of negative for a gene
				my @nts=(); #nutrient names
				my $gn=$tt[1];
				LABLE:
				while(<WGNI>){
					if(/Nutrients/){
						while(<WGNI>){
							chomp;
							if(/^$/){last LABLE;}
							my @tt=split /\t/;
							my @wp=split(/\s/,$tt[1]);
							my @wn=split(/\s/,$tt[2]);
							if($i==0){push @nts,$tt[0];}
							push @Npn, ($wp[3],$wn[3]);
						}
					}				
				}
				if($i==0){
					foreach my $nt (@nts){
						print OUT "\t$nt.WP";
						print OUT "\t$nt.WN";
					}
					print OUT "\n";
				}
				print OUT join("\t",($gn,@Npn)),"\n";
				$i++;				
			}					
		}
	}
	close (WGNI) or die "\nERROR: Could not close $infile1\n";
	close (GESS) or die "\nERROR: Could not close $infile2\n";
	close (OUT) or die "\nERROR: Could not close $outfile\n";
}
else{
	print "Extracting the numbers of essential media of wpGNI and wnGNI into a table format\n";
	print "Usage:\n";
	print "perl wgni2table.pl infile1 [infile2] outfile\n";
	print "infile1: the predicted wgni results\n";
	print "infile2: a list of essential genes where one gene name at each line\n";
	print "outfile: the output table file used for R statstical computing"
}

