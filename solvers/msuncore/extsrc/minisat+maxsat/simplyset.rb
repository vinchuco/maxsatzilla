#!/usr/bin/env ruby
#
# simplyset.rb - defines the SimplySet class
#

class SimplySet
  include Enumerable 

  def initialize( hash = Hash.new, array = Array.new )
    @hash = hash
    @array = array
  end

  def clone
    SimplySet.new( @hash.dup, @array.dup )
  end

  def size
    @array.size
  end

  def add( element )
    if include?( element ) then return self end
    @hash[ element ] = true
    @array.push( element )
    self
  end

  def merge( list )
    list.is_a?(Enumerable) or raise ArgumentError, "value must be enumerable"
    list.each { |o| add( o) }
    self
  end

  def delete( element )
    @hash.delete( element )
    @array.delete( element )
    self
  end

  def subtract( list )
    list.is_a?(Enumerable) or raise ArgumentError, "value must be enumerable"
    list.each { |o| delete( o ) }
    self
  end

  def include?( element )
    @hash[ element ]
  end

  def each
    @array.each { |o| yield(o) }
    self
  end
end

if __FILE__ == $0
  set = SimplySet.new
  set.merge( [1,2,3] )
  puts 'First'
  set.each do |e|
    if e == 2 then set.add( 4 ) end
    puts e
  end
  
  puts 'Second'
  set = SimplySet.new
  set.merge( [1,3,4] )
  set.each do |e|
    if e == 4 then set.add( 2 ) end
    puts e
  end
end
