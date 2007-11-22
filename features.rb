#!/usr/bin/env ruby

`#{ARGV[0]} #{ARGV[1]}`.each_line do |line|
  case line
  when /Number of Variables: (\d+)/
    puts 'Vrs ' + $1
  when /Number of Clauses: (\d+)/
    puts 'Cls ' + $1
  when /Ratio Clasues\/Variables: (\d+.\d+)/
    puts 'CoV ' + $1
  when /Ratio Negative Clauses: (\d+.\d+)/
    puts 'Neg ' + $1
  when /Ratio Positive Clauses: (\d+.\d+)/
    puts 'Pos ' + $1
  when /Ratio Unit Clauses: (\d+.\d+)/
    puts 'Uni ' + $1
  when /Ratio Binary Clauses: (\d+.\d+)/
    puts 'Bin ' + $1
  when /Ratio Ternary Clauses: (\d+.\d+)/
    puts 'Ter ' + $1
  when /feature saps_BestSolution_Mean: (\d+.\d+)/
    puts 'SBC ' + $1
  when /feature saps_BestStep_Mean: (\d+.\d+)/
    puts 'SBM ' + $1
  when /feature saps_BestStep_CoeffVariance: (\d+.\d+)/
    puts 'SBV ' + $1
  when /feature saps_BestStep_Median: (\d+.\d+)/
    puts 'SMM ' + $1
  when /feature saps_BestStep_Q.10: (\d+.\d+)/
    puts 'SB1 ' + $1
  when /feature saps_BestStep_Q.90: (\d+.\d+)/
    puts 'SB9 ' + $1
  when /feature saps_AvgImproveToBest_Mean: (\d+.\d+)/
    puts 'SAM ' + $1
  when /feature saps_AvgImproveToBest_CoeffVariance: (\d+.\d+)/
    puts 'SAV ' + $1
  when /feature saps_FirstLMRatio_Mean: (\d+.\d+)/
    puts 'SFR ' + $1
  when /feature saps_FirstLMRatio_CoeffVariance: (\d+.\d+)/
    puts 'SFV ' + $1
  when /feature saps_BestCV_Mean: (\d+.\d+)/
    puts 'SCV ' + $1
  when /feature saps_totaltime: (\d+.\d+)/
    puts 'STT ' + $1
  when /feature gsat_BestSolution_Mean: (\d+.\d+)/
    puts 'GBC ' + $1
  when /feature gsat_BestSolution_CoeffVariance: (\d+.\d+)/
    puts 'GBV ' + $1
  when /feature gsat_BestStep_Mean: (\d+.\d+)/
    puts 'GBM ' + $1
  when /feature gsat_BestStep_CoeffVariance: (\d+.\d+)/
    puts 'GVV ' + $1
  when /feature gsat_BestStep_Median: (\d+.\d+)/
    puts 'GMM ' + $1
  when /feature gsat_BestStep_Q.10: (\d+.\d+)/
    puts 'GB1 ' + $1
  when /feature gsat_BestStep_Q.90: (\d+.\d+)/
    puts 'GB9 ' + $1
  when /feature gsat_AvgImproveToBest_Mean: (\d+.\d+)/
    puts 'GAM ' + $1
  when /feature gsat_AvgImproveToBest_CoeffVariance: (\d+.\d+)/
    puts 'GAV ' + $1
  when /feature gsat_FirstLMRatio_Mean: (\d+.\d+)/
    puts 'GFR ' + $1
  when /feature gsat_FirstLMRatio_CoeffVariance: (\d+.\d+)/
    puts 'GFV ' + $1
  when /feature gsat_BestCV_Mean: (\d+.\d+)/
    puts 'GSV ' + $1
  when /feature gsat_totaltime: (\d+.\d+)/
    puts 'GTT ' + $1
  end
end
