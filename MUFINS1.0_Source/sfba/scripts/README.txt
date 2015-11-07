[Perl script] extract count table from output of wgni:
perl wgni2table.pl out_gni_wgni table_wgni
perl wgni2table.pl out_gni_wgni ess_genes table_wgni_ess

[R script] get statistical significance for each count in count table:
wgniStat("table_wgni", "stat_wgni")

[R script] predicting growth medium composition:
predictMedia("table_wgni", "media", 1e-7)