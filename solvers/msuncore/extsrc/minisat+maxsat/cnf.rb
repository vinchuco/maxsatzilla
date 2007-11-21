#
# cnf.rb -- loading and saving CNF formulas
#
# Author : Jordi Planes
# 
# == Overview
#
# Module to read and wright CNF formulas and store them in PBO format
#
# == Example
# 
# require 'cnf'
# f = Formula.new
# f.load_cnf( 'filename.cnf' )
# f.to_pbo( 'filename.pbo' )
# f.to_cnf( 'filename2.cnf' )
#

require 'simplyset'

#module CNF
  
# = CONSTANTS

# minisat+
PBO_Solver = 'minisat+/minisat+'
PBO_Mult = '*'
PBO_Pos = '1 '
PBO_Neg = '-1 '
# opbdp - does not accept rhs = 0
#PBO_Solver = 'opbdp'
#PBO_Mult = ''
#PBO_Pos = '+1 '
#PBO_Neg = '-1 '

class Clause
  attr_accessor :literals
  def initialize( literals )
    @literals_to_s = literals
    @literals = literals.map{ |l| l.to_i }
  end

  def to_cnf
    @literals_to_s.join(' ') + ' 0'
  end

  def to_pbo
    right = 1
    @literals.map do |l| 
      if l > 0 
        PBO_Pos + PBO_Mult + ' x' + l.to_s
      else
        right -= 1
        PBO_Neg + PBO_Mult + ' x' + (-l).to_s
      end
    end.join(' ') + ' >= ' + right.to_s + ' ;'
  end
end

class Formula
  @@clauses = Array.new
  @@number_of_variables = nil

  def Formula.get_clause( id ) @@clauses[ id ] end

  attr_accessor :clause_ids, :clauses_with_literal

  def initialize
    @clause_ids = SimplySet.new
    @clauses_with_literal = Hash.new
    initialize_clauses_with_literal if @@number_of_variables
  end

  def clone #! TO BE MODIFIED
    formula = Formula.new
    formula.clause_ids = @clause_ids.clone 
    formula.clauses_with_literal = @clauses_with_literal.clone
    formula
  end

  def number_of_clauses
    @clause_ids.size
  end

  def empty?
    @clause_ids.size == 0
  end

  def contains_literal?( literal )
    not @clauses_with_literal[ literal ].empty?
  end

  def add( element )
    if element.kind_of?( Formula )
      add( element.clause_ids )
    elsif element.kind_of?( SimplySet )
      @clause_ids.merge( element )
    elsif element.kind_of?( Fixnum )
      @clause_ids.add( element )
      @@clauses[ element ].literals.each do |l|
        @clauses_with_literal[ l ][ element ] = true
      end
    else
      throw 'Not supported class for Formula : ' + element.class.to_s
    end
    self
  end

  def delete( element )
    if element.kind_of?( Formula )
      delete( element.clause_ids )
    elsif element.kind_of?( SimplySet )
      element.each{ |e| delete( e ) }
      # @clause_ids.subtract( element ) #!
    elsif element.kind_of?( Fixnum )
      raise 'Element not found' if not @clause_ids.include? element #!
      @clause_ids.delete( element )
      @@clauses[ element ].literals.each do |l| 
        @clauses_with_literal[ l ].delete( element )
      end
    else
      throw 'Not supported class for Formula : ' + element.class.to_s
    end
    self
  end

  def initialize_clauses_with_literal
        @@number_of_variables.times do |v|
          var = v.to_i + 1
          @clauses_with_literal[ var ] = Hash.new
          @clauses_with_literal[ -var ] = Hash.new
        end
  end

  def load_cnf( filename )
    local_number_of_clauses = 0
    puts 'Loading CNF file ' + filename
    File.open( filename, 'r').each_line do |line|
      case line
      when /^$/ 
      when /^c.*/
      when /^p cnf (\d+) (\d+)/
        @@number_of_variables = $1.to_i
        local_number_of_clauses = $2.to_i
        puts 'Number of variables ' + @@number_of_variables.to_s
        puts 'Number of clauses ' + local_number_of_clauses.to_s
        initialize_clauses_with_literal
      else 
        literals = line.split
        literals.pop # the last 0 is removed
        @@clauses.push( Clause.new( literals ) )
        add( @@clauses.size-1 )
        # @clause_ids.add( @@clauses.size )
        # @clause_ids[ @@clauses.size ].literals .each { |l| @clauses_with_literal[ l ][ @@clauses.size ] = true }
      end
    end
    unless @@clauses.size == local_number_of_clauses
      throw 'Number of clauses does not match ' \
      + local_number_of_clauses.to_s + ' != ' + number_of_clauses.to_s
    end
  end

  def load_mus( filename )
    File.open( filename, 'r' ).each_line do |line|
      @clause_ids.merge( line.split.map{ |x| x.to_i } )
    end
  end

  def to_s
    'Formula: ' + @clause_ids.to_a.join(' ')
  end

=begin
  def to_cnf( filename )
    File.open( filename, 'w' ) do |f|
      f.puts 'p cnf ' + @@number_of_variables.to_s + ' ' + number_of_clauses.to_s
      @clause_ids.each { |i| f.puts @@clauses[i].to_cnf }
    end    
  end
=end

  def to_cnf( filename )
    mapping = Hash.new
    File.open( filename, 'w' ) do |f|
      f.puts 'p cnf ' + @@number_of_variables.to_s + ' ' + number_of_clauses.to_s
      @clause_ids.each_with_index do |c, pos| 
        mapping[ pos ] = c; f.puts @@clauses[c].to_cnf
      end
    end
    mapping
  end

  def to_wcnf_with_mus( filename, mus_clauses )
    hardWeight = number_of_clauses.to_s
    File.open( filename, 'w') do |f|
      f.puts 'p wcnf ' + @@number_of_variables.to_s + ' ' + number_of_clauses.to_s
      number_of_clauses.times do |c|
        unless mus_clauses.clause_ids.include?( c )
          f.puts hardWeight + ' ' + @@clauses[c].to_cnf
        else
          f.puts '1 ' + @@clauses[c].to_cnf
        end
      end
    end
  end

  def to_pbo( filename )
    File.open( filename, 'w') do |f|
      f.puts '* #variable= ' + (@@number_of_variables + number_of_clauses).to_s \
      + ' #constraint= ' + number_of_clauses.to_s
      f.puts 'min: ' + @clause_ids.map{ |c| PBO_Pos + PBO_Mult + ' x' \
        + (c + number_of_clauses).to_s }.join(' ') + ' ;'
      number_of_clauses.times do |c|
        f.print PBO_Pos + PBO_Mult + ' x' + (c+number_of_clauses).to_s + ' '
        f.puts @@clauses[c].to_pbo        
      end
    end    
  end

  def to_pbo_with_mus( filename, mus_clauses )
    File.open( filename, 'w') do |f|
      f.puts '* #variable= ' + (@@number_of_variables + number_of_clauses).to_s \
      + ' #constraint= ' + number_of_clauses.to_s
      f.puts 'min: ' + mus_clauses.clause_ids.map{ |c| PBO_Pos + PBO_Mult + ' x' \
        + (c + number_of_clauses).to_s }.join(' ') + ' ;'
      number_of_clauses.times do |c|
        if mus_clauses.clause_ids.include?( c )
          f.print PBO_Pos + PBO_Mult + ' x' + (c+number_of_clauses).to_s + ' '
        end
        f.puts @@clauses[c].to_pbo        
      end
    end
  end

end

#end
