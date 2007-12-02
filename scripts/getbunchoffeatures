#!/bin/csh
set bdir = $argv[1]
set ref = $argv[2]

set ref_log = "coaching/features/$ref.features"

foreach bench ($bdir/*)
	echo "Obtaining features from " $bench
	./getfeatures $bench >> $ref_log
end

